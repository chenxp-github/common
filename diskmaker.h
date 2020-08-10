// DiskMaker.h: interface for the CDiskMaker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISKMAKER_H__608B2CE1_3036_4D40_BF93_D6E1A2B83BFD__INCLUDED_)
#define AFX_DISKMAKER_H__608B2CE1_3036_4D40_BF93_D6E1A2B83BFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xml.h"
#include "memstk.h"
#include "headerfile.h"
#include "disk.h"

class CDiskMaker:public CDisk{
public:
    CMemStk *file_tab;
    CXml *xml_header;
public:
    int LoadDir(char *dir_name,char *ext_filter);
    int AddFile(char *fn);
    int WriteToHeaderFile(char *fn);
    int WriteToHeaderFile(CFileBase *header_file);
    long GetCurPos();
    int AddFile(CFileBase *file,char *file_name);
    int MkDir(char *dir_name);
    CDiskMaker();
    ~CDiskMaker();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_DISKMAKER_H__608B2CE1_3036_4D40_BF93_D6E1A2B83BFD__INCLUDED_)
