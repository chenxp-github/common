// WindowFinder.cpp: implementation of the CWindowFinder class.
//
//////////////////////////////////////////////////////////////////////
#include "WindowFinder.h"
#include "mem.h"
#include "syslog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWindowFinder::CWindowFinder()
{
    this->InitBasic();
}
CWindowFinder::~CWindowFinder()
{
    this->Destroy();
}
status_t CWindowFinder::InitBasic()
{
    //add your code
    return OK;
}
status_t CWindowFinder::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CWindowFinder::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
status_t CWindowFinder::Print()
{
    //add your code
    return TRUE;
}

HWND CWindowFinder::FindWindowByTitle(wchar_t *title)
{
    ASSERT(title);
    
    HWND hdesk,hchild,hfirst;
    LOCAL_MEM(mem);

    hdesk = ::GetDesktopWindow();
    ASSERT(hdesk);
    
    hchild = ::GetWindow(hdesk,GW_CHILD);
    hfirst = hchild;

    while(hchild)
    {
        CWindowFinder::GetWindowTitle(hchild,&mem);
        mem.Seek(0);
        if(mem.StrCmpW(title) == 0)
            return hchild;
        hchild = ::GetWindow(hchild,GW_HWNDNEXT);
        if(hchild == hfirst)
            break;
    }

    return NULL;
}

status_t CWindowFinder::GetWindowTitle(HWND hwnd, CFileBase *file)
{
    ASSERT(file);
    LOCAL_MEM(mem);
    file->SetSize(0);
    CWindowFinder::SendMessage(hwnd,WM_GETTEXT,(int_ptr_t)(mem.GetMaxSize()/sizeof(wchar_t)),(LPARAM)mem.GetRawBuf());  
    file->PutsW((wchar_t*)mem.GetRawBuf());
    return OK;
}

HWND CWindowFinder::FindWindowByPartTitle(wchar_t *part_title)
{
    return FindWindowByPartTitle(GetDesktopWindow(),part_title);
}

HWND CWindowFinder::FindWindowByPartTitle(HWND hroot,wchar_t *part_title)
{
    ASSERT(part_title && hroot);
    
    HWND hchild,hfirst;
    LOCAL_MEM(mem);
    
    hchild = ::GetWindow(hroot,GW_CHILD);
    hfirst = hchild;
    
    while(hchild)
    {
        CWindowFinder::GetWindowTitle(hchild,&mem);     
        mem.Seek(0);
        if(mem.SearchStrW(part_title,1,0) >= 0)
            return hchild;
        hchild = ::GetWindow(hchild,GW_HWNDNEXT);
        if(hchild == hfirst)
            break;
    }
    
    return NULL;
}

status_t CWindowFinder::GetWindowText(HWND hwnd,CMem *mem)
{
    ASSERT(mem);
    int size;
    size =  CWindowFinder::SendMessage(hwnd,WM_GETTEXTLENGTH,0,0);  
    CWindowFinder::SendMessage(hwnd,WM_GETTEXT,(int_ptr_t)(mem->GetMaxSize()/sizeof(wchar_t))-10,(LPARAM)mem->GetRawBuf());
    mem->SetSize(size*sizeof(wchar_t));
    return OK;
}

status_t CWindowFinder::SendKeyDown(HWND hwnd,int vk)
{
    uint32_t lparam = 0;
    lparam = 1 | (1 < 30);
    return CWindowFinder::SendMessage(hwnd,WM_KEYDOWN,vk,lparam);   
}
status_t CWindowFinder::SendKeyUp(HWND hwnd,int vk)
{
    uint32_t lparam = 0;

    lparam = 1 | (1 < 30) | (1 << 31);
    return CWindowFinder::SendMessage(hwnd,WM_KEYUP,vk,lparam); 
}
status_t CWindowFinder::EditSelAll(HWND hwnd)
{   
    return CWindowFinder::SendMessage(hwnd,EM_SETSEL,0,-1);
}

status_t CWindowFinder::SendMessage(HWND hwnd, uint32_t message,WPARAM wparam, LPARAM lparam)
{
#if _WIN32_
    int_ptr_t t;    
    ::SendMessageTimeoutW(hwnd,message,wparam,lparam,SMTO_NORMAL,10000,(ULONG*)&t);
#endif

#if _WIN64_
    int_ptr_t t;
    ::SendMessageTimeoutW(hwnd,message,wparam,lparam,SMTO_NORMAL,10000,(PDWORD_PTR)&t);
#endif

#if _WINCE_
    ::SendMessageTimeout(hwnd,message,wparam,lparam,SMTO_NORMAL,10000,&t);
#endif

    return t;
}

status_t CWindowFinder::SendCopyCommand(HWND hwnd)
{
    return CWindowFinder::SendMessage(hwnd,WM_COPY,0,0);
}

HWND CWindowFinder::FindChildWindow(HWND hp, int index)
{   
    return FindChildWindow(hp,index,NULL);
}

HWND CWindowFinder::FindChildWindow(HWND hp, int index,const wchar_t *classname)
{   
    HWND hchild,hfirst;
    int count = -1;
    
    LOCAL_MEM(mem);

    hchild = ::GetWindow(hp,GW_CHILD);
    hfirst = hchild;
    while(hchild)
    {

        if(classname)
        {
            GetClassName(hchild,&mem);
            if(mem.StrCmpW(classname) == 0)
                count++;
        }
        else
        {
            count++;
        }

        if(count == index)return hchild;
        hchild = ::GetWindow(hchild,GW_HWNDNEXT);
        if(hchild == hfirst)
            break;      
    }
    
    return NULL;
}

status_t CWindowFinder::SendLButtonDown(HWND hwnd,int x,int y)
{
    return CWindowFinder::SendMessage(hwnd,WM_LBUTTONDOWN,0,MAKELONG(x,y));
}
status_t CWindowFinder::SendLButtonUp(HWND hwnd,int x,int y)
{
    return CWindowFinder::SendMessage(hwnd,WM_LBUTTONUP,0,MAKELONG(x,y));
}
status_t CWindowFinder::GetListItems(HWND hwnd)
{
    return CWindowFinder::SendMessage(hwnd,LB_GETCOUNT,0,0);
}

status_t CWindowFinder::GetComboCount(HWND hwnd)
{
    return CWindowFinder::SendMessage(hwnd,CB_GETCOUNT,0,0);
}

status_t CWindowFinder::GetComboText(HWND hwnd,int index, CFileBase *file)
{
    int ret;
    LOCAL_MEM(mem);
    ret = CWindowFinder::SendMessage(hwnd,CB_GETLBTEXT,(WPARAM)index,(LPARAM)mem.GetRawBuf());
    file->SetSize(0);
    file->PutsW((wchar_t*)mem.GetRawBuf());
    return ret;
}

status_t CWindowFinder::SetWindowText(HWND hwnd, const wchar_t *text)
{
    return CWindowFinder::SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)text);
}

status_t CWindowFinder::SendChar(HWND hwnd,wchar_t ch)
{
    return CWindowFinder::SendMessage(hwnd,WM_CHAR,ch,0);
}

uint32_t CWindowFinder::GetProcessID(HWND hwnd)
{
    uint32_t pid = 0;
    ::GetWindowThreadProcessId(hwnd,(LPDWORD)&pid);
    return pid;
}

status_t CWindowFinder::Click(HWND hwnd)
{
    return Click(hwnd,3,3);
}

status_t CWindowFinder::Click(HWND hwnd,int x,int y)
{
    CWindowFinder::SendLButtonDown(hwnd,x,y);
    CWindowFinder::SendLButtonUp(hwnd,x,y);
    return OK;
}


status_t CWindowFinder::ListSetSel(HWND hwnd, int index)
{
    return CWindowFinder::SendMessage(hwnd,LB_SETCURSEL,index,0); 
}

status_t CWindowFinder::CloseWindow(HWND hwnd)
{
    return CWindowFinder::SendMessage(hwnd,WM_CLOSE,0,0);
}

status_t CWindowFinder::GetClassName(HWND hwnd, CFileBase *file)
{
    ASSERT(file);
    file->SetSize(0);
    
    LOCAL_MEM(mem);

    ::GetClassNameW(hwnd,(wchar_t*)mem.GetRawBuf(),(int_ptr_t)(mem.GetMaxSize()>>1));
    file->PutsW((wchar_t*)mem.GetRawBuf());
    return OK;
}
status_t CWindowFinder::FindAllChildWindow(HWND hparent, CPtrStk *stk)
{
    ASSERT(stk && hparent);
    
    stk->Clear();
    int i = 0;
    HWND hwnd;

    CWindowFinder::FindChildWindow(hparent,stk);
    while(i < stk->GetLen())
    {
        hwnd = (HWND)stk->GetElem(i++);
        CWindowFinder::FindChildWindow(hwnd,stk);
    }
    
    return OK;
}

status_t CWindowFinder::FindChildWindow(HWND hparent, CPtrStk *stk)
{
    ASSERT(stk && hparent);

    HWND hchild,hfirst;

    hchild = ::GetWindow(hparent,GW_CHILD);
    hfirst = hchild;    
    while(hchild)
    {
        stk->Push(hchild);
        hchild = ::GetWindow(hchild,GW_HWNDNEXT);
        if(hchild == hfirst)
            break;
    }
    return OK;
}

status_t CWindowFinder::ClickKey(HWND hwnd,uint32_t vk,int interval)
{
    ASSERT(hwnd);
    CWindowFinder::SendKeyDown(hwnd,vk);
    if(interval > 0)crt_msleep(interval);
    CWindowFinder::SendKeyUp(hwnd,vk);
    if(interval > 0)crt_msleep(interval);
    return OK;
}

HWND CWindowFinder::FindWindowByRect(HWND hparent, RECT *r)
{
    RECT rt,rc;

    ::GetWindowRect(hparent,&rt);

    CPtrStk stk;
    stk.Init(1024);

    FindAllChildWindow(hparent,&stk);

    for(int i = 0; i < stk.GetLen(); i++)
    {
        HWND hwnd = (HWND)stk.GetElem(i);
        ASSERT(hwnd);
        ::GetWindowRect(hwnd,&rc);
        rc.left -= rt.left;
        rc.top -= rt.top;
        rc.right -= rt.left;
        rc.bottom -= rt.top;
        if(EqualRect(&rc,r))
            return hwnd;
    }

    return NULL;
}

void CWindowFinder::PressKey(int vk)
{
    keybd_event(vk,0,0,0);
}

void CWindowFinder::ReleaseKey(int vk)
{
    keybd_event(vk,0,KEYEVENTF_KEYUP,0);
}

HWND CWindowFinder::FindChildWindowByPath(HWND root, const int path[])
{
    int i = 0;
    HWND hwnd = root;

    while(path[i] >= 0)
    {
        hwnd = FindChildWindow(hwnd,path[i]);
        if(hwnd == NULL)
            break;
        i++;
    }

    if(path[i] < 0)
        return hwnd;

    return NULL;
}
