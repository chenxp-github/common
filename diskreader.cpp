// DiskReader.cpp: implementation of the CDiskReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DiskReader.h"
#include "headerfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiskReader::CDiskReader()
{
    this->InitBasic();
}
CDiskReader::~CDiskReader()
{
    this->Destroy();
}
int CDiskReader::InitBasic()
{
    CDisk::InitBasic();
    this->xml_header = NULL;
    this->header_size = 0;

    return OK;
}
int CDiskReader::Init()
{
    CDisk::Init();
    this->InitBasic();

    NEW(this->xml_header,CXml);
    this->xml_header->Init();

    return OK;
}
int  CDiskReader::Destroy()
{
    CDisk::Destroy();
    DEL(this->xml_header);
    this->InitBasic();

    return OK;
}
int  CDiskReader::Print()
{
    PD(this->header_size);

    return TRUE;
}
int CDiskReader::LoadFile(CFileBase *file)
{
    ASSERT(file);

    this->header_size = CHeaderFile::LoadHeader(file,this->xml_header);
    this->i_xml = this->xml_header;

    ASSERT(this->i_xml);    

    return OK;
}
