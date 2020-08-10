// ImgMaker.cpp: implementation of the CImgMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImgMaker.h"
#include "dirmgr.h"
#include "errorparser.h"
#include "notepad.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CImgMaker::CImgMaker()
{
    this->InitBasic();
}
CImgMaker::~CImgMaker()
{
    this->Destroy();
}
int CImgMaker::InitBasic()
{
    this->i_objbox = NULL;
    this->i_xml = NULL;
    this->disk_maker = NULL;
    this->asm_cache = NULL;
    this->i_wnd_log = NULL;
    this->i_file_img = NULL;
    this->ret_error = FALSE;

    return OK;
}
int CImgMaker::Init()
{
    this->InitBasic();
    
    NEW(this->disk_maker,CDiskMaker);
    this->disk_maker->Init();
    
    NEW(this->asm_cache,CMemStk);
    this->asm_cache->Init(100);
    
    return OK;
}
int  CImgMaker::Destroy()
{
    DEL(this->asm_cache);
    DEL(this->disk_maker);

    this->InitBasic();

    return OK;
}
int CImgMaker::MainLoop()
{
    ASSERT(this->i_objbox);
    ASSERT(this->i_xml);
    ASSERT(this->i_wnd_log);
    ASSERT(this->i_file_img);

    CProject prj;
    CMemFile mf_result;
    CMemFile mf_disk;
    CMem old_path,mem_err;
    CMem exe_path;
    CErrorParser err_parser;
    BOOL is_hide = TRUE;

    LOCAL_MEM(old_path);
    LOCAL_MEM(exe_path);

    prj.Init();
    mf_disk.Init();
    mf_result.Init();
    err_parser.Init();
    mem_err.Init();
    this->ret_error = FALSE;

    this->i_file_img->SetSize(0);
    prj.i_asm_cache = this->asm_cache;
    prj.i_dest_img_file = this->i_file_img;
    prj.i_file_result = &mf_result;

    this->LoadAllFiles(&mf_disk);
    prj.SetDisk(&mf_disk);
    prj.SetDestDir("temp");
    prj.AddExceptName("main.cpp");
    
    CDirMgr::GetCurDir(&old_path);

    GetModuleFileName(GetModuleHandle(NULL),exe_path.p,exe_path.GetMaxSize());
    CDirMgr::GetFileName(exe_path.p,&exe_path,FN_PATH);
    
    CDirMgr::ChDir(exe_path.p);
    if(CDirMgr::ChDir("VCL") == ERROR)
    {
        this->ret_error = TRUE;
        LOG("change directory to 'VCL' error\r\n");
        goto end;
    }
    
    is_hide = !(this->i_wnd_log->GetStyle() & WS_VISIBLE);

    LOG("%s\r\n",this->i_objbox->GetName());
    LOG("complie...");
    prj.ComplieAll();
    LOG("OK\r\n");
    
    this->i_file_img->SeekEnd();
    this->i_file_img->Putc(0);
    mem_err.Malloc(this->i_file_img->GetSize() + 10);
    err_parser.i_file_src = &mf_result;
    err_parser.i_file_filted = &mem_err;
    if(err_parser.HaveErrors())
    {
        this->ret_error = TRUE;
        err_parser.FilteErrors();
        syslog.Puts(mem_err.p);
        goto end;
    }

    if(is_hide)
        this->i_wnd_log->Hide();
end:
    CDirMgr::ChDir(old_path.p);

    return OK;
}

int CImgMaker::LoadAllFiles(CFileBase *file_disk)
{
    ASSERT(file_disk);  
    ASSERT(this->i_xml);
    
    CMem mem_inc,mem_name;
    CMemFile mf_user;

    LOCAL_MEM(mem_inc);
    LOCAL_MEM(mem_name);

    mf_user.Init();
    this->disk_maker->Destroy();
    this->disk_maker->Init();

    CXmlNode *px = this->i_xml->GetNodeByPath("/WorkShop/CommonFiles");
    this->LoadFiles(px);

    this->MakeUserFile(&mf_user);   
    this->disk_maker->AddFile(&mf_user,"process.h");
    this->i_objbox->i_xml_node->mf_attrib->Seek(0);
    while(this->i_objbox->i_xml_node->GetNextAttrib(&mem_name,&mem_inc))
    {
        if(mem_name.StrICmp("include") == 0)
        {
            
            px = this->i_xml->GetNodeByPath(mem_inc.p);
            if(px)
                this->LoadFiles(px);
        }
    }

    this->disk_maker->WriteToHeaderFile(file_disk);

    return OK;
}

int CImgMaker::LoadFiles(CXmlNode *px)
{
    static char buf[1024];
    CXmlNode *pnode;
    CMem mem_buf;

    if(px == NULL)
        return ERROR;

    mem_buf.Init();
    mem_buf.SetP(buf,1024);
    if(strcmp(px->GetName(),"file") == 0)
    {
        px->GetAttrib("name",&mem_buf);

        if(buf[0])
        {       
            if(px->mf_value)
            {           
                //add this file
                this->disk_maker->AddFile(px->mf_value,buf);
            }
        }
    }

    pnode = px->child;
    while(pnode)
    {
        this->LoadFiles(pnode);
        pnode = pnode->next;
    }

    return OK;
}

int CImgMaker::MakeUserFile(CFileBase *file)
{
    CXmlNode *px;

    ASSERT(file && this->i_objbox); 
    
    file->SetSize(0);

    px = this->i_objbox->i_xml_node->GetChildByName("code");
    if(px == NULL)
        return ERROR;

    file->WriteFile(px->mf_value);
    
    file->Puts("\r\nint _init_attrib()\r\n");
    file->Puts("{\r\n");
    
    if(this->i_objbox->mf_user_attrib)
        file->WriteFile(this->i_objbox->mf_user_attrib);
    
    file->Puts("\r\nreturn OK;\r\n");   
    file->Puts("}\r\n");    

    return OK;
}
