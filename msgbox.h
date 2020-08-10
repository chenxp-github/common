// MsgBox.h: interface for the CMsgBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGBOX_H__FC2C7834_DB77_4D8F_B392_33EFBFF95AAC__INCLUDED_)
#define AFX_MSGBOX_H__FC2C7834_DB77_4D8F_B392_33EFBFF95AAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wnd.h"
#include "comnctls.h"

#define MB_ERR      (-1)
#define MB_CANCEL   (MB_OK+101)

class CMsgBox:public CWnd{
public:
    int type,user_code;
    CButton *pb_1;
    CButton *pb_2;
    CButton *pb_3;
    CEditBox *eb_txt;
public:
    int Puts(WORD *str);
    int SetMsgBody(WORD *str);
    int CreateButtonsByType();
    int Show(HWND hp,WORD *title,WORD *msg_body,int type);
    CMsgBox();
    virtual ~CMsgBox();
    int Init();
    int Destroy();
    int InitBasic();
    int OnCreate(WPARAM wparam, LPARAM lparam);
    int OnClose(WPARAM wparam, LPARAM lparam);
    int OnSize(WPARAM wparam, LPARAM lparam);
    int OnCommand(WPARAM wparam, LPARAM lparam);
    int PreTransMsg(MSG *msg);
};


#endif // !defined(AFX_MSGBOX_H__FC2C7834_DB77_4D8F_B392_33EFBFF95AAC__INCLUDED_)
