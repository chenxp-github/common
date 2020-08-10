// Disk.cpp: implementation of the CDisk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Disk.h"
#include "misc.h"

#define _ROOT_STR   "/header/filelist/root"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDisk::CDisk()
{
    this->InitBasic();
}
CDisk::~CDisk()
{
    this->Destroy();
}
int CDisk::InitBasic()
{
    this->cur_dir = NULL;
    this->i_xml = NULL;
    return OK;
}
int CDisk::Init()
{
    this->InitBasic();
    return OK;
}
int  CDisk::Destroy()
{
    this->InitBasic();
    return OK;
}
int  CDisk::Print()
{
    return TRUE;
}

int CDisk::GetCurPath(CMem *path)
{
    ASSERT(path);
    ASSERT(this->cur_dir);

    CMem mem,buf;

    LOCAL_MEM(mem);
    LOCAL_MEM(buf);

    path->SetSize(0);
    this->cur_dir->GetPath(&mem);

    mem.Seek(0);
    mem.SetSplitChars("/");
    
    mem.ReadString(&buf);
    mem.ReadString(&buf);

    path->WriteFile(&mem,mem.GetOffset(),mem.GetSize() - mem.GetOffset());

    if(path->p[0] == 0)
    {
        path->p[0] = '/';
        path->p[1] = 0;
    }

    return OK;
}

int CDisk::ChDir(char *dir_name)
{
    ASSERT(dir_name);
    ASSERT(this->i_xml);

    CMem mem;

    LOCAL_MEM(mem);

    if(strcmp(dir_name,"..") == 0)
    {
        CXmlNode *root = this->i_xml->GetNodeByPath(_ROOT_STR);
        if(this->cur_dir == root)
            return ERROR;

        CXmlNode *px = this->cur_dir->GetParent();
        ASSERT(px && this->IsDir(px));
        this->cur_dir = px;
    }

    if(dir_name[0] == '/')
    {
        mem.Puts(_ROOT_STR);
        mem.Puts(dir_name);
        mem.Putc(0);

        CXmlNode *px = this->i_xml->GetNodeByPath(mem.p);
        if(px) this->cur_dir = px;
    }
    else
    {
        ASSERT(this->cur_dir);

        this->cur_dir->GetPath(&mem);       

        mem.SeekBack(1);
        mem.Putc('/');
        mem.Puts(dir_name);
        mem.Putc(0);

        CXmlNode *px = this->i_xml->GetNodeByPath(mem.p);
        if(px) this->cur_dir = px;
    }

    return OK;
}

int CDisk::ChDir(CXmlNode *px)
{
    ASSERT(px);
    
    if(!this->IsDir(px))
        return ERROR;

    this->cur_dir = px;

    return OK;
}

int CDisk::IsDir(CXmlNode *px)
{
    ASSERT(px);

    CMem mem;

    LOCAL_MEM(mem);

    px->GetAttrib("isdir",&mem);

    if(mem.StrICmp("true") == 0)
        return TRUE;

    return FALSE;
}

CXmlNode * CDisk::GetNextFile(CXmlNode *px)
{
    //if px = NULL then return the first file
    if(px == NULL)
    {
        ASSERT(this->i_xml);

        CXmlNode *proot = this->i_xml->GetNodeByPath(_ROOT_STR);
        ASSERT(proot);
        
        return proot->GetChild(0); //first child
    }

    return px->next;
}

int CDisk::GetFileName(CXmlNode *px,CMem *mem)
{
    ASSERT(px);
    ASSERT(mem);

    px->GetAttrib("name",mem);

    return OK;
}

long CDisk::GetFileStartPos(CXmlNode *px)
{
    ASSERT(px);

    CMem mem;
    
    LOCAL_MEM(mem);

    px->GetAttrib("offset",&mem);
    
    return CMisc::hex2dec_32(mem.p);    
}

long CDisk::GetFileSize(CXmlNode *px)
{
    ASSERT(px);

    CMem mem;

    LOCAL_MEM(mem);

    px->GetAttrib("size",&mem);
    
    return CMisc::hex2dec_32(mem.p);    
}

