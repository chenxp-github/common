// RemoteTab.cpp: implementation of the CRemoteTab class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RemoteTab.h"
#include "mem_tool.h"
#include "syslog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemoteTab::CRemoteTab()
{
    this->InitBasic();
}
CRemoteTab::~CRemoteTab()
{
    this->Destroy();
}
status_t CRemoteTab::InitBasic()
{
    this->mHwnd = NULL;
    this->mRemoteMem = NULL;
    return OK;
}
status_t CRemoteTab::Init()
{
    this->InitBasic();
    NEW(this->mRemoteMem,CRemoteMem);
    this->mRemoteMem->Init();

    return OK;
}
status_t CRemoteTab::Destroy()
{
    DEL(this->mRemoteMem);
    this->InitBasic();
    return OK;
}

status_t CRemoteTab::SetWnd(HWND hwnd)
{
    ASSERT(hwnd);

    this->mHwnd = hwnd;
    this->mRemoteMem->Destroy();
    this->mRemoteMem->Init();
    ASSERT(this->mRemoteMem->OpenProcessByHwnd(hwnd));
    this->mRemoteMem->Alloc(1024);
    return OK;
}   

int32_t CRemoteTab::GetItemCount()
{
    return ::SendMessageW(this->mHwnd,TCM_GETITEMCOUNT,0,0);
}

status_t CRemoteTab::GetItemText(int32_t index, CMem *mem)
{
    ASSERT(mem);

    TCITEMW ti;

    memset(&ti,0,sizeof(ti));
    ti.mask = TCIF_TEXT;
    ti.pszText = (wchar_t*)(this->mRemoteMem->GetRemotePtr() + sizeof(ti));
    ti.cchTextMax = this->mRemoteMem->GetSize() - sizeof(ti) - 1;
    this->mRemoteMem->Write(&ti,sizeof(ti));
    
    ::SendMessageW(this->mHwnd,TCM_GETITEMW,index,(LPARAM)this->mRemoteMem->GetRemotePtr());

    CMem buf;
    LOCAL_MEM(buf);

    ASSERT(buf.GetMaxSize() >= this->mRemoteMem->GetSize());
    this->mRemoteMem->Read(buf.p,this->mRemoteMem->GetSize());

    mem->SetSize(0);
    mem->PutsW((wchar_t*)(buf.p + sizeof(ti)));
    mem->PutcW(0);

    return OK;
}

status_t CRemoteTab::SetSel(int32_t index)
{       
    NMHDR nm;
    nm.code = TCN_SELCHANGE;
    this->mRemoteMem->Write(&nm,sizeof(nm));
    ::SendMessageW(this->mHwnd,TCM_SETCURSEL,index,0);
    ::SendMessage(this->mHwnd, 0xbc4e , 0, (LPARAM)this->mRemoteMem->GetRemotePtr());
    return OK;
}
