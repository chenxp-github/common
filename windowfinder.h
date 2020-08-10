// WindowFinder.h: interface for the CWindowFinder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWFINDER_H__EB63A767_C303_4D8D_9728_F6B57CC034DD__INCLUDED_)
#define AFX_WINDOWFINDER_H__EB63A767_C303_4D8D_9728_F6B57CC034DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"
#include "mem.h"
#include "ptrstk.h"

#undef SendMessage
#undef SendMessageW

class CWindowFinder{
public:
    static HWND FindChildWindowByPath(HWND root, const int path[]);
    static void PressKey(int vk);
    static void ReleaseKey(int vk);
    static status_t Click(HWND hwnd,int x,int y);
    static HWND FindWindowByRect(HWND hparent, RECT *r);
    static status_t ClickKey(HWND hwnd,uint32_t vk,int interval);
    static status_t FindAllChildWindow(HWND hparent, CPtrStk *stk);
    static status_t FindChildWindow(HWND hparent,CPtrStk *stk);
    static status_t GetClassName(HWND hwnd,CFileBase *file);
    static status_t CloseWindow(HWND hwnd);
    static status_t ListSetSel(HWND hwnd,int index);
    static status_t Click(HWND hwnd);
    static uint32_t GetProcessID(HWND hwnd);
    static status_t SendChar(HWND hwnd,wchar_t ch);
    static status_t SendKeyUp(HWND hwnd,int vk);
    static status_t SetWindowText(HWND hwnd,const wchar_t *text);
    static status_t GetComboText(HWND hwnd,int index,CFileBase *file);
    static status_t GetComboCount(HWND hwnd);
    static status_t GetListItems(HWND hwnd);
    static status_t SendLButtonDown(HWND hwnd,int x,int y);
    static status_t SendLButtonUp(HWND hwnd,int x,int y);
    static HWND FindChildWindow(HWND hp,int index);
    static HWND FindChildWindow(HWND hp, int index,const wchar_t *classname);
    static status_t SendCopyCommand(HWND hwnd);
    static status_t SendMessage(HWND hwnd,uint32_t message,WPARAM wparam,LPARAM lparam);
    static status_t EditSelAll(HWND hwnd);
    static status_t SendKeyDown(HWND hwnd,int vk);
    static status_t GetWindowText(HWND hwnd,CMem *mem);
    static HWND FindWindowByPartTitle(wchar_t *part_title);
    static HWND FindWindowByPartTitle(HWND hroot,wchar_t *part_title);
    static status_t GetWindowTitle(HWND hwnd,CFileBase *file);
    static HWND FindWindowByTitle(wchar_t *title);
    CWindowFinder();
    virtual ~CWindowFinder();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_WINDOWFINDER_H__EB63A767_C303_4D8D_9728_F6B57CC034DD__INCLUDED_)
