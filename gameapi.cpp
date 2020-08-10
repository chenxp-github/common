// GameAPI.cpp: implementation of the CGameAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameapi.h"
#include "assert.h"
#include "mem_tool.h"

CGameAPI g_gapi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGameAPI::CGameAPI()
{
    this->InitBasic();
}
CGameAPI::~CGameAPI()
{
    this->Destroy();
}
int CGameAPI::InitBasic()
{
    this->vram = NULL;
    return OK;
}
int CGameAPI::Init()
{
    this->InitBasic();
    return OK;
}
int CGameAPI::Destroy()
{
    this->EndDraw();
    this->CloseDisplay();
    this->InitBasic();
    return OK;
}

int CGameAPI::OpenDisplay(HWND hwnd)
{
    GXOpenDisplay(hwnd,GX_FULLSCREEN); 
    return OK;
}

int CGameAPI::CloseDisplay()
{
    GXCloseDisplay();
    return OK;
}

int CGameAPI::BeginDraw()
{
    this->vram = (char*)GXBeginDraw();
    return OK;
}

int CGameAPI::EndDraw()
{
    GXEndDraw();
    return OK;
}

int CGameAPI::Suspend()
{
    GXSuspend();
    return OK;
}

int CGameAPI::Resume()
{
    GXResume();
    return OK;
}

int CGameAPI::GetDisplayProperties(GXDisplayProperties *pro)
{
    ASSERT(pro);

    *pro =  GXGetDisplayProperties();
    return OK;
}
int CGameAPI::GetScreenWidth()
{
    GXDisplayProperties pro;
    this->GetDisplayProperties(&pro);
    return pro.cxWidth;
}

int CGameAPI::GetScreenHeight()
{
    GXDisplayProperties pro;
    this->GetDisplayProperties(&pro);
    return pro.cyHeight;
}
