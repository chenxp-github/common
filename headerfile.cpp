// HeaderFile.cpp: implementation of the CHeaderFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HeaderFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char *CHeaderFile::str_end_of_xml = "[This is the end of XML!]\r\n";
int CHeaderFile::max_header_size = 128 * 1024;

CHeaderFile::CHeaderFile()
{
    this->InitBasic();
}
CHeaderFile::~CHeaderFile()
{
    this->Destroy();
}
int CHeaderFile::InitBasic()
{
    return OK;
}
int CHeaderFile::Init()
{
    this->InitBasic();  
    return OK;
}
int  CHeaderFile::Destroy()
{
    this->InitBasic();
    return OK;
}

int CHeaderFile::WriteToFile(CFileBase *file_des, CFileBase *file_src, CXml *pxml)
{
    //pxml can be NULL, if so only file_src is written to the file_des
    ASSERT(file_des);
    ASSERT(file_src);

    if(pxml)
    {
        pxml->WriteToFile(file_des);
        file_des->Puts(CHeaderFile::str_end_of_xml);
    }
    file_des->WriteFile(file_src);
    
    return OK;
}

int CHeaderFile::WriteToFile(char *fn_des, CFileBase *file_src, CXml *pxml)
{
    CFile file;
    int ret;
    
    ASSERT(file_src);
    ASSERT(pxml);
    
    file.Init();
    ret = file.OpenFile(fn_des,"wb+");
    ASSERT(ret);

    CHeaderFile::WriteToFile(&file,file_src,pxml);

    file.Destroy();

    return OK;
}

//return value is the header size;
int CHeaderFile::LoadHeader(CFileBase *header_file, CXml *pxml)
{
    CMem mem;
    int pos,hsize = -1;
    
    ASSERT(header_file);
    ASSERT(pxml);

    mem.Init();
    mem.Malloc(CHeaderFile::max_header_size + 10);

    header_file->Seek(0);
    header_file->WriteToFile(&mem,CHeaderFile::max_header_size);

    mem.Seek(0);
    pos = mem.SearchStr(CHeaderFile::str_end_of_xml,0,0);

    if(pos >= 0)
    {
        hsize = pos + strlen(CHeaderFile::str_end_of_xml);
    }

    mem.SetChar(pos,0);
    mem.SetSize(pos);

    pxml->LoadXml(&mem);

    mem.Destroy();

    return hsize;
}

int CHeaderFile::LoadHeader(char *fn, CXml *pxml)
{
    int pos;

    ASSERT(pxml);

    CFile file;

    file.Init();
    file.OpenFile(fn,"rb");

    pos = CHeaderFile::LoadHeader(&file,pxml);

    file.Destroy();

    return pos;
}
