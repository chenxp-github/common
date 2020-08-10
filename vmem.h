// VMem.h: interface for the CVMem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VMEM_H__339F2711_6EF0_4C75_A9EB_4519542019BD__INCLUDED_)
#define AFX_VMEM_H__339F2711_6EF0_4C75_A9EB_4519542019BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "assert.h"
#include "syslog.h"
#include "mem.h"
#include "common.h" // Added by ClassView

class CVMem{
public:
    int *err_flag_ptr;
public:
    int IsError();
    int SetError();
    int ReadMem(long vaddr,void *buf,long size);
    int WriteMem(long vaddr,void *buf,long size);
    virtual int ReadString(long vaddr,CFileBase *str_des);
    virtual int Write64Mem(long addr,QWORD qw);
    virtual QWORD Read64Mem(long addr);
    virtual int Write32Mem(long addr,DWORD mem32);
    virtual DWORD Read32Mem(long addr);
    virtual int Write16Mem(long addr,WORD mem16);
    virtual WORD Read16Mem(long addr);
    CVMem();
    virtual ~CVMem();
    virtual int Init();
    virtual int Destroy();
    virtual int InitBasic();
    ///////////////////////////////////////
    virtual BYTE Read8Mem(long addr) = 0;
    virtual int Write8Mem(long addr,BYTE b) = 0;
    virtual int Write8Mem_Out(long addr,BYTE b) = 0;
};

#endif // !defined(AFX_VMEM_H__339F2711_6EF0_4C75_A9EB_4519542019BD__INCLUDED_)
