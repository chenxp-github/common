// Disk.h: interface for the CDisk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISK_H__E140D3A4_235C_4B19_8DA2_EB4F4CB04AD0__INCLUDED_)
#define AFX_DISK_H__E140D3A4_235C_4B19_8DA2_EB4F4CB04AD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xml.h"

class CDisk{
public:
    CXmlNode *cur_dir;
    CXml *i_xml;
public:
    long GetFileSize(CXmlNode *px);
    long GetFileStartPos(CXmlNode *px);
    int GetFileName(CXmlNode *px,CMem *mem);
    CDisk();
    ~CDisk();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
    int GetCurPath(CMem *path);
    int ChDir(char *dir_name);
    int ChDir(CXmlNode *px);
    int IsDir(CXmlNode *px);
    CXmlNode * CDisk::GetNextFile(CXmlNode *px);
};

#endif // !defined(AFX_DISK_H__E140D3A4_235C_4B19_8DA2_EB4F4CB04AD0__INCLUDED_)
