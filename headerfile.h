// HeaderFile.h: interface for the CHeaderFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEADERFILE_H__E1E11975_FB47_4DD2_A7B0_2F6B0DF3D903__INCLUDED_)
#define AFX_HEADERFILE_H__E1E11975_FB47_4DD2_A7B0_2F6B0DF3D903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "memfile.h"
#include "mem.h"
#include "xml.h"

class CHeaderFile{
public:
    static char *str_end_of_xml;
    static int max_header_size;
public:
    static int LoadHeader(char *fn,CXml *pxml);
    static int LoadHeader(CFileBase *header_file,CXml *pxml);
    static int WriteToFile(char *fn_des,CFileBase *file_src,CXml *pxml);
    static int WriteToFile(CFileBase *file_des,CFileBase *file_src,CXml *pxml);
    CHeaderFile();
    ~CHeaderFile();
    int Init();
    int Destroy();
    int InitBasic();
};

#endif // !defined(AFX_HEADERFILE_H__E1E11975_FB47_4DD2_A7B0_2F6B0DF3D903__INCLUDED_)
