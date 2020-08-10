// RemoteMem.h: interface for the CRemoteMem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTEMEM_H__FDE124EA_6993_4DFD_BB48_27D7E4163967__INCLUDED_)
#define AFX_REMOTEMEM_H__FDE124EA_6993_4DFD_BB48_27D7E4163967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"

class CRemoteMem{
public:
    HANDLE hProcess;
    void *mPtr;
    int_ptr_t mSize;
public:
    int_ptr_t GetSize();
    char * GetRemotePtr();
    status_t OpenProcessByHwnd(HWND hwnd);
    int OpenProcess(uint32_t pid);
    int_ptr_t Read(void *buf, uint32_t size);
    int_ptr_t Write(void *buf, uint32_t size);
    status_t Alloc(uint32_t size);
    CRemoteMem();
    virtual ~CRemoteMem();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};


#endif // !defined(AFX_REMOTEMEM_H__FDE124EA_6993_4DFD_BB48_27D7E4163967__INCLUDED_)
