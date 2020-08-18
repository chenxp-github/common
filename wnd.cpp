// Wnd.cpp: implementation of the CWnd class.
//
//////////////////////////////////////////////////////////////////////
#include "Wnd.h"
#include "syslog.h"
#include "mem_tool.h"
#include "filebase.h"

#define DEF_WND_CLASS           L"DEF_WND_CLASS"
#define WND_STATE_NORMAL        0
#define WND_STATE_ON_CREATE     1
#define WND_STATE_ON_DESTROY    2
#define MAX_USER_DATA           16

HINSTANCE __hinstance;
HINSTANCE GetSelfInstance()
{
    return __hinstance;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWnd::CWnd()
{
    this->InitBasic();
}
CWnd::~CWnd()
{
    this->Destroy();
}
status_t CWnd::InitBasic()
{
    WEAK_REF_CLEAR();

    this->hwnd = NULL;
    this->is_attached = FALSE;
    this->wnd_create = NULL;
    this->id = 0;
    this->vir_offx = 0;
    this->vir_offy = 0;
    this->vir_h = 0;
    this->vir_w = 0;
    this->state = WND_STATE_NORMAL;
    this->user_data = NULL;
    this->client_rect.left = 0;
    this->client_rect.top = 0;
    this->client_rect.right = 0;
    this->client_rect.bottom = 0;
    this->hfont = 0;
    return OK;
}
status_t CWnd::Init()
{
    this->InitBasic();
    

    NEW(this->wnd_create,WndCreate);
    memset(this->wnd_create,0,sizeof(WndCreate));
    MALLOC(this->user_data,int_ptr_t,MAX_USER_DATA);
#if _WINCE_ || _WM6X_
    DWORD WS_OVERLAPPEDWINDOW = 0;
#endif
    this->wnd_create->x = CW_USEDEFAULT;
    this->wnd_create->y = CW_USEDEFAULT;
    this->wnd_create->w = CW_USEDEFAULT;
    this->wnd_create->h = CW_USEDEFAULT;
    this->wnd_create->class_name = DEF_WND_CLASS;
    this->wnd_create->style = WS_VISIBLE|WS_OVERLAPPEDWINDOW;
    this->wnd_create->hinst = GetSelfInstance();    
    this->wnd_create->id = (HMENU)this->GetUniqueID();
    
    return OK;
}
status_t CWnd::Destroy()
{
    WEAK_REF_DESTROY();
    if(this->hfont)
    {
        DeleteObject(this->hfont);
        this->hfont = 0;
    }

    FREE(this->user_data);
    if(wnd_create)
    {
        FREE(wnd_create->title);
        DEL(this->wnd_create);
    }

    if(this->is_attached)
    {
        this->hwnd = NULL;
    }
    else
    {
        if(this->hwnd)
        {
            CWnd::SetWndLong(this->hwnd,GWL_USERDATA,0);
            ::DestroyWindow(this->hwnd);
            this->hwnd = NULL;
        }
    }
    this->InitBasic();
    return OK;
}
status_t CWnd::Create()
{
    ASSERT(this->hwnd == NULL);
    WndCreate *w = this->wnd_create;
    HWND hp = w->hparent;

    this->hwnd = CreateWindowExW(w->ex_style,
                                w->class_name,
                                w->title,
                                w->style,
                                w->x,w->y,w->w,w->h,
                                hp,
                                NULL,
                                w->hinst,
                                (LPVOID)this);
    ASSERT(this->hwnd);
    this->id = w->id;
    this->is_attached = FALSE;
    return OK;
}
LRESULT CALLBACK CWnd::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CWnd *self = NULL;
    
#if _WIN32_ || _WIN64_
    DWORD first_msg = WM_NCCREATE;
#endif

#if _WINCE_ || _WM6X_
    DWORD first_msg = WM_CREATE;
#endif
        
    if(message == first_msg)
    {
        LPCREATESTRUCT pc = (LPCREATESTRUCT)lParam;
        ASSERT(pc);     
        self = (CWnd*)(pc->lpCreateParams);
        ASSERT(self);
        self->hwnd = hwnd;
        CWnd::SetWndLong(hwnd,GWL_USERDATA,(int_ptr_t)(&self->user_data));
        self->SetUserData(0,(int_ptr_t)self);
    }
    else
    {
        int_ptr_t data;
        if(CWnd::GetUserData(hwnd,0,&data))
        {
            self = (CWnd*)data;
        }
    }
    if(self == NULL)
    {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
    return self->ProcessAllMsg(message,wParam,lParam);
}
LRESULT CWnd::ProcessAllMsg(UINT message, WPARAM wparam, LPARAM lparam)
{
    if(this->state == WND_STATE_ON_CREATE || this->state == WND_STATE_ON_DESTROY || this->hwnd == NULL)
        return ::DefWindowProcW(this->hwnd, message, wparam, lparam);
    
    switch(message)
    {
        case WM_CREATE:  
            this->state = WND_STATE_ON_CREATE;
            this->OnCreate(wparam,lparam);
            this->state = WND_STATE_NORMAL;
        break;
        case WM_CLOSE: 
            this->state = WND_STATE_ON_DESTROY;
            this->OnClose(wparam,lparam);
            this->state = WND_STATE_NORMAL;
        return TRUE;
        case WM_PAINT: this->OnPaint(wparam,lparam);break;
        case WM_SIZE: this->OnSize(wparam,lparam);break;
        case WM_COMMAND:  this->OnCommand(wparam,lparam);break;
        case WM_NOTIFY:  this->OnNotify(wparam,lparam);break;
        case WM_LBUTTONDOWN:  this->OnLButtonDown(wparam,lparam);break;
        case WM_LBUTTONUP:  this->OnLButtonUp(wparam,lparam);break;
        case WM_RBUTTONDOWN:  this->OnRButtonDown(wparam,lparam);break;
        case WM_RBUTTONUP:  this->OnRButtonUp(wparam,lparam);break;
        case WM_HSCROLL:  this->OnHScroll(wparam,lparam);break;
        case WM_VSCROLL:  this->OnVScroll(wparam,lparam);break;
        case WM_MOUSEMOVE:  this->OnMouseMove(wparam,lparam);break;
        case WM_CHAR:  this->OnChar(wparam,lparam);break;
        case WM_KEYDOWN:  this->OnKeyDown(wparam,lparam);break;
        case WM_KEYUP:  this->OnKeyUp(wparam,lparam);break;
        case WM_WINDOWPOSCHANGED: this->OnPosChanged(wparam,lparam);break;
        case WM_UNKNOWN:this->OnUnknown(wparam,lparam);break;
        case WM_TIMER:this->OnTimer(wparam,lparam);break;
    }
    return ::DefWindowProcW(this->hwnd, message, wparam, lparam);
}
status_t CWnd::AttachWnd(HWND hwnd)
{
    this->is_attached = TRUE;
    this->hwnd = hwnd;
    return OK;
}
status_t CWnd::RegDefWnd()
{
#if _WIN32_ || _WIN64_
    WNDCLASSEXW wcex;    
    wcex.cbSize             = sizeof(WNDCLASSEX); 
    wcex.style              = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc        = (WNDPROC)CWnd::WndProc;
    wcex.cbClsExtra         = 0;
    wcex.cbWndExtra         = 0;
    wcex.hInstance          = GetSelfInstance();
    wcex.hIcon              = NULL;
    wcex.hCursor            = LoadCursor(NULL,IDC_ARROW);
    wcex.hbrBackground      = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName       = NULL;
    wcex.lpszClassName      = DEF_WND_CLASS;
    wcex.hIconSm            = NULL;
    
    return RegisterClassExW(&wcex);
#endif

#if _WINCE_ || _WM6X_
    WNDCLASSW   wc;
        
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC) WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = GetSelfInstance();
    wc.hIcon            = NULL;
    wc.hCursor          = 0;
    wc.hbrBackground    = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = 0;
    wc.lpszClassName    = DEF_WND_CLASS;
    return RegisterClass(&wc);
#endif
}
status_t CWnd::SetText(const wchar_t *str)
{
    if(this->wnd_create)
    {
        FREE(wnd_create->title);
        ZALLOC(wnd_create->title,wchar_t,crt_strlen_w(str)+1);
        crt_strcpy_w(wnd_create->title,str);
    }
    return (status_t)SendMessageW(hwnd,WM_SETTEXT,0,(LPARAM)str);    
}
status_t CWnd::GetText(wchar_t *buf, int32_t max)
{
    return (status_t)SendMessageW(hwnd,WM_GETTEXT,(WPARAM)max,(LPARAM)buf);
}
status_t CWnd::GetText_File(CMem *mem)
{
    ASSERT(mem);
    mem->SetSize(0);
    int32_t len = this->GetTextLen();
    if(len < 0)return OK;
    if(mem->GetMaxSize() <= len*sizeof(wchar_t))
    {
        mem->Destroy();
        mem->Init();
        mem->Malloc((len+1)*sizeof(wchar_t));
    }
    int32_t size = this->GetText((wchar_t*)mem->GetRawBuf(),(int)mem->GetMaxSize()/sizeof(wchar_t));
    mem->SetSize(size * sizeof(wchar_t));
    return OK;
}
status_t CWnd::MoveWindow(int32_t x, int32_t y, int32_t w, int32_t h)
{
    if(this->wnd_create)
    {
        this->wnd_create->x = x;
        this->wnd_create->y = y;
        this->wnd_create->w = w;
        this->wnd_create->h = h;
    }
    if(this->hwnd)
    {
        ::MoveWindow(this->hwnd,x,y,w,h,TRUE);
    }
    return OK;
}
status_t CWnd::Show()
{
    return ShowWindow(hwnd,SW_SHOW);
}
status_t CWnd::Hide()
{
    return ShowWindow(hwnd,SW_HIDE);
}
status_t CWnd::Close()
{
    return PostMessageW(hwnd,WM_CLOSE,0,(LPARAM)this);
}
status_t CWnd::OnCommand(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnNotify(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnLButtonDown(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnLButtonUp(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnRButtonDown(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnRButtonUp(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnHScroll(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnVScroll(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnMouseMove(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnChar(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnKeyDown(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnKeyUp(WPARAM wparam,LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnCreate(WPARAM wparam, LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnClose(WPARAM wparam, LPARAM lparam)
{
    this->Destroy();
    return OK;
}
status_t CWnd::OnPaint(WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
    ::BeginPaint(hwnd,&ps);
    ::EndPaint(hwnd,&ps);
    return OK;
}
status_t CWnd::OnSize(WPARAM wparam, LPARAM lparam)
{   
    return OK;
}
status_t CWnd::OnPosChanged(WPARAM wparam, LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnUnknown(WPARAM wparam, LPARAM lparam)
{
    return OK;
}
status_t CWnd::OnTimer(WPARAM wparam, LPARAM lparam)
{
    return OK;
}
status_t CWnd::GetTextLen()
{
    return (int32_t)GetWindowTextLength(hwnd);
}
status_t CWnd::GetUniqueID()
{
    static int32_t i = 1000;
    return i++;
}
status_t CWnd::IsMyCommand(WPARAM wparam)
{
    if((HMENU)LOWORD(wparam) == this->id)
        return TRUE;
    return FALSE;
}
status_t CWnd::IsMyNotify(LPARAM lparam)
{
    LPNMHDR p;
    
    p=(LPNMHDR)lparam;   
    ASSERT(p);
    if(p->hwndFrom == this->hwnd)
        return TRUE;
    return FALSE;
}
status_t CWnd::GetNotifyCode(LPARAM lparam)
{
    LPNMHDR p;
    
    p=(LPNMHDR)lparam;
    ASSERT(p);
    return p->code;
}
status_t CWnd::SetTextF(const wchar_t *szFormat, ...)
{
     wchar_t szBuffer [2048] ;
     
     va_list pArgList ;
     va_start (pArgList, szFormat) ;
     wvsprintfW(szBuffer, szFormat, pArgList) ;
     va_end (pArgList) ;
     
     this->SetText(szBuffer);
     return OK;
}
status_t CWnd::CreateCtrl(const wchar_t *ctrl_name)
{
    ASSERT(this->hwnd == NULL);
    WndCreate *w = this->wnd_create;
    w->class_name = ctrl_name;
    this->hwnd = CreateWindowExW(w->ex_style,
                                w->class_name,
                                w->title,
                                w->style,
                                w->x,w->y,w->w,w->h,
                                w->hparent,
                                w->id,
                                w->hinst,
                                NULL);
    if(!hwnd)
    {
        PD(GetLastError());
    }
    ASSERT(this->hwnd);
    CWnd::SetWndLong(this->hwnd,GWL_USERDATA,(int_ptr_t)(&this->user_data));
    this->SetUserData(0,(int_ptr_t)this);
    this->id = w->id;
    return OK;
}
status_t CWnd::SetParent(HWND hp)
{
    if(this->wnd_create)
        wnd_create->hparent = hp;
    return OK;
}
status_t CWnd::GetCommandCode(WPARAM wparam)
{
    return HIWORD(wparam);
}
status_t CWnd::GetWidth()
{
    RECT r;
    ::GetWindowRect(hwnd,&r);
    return r.right - r.left;
}
status_t CWnd::GetHeight()
{
    RECT r;
    ::GetWindowRect(hwnd,&r);
    return r.bottom - r.top;
}
status_t CWnd::SetStockFont(int32_t stock_font)
{
    HFONT hfont = (HFONT)::GetStockObject(stock_font);
    ::SendMessageW(hwnd,WM_SETFONT,(WPARAM)hfont,MAKELPARAM(1, 0));    
    return OK;
}
DWORD CWnd::GetStyle()
{
    if(this->hwnd)
        return ::GetWindowLongW(hwnd,GWL_STYLE);
    else if(this->wnd_create)
        return this->wnd_create->style;
    return 0;
}
DWORD CWnd::GetExStyle()
{
    if(this->hwnd)
        return ::GetWindowLongW(hwnd,GWL_EXSTYLE);
    else if(this->wnd_create)
        return this->wnd_create->ex_style;
    return 0;
}
status_t CWnd::IsVisible()
{
    return this->GetStyle() & WS_VISIBLE;
}
status_t CWnd::MoveWindow(RECT *pr)
{
    ASSERT(pr);
    this->MoveWindow(pr->left,pr->top,pr->right - pr->left,pr->bottom - pr->top);
    return OK;
}
status_t CWnd::SetStyle(DWORD style)
{
    if(this->wnd_create)
        this->wnd_create->style = style;
    
    if(this->hwnd)
    {       
        ::SetWindowLongW(this->hwnd,GWL_STYLE,style);
    }
    return OK;
}
status_t CWnd::AddStyle(DWORD style)
{
    return this->SetStyle(style | this->GetStyle());
}
status_t CWnd::RemoveStyle(DWORD style)
{
    return this->SetStyle((~style) & this->GetStyle());
}
status_t CWnd::SetExStyle(DWORD ex_style)
{
    if(this->wnd_create)
        this->wnd_create->ex_style = ex_style;
    if(this->hwnd)
    {       
        ::SetWindowLongW(this->hwnd,GWL_EXSTYLE,ex_style);
    }
    return OK;
}
status_t CWnd::AddExStyle(DWORD ex_style)
{
    return this->SetExStyle(ex_style | this->GetExStyle());
}
HWND CWnd::GetChild(int32_t index)
{
    HWND hchild;
    int32_t c = 0;
    hchild = ::GetWindow(hwnd,GW_CHILD);
    while(hchild)
    {
        if(c == index)
            return hchild;
        hchild = ::GetWindow(hchild,GW_HWNDNEXT);
        c++;
    }
    return NULL;
}
status_t CWnd::SetVirtualSize(int32_t w, int32_t h)
{
    this->vir_w = w;
    this->vir_h = h;
    return OK;
}
status_t CWnd::VirtualRectToReal(RECT *vr)
{
    ASSERT(vr);
    
    RECT rt;
    int32_t rw ,rh;
    
    CopyRect(&rt,&client_rect);
    rw = rt.right - rt.left;
    rh = rt.bottom - rt.top;
    if(!( rw && rh && vir_w && vir_h))
        return ERROR;
    vr->left = client_rect.left + vr->left * rw / vir_w;
    vr->top = client_rect.top + vr->top * rh / vir_h;
    vr->right = client_rect.left + vr->right * rw / vir_w;
    vr->bottom = client_rect.top + vr->bottom * rh / vir_h;
    return OK;
}
status_t CWnd::RealRectToVirtual(RECT *rr)
{
    ASSERT(rr);
    RECT rt;
    int32_t rw ,rh;
    CopyRect(&rt,&client_rect);
    rr->left -=  rt.left;
    rr->top -= rt.top;
    rr->right -= rt.left;
    rr->bottom -= rt.top;
    rw = rt.right - rt.left;
    rh = rt.bottom - rt.top;
    if(!( rw && rh && vir_w && vir_h))
        return ERROR;
    rr->left = rr->left * vir_w / rw;
    rr->top = rr->top * vir_h / rh;
    rr->right = rr->right * vir_w / rw;
    rr->bottom = rr->bottom * vir_h / rh;
    return OK;
}
status_t CWnd::MoveChildWindow(HWND hwnd,int32_t x, int32_t y, int32_t w, int32_t h)
{
    RECT r;
    r.left = x + this->vir_offx;
    r.top = y + this->vir_offy;
    r.right = r.left + w;
    r.bottom = r.top + h;
    this->VirtualRectToReal(&r);
    return ::MoveWindow(hwnd,r.left,r.top,r.right - r.left,r.bottom - r.top,1);
}
HWND CWnd::GetParent()
{
    if(this->wnd_create)
        return this->wnd_create->hparent;
    return NULL;
}
status_t CWnd::SendPasteMessage()
{
    return (status_t)::SendMessageW(hwnd,WM_PASTE,0,0);
}
status_t CWnd::IsDisabled(HWND hwnd)
{
    DWORD style = ::GetWindowLong(hwnd,GWL_STYLE);
    
    if(style & WS_DISABLED)
        return TRUE;
    return FALSE;
}
status_t CWnd::IsVisible(HWND hwnd)
{
    DWORD style = ::GetWindowLong(hwnd,GWL_STYLE);
    if(style & WS_VISIBLE)
        return TRUE;
    return FALSE;
}
status_t CWnd::SetUserData(int32_t index, int_ptr_t param)
{
    ASSERT(this->user_data);
    ASSERT(index >= 0 && index < MAX_USER_DATA);
    this->user_data[index] = param;
    return OK;
}
status_t CWnd::GetUserData(int32_t index,int_ptr_t *data)
{
    ASSERT(this->user_data);
    ASSERT(index >= 0 && index < MAX_USER_DATA);
    *data = this->user_data[index];
    return OK;
}
status_t CWnd::GetUserData(HWND hwnd, int32_t index,int_ptr_t *data)
{
    ASSERT(index >= 0 && index < MAX_USER_DATA);
    int_ptr_t **_p = (int_ptr_t **)CWnd::GetWndLong(hwnd,GWL_USERDATA);
    if(_p == NULL) return ERROR;
    int_ptr_t *p = *_p; 
    if(p)
    {
        *data = p[index];
        return OK;
    }
    return ERROR;
}
status_t CWnd::SetVirtualCtrlsOffset(int32_t vx, int32_t vy)
{
    this->vir_offx = vx;
    this->vir_offy = vy;
    return OK;
}

int_ptr_t CWnd::GetWndLong(HWND hwnd, uint32_t option)
{
#if _WIN32_ || _WINCE_ || _WM6X_
    return ::GetWindowLongW(hwnd,option);
#endif

#if _WIN64_
    return ::GetWindowLongPtrW(hwnd,option);
#endif

    ASSERT(0);
    return ERROR;
}
status_t CWnd::SetWndLong(HWND hwnd, uint32_t option,int_ptr_t data)
{   
#if _WIN32_ || _WINCE_ || _WM6X_
    return ::SetWindowLongW(hwnd,option,data);
#endif

#if _WIN64_
    return (status_t)::SetWindowLongPtrW(hwnd,option,data);
#endif

    ASSERT(0);
    return ERROR;
}

status_t CWnd::SetRealClientRect(RECT *r)
{
    if(r != NULL)
    {
        this->client_rect.left = r->left;
        this->client_rect.top = r->top;
        this->client_rect.right = r->right;
        this->client_rect.bottom = r->bottom;
    }
    else
    {
        ::GetClientRect(hwnd,&this->client_rect);
    }
    return OK;
}

status_t CWnd::SetFont(LOGFONT *font)
{
    ASSERT(font);
    if(this->hfont)
        ::DeleteObject(this->hfont);
    this->hfont = CreateFontIndirect(font);
    ::SendMessageW(hwnd,WM_SETFONT,(WPARAM)this->hfont,MAKELPARAM(1, 0));
    return OK;
}
