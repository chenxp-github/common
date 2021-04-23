// WndSplit.cpp: implementation of the CWndSplit class.
//
//////////////////////////////////////////////////////////////////////

#include "WndSplit.h"
#include "mem_tool.h"
#include "memdc.h"

#define SPLIT_WND_CLASS_NAME_V L"split_simple_wnd_v"
#define SPLIT_WND_CLASS_NAME_H L"split_simple_wnd_h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWndSplit::CWndSplit()
{
    this->InitBasic();
}
CWndSplit::~CWndSplit()
{
    this->Destroy();
}
int CWndSplit::InitBasic()
{
    CWnd::InitBasic();

    m_nWidth = 8;
    m_hbm = NULL;
    m_hbrDithered = NULL;
    m_nOldBarPos = 0;
    m_nBarpreMove = 0;
    this->m_nBarPos = 0;
    this->m_bCapture = false;
    this->m_bVertical = true;
    this->pane0 = NULL;
    this->pane1 = NULL;
	m_Callback.InitBasic();
    memset(&this->border_rect,0,sizeof(RECT));
    return OK;
}
int CWndSplit::Init(int vert)
{
    this->InitBasic();
    CWnd::Init();
    
	m_Callback.Init();

    m_nWidth = 6;
    WORD HashPattern[] = {0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA};
    m_hbm = ::CreateBitmap (8, 8, 1, 1, HashPattern);
    m_hbrDithered = ::CreatePatternBrush (m_hbm);
    m_nOldBarPos = m_nBarPos;
    m_nBarpreMove = m_nBarPos;
    this->m_bCapture = false;
    this->m_bVertical = (vert != 0);
    
    this->wnd_create->style = WS_CHILD|WS_VISIBLE;  
    if(this->m_bVertical)
        this->wnd_create->class_name = SPLIT_WND_CLASS_NAME_V;
    else
        this->wnd_create->class_name = SPLIT_WND_CLASS_NAME_H;

    return OK;
}

int  CWndSplit::Destroy()
{
    if(this->m_hbrDithered)
    {
        ::DeleteObject(m_hbrDithered);
        this->m_hbrDithered = NULL;
    }
    if(this->m_hbm)
    {
        ::DeleteObject(m_hbm);
        this->m_hbm = NULL;
    }
    
	m_Callback.Destroy();
    CWnd::Destroy();
    this->InitBasic();
    return OK;
}
int CWndSplit::OnLButtonDown(WPARAM wparam,LPARAM lparam)
{
    ::SetCapture(hwnd);
    m_bCapture = true;
    DrawBar(m_nBarPos);
    m_nBarpreMove = m_nBarPos;

    return OK;
}
int CWndSplit::OnPaint(WPARAM wparam,LPARAM lparam)
{   
    CMemDC can;
    PAINTSTRUCT ps;
    RECT r;

    can.Init();

    HDC hdc = ::BeginPaint(this->hwnd,&ps);

    ::GetClientRect(this->hwnd,&r);
    can.AttachDC(hdc);  
    can.SetPen(PS_SOLID,0,RGB(120,152,181));

    if(this->m_bVertical)
    {
        can.MoveTo(0,0);can.LineTo(0,r.bottom);
        can.MoveTo(r.right-1,0);can.LineTo(r.right-1,r.bottom);
    }
    else
    {
        can.MoveTo(0,0);can.LineTo(r.right-1,0);
        can.MoveTo(0,r.bottom-1);can.LineTo(r.right-1,r.bottom-1);
    }

    ::EndPaint(this->hwnd,&ps);
    return OK;
}
int CWndSplit::OnLButtonUp(WPARAM wparam,LPARAM lparam)
{
    DrawBar(m_nBarPos);
    ::ReleaseCapture();
    RecalcLayout(this->border_rect);
    m_bCapture = false;
    SendMessageW(this->wnd_create->hparent,WM_UNKNOWN,0,(LPARAM)this);
    return OK;
}
int CWndSplit::OnMouseMove(WPARAM wparam,LPARAM lparam)
{
    if (m_bCapture)
    {
        POINT pt = {0};
        static POINT Oldpt = {0};
        ::GetCursorPos(&pt);
        ::ScreenToClient (this->wnd_create->hparent, &pt);

        if ((pt.x == Oldpt.x) && (pt.y == Oldpt.y))
            return ERROR;

        Oldpt.x = pt.x;
        Oldpt.y = pt.y;

        int cx = this->border_rect.right - this->border_rect.left;
        int cy = this->border_rect.bottom - this->border_rect.top;

        if (m_bVertical)
        {
            m_nBarPos = pt.x;
            if (m_nBarPos < m_nWidth/2)
                m_nBarPos = m_nWidth/2;

            if (m_nBarPos > (cx - m_nWidth/2))
                m_nBarPos = cx - m_nWidth/2;
        }
        else
        {
            m_nBarPos = pt.y;
            if (m_nBarPos < (m_nWidth/2))
                m_nBarPos = m_nWidth/2;

            if (m_nBarPos > (cy - m_nWidth/2))
                m_nBarPos = cy - m_nWidth/2;
        }

        DrawBar(m_nOldBarPos);
        DrawBar(m_nBarPos);

        m_nOldBarPos = m_nBarPos;
    }

    return OK;
}
int CWndSplit::Registe()
{       
    WNDCLASSEXW wcex;    

    wcex.cbSize             = sizeof(WNDCLASSEXW); 
    wcex.style              = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc        = (WNDPROC)WndProc;
    wcex.cbClsExtra         = 0;
    wcex.cbWndExtra         = 0;
    wcex.hInstance          = ::GetModuleHandle(NULL);
    wcex.hIcon              = NULL;
    if (m_bVertical)
    {
        wcex.hCursor            = ::LoadCursor (NULL, IDC_SIZEWE);
        wcex.lpszClassName      = SPLIT_WND_CLASS_NAME_V;
    }
    else
    {
        wcex.hCursor            = ::LoadCursor (NULL, IDC_SIZENS);
        wcex.lpszClassName      = SPLIT_WND_CLASS_NAME_H;
    }
    wcex.hbrBackground      = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName       = NULL;
    
    wcex.hIconSm            = NULL;
    
    RegisterClassExW(&wcex);
    
    return OK;
}

int CWndSplit::DrawBar(int Pos)
{
    if (m_bCapture)
    {
        HDC hDC = ::GetDC(this->wnd_create->hparent);
        HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, (HBRUSH)m_hbrDithered);

        int cx = this->border_rect.right - this->border_rect.left;
        int cy = this->border_rect.bottom - this->border_rect.top;

        if (m_bVertical)
            ::PatBlt (hDC, Pos - m_nWidth/2, this->border_rect.top, m_nWidth, cy, PATINVERT);
        else
            ::PatBlt (hDC, this->border_rect.left, Pos - m_nWidth/2, cx, m_nWidth, PATINVERT);

        // Clean up
        ::SelectObject(hDC, hOldBrush);
        ::ReleaseDC(this->wnd_create->hparent, hDC);
    }

    return OK;
}

int CWndSplit::RecalcLayout(RECT r)
{
    int cx = r.right - r.left;
    int cy = r.bottom - r.top;
    int w2 = m_nWidth / 2;
    
    this->border_rect.left = r.left;
    this->border_rect.top = r.top;
    this->border_rect.right = r.right;
    this->border_rect.bottom = r.bottom;

    if (m_bVertical)
    {
        // Reposition our window panes and bar
        if(this->pane0)
            ::SetWindowPos(pane0, NULL, 0, 0, m_nBarPos - w2, cy, SWP_SHOWWINDOW );
        if(this->pane1)
            ::SetWindowPos(pane1, NULL, m_nBarPos - w2 + m_nWidth, r.top, cx - m_nBarPos - m_nWidth + w2, cy, SWP_SHOWWINDOW );
        ::SetWindowPos(this->hwnd, NULL, m_nBarPos - w2, r.top, m_nWidth, cy, SWP_SHOWWINDOW );
    }
    else
    {
        // Reposition our window panes and bar
        if(this->pane0)
            ::SetWindowPos(pane0, NULL, 0, 0, cx, m_nBarPos - w2, SWP_SHOWWINDOW );
        if(this->pane1)
            ::SetWindowPos(pane1, NULL, 0, m_nBarPos - w2 + m_nWidth, cx, cy - m_nBarPos - m_nWidth + w2, SWP_SHOWWINDOW );
        ::SetWindowPos(this->hwnd, NULL, r.left, m_nBarPos - w2, cx, m_nWidth , SWP_SHOWWINDOW );
    }

	m_Callback.Run(EVENT_POS_CHANGED);

    return OK;
}

int CWndSplit::SetBarPos(int pos)
{
    this->m_nOldBarPos = pos;
    this->m_nBarPos = pos;
    this->RecalcLayout(this->border_rect);

    return OK;
}

int CWndSplit::GetPos() 
{
    return this->m_nBarPos;
}

int CWndSplit::GetBarWidth()
{
    return this->m_nWidth;
}

CClosure* CWndSplit::Callback()
{
	return &m_Callback;
}
