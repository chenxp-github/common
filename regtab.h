// RegTab.h: interface for the CRegTab class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGTAB_H__5ABBBD4B_4FE5_4E7E_9D37_6004A0D04165__INCLUDED_)
#define AFX_REGTAB_H__5ABBBD4B_4FE5_4E7E_9D37_6004A0D04165__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "reg.h"

class CRegTab{
public:
    CReg **index;
    long top , bottom;
    long size;
public:
    CReg * GetRegFromNum(int reg_num);
    int SetRegVal(long index,_RegVal val);
    int SetRegVal(char *str_reg,_RegVal val);
    CReg * BSearch(char *reg_name);
    int GetRegNum(char *reg_str);
    int IsReg(char *str);
    int InitDefault();
    long BSearch_Pos(CReg *node,int order,int *find_flag);
    int InsOrdered(CReg *str,int order,int unique);
    int DelElem(long i);
    int InsertElem(long i,CReg *node);
    long BSearch(CReg *node,int order);
    CReg * BSearch_Node(CReg *node,int order);
    int Sort(int order);
    CReg * GetElem(long index);
    CReg * GetTop();
    CReg * Search(CReg *node);
    CReg *PushEmpty();
    CRegTab();
    int Clear();
    int DelTop();
    int Destroy();
    int Init(long init_size);
    int IsEmpty();
    int IsFull();
    int Pop(CReg *node);
    int Print();
    int Push(CReg *node);
    long GetLen();
    ~CRegTab();
};

#endif // !defined(AFX_REGTAB_H__5ABBBD4B_4FE5_4E7E_9D37_6004A0D04165__INCLUDED_)
