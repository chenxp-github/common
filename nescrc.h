// NesCrc.h: interface for the CNesCrc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESCRC_H__4A15CAF5_909A_4EA5_8614_33E3FFDC092F__INCLUDED_)
#define AFX_NESCRC_H__4A15CAF5_909A_4EA5_8614_33E3FFDC092F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"
#include "common.h" // Added by ClassView
#include "filebase.h"

class CNesCrc{
public:
    DWORD   *m_CrcTable;
    DWORD   *m_CrcTableRev;
public:
    DWORD CrcRev(CFileBase *file, int start, int size);
    DWORD Crc(CFileBase *file, int start,int size);
    DWORD CrcRev( int size, BYTE *c );
    DWORD Crc( int size, BYTE *c );
    int MakeTable();
    int MakeTableRev();
    CNesCrc();
    virtual ~CNesCrc();
    int Init();
    int Destroy();
    int Copy(CNesCrc *p);
    int Comp(CNesCrc *p);
    int Print();
    int InitBasic();
    int CheckLinkPtr();
};

#endif // !defined(AFX_NESCRC_H__4A15CAF5_909A_4EA5_8614_33E3FFDC092F__INCLUDED_)
