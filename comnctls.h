#ifndef __COMNCTLS_H
#define __COMNCTLS_H

#include "cruntime.h"
#include "wnd.h"
#include "mem.h"

#include <commctrl.h>
#include <shellapi.h>

#if _WIN32_ || _WIN64_
#include <richedit.h.>
#pragma comment(lib, "COMCTL32.LIB")
#define HAVE_RICH_EDIT  1
#define HAVE_TOOL_TIP   1
#endif

#if _WINCE_ || _WM6X_
#define HAVE_COMMAND_BAR    1
#endif

class CCtrl : public CWnd{
public:
public:
    status_t EmulateClick();
    CCtrl();
    ~CCtrl();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
/////////////////////////////////
class CButton : public CCtrl{
public:
    virtual int32_t Create();
};
/////////////////////////////////
class CStatic : public CCtrl{
public:
    virtual int32_t Create();
};
/////////////////////////////////
class CGroupBox : public CCtrl{
public:
    virtual int32_t Create();
};
/////////////////////////////////
class CCheckBox : public CCtrl{
public:
    status_t GetCheck();
    status_t SetCheck(BOOL flag);
    virtual int32_t Create();
};
/////////////////////////////////
class CRadioBox : public CCtrl{
public:
    status_t GetCheck();
    status_t SetCheck(BOOL flag);
    virtual int32_t Create();
};
/////////////////////////////////
class CScrollBar : public CCtrl{
public:
    int32_t min;
    int32_t max;
    int32_t page_size;
    int32_t cur_pos;
    int32_t line_step;
    bool is_control;
    int32_t inc;
public:
    uint32_t GetScrollBarFlag();
    status_t IsVertical();
    status_t AttachWindowWithScrollBar(HWND hwnd);
    status_t SetPageSize(int32_t proportion);
    status_t SetDir(BOOL is_vertical);
    status_t OnScroll(WPARAM wparam,LPARAM lparam);
    status_t SetLineStep(int32_t line_step);
    status_t GetPos();
    status_t SetPos(int32_t pos);
    status_t SetRange(int32_t min,int32_t max);
    status_t Init();
    virtual int32_t Create();
};
///////////////////////////////////
class CEditBox : public CCtrl{
public:
    status_t SetRectNp(RECT *r);
    status_t GetTextHeight(int32_t lfHeight);
    status_t SetTabStops(int32_t nSpace);
    status_t LineScroll(int32_t lines);
    status_t SetText_File(CFileBase *file);
    status_t Puts_File(CFileBase *file);
    status_t GetSel(int32_t *start,int32_t *end);
    status_t SetModifyFlag(BOOL flag);
    status_t Modified();
    status_t Putc(wchar_t ch);
    status_t Printf (const wchar_t * szFormat, ...);
    status_t Puts(const wchar_t *str);
    status_t MoveToEnd();
    status_t SetReadOnly(BOOL flag);
    status_t SetMaxText(int32_t max);
    status_t ReplaceSel(const wchar_t *buf);
    status_t SetSel(int32_t nStart,int32_t nEnd);
    status_t Create();
    status_t Init();
};
///////////////////////////////////
class CListBox : public CCtrl{
public:
    status_t SetViewWidth(int32_t width);
    status_t AddAllLines(CFileBase *file);
    status_t AddString(const wchar_t * str);
    status_t InsertString(int32_t i,wchar_t * str);
    status_t GetString(int32_t i,CMem *mem);
    status_t GetString(int32_t i,wchar_t * buf);
    status_t SetString(int32_t i,wchar_t * str);
    status_t DeleteString(int32_t i);
    status_t DeleteAllItems();
    int32_t FindString(wchar_t * str);
    status_t GetSelected();
    status_t SetSelect(int32_t i);
    status_t SetSelect(wchar_t * str);
    status_t GetMaxItems();
    status_t Init();
    virtual int32_t Create();
};
///////////////////////////////////
class CCombBox : public CCtrl{
public:
    status_t SetListItemHeight(int height);
    status_t SetButtonHeight(int height);
    status_t IsDropDownVisible();
    status_t GetItemText_File(int32_t index,CMem *mem);
    status_t GetItemText(int32_t index,wchar_t *str);
    status_t ShowDropDown(int32_t enable);
    status_t GetEditText_File(CMem *mem);
    int32_t FindString(const wchar_t *str);
    status_t AddString(const wchar_t *str);
    status_t InsertString(int32_t i,wchar_t *str);
    status_t DeleteString(int32_t i);
    status_t SetString(int32_t i,wchar_t *str);
    status_t DeleteAllItems();
    status_t GetSelected();
    status_t GetString(int32_t i,wchar_t *str);
    status_t GetEditText(wchar_t *str,int32_t max);
    status_t SetEditText(const wchar_t *str);
    status_t SetSelect(int32_t i);
    status_t SetSelect(const wchar_t *str);
    status_t Init();
    virtual int32_t Create();
};
///////////////////////////////////
class CMenu{
public:
    HMENU hmenu;
    HMENU hpmenu;
    const wchar_t *name;
    DWORD id;
    HWND hpwnd;
public:
    status_t EmuPush(HWND hwnd);
    status_t IsMyCommand(WPARAM wparam);
    status_t Enable(BOOL flag);
    status_t IsChecked();
    status_t CheckItem(BOOL flag);
    status_t DelItem();
#if _WIN32_ || _WIN64_
    status_t SetToWnd(HWND hwnd);
#endif
    status_t AddSeparator();
    status_t AddItem(CMenu *item);
    status_t AddMenu(CMenu *menu);
    status_t Create();
    status_t SetName(const wchar_t *name);
    CMenu();
    virtual ~CMenu();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
///////////////////////////////////
class CPopupMenu : public CMenu{
public:
    status_t Show();
    status_t Show(int32_t x,int32_t y);
    status_t SetParent(HWND hwnd);
    status_t Create();
};
///////////////////////////////////
class CStatusBar:public CCtrl{
public:
    status_t *pos;
    status_t parts;
public:
    status_t SetPartText(int32_t i,const wchar_t *str);
    status_t OnSize();
    status_t SetAveragePos();
    status_t SetPartPos(int32_t i,int32_t pos);
    status_t RefreshUI();
    status_t SetParts(int32_t parts);
    status_t Create();
    CStatusBar();
    virtual ~CStatusBar();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
///////////////////////////////////
class CProgressBar:public CCtrl{
public:
    status_t min,max;
    status_t step;
public:
    status_t SetColor(COLORREF fc,COLORREF bc);
    status_t SetPos(int32_t pos);
    status_t GoAStep();
    status_t SetStep(int32_t step);
    status_t SetRange(int32_t min,int32_t max);
    status_t Create();
    CProgressBar();
    virtual ~CProgressBar();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
//////////////////////////////////
class CImageList{
public:
    HIMAGELIST hImages;
    HBITMAP hBitmap;
public:
    status_t CreateImageList(DWORD bmp_id,int32_t img_width,int32_t img_height,int32_t color_depth,COLORREF mask_color) ;
    status_t GetImageCount();
    CImageList();
    virtual ~CImageList();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
//////////////////////////////////
class CTreeView : public CCtrl{
public:
    CImageList *image_list;
public:
    status_t GetPath(HTREEITEM hitem,CFileBase *file);
    status_t GetItemText(HTREEITEM hitem,CMem *mem);
    HTREEITEM GetNextItem(HTREEITEM hti, int32_t code);
    status_t SetItemImg(HTREEITEM htv,int32_t img_index);
    status_t CreateImageList(DWORD bmp_id,int32_t img_width,int32_t img_height,int32_t color_depth,COLORREF mask_color);
    HTREEITEM AddRoot(wchar_t *str);
    HTREEITEM AddItem(HTREEITEM hparent,wchar_t *str);
    status_t GetItemText(HTREEITEM hitem,wchar_t *buf,int32_t max);
    HTREEITEM GetParent(HTREEITEM hitem);
    status_t GetParentText(HTREEITEM hitem,wchar_t *buf,int32_t max);
    status_t OnSelChange(WPARAM wparam,LPARAM lparam);
    HTREEITEM GetSelectItem(LPARAM lparam);
    HTREEITEM GetSelectItem();
    status_t SetSelect(HTREEITEM hItem);
    HTREEITEM ItemFromPoint(int32_t x,int32_t y);
    status_t OnRClick(WPARAM wparam,LPARAM lparam);
    status_t DeleteItem(HTREEITEM hItem);
    status_t GetDepth(HTREEITEM hItem);
    HTREEITEM SearchChild(HTREEITEM hParent,wchar_t *buf);
    HTREEITEM GetRoot();
    HTREEITEM GetChild(HTREEITEM hParent);
    HTREEITEM GetSibling(HTREEITEM htv);
    status_t SetItemText(HTREEITEM htv,wchar_t *str);
    status_t IsLeaf(HTREEITEM htv);
    status_t Expand(HTREEITEM htv,UINT flag,int32_t recursive);
    status_t Create();
    CTreeView();
    virtual ~CTreeView();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
//////////////////////////////////
class CTBButton{
public:
    TBBUTTON *tbb;
public:
    status_t IsMyCommand(WPARAM wparam);
    status_t SetImgIndex(int32_t index);
    status_t SetToolTip(const wchar_t *caption);
    status_t SetState(BYTE state);
    status_t SetStyle(BYTE style);
    CTBButton();
    virtual ~CTBButton();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
//////////////////////////////////
class CToolBar:public CCtrl{
public:
    CImageList *image_list;
public:
    status_t OnSize();
    status_t AddSeparator();
    status_t GetButtonByCmd(int32_t cmd,CTBButton *ptbb);
    status_t OnNotify(WPARAM wparam,LPARAM lparam);
    status_t AddButton(CTBButton *tbb);
    status_t CreateImageList(DWORD bmp_id,int32_t img_width,int32_t img_height,int32_t color_depth,COLORREF mask_color);
    status_t SetButtonSize(int32_t cx,int32_t cy);
    status_t SetAutoSize();
    status_t Create();
    CToolBar();
    virtual ~CToolBar();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
//////////////////////////////////
#define LISTVIEW_MAX_COLUMNS    64

class CListView:public CCtrl{
public:
    int _column_widths[LISTVIEW_MAX_COLUMNS];
    int _column_count;
public:
    int GetImgIndex(int row);
    status_t SeleteItem(int32_t index);
    status_t SetImageList(CImageList *list, int32_t type);
    status_t IsSelected(int32_t index);
    status_t CheckAll(BOOL flag);
    status_t SetCheck(int32_t index,BOOL flag);
    status_t AddSubItem(int32_t col,const wchar_t *str);
    status_t IsChecked(int32_t index);
    status_t AddItem(int32_t img_index,const wchar_t *str);
    status_t SetListViewExStyle(DWORD style);
    status_t GetColumnWidth(int32_t index);
    status_t OnSize();
    status_t AddColumn(wchar_t *str,int32_t width);
    status_t FindItem(int32_t start,const wchar_t *str);
    status_t EnsureVisible(int32_t index);
    status_t GetItemText(int32_t row,int32_t col,CMem *mem);
    status_t SetSelectionMark(int32_t index);
    int GetFirstSelection();
    status_t GetItemCount();
    status_t GetSelectedCount();
    status_t SetTextColor(COLORREF color);
    status_t SetTextBkColor(COLORREF color);
    status_t SetBkColor(COLORREF color);
    status_t SetItemText(int32_t row,const wchar_t *str);
    status_t SetItemText(int32_t row,int32_t col,const wchar_t *str);
    status_t SetColumnWidth(int32_t index,int32_t width);
    status_t DeleteItem(int32_t index);
    status_t DeleteColumn(int32_t index);
    status_t Clear();
    status_t InsertSumItem(int32_t row,int32_t col,const wchar_t *str);
    status_t InsertItem(int32_t img_index,int32_t index,const wchar_t *str);
    status_t InsertColumn(int32_t index,const wchar_t *str,int32_t width);
    status_t Create();
    CListView();
    virtual ~CListView();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
//////////////////////////////////
class CSpinBox : public CCtrl{
public:
    status_t SetRange(int32_t min,int32_t max);
    status_t SetBuddy(HWND hwnd);
    status_t Init();
    virtual int32_t Create();
};
//////////////////////////////////
#if HAVE_RICH_EDIT
class CRichEdit: public CEditBox{
public:
    static HMODULE hmodule;
    CPopupMenu *menu_mouse;
    CMenu *mitem_copy;
    CMenu *mitem_paste;
    CMenu *mitem_cut;
    CMenu *mitem_sel_all;
    WNDPROC old_proc;
public:
    status_t GetRtfText(CFileBase *file);
    status_t SetRtfText(CFileBase *file);
    static DWORD CALLBACK callback_set_rtf(int_ptr_t dwCookie,LPBYTE pbBuff, LONG cb,LONG *pcb);
    static DWORD CALLBACK callback_get_rtf(int_ptr_t dwCookie,LPBYTE pbBuff, LONG cb,LONG *pcb);
    status_t InitBasic();
    CRichEdit();
    ~CRichEdit();
    static LRESULT NewEditProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    status_t UpdateMenuStatus();
    static int32_t UnloadModule();
    static int32_t LoadModule();
    status_t SetTextMode(DWORD mode);
    status_t GetSelectTextRange(int32_t *min,int32_t *max);
    status_t SetFontName(const wchar_t *name);
    status_t SetCharSet(BYTE char_set);
    status_t SetSelectTextWeight(WORD wWeight);
    status_t SetSelectTextHeight(int32_t height);
    status_t SetAllTextColor(COLORREF color);
    status_t SetSelectTextColor(COLORREF color);
    status_t SetBkColor(COLORREF color);
    status_t Init();
    virtual int32_t Destroy();
    status_t Create();
};
#endif
//////////////////////////////////
#if HAVE_TOOL_TIP
class CToolTip : public CCtrl{
public:
    status_t SetDelayTime(int32_t ms);
    status_t SetToolTextByIndex(int32_t index,const wchar_t *text);
    status_t SetToolInfo(TOOLINFOW *ti);
    status_t SetToolRectByIndex(int32_t index,RECT *r);
    status_t SetToolRect(TOOLINFOW *ti);
    status_t AddTool(HWND hwnd,const wchar_t *text);
    status_t AddTool(RECT *r,const wchar_t *text);
    status_t GetToolByIndex(int32_t index,TOOLINFOW *ti);
    status_t GetToolById(DWORD uid,TOOLINFOW *ti);
    status_t GetToolCount();
    status_t AddTool(TOOLINFOW *ti);
    status_t Init();
    status_t Destroy();
    CToolTip();
    virtual ~CToolTip();
    virtual int32_t Create();
};
#endif
//////////////////////////////////
class CTrayIcon{
public:
    static UINT s_uTaskbarRestart;
    HICON hico;
    NOTIFYICONDATAW notify_data;
public:
    UINT GetTrayMessage(WPARAM wparam,LPARAM lparam);
    status_t DeleteTrayIcon();
    status_t AddTrayIcon();
    status_t SetCallBackMessage(UINT message);
    status_t SetParent(HWND hwnd);
    status_t SetToolTip(const wchar_t *str);
    status_t SetIcon(UINT res_id);
    CTrayIcon();
    virtual ~CTrayIcon();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
//////////////////////////////////
#if HAVE_COMMAND_BAR
class CCommandBar{
public:
    HWND hwndCB;
public:
    status_t AddAdornments(uint32_t flag);
    status_t InsertMenu(CMenu *menu);
    status_t Create(HWND hparent);
    CCommandBar();
    virtual ~CCommandBar();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};
#endif
//////////////////////////////////
#endif
