// Reg.h: interface for the CReg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REG_H__AAB47BFB_75A5_4570_891C_7704EB29EDC0__INCLUDED_)
#define AFX_REG_H__AAB47BFB_75A5_4570_891C_7704EB29EDC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "mem.h"

typedef union{
    BYTE *ptr_8;
    WORD *ptr_16;
    DWORD val_32;
    QWORD val_64;
}_RegVal;

class CReg{
public:
    char *name;
    int bytes;
    long index;
    _RegVal val;
public:
    int GetVal(_RegVal *val);
    int SetVal(_RegVal *v);
    CReg();
    ~CReg();
    int Init();
    int Destroy();
    int Copy(CReg *data);
    int Comp(CReg *data);
    int Print();
};

#endif // !defined(AFX_REG_H__AAB47BFB_75A5_4570_891C_7704EB29EDC0__INCLUDED_)
