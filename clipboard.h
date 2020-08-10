// ClipBoard.h: interface for the CClipBoard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPBOARD_H__346B828A_1F71_4569_8D12_C18733DB651C__INCLUDED_)
#define AFX_CLIPBOARD_H__346B828A_1F71_4569_8D12_C18733DB651C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"

class CClipBoard{
public:
    char *data;
    int size;
    HGLOBAL hmem;
public:
    int SetData(DWORD format,CFileBase *file);
    int Empty();
    int Open();
    int IsTextContent();
    CClipBoard();
    virtual ~CClipBoard();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_CLIPBOARD_H__346B828A_1F71_4569_8D12_C18733DB651C__INCLUDED_)
