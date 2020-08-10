// RemoteTreeView.h: interface for the CRemoteTreeView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTETREEVIEW_H__DE7C9A11_6A6F_467E_970C_87CAC281161F__INCLUDED_)
#define AFX_REMOTETREEVIEW_H__DE7C9A11_6A6F_467E_970C_87CAC281161F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "remotemem.h"
#include "mem.h"

class CRemoteTreeView{
public:
    CRemoteMem *mRemoteMem;
    HWND mHwnd;
public:
    status_t SetSelect(const wchar_t *name);
    status_t SetSelect(HTREEITEM hitem);
    HTREEITEM SearchItem(HTREEITEM root, const wchar_t *str);
    HTREEITEM SearchChild(HTREEITEM hParent,const wchar_t *buf);
    HTREEITEM GetChild(HTREEITEM hParent);
    HTREEITEM GetSibling(HTREEITEM htv);
    HTREEITEM GetParent(HTREEITEM hitem);
    status_t GetItemText(HTREEITEM hitem,CMem *mem);
    HTREEITEM GetRoot();
    HTREEITEM GetNextItem(HTREEITEM hti, int32_t code);
    status_t AttachWnd(HWND hwnd);
    CRemoteTreeView();
    virtual ~CRemoteTreeView();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
#endif // !defined(AFX_REMOTETREEVIEW_H__DE7C9A11_6A6F_467E_970C_87CAC281161F__INCLUDED_)
