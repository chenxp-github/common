// DanciDict.h: interface for the CDanciDict class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DANCIDICT_H__0D27C0BC_E9EF_4F24_9854_A8272CC9EC6A__INCLUDED_)
#define AFX_DANCIDICT_H__0D27C0BC_E9EF_4F24_9854_A8272CC9EC6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "indexfile.h"
#include "danci.h"

class CDanciDict{
public:
    CIndexFile *i_index_file;
public:
    int GetDictType();
    int GetDictName(CFileBase *file);
    int BSearch_Pos(char *str, int order, int *find_flag);
    int BSearch_Pos(CDanci *node, int order, int *find_flag);
    int GetDanci(int index,CDanci *danci);
    int GetMaxDanci();
    int LoadIndexFile(CFileBase *file);
    CDanciDict();
    virtual ~CDanciDict();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_DANCIDICT_H__0D27C0BC_E9EF_4F24_9854_A8272CC9EC6A__INCLUDED_)
