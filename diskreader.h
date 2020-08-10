// DiskReader.h: interface for the CDiskReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISKREADER_H__0E44821E_1028_434E_A44B_82B18463C487__INCLUDED_)
#define AFX_DISKREADER_H__0E44821E_1028_434E_A44B_82B18463C487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Disk.h"

class CDiskReader : public CDisk{
public:
    CXml *xml_header;
    long header_size;
public:
    int LoadFile(CFileBase *file);
    CDiskReader();
    ~CDiskReader();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_DISKREADER_H__0E44821E_1028_434E_A44B_82B18463C487__INCLUDED_)
