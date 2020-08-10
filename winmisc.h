// Misc.h: interface for the CWinMisc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MISC_H__11127D51_8EB8_4E4F_BE5C_AFECB701BD11__INCLUDED_)
#define AFX_MISC_H__11127D51_8EB8_4E4F_BE5C_AFECB701BD11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"
#include "filebase.h"
#include "mem.h"
#include "comnctls.h"

#include <shlobj.h>
#include <commdlg.h>
#include <shellapi.h>
#include <mmsystem.h>

#if _WIN32_ || _WIN64_
#include <io.h>
#pragma comment(lib, "winmm.lib")
#endif

#if _WM6X_
#include "aygshell.h"
#endif

class CWinMisc  
{
public:     
    static status_t SetWndTransparent(HWND hwnd, BYTE alpha);
    static status_t TabFocus(HWND hwnd_parent,HWND *tab_list, int32_t max, MSG *msg);
    static status_t IsButton(HWND hwnd);
    static status_t IsMultiLineEditBox(HWND hwnd);
    static status_t IsListBox(HWND hwnd);
    static status_t IsListView(HWND hwnd);
    static status_t IsTreeView(HWND hwnd);
    static status_t IsSingleLineEditBox(HWND hwnd);
    static status_t IsCombBox(HWND hwnd);
    static status_t GetMultiFileNames(HWND hwnd,CFileBase *file,const wchar_t *filter);
    static status_t CreateMenuBar(HWND hwnd,CMenu *menu);
    static status_t GetSaveFileName(HWND hwnd, CFileBase *file,const wchar_t *filter);
    static status_t GetScreenRes(int32_t *width,int32_t *height);
    static status_t GetSelfPath(CMem *mem);
    static status_t FolderSelect(HWND hp,CFileBase *file);
    static status_t GetAFileName(HWND hp,CFileBase *file,const wchar_t *filter);
    static status_t ChooseColor(HWND hp,COLORREF *color);
    static status_t ChooseFont(HWND hp,LOGFONTW *lf,COLORREF *color);
    static HRESULT CreateLink(const wchar_t *lpszPathObj,const wchar_t* lpszPathLink,const wchar_t* WorkDir);
    static status_t DeleteFolder(const wchar_t* pszFolder);
    static status_t IsKeyDown(int32_t nVirtKey);
    static status_t SetTopMost(HWND hwnd,bool flag);
    static status_t PlaySound(void *buf);
    static status_t MsgLoop();
    static void* GetRecourceAddr(uint32_t id,const wchar_t *type,int32_t *size);
    static status_t GetModulePath(HINSTANCE hinstance,CMem *mem);
};

#endif // !defined(AFX_MISC_H__11127D51_8EB8_4E4F_BE5C_AFECB701BD11__INCLUDED_)
