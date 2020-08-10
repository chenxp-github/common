// MsgBox.cpp: implementation of the CMsgBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgBox::CMsgBox()
{
    this->InitBasic();
}
CMsgBox::~CMsgBox()
{
    this->Destroy();
}
int CMsgBox::InitBasic()
{
    CWnd::InitBasic();
    this->type = MB_ERR;

    return OK;
}
int CMsgBox::Init()
{
    this->InitBasic();
    CWnd::Init();
    this->pb_1 = NULL;
    this->pb_2 = NULL;
    this->pb_3 = NULL;
    this->eb_txt = NULL;
    this->user_code = 0;
    this->SetStyle(WS_VISIBLE | WS_BORDER | WS_CAPTION);

    return OK;
}
int  CMsgBox::Destroy()
{
    DEL(this->pb_1);
    DEL(this->pb_2);
    DEL(this->pb_3);
    DEL(this->eb_txt);

    CWnd::Destroy();
    this->InitBasic();
    return OK;
}
int CMsgBox::OnCreate(WPARAM wparam, LPARAM lparam)
{
    this->CreateButtonsByType();

    NEW(this->eb_txt,CEditBox);
    this->eb_txt->Init();
    this->eb_txt->SetParent(hwnd);
    this->eb_txt->AddStyle(ES_MULTILINE);
    this->eb_txt->AddStyle(WS_VSCROLL);
    this->eb_txt->AddStyle(ES_AUTOVSCROLL);
    this->eb_txt->Create();
    this->eb_txt->SetStockFont(DEFAULT_GUI_FONT);
    this->eb_txt->SetReadOnly(1);

    this->SetVirtualSize(211,171);
    this->OnSize(0,0);

    return OK;
}
int CMsgBox::OnClose(WPARAM wparam, LPARAM lparam)
{
    if(lparam != (LPARAM)this)  //if click close box
        return ERROR;   
    this->Destroy();
    return OK;
}
int CMsgBox::OnSize(WPARAM wparam, LPARAM lparam)
{
    if(this->pb_1)
        this->MoveChildWindow(pb_1->hwnd,10,140,61,21);
    if(this->pb_2)
        this->MoveChildWindow(pb_2->hwnd,75,140,61,21);
    if(this->pb_3)
        this->MoveChildWindow(pb_3->hwnd,140,140,61,21);

    this->MoveChildWindow(eb_txt->hwnd,5,5,201,126);

    return OK;
}
int CMsgBox::OnCommand(WPARAM wparam, LPARAM lparam)
{
    if(this->type == MB_OK)
    {
        if(this->pb_2->IsMyCommand(wparam))
        {
            ::SendMessage(this->GetParent(),WM_WND_USER_RET,IDOK,(LPARAM)this);
            this->Hide();
            this->Close();
        }
    }

    if(this->type == MB_CANCEL)
    {
        if(this->pb_2->IsMyCommand(wparam))
        {
            ::SendMessage(this->GetParent(),WM_WND_USER_RET,IDCANCEL,(LPARAM)this);
            this->Hide();
            this->Close();
        }
    }

    if(this->type == MB_YESNO)
    {
        if(this->pb_2->IsMyCommand(wparam))
        {
            ::SendMessage(this->GetParent(),WM_WND_USER_RET,IDYES,(LPARAM)this);
            this->Hide();
            this->Close();
        }

        if(this->pb_3->IsMyCommand(wparam))
        {
            ::SendMessage(this->GetParent(),WM_WND_USER_RET,IDNO,(LPARAM)this);
            this->Hide();
            this->Close();
        }
    }

    return OK;
}
int CMsgBox::PreTransMsg(MSG *msg)
{
    HWND hlist[100];

    int i = 0;
    
    if(this->type == MB_OK || this->type == MB_CANCEL)
        hlist[i++] = pb_2->hwnd;

    if(this->type == MB_YESNO)
    {
        hlist[i++] = pb_2->hwnd;
        hlist[i++] = pb_3->hwnd;
    }
    
    hlist[i++] = eb_txt->hwnd;

    return this->TabFocus(hlist,i,msg);
}

int CMsgBox::Show(HWND hp,WORD *title, WORD *msg_body, int type)
{
    RECT r;
    int w,h,div;

    div = 4;

#if _WINCE_
    div = 6;
#endif

    ::GetWindowRect(hp,&r);
    
    w = r.right - r.left;
    h = r.bottom - r.top;

    ASSERT(w && h);
    
    r.left = r.left + w / div;
    r.top = r.top + h / div;
    r.right = r.right - w/div;
    r.bottom = r.bottom - h/div;
    
    this->Destroy();
    this->Init();
    this->SetParent(hp);
    this->SetText(title);
    this->MoveWindow(&r);
    this->type = type;

    this->Create();

    this->eb_txt->SetText(msg_body);

    return OK;
}

int CMsgBox::CreateButtonsByType()
{
    if(this->type == MB_OK)
    {
        NEW(this->pb_2,CButton);
        this->pb_2->Init();
        this->pb_2->SetParent(hwnd);
        this->pb_2->SetText(L"确定");
        this->pb_2->Create();
        this->pb_2->SetStockFont(DEFAULT_GUI_FONT);
    }

    if(this->type == MB_CANCEL)
    {
        NEW(this->pb_2,CButton);
        this->pb_2->Init();
        this->pb_2->SetParent(hwnd);
        this->pb_2->SetText(L"取消");
        this->pb_2->Create();
        this->pb_2->SetStockFont(DEFAULT_GUI_FONT);
    }

    if(this->type == MB_YESNO)
    {
        NEW(this->pb_2,CButton);
        this->pb_2->Init();
        this->pb_2->SetParent(hwnd);
        this->pb_2->SetText(L"是");
        this->pb_2->Create();
        this->pb_2->SetStockFont(DEFAULT_GUI_FONT);
        NEW(this->pb_3,CButton);
        this->pb_3->Init();
        this->pb_3->SetParent(hwnd);
        this->pb_3->SetText(L"否");
        this->pb_3->Create();
        this->pb_3->SetStockFont(DEFAULT_GUI_FONT);
    }

    return OK;
}

int CMsgBox::SetMsgBody(WORD *str)
{
    ASSERT(str);
    ASSERT(this->eb_txt);
    return this->eb_txt->SetText(str);
}

int CMsgBox::Puts(WORD *str)
{
    ASSERT(str);
    ASSERT(this->eb_txt);
    return this->eb_txt->Puts(str);
}

