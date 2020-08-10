// DanciStk.h: interface for the DanciStk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DANCISTK_H__9F4530F4_D360_4DC4_BA39_A5C92DBAADDA__INCLUDED_)
#define AFX_DANCISTK_H__9F4530F4_D360_4DC4_BA39_A5C92DBAADDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "danci.h"

class CDanciStk{
public:
    CDanci **index;
    long top;
    long size;
public:
    int LoadDa3File(char *fn);
    static int call_back_get_danci(int *param,CDanci *d);
    int LoadDa3File(CFileBase *file_da3);
    static int call_back_mk_index_file(int *param,CFileBase *file);
    int MkIndexFile(CFileBase *file,char *dict_name,int type);
    int WriteToFile(char *fn);
    int WriteToFile(CFileBase *file);
    int BSearch_Pos(char *word,int order,int *find_flag);
    int InitBasic();  
    long BSearch_Pos(CDanci *node,int order,int *find_flag);
    int InsOrdered(CDanci *str,int order,int unique);
    int DelElem(long i);
    int InsertElem(long i,CDanci *node);
    long BSearch(CDanci *node,int order);
    CDanci * BSearch_CDanci(CDanci *node,int order);
    int Sort(int order);
    CDanci * GetElem(long index);
    CDanci * GetTop();
    CDanci * Search(CDanci *node);
    long Search_Pos(CDanci *node);
    CDanci *PushEmpty();
    CDanciStk();
    int Clear();
    int DelTop();
    int Destroy();
    int Init(long init_size);
    int IsEmpty();
    int IsFull();
    int Pop(CDanci *node);
    int Print();
    int Push(CDanci *node);
    long GetLen();
    ~CDanciStk();
};

#endif // !defined(AFX_DANCISTK_H__9F4530F4_D360_4DC4_BA39_A5C92DBAADDA__INCLUDED_)
