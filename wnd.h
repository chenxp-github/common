// Wnd.h: interface for the CWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WND_H__D77413FD_4C61_482D_84C4_6DC0828EABCE__INCLUDED_)
#define AFX_WND_H__D77413FD_4C61_482D_84C4_6DC0828EABCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"
#include "mem.h"

#if _WINCE_ || _WM6X_
#define DEFAULT_GUI_FONT SYSTEM_FONT
#endif

#define WM_UNKNOWN      (WM_USER+102)

#ifndef GWL_USERDATA
#define GWL_USERDATA    GWLP_USERDATA
#endif

#ifndef GWL_WNDPROC
#define GWL_WNDPROC     GWLP_WNDPROC
#endif

extern HINSTANCE __hinstance;
HINSTANCE GetSelfInstance();

typedef struct{
    wchar_t *title;
    int32_t x,y,w,h;
    DWORD style;
    DWORD ex_style;
    const wchar_t *class_name;
    HWND hparent;
    HINSTANCE hinst;
    HMENU id;
}WndCreate;

class CWnd{
public:
    WEAK_REF_DEFINE();

    HMENU id;
    HWND hwnd;
    status_t is_attached;
    WndCreate *wnd_create;
    status_t state;
    int_ptr_t *user_data;
    HFONT hfont;

    RECT client_rect;
    int32_t vir_offx,vir_offy;
    int32_t vir_w,vir_h;
public:     
    status_t SetFont(LOGFONT *font);
    CWnd();
    ~CWnd();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Create();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    status_t AttachWnd(HWND hwnd);
    static status_t RegDefWnd();
    status_t SetText(const wchar_t *str);
    status_t GetText(wchar_t *buf, int32_t max);
    status_t GetText_File(CMem *mem);
    status_t MoveWindow(int32_t x, int32_t y, int32_t w, int32_t h);
    status_t Show();
    status_t Hide();
    status_t Close();
    status_t GetTextLen();
    static status_t GetUniqueID();
    status_t IsMyCommand(WPARAM wparam);
    status_t IsMyNotify(LPARAM lparam);
    status_t GetNotifyCode(LPARAM lparam);
    status_t SetTextF(const wchar_t *szFormat, ...);
    status_t CreateCtrl(const wchar_t *ctrl_name);
    status_t SetParent(HWND hp);
    status_t GetCommandCode(WPARAM wparam);
    status_t GetWidth();
    status_t GetHeight();
    status_t SetStockFont(int32_t stock_font);
    DWORD GetStyle();
    DWORD GetExStyle();
    status_t IsVisible();
    status_t MoveWindow(RECT *pr);
    status_t SetStyle(DWORD style);
    status_t AddStyle(DWORD style);
    status_t RemoveStyle(DWORD style);
    status_t SetExStyle(DWORD ex_style);
    status_t AddExStyle(DWORD ex_style);
    HWND GetChild(int32_t index);
    status_t SetVirtualSize(int32_t w, int32_t h);
    status_t VirtualRectToReal(RECT *vr);
    status_t RealRectToVirtual(RECT *rr);
    status_t MoveChildWindow(HWND hwnd,int32_t x, int32_t y, int32_t w, int32_t h);
    HWND GetParent();
    status_t SendPasteMessage();
    static status_t IsDisabled(HWND hwnd);
    static status_t IsVisible(HWND hwnd);
    status_t SetUserData(int32_t index, int_ptr_t param);
    status_t GetUserData(int32_t index, int_ptr_t *data);   
    static status_t GetUserData(HWND hwnd, int32_t index,int_ptr_t *data);
    status_t SetVirtualCtrlsOffset(int32_t vx, int32_t vy);
    static status_t SetWndLong(HWND hwnd,uint32_t option, int_ptr_t data);
    static int_ptr_t GetWndLong(HWND hwnd,uint32_t option);
    status_t SetRealClientRect(RECT *r);
    //////////////////////////////////////////////////
    virtual LRESULT ProcessAllMsg(UINT message,WPARAM wparam,LPARAM lparam);
    virtual status_t OnClose(WPARAM wparam,LPARAM lparam);
    virtual status_t OnPaint(WPARAM wparam,LPARAM lparam);
    virtual status_t OnCreate(WPARAM wparam,LPARAM lparam); 
    virtual status_t OnSize(WPARAM wparam,LPARAM lparam);   
    virtual status_t OnCommand(WPARAM wparam,LPARAM lparam);    
    virtual status_t OnNotify(WPARAM wparam,LPARAM lparam); 
    virtual status_t OnLButtonDown(WPARAM wparam,LPARAM lparam);    
    virtual status_t OnLButtonUp(WPARAM wparam,LPARAM lparam);  
    virtual status_t OnRButtonDown(WPARAM wparam,LPARAM lparam);    
    virtual status_t OnRButtonUp(WPARAM wparam,LPARAM lparam);  
    virtual status_t OnHScroll(WPARAM wparam,LPARAM lparam);    
    virtual status_t OnVScroll(WPARAM wparam,LPARAM lparam);    
    virtual status_t OnMouseMove(WPARAM wparam,LPARAM lparam);  
    virtual status_t OnChar(WPARAM wparam,LPARAM lparam);   
    virtual status_t OnKeyDown(WPARAM wparam,LPARAM lparam);    
    virtual status_t OnKeyUp(WPARAM wparam,LPARAM lparam);  
    virtual status_t OnPosChanged(WPARAM wparam,LPARAM lparam);
    virtual status_t OnUnknown(WPARAM wparam,LPARAM lparam);
    virtual status_t OnTimer(WPARAM wparam,LPARAM lparam);
};

#endif // !defined(AFX_WND_H__D77413FD_4C61_482D_84C4_6DC0828EABCE__INCLUDED_)
