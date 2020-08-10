// DiskMaker.cpp: implementation of the CDiskMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DiskMaker.h"
#include "dirmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiskMaker::CDiskMaker()
{
    this->InitBasic();
}
CDiskMaker::~CDiskMaker()
{
    this->Destroy();
}
int CDiskMaker::InitBasic()
{
    CDisk::InitBasic();
    this->file_tab = NULL;
    this->cur_dir = NULL;
    this->xml_header = NULL;

    return OK;
}
int CDiskMaker::Init()
{
    CDisk::Init();
    this->InitBasic();

    CXmlNode *px,*px1;

    NEW(this->file_tab,CMemStk);
    this->file_tab->Init(256);
    
    NEW(this->xml_header,CXml);
    this->xml_header->Init();
    
    this->i_xml = this->xml_header;

    NEW(px,CXmlNode);
    px->Init();
    px->SetName("header");

    this->xml_header->AddRoot(px);

    NEW(px,CXmlNode);
    px->Init();
    px->SetName("filelist");

    px1 = px;

    this->xml_header->GetRoot()->AddChild(px);
    
    NEW(px,CXmlNode);
    px->Init();
    px->SetName("root");
    px->AddAttrib("isdir","true");
    
    px1->AddChild(px);
    
    this->ChDir(px); //set the root dir be current dir

    return OK;
}
int  CDiskMaker::Destroy()
{
    CDisk::Destroy();
    
    DEL(this->file_tab);
    DEL(this->xml_header);

    this->InitBasic();

    return OK;
}
int  CDiskMaker::Print()
{
    return TRUE;
}

int CDiskMaker::MkDir(char *dir_name)
{
    ASSERT(dir_name);
    ASSERT( !strchr(dir_name,'/') && !strchr(dir_name,' '));
    ASSERT(this->cur_dir);

    CXmlNode *px;
    
    NEW(px,CXmlNode);
    px->Init();
    px->AddAttrib("isdir","true");
    px->SetName(dir_name);

    this->cur_dir->AddChild(px);
    
    return OK;
}

int CDiskMaker::AddFile(CFileBase *file,char *file_name)
{
    ASSERT(file_name);
    ASSERT(file);
    ASSERT(this->cur_dir);

    CXmlNode *px;
    CMem mem;

    LOCAL_MEM(mem);

    NEW(px,CXmlNode);
    px->Init();
    px->SetName("file");

    px->AddAttrib("name",file_name);
    
    mem.SetSize(0);
    mem.Printf("%x",this->GetCurPos());
    mem.Putc(0);
    px->AddAttrib("offset",mem.p);

    mem.SetSize(0);
    mem.Printf("%x",file->GetSize());
    mem.Putc(0);
    px->AddAttrib("size",mem.p);
    
    this->cur_dir->AddChild(px);

    CMem *mf = this->file_tab->PushEmpty();
    ASSERT(mf);

    mf->Malloc(file->GetSize());
    mf->WriteFile(file);

    return OK;
}

long CDiskMaker::GetCurPos()
{
    int i;
    long s = 0;
    CMem *p;

    for(i = 0; i < this->file_tab->GetLen();i++)
    {
        p = this->file_tab->GetElem(i);
        ASSERT(p);
        s += p->GetSize();
    }

    return s;
}

int CDiskMaker::WriteToHeaderFile(CFileBase *header_file)
{
    ASSERT(header_file);

    int i;

    header_file->SetSize(0);

    if(this->file_tab->GetLen() <= 0)
    {
        this->i_xml->WriteToFile(header_file);
    }
    
    CMem *pmem = this->file_tab->GetElem(0);
    ASSERT(pmem);

    CHeaderFile::WriteToFile(header_file,pmem,this->xml_header);
    for(i = 1; i < this->file_tab->GetLen(); i++)
    {
        pmem = this->file_tab->GetElem(i);
        ASSERT(pmem);
        CHeaderFile::WriteToFile(header_file,pmem,NULL);
    }

    return OK;
}

int CDiskMaker::WriteToHeaderFile(char *fn)
{
    CFile file;

    file.Init();
    ASSERT(file.OpenFile(fn,"wb+"));
    
    this->WriteToHeaderFile(&file);

    file.Destroy();

    return OK;
}

int CDiskMaker::AddFile(char *fn)
{
    CFile file;
    CMem mem;

    LOCAL_MEM(mem);

    file.Init();

    ASSERT(file.OpenFile(fn,"rb"));

    CDirMgr::GetFileName(fn,&mem,FN_FILENAME);
    int ret = this->AddFile(&file,mem.p);

    file.Destroy();

    return ret;
}
//注意这个函数不能递归地创建目录，紧紧是把一个目录下的所有文件添加到当前目录
int CDiskMaker::LoadDir(char *dir_name,char *ext_filter)
{
    CDirMgr dir;
    CMem mem,old_path,ext,filter,buf;
    CMemFile mf_list;

    LOCAL_MEM(mem);
    LOCAL_MEM(old_path);
    LOCAL_MEM(ext);
    LOCAL_MEM(buf);
    
    mf_list.Init();

    dir.GetCurDir(&old_path);
    
    dir.Init();
    dir.ChDir(dir_name);
    dir.OpenDir();
    
    dir.SearchDir(0,&mf_list);

    mf_list.Seek(0);
    while(mf_list.ReadLine(&mem))
    {
        if( (mem.p[0] == '<') || (mem.p[0] == 0) )
            continue;

        if(ext_filter == NULL)
        {
            this->AddFile(mem.p);
        }
        else
        {
            CDirMgr::GetFileName(mem.p,&ext,FN_EXT);            
            filter.Init();
            filter.SetP(ext_filter,strlen(ext_filter));
            filter.Seek(0);
            filter.SetSplitChars(",");
            while(filter.ReadString(&buf))
            {
                if(ext.StrICmp(buf.p) == 0)
                {
                    this->AddFile(mem.p);
                    break;
                }
            }
        }
    }
    
    dir.ChDir(old_path.p);
    dir.Destroy();
    
    mf_list.Destroy();

    return OK;
}
