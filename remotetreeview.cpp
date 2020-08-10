#include "RemoteTreeView.h"
#include "mem_tool.h"
#include "syslog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemoteTreeView::CRemoteTreeView()
{
    this->InitBasic();
}
CRemoteTreeView::~CRemoteTreeView()
{
    this->Destroy();
}
status_t CRemoteTreeView::InitBasic()
{
    this->mHwnd = NULL;
    this->mRemoteMem = NULL;
    return OK;
}
status_t CRemoteTreeView::Init()
{
    this->InitBasic();
    NEW(this->mRemoteMem,CRemoteMem);
    this->mRemoteMem->Init();
    return OK;
}
status_t CRemoteTreeView::Destroy()
{
    DEL(this->mRemoteMem);
    this->InitBasic();
    return OK;
}

status_t CRemoteTreeView::AttachWnd(HWND hwnd)
{
    ASSERT(hwnd);

    this->mHwnd = hwnd;
    this->mRemoteMem->Destroy();
    this->mRemoteMem->Init();
    ASSERT(this->mRemoteMem->OpenProcessByHwnd(hwnd));
    this->mRemoteMem->Alloc(1024);
    return OK;
}   

HTREEITEM CRemoteTreeView::GetNextItem(HTREEITEM hti, int32_t code)
{
    return (HTREEITEM)::SendMessageW(this->mHwnd, TVM_GETNEXTITEM, code, (LPARAM)(hti));
}

HTREEITEM CRemoteTreeView::GetRoot()
{
    return this->GetNextItem(NULL,TVGN_ROOT);
}

status_t CRemoteTreeView::GetItemText(HTREEITEM hitem, CMem *mem)
{
    TVITEMW item;
    
    int_ptr_t max = ((int_ptr_t)mem->GetMaxSize()) >> 1;

    item.mask=TVIF_TEXT|TVIF_HANDLE;
    item.hItem=hitem;
    item.cchTextMax=max;
    item.pszText=(wchar_t*)((char*)(this->mRemoteMem->GetRemotePtr()) + sizeof(item));
    this->mRemoteMem->Write(&item,sizeof(item));

    ::SendMessageW(this->mHwnd, TVM_GETITEMW, 0, (LPARAM)this->mRemoteMem->GetRemotePtr());
    
    LOCAL_MEM(buf);
    ASSERT(this->mRemoteMem->GetSize() <= buf.GetMaxSize());

    this->mRemoteMem->Read(buf.GetRawBuf(),this->mRemoteMem->GetSize());

    mem->SetSize(0);
    mem->PutsW((wchar_t*)(buf.GetRawBuf() + sizeof(item)));

    return OK;
}

HTREEITEM CRemoteTreeView::GetParent(HTREEITEM hitem)
{
    return this->GetNextItem(hitem,TVGN_PARENT);
}

HTREEITEM CRemoteTreeView::GetSibling(HTREEITEM htv)
{
    return this->GetNextItem(htv,TVGN_NEXT);
}

HTREEITEM CRemoteTreeView::GetChild(HTREEITEM hParent)
{
    return this->GetNextItem(hParent,TVGN_CHILD);
}

HTREEITEM CRemoteTreeView::SearchChild(HTREEITEM hParent,const wchar_t *buf)
{
    HTREEITEM hChild;
    if(hParent!=NULL)
        hChild = this->GetNextItem(hParent,TVGN_CHILD);
    else
        hChild=GetRoot();
    
    if(hChild==NULL)
        return NULL;
    
    LOCAL_MEM(mem);
    
    while(hChild!=NULL);
    {
        this->GetItemText(hChild,&mem);
        if(mem.StrICmpW(buf) == 0)
            return hChild;
        hChild= this->GetNextItem(hChild,TVGN_NEXT);
    }
    
    return NULL;
}

HTREEITEM CRemoteTreeView::SearchItem(HTREEITEM root, const wchar_t *str)
{
    ASSERT(str);

    if(root == NULL)
        return NULL;

    LOCAL_MEM(mem);
    HTREEITEM  hchild, next = root;
    while(next)
    {
        this->GetItemText(next,&mem);
        if(mem.StrICmpW(str) == 0)
            return next;
        hchild = this->GetNextItem(next,TVGN_CHILD);
        if(hchild)
        {
            HTREEITEM h = this->SearchItem(hchild,str);
            if(h) return h;
        }
        next = this->GetNextItem(next,TVGN_NEXT);
    }

    return NULL;
}

status_t CRemoteTreeView::SetSelect(HTREEITEM hitem)
{
    return (status_t)::SendMessageW(this->mHwnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hitem);
}

status_t CRemoteTreeView::SetSelect(const wchar_t *name)
{
    ASSERT(name);
    HTREEITEM item = this->SearchItem(this->GetRoot(),name);
    if(item != NULL)
        this->SetSelect(item);
    return OK;
}
