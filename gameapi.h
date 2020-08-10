// GameAPI.h: interface for the CGameAPI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEAPI_H__30B4FFB7_1DE1_4D4F_BE48_C49B3A17B620__INCLUDED_)
#define AFX_GAMEAPI_H__30B4FFB7_1DE1_4D4F_BE48_C49B3A17B620__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"

#if _WINCE_
#include "gx.h"
#pragma comment(lib, "gx.lib")
#endif

class CGameAPI{
public:
    char *vram;
public:
    int GetScreenHeight();
    int GetScreenWidth();
    int GetDisplayProperties(GXDisplayProperties *pro);
    int Resume();
    int Suspend();
    int EndDraw();
    int BeginDraw();
    int CloseDisplay();
    int OpenDisplay(HWND hwnd);
    CGameAPI();
    virtual ~CGameAPI();
    int Init();
    int Destroy();
    int InitBasic();
};

extern CGameAPI g_gapi;

#endif // !defined(AFX_GAMEAPI_H__30B4FFB7_1DE1_4D4F_BE48_C49B3A17B620__INCLUDED_)
