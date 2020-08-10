// RemoteTab.h: interface for the CRemoteTab class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTETAB_H__4FE119D6_1672_426F_AC20_A72083BB26B7__INCLUDED_)
#define AFX_REMOTETAB_H__4FE119D6_1672_426F_AC20_A72083BB26B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "remotemem.h"
#include "mem.h"

class CRemoteTab{
public:
    CRemoteMem *mRemoteMem;
    HWND mHwnd;
public:
    status_t SetSel(int32_t index);
    status_t GetItemText(int32_t index, CMem *mem);
    int32_t GetItemCount();
    status_t SetWnd(HWND hwnd);
    CRemoteTab();
    virtual ~CRemoteTab();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
#endif // !defined(AFX_REMOTETAB_H__4FE119D6_1672_426F_AC20_A72083BB26B7__INCLUDED_)
