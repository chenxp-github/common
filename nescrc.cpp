// NesCrc.cpp: implementation of the CNesCrc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NesCrc.h"
#include "mem_tool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNesCrc::CNesCrc()
{
    this->InitBasic();
}
CNesCrc::~CNesCrc()
{
    this->Destroy();
}
int CNesCrc::InitBasic()
{
    this->m_CrcTable = NULL;
    this->m_CrcTableRev = NULL;

    return OK;
}
int CNesCrc::Init()
{
    this->InitBasic();
    MALLOC(this->m_CrcTable,DWORD,256);
    MALLOC(this->m_CrcTableRev,DWORD,256);

    this->MakeTable();
    this->MakeTableRev();

    return OK;
}
int CNesCrc::Destroy()
{
    FREE(this->m_CrcTable);
    FREE(this->m_CrcTableRev);

    this->InitBasic();
    return OK;
}
int CNesCrc::Copy(CNesCrc *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
int CNesCrc::Comp(CNesCrc *p)
{
    return 0;
}
int CNesCrc::Print()
{
    //add your code
    return TRUE;
}
int CNesCrc::CheckLinkPtr()
{
    //add your code
    return TRUE;
}

int CNesCrc::MakeTable()
{
    int i, j;
    DWORD   r;

    for( i = 0; i < 256; i++ ) 
    {
        r = (DWORD)i << (32 - 8);
        for( j = 0; j < 8; j++ ) 
        {
            if( r & 0x80000000 ) r = (r << 1) ^ 0x04C11DB7;
            else                   r <<= 1;
        }
        m_CrcTable[i] = r & 0xFFFFFFFFUL;
    }

    return OK;
}
int CNesCrc::MakeTableRev()
{
    int i, j;
    DWORD   r;

    for( i = 0; i < 256; i++ ) 
    {
        r = i;
        for( j = 0; j < 8; j++ ) 
        {
            if( r & 1 ) r = (r >> 1) ^ 0xEDB88320;
            else        r >>= 1;
        }
        m_CrcTableRev[i] = r;
    }

    return OK;
}

DWORD CNesCrc::Crc(int size, BYTE *c)
{
    DWORD   r = 0xFFFFFFFF;

    while( --size >= 0 ) 
    {
        r = (r << 8) ^ m_CrcTable[(BYTE)(r >> (32 - 8)) ^ *c++];
    }
    return ~r & 0xFFFFFFFF;

}

DWORD CNesCrc::CrcRev(int size, BYTE *c)
{
    DWORD   r = 0xFFFFFFFF;
    while( --size >= 0 ) 
    {
        r = (r >> 8) ^ m_CrcTableRev[(BYTE)r ^ *c++];
    }
    return r ^ 0xFFFFFFFF;
}

DWORD CNesCrc::Crc(CFileBase *file, int start, int size)
{
    ASSERT(file);

    DWORD   r = 0xFFFFFFFF;
    file->Seek(start);

    while( --size >= 0 ) 
    {
        r = (r << 8) ^ m_CrcTable[(BYTE)(r >> (32 - 8)) ^ ((BYTE)file->Getc())];
    }

    return ~r & 0xFFFFFFFF;
}

DWORD CNesCrc::CrcRev(CFileBase *file, int start, int size)
{
    ASSERT(file);

    DWORD   r = 0xFFFFFFFF;

    file->Seek(start);
    while( --size >= 0 ) 
    {
        r = (r >> 8) ^ m_CrcTableRev[(BYTE)r ^ ((BYTE)file->Getc())];
    }
    return r ^ 0xFFFFFFFF;
}
