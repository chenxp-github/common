#include "comnctls.h"
#include "syslog.h"
#include "mem_tool.h"
#include "mem.h"

CCtrl::CCtrl()
{
    this->InitBasic();
}
CCtrl::~CCtrl()
{
    this->Destroy();
}
status_t CCtrl::InitBasic()
{
    CWnd::InitBasic();
    return OK;
}
status_t CCtrl::Init()
{
    this->InitBasic();  
    CWnd::Init();
    
    this->wnd_create->style = WS_VISIBLE|WS_CHILD|WS_TABSTOP;
    
    return OK;
}
status_t CCtrl::Destroy()
{
    CWnd::Destroy();
    //add your code
    this->InitBasic();
    return OK;
}
status_t CCtrl::EmulateClick()
{
    ::SendMessageW(hwnd,WM_LBUTTONDOWN,(WPARAM)0,MAKELPARAM(1,1));
    ::SendMessageW(hwnd,WM_LBUTTONUP,(WPARAM)0,MAKELPARAM(1,1));
    return OK;
}
/***********************************************************/
/*                          PushButton                     */
/***********************************************************/
status_t CButton::Create()
{
    return this->CreateCtrl(L"BUTTON");
}
/***********************************************************/
/*                          PushButton                     */
/***********************************************************/
status_t CGroupBox::Create()
{
    this->wnd_create->style = BS_GROUPBOX|WS_CHILD|WS_VISIBLE;
    return this->CreateCtrl(L"BUTTON");
}
/***********************************************************/
/*                          Static                         */
/***********************************************************/
status_t CStatic::Create()
{
    return this->CreateCtrl(L"STATIC");
}
/***********************************************************/
/*                          CheckBox                       */
/***********************************************************/
status_t CCheckBox::Create()
{
    this->wnd_create->style |= (BS_AUTOCHECKBOX|BS_CHECKBOX);
    return this->CreateCtrl(L"BUTTON");
}
status_t CCheckBox::SetCheck(BOOL flag)
{
    return (status_t)::SendMessageW(hwnd,BM_SETCHECK,(WPARAM)(flag != 0),0);
}
status_t CCheckBox::GetCheck()
{
    return (status_t)::SendMessageW(hwnd,BM_GETCHECK,0,0);
}
/***********************************************************/
/*                          RadioBox                          */
/***********************************************************/
status_t CRadioBox::Create()
{
    this->wnd_create->style |= BS_AUTORADIOBUTTON;
    return this->CreateCtrl(L"BUTTON");
}
status_t CRadioBox::SetCheck(BOOL flag)
{
    return (status_t)::SendMessageW(hwnd,BM_SETCHECK,(WPARAM)(flag != 0),0);
}
status_t CRadioBox::GetCheck()
{
    return (status_t)::SendMessageW(hwnd,BM_GETCHECK,0,0);
}
/***********************************************************/
/*                          ScrollBar                      */
/***********************************************************/
status_t CScrollBar::Create()
{
    this->CreateCtrl(L"SCROLLBAR");
    this->is_control = true;
    if(this->hwnd)
    {
        SetScrollRange(hwnd,SB_CTL,min,max,TRUE);
        SetScrollPos(hwnd,SB_CTL,cur_pos,TRUE);     
        return OK;
    }
    
    return ERROR;
}
status_t CScrollBar::Init()
{
    CCtrl::Init();  
    this->inc = 0;
    this->cur_pos = 0;
    this->min = 0;
    this->max = 100;
    this->line_step = 1;
    this->page_size = 10;
    this->is_control = true;
    this->wnd_create->style = SBS_VERT|WS_VISIBLE|WS_CHILD; 
    return OK;
}
status_t CScrollBar::SetRange(int32_t min, int32_t max)
{
    this->max = max;
    this->min = min;
    
    if(this->hwnd)
    {
        SetScrollRange(this->hwnd,GetScrollBarFlag(),min,max,TRUE);
    }
    
    return OK;
}
status_t CScrollBar::SetPos(int32_t pos)
{
    if(pos < this->min)
        pos = this->min;
    if(pos > this->max - this->page_size)
        pos = this->max - this->page_size;
    this->cur_pos = pos;
    if(hwnd)
        SetScrollPos(hwnd,GetScrollBarFlag(),pos,TRUE);
    return OK;
}
status_t CScrollBar::GetPos()
{
    if(this->cur_pos > this->max - this->page_size)
        return this->max - this->page_size;
    else
        return this->cur_pos;
}
status_t CScrollBar::SetLineStep(int32_t line_step)
{
    this->line_step = line_step;
    return OK;
}
status_t CScrollBar::SetPageSize(int32_t proportion)
{
    if(this->hwnd == NULL)
        return ERROR;
    
    SCROLLINFO info;
    
    info.cbSize=sizeof(SCROLLINFO);
    info.fMask=SIF_PAGE;
    info.nPage = proportion;
    
    this->page_size = proportion;
    SetScrollInfo(hwnd,GetScrollBarFlag(),&info,TRUE);
    
    return OK;
}
status_t CScrollBar::OnScroll(WPARAM wparam, LPARAM lparam)
{   
    if(this->is_control)
    {
        if((HWND)(lparam)!= this->hwnd)
            return ERROR;
    }
    
    int32_t old_pos = cur_pos;

    switch(LOWORD(wparam))
    {
    case SB_LINEDOWN:
        cur_pos+=line_step;
        break;
    case SB_LINEUP:
        cur_pos-=line_step;
        break;
    case SB_PAGEUP:
        cur_pos-=page_size;
        break;
    case SB_PAGEDOWN:
        cur_pos+=page_size;
        break;
    case SB_THUMBTRACK:
        SCROLLINFO info;
        info.cbSize=sizeof(SCROLLINFO);
        info.fMask=SIF_TRACKPOS;
        GetScrollInfo(hwnd,GetScrollBarFlag(),&info);
        cur_pos=info.nTrackPos;
        break;
    }   
    this->SetPos(cur_pos);  
    this->inc = cur_pos - old_pos;
    return OK;
}
status_t CScrollBar::SetDir(BOOL is_vertical)
{
    this->wnd_create->style &= (~SBS_VERT);
    this->wnd_create->style &= (~SBS_HORZ);
    
    if(is_vertical)
        this->wnd_create->style |= SBS_VERT;
    else
        this->wnd_create->style |= SBS_HORZ;
    
    return OK;
}

status_t CScrollBar::IsVertical()
{
    return (this->wnd_create->style & SBS_VERT) != 0;
}

status_t CScrollBar::AttachWindowWithScrollBar(HWND hwnd)
{
    this->is_control = false;
    this->AttachWnd(hwnd);
    return OK;
}
uint32_t CScrollBar::GetScrollBarFlag()
{
    if(this->is_control) 
        return SB_CTL;
    if(this->IsVertical())
        return SB_VERT;
    return SB_HORZ;
}

/***********************************************************/
/*                          EditBox                        */
/***********************************************************/
status_t CEditBox::Init()
{
    CCtrl::Init();
    this->wnd_create->style = WS_CHILD|WS_VISIBLE| WS_TABSTOP;
    return OK;
}
status_t CEditBox::Create()
{
    return this->CreateCtrl(L"EDIT");
}
status_t CEditBox::SetSel(int32_t nStart, int32_t nEnd)
{
    return (status_t)::SendMessageW(hwnd,EM_SETSEL,(WPARAM)nStart,(LPARAM)nEnd);
}
status_t CEditBox::ReplaceSel(const wchar_t *buf)
{
    return (status_t)::SendMessageW(hwnd,EM_REPLACESEL,0,(LPARAM)buf);
}
status_t CEditBox::SetMaxText(int32_t max)
{
    return (status_t)::SendMessageW(hwnd,EM_SETLIMITTEXT,(WPARAM)max,0);
}
status_t CEditBox::SetReadOnly(BOOL flag)
{
    return (status_t)::SendMessageW(hwnd,EM_SETREADONLY,(WPARAM)flag,0);
}
status_t CEditBox::MoveToEnd()
{
    int32_t size = this->GetTextLen();
    
    this->SetSel(size,size);
    
    return OK;
}
status_t CEditBox::Puts(const wchar_t *str)
{
    this->MoveToEnd();
    this->ReplaceSel(str);
    return OK;
}
status_t CEditBox::Printf (const wchar_t * szFormat, ...)
{
    wchar_t   szBuffer [2048];
    
    va_list pArgList ;
    va_start (pArgList, szFormat) ;
    wvsprintfW (szBuffer, szFormat, pArgList) ;
    va_end (pArgList) ;
    
    Puts(szBuffer);
    
    return OK;  
}
status_t CEditBox::Putc(wchar_t ch)
{
    wchar_t str[2];
    str[0] = ch;
    str[1] = 0;
    return this->Puts(str);
}
status_t CEditBox::Modified()
{
    return (status_t)::SendMessageW(hwnd,EM_GETMODIFY,0,0);
}
status_t CEditBox::SetModifyFlag(BOOL flag)
{
    return (status_t)::SendMessageW(hwnd,EM_SETMODIFY,(WPARAM)flag,0);
}
status_t CEditBox::GetSel(int32_t *start,int32_t *end)
{
    return (status_t)::SendMessageW(hwnd,EM_GETSEL,(WPARAM)start,(LPARAM)end);
}
status_t CEditBox::Puts_File(CFileBase *file)
{
    ASSERT(file);
    
    int_ptr_t rs;
    LOCAL_MEM(mem);
    
    file->Seek(0);
    while(!file->IsEnd())
    {
        rs = file->Read(mem.GetRawBuf(),(int_ptr_t)(mem.GetMaxSize()-sizeof(wchar_t)));
        if(rs > 0)
        {
            mem.SetSize(rs);
            this->Puts(mem.CStrW());
        }
    }
    
    return OK;
}
status_t CEditBox::SetText_File(CFileBase *file)
{
    ASSERT(file);
    
    this->SetText(L"");
    this->Puts_File(file);
    this->LineScroll(-0x7fffffff);
    return OK;
}
status_t CEditBox::LineScroll(int32_t lines)
{
    return (status_t)::SendMessageW(hwnd,EM_LINESCROLL,0,lines);
}
status_t CEditBox::SetTabStops(int32_t nSpace)
{   
    return (status_t)::SendMessageW(hwnd,EM_SETTABSTOPS,1,(LPARAM)(&nSpace));
}
//lfHeight=-MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
status_t CEditBox::GetTextHeight(int32_t lfHeight)
{
    HDC hdc = ::GetDC(this->hwnd);  
    int32_t pixelsy = GetDeviceCaps(hdc, LOGPIXELSY);
    ::ReleaseDC(this->hwnd,hdc);
    int32_t h = lfHeight*pixelsy /72 ;
    if(h < 0) h = -h;
    return h;
}
status_t CEditBox::SetRectNp(RECT *r)
{
    //multiple line edit only
    ASSERT(r);
    return (status_t)::SendMessageW(hwnd,EM_SETRECTNP,0,(LPARAM)r);
}
/***********************************************************/
/*                          ListBox                        */
/***********************************************************/
status_t CListBox::Create()
{
    return this->CreateCtrl(L"LISTBOX");
}
status_t CListBox::Init()
{
    CCtrl::Init();
    this->wnd_create->style=WS_VISIBLE|WS_CHILD|LBS_NOTIFY|WS_VSCROLL|WS_TABSTOP|LBS_NOINTEGRALHEIGHT;
    this->wnd_create->ex_style = WS_EX_CLIENTEDGE;
    
    return OK;
}
status_t CListBox::AddString(const wchar_t * str)
{
    return (status_t)::SendMessageW(this->hwnd,LB_ADDSTRING,0,(LPARAM)str);
}
status_t CListBox::InsertString(int32_t i,wchar_t * str)
{
    return (status_t)::SendMessageW(this->hwnd,LB_INSERTSTRING,(WPARAM)i,(LPARAM)str);
}
status_t CListBox::GetString(int32_t i,wchar_t * buf)
{
    return (status_t)::SendMessageW(this->hwnd,LB_GETTEXT,(WPARAM)i,(LPARAM)buf); 
}
status_t CListBox::GetString(int32_t i,CMem *mem)
{
    this->GetString(i,(wchar_t*)mem->GetRawBuf());
    mem->SetSize(crt_strlen_w((wchar_t*)mem->GetRawBuf())*sizeof(wchar_t));
    return OK;
}
status_t CListBox::SetString(int32_t i,wchar_t * str)
{
    DeleteString(i);
    InsertString(i,str);
    return OK;
}
status_t CListBox::DeleteString(int32_t i)
{
    return (status_t)::SendMessageW(this->hwnd,LB_DELETESTRING,(WPARAM)i,0);
}
status_t CListBox::DeleteAllItems()
{
    return (status_t)::SendMessageW(this->hwnd,LB_RESETCONTENT,0,0);
}

int32_t CListBox::FindString(wchar_t * str)
{
    int32_t ret = (int32_t)::SendMessageW(this->hwnd,LB_FINDSTRINGEXACT,(WPARAM)(-1),(LPARAM)str);  
    if(ret ==LB_ERR)
        return -1;  
    return ret;
}
status_t CListBox::GetSelected()
{
    //Do not send this message to a multiple-selection list box
    return (status_t)::SendMessageW(this->hwnd,LB_GETCURSEL,0,0);
}
status_t CListBox::SetSelect(int32_t i)
{
    return (status_t)::SendMessageW(this->hwnd,LB_SETCURSEL,i,0); 
}
status_t CListBox::SetSelect(wchar_t * str)
{
    int32_t index = FindString(str);
    
    if(index >= 0)
        SetSelect(index);
    
    return OK;
}
status_t CListBox::GetMaxItems()
{
    return (status_t)::SendMessageW(this->hwnd,LB_GETCOUNT,0,0);
}
status_t CListBox::AddAllLines(CFileBase *file)
{
    ASSERT(file);
    LOCAL_MEM(mem);
    file->Seek(0);
    while(file->ReadLineW(&mem))
    {
        this->AddString(mem.CStrW());
    }
    
    return OK;
}
status_t CListBox::SetViewWidth(int32_t width)
{
    return (status_t)::SendMessageW(hwnd,LB_SETHORIZONTALEXTENT,width,0);
}
/***********************************************************/
/*                          CombBox                        */
/***********************************************************/
status_t CCombBox::Create()
{
    return this->CreateCtrl(L"COMBOBOX");
}
status_t CCombBox::Init()
{
    CCtrl::Init();
    this->wnd_create->style=  WS_VISIBLE|WS_CHILD|CBS_DROPDOWN
        | WS_BORDER|WS_VSCROLL|WS_TABSTOP;
    return OK;
}
int32_t CCombBox::FindString(const wchar_t *str)
{
    int32_t ret = (int32_t)::SendMessageW(this->hwnd,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)str);
    if(ret==CB_ERR)
        return -1;
    return ret;
}
status_t CCombBox::AddString(const wchar_t *str)
{
    return (status_t)::SendMessageW(this->hwnd,CB_ADDSTRING,0,(LPARAM)str);
}
status_t CCombBox::InsertString(int32_t i,wchar_t *str)
{
    return (status_t)::SendMessageW(this->hwnd,CB_INSERTSTRING,(WPARAM)i,(LPARAM)str);
}
status_t CCombBox::DeleteString(int32_t i)
{
    return (status_t)::SendMessageW(this->hwnd,CB_DELETESTRING,(WPARAM)i,0);
}
status_t CCombBox::SetString(int32_t i,wchar_t *str)
{
    DeleteString(i);
    InsertString(i,str);
    return OK;
}
status_t CCombBox::DeleteAllItems()
{
    return (status_t)::SendMessageW(this->hwnd,CB_RESETCONTENT,0,0);
}
status_t CCombBox::GetSelected()
{
    int32_t i;
    //Do not send this message to a multiple-selection list box
    i = (status_t)::SendMessageW(this->hwnd,CB_GETCURSEL,0,0);
    if(i == CB_ERR)
        return -1;
    return i;
}
status_t CCombBox::GetString(int32_t i,wchar_t *str)
{
    return (status_t)::SendMessageW(this->hwnd,CB_GETLBTEXT,(WPARAM)i,(LPARAM)str);
}
status_t CCombBox::GetEditText(wchar_t *str,int32_t max)
{
    return (status_t)::SendMessageW(this->hwnd,WM_GETTEXT,(WPARAM)max,(LPARAM)str);
}
status_t CCombBox::SetEditText(const wchar_t *str)
{
    return (status_t)::SendMessageW(this->hwnd,WM_SETTEXT,0,(LPARAM)str);
}
status_t CCombBox::SetSelect(int32_t i)
{
    return (status_t)::SendMessageW(this->hwnd,CB_SETCURSEL,(WPARAM)i,0);
}
status_t CCombBox::SetSelect(const wchar_t *str)
{
    int32_t index;
    index=FindString(str);
    if(index>=0)
        SetSelect(index);
    return OK;
}
status_t CCombBox::GetEditText_File(CMem *mem)
{
    ASSERT(mem);
    
    mem->SetSize(0);
    int32_t size = this->GetEditText((wchar_t*)mem->GetRawBuf(),(int_ptr_t)(mem->GetMaxSize()/sizeof(wchar_t)));
    if(size > 0)
    {
        mem->SetSize(size*sizeof(wchar_t));
    }
    
    return OK;
}
status_t CCombBox::ShowDropDown(int32_t enable)
{
    return PostMessageW(hwnd,CB_SHOWDROPDOWN,enable,0);
}
status_t CCombBox::GetItemText(int32_t index,wchar_t *str)
{
    ASSERT(str);
    return (status_t)::SendMessageW(this->hwnd,CB_GETLBTEXT,index,(LPARAM)str);
}
status_t CCombBox::GetItemText_File(int32_t index,CMem *mem)
{
    ASSERT(mem);
    mem->SetSize(0);
    int32_t size = this->GetItemText(index,(wchar_t*)mem->GetRawBuf());
    if(size > 0)
    {
        mem->SetSize(size*sizeof(wchar_t));
    }
    return OK;
}
status_t CCombBox::IsDropDownVisible()
{
    return (status_t)::SendMessageW(hwnd,CB_GETDROPPEDSTATE,0,0);
}
status_t CCombBox::SetButtonHeight(int height)
{
    return (status_t)::SendMessageW(hwnd,CB_SETITEMHEIGHT,(WPARAM)-1,height);
}
status_t CCombBox::SetListItemHeight(int height)
{
    return (status_t)::SendMessageW(hwnd,CB_SETITEMHEIGHT,(WPARAM)0,height);
}
/***********************************************************/
/*                          Menu                           */
/***********************************************************/
CMenu::CMenu()
{
    this->InitBasic();
}
CMenu::~CMenu()
{
    this->Destroy();
}
status_t CMenu::InitBasic()
{
    this->hmenu = NULL;
    this->name = NULL;
    this->id = CWnd::GetUniqueID();
    this->hpmenu = NULL;
    this->hpwnd = NULL;
    
    return OK;
}
status_t CMenu::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t  CMenu::Destroy()
{
    if(this->hmenu)
    {
        ::DestroyMenu(this->hmenu);
        this->hmenu = NULL;
    }
    
    this->InitBasic();
    
    return OK;
}
status_t CMenu::SetName(const wchar_t *name)
{
    ASSERT(name);
    this->name = (wchar_t*)name;
    return OK;
}
status_t CMenu::Create()
{
    this->hmenu = ::CreateMenu();
    return OK;
}
status_t CMenu::AddMenu(CMenu *menu)
{
    ASSERT(menu);
    
    menu->hpmenu = this->hmenu;
    return AppendMenuW(hmenu, MF_POPUP, (UINT)menu->hmenu,menu->name);
}
status_t CMenu::AddItem(CMenu *item)
{
    ASSERT(item);
    
    item->hpmenu = this->hmenu;
    return AppendMenuW(hmenu,MF_STRING,item->id, item->name);   
}
status_t CMenu::AddSeparator()
{
    return AppendMenuW(hmenu, MF_SEPARATOR, 0,NULL);
}
#if _WIN32_ || _WIN64_
status_t CMenu::SetToWnd(HWND hwnd)
{
    this->hpwnd = hwnd;
    return SetMenu(hwnd,this->hmenu);
}
#endif
status_t CMenu::DelItem()
{
    DeleteMenu(hpmenu,id,MF_BYCOMMAND);
    this->hmenu = NULL;
    
    return OK;
}
status_t CMenu::CheckItem(BOOL flag)
{
    if(flag)
        CheckMenuItem(hpmenu,id,MF_BYCOMMAND|MF_CHECKED);
    else
        CheckMenuItem(hpmenu,id,MF_BYCOMMAND|MF_UNCHECKED);
    
    return OK;
}
status_t CMenu::IsChecked()
{
    MENUITEMINFOW minfo;
    memset(&minfo,0,sizeof(minfo));
    minfo.cbSize=sizeof(MENUITEMINFOW);
    minfo.fMask=MIIM_STATE;
    
    GetMenuItemInfoW(hpmenu,id,false,&minfo);
    
    if(minfo.fState&MFS_CHECKED)
        return TRUE;
    else
        return FALSE;
}
status_t CMenu::Enable(BOOL flag)
{
#if _WINCE_ || _WM6X_
    DWORD MF_DISABLED = 0;
#endif
    
    if(flag)
        EnableMenuItem(hpmenu,id,MF_BYCOMMAND|MF_ENABLED);
    else
        EnableMenuItem(hpmenu,id,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
    
    return OK;
}
status_t CMenu::IsMyCommand(WPARAM wparam)
{
    if(LOWORD (wparam)==id)
        return TRUE;
    
    return FALSE;
}
status_t CMenu::EmuPush(HWND hwnd)
{
    return (status_t)::SendMessageW(hwnd,WM_COMMAND,MAKEWPARAM(id,0),0);
}
/***********************************************************/
/*                          Menu                           */
/***********************************************************/
status_t CPopupMenu::Create()
{
    this->hmenu = CreatePopupMenu();
    return OK;
}
status_t CPopupMenu::SetParent(HWND hwnd)
{
    this->hpwnd = hwnd;
    
    return OK;
}
status_t CPopupMenu::Show(int32_t x, int32_t y)
{
#if _WIN32_ || _WIN64_
    DWORD flag = TPM_RETURNCMD|TPM_RIGHTBUTTON;
#endif
    
#if _WINCE_ || _WM6X_
    DWORD flag = TPM_RETURNCMD|TPM_BOTTOMALIGN;
#endif
    
    DWORD id = (UINT)TrackPopupMenu(hmenu,flag,x,y,0,hpwnd,NULL);
    (status_t)::SendMessageW(hpwnd,WM_COMMAND,MAKEWPARAM(id,0),0);
    
    return OK;
}
status_t CPopupMenu::Show()
{
    POINT p;
    
    GetCursorPos(&p);
    this->Show(p.x,p.y);
    
    return OK;
}
/***********************************************************/
/*                          StatusBar                      */
/***********************************************************/
CStatusBar::CStatusBar()
{
    this->InitBasic();
}
CStatusBar::~CStatusBar()
{
    this->Destroy();
}
status_t CStatusBar::InitBasic()
{
    CCtrl::InitBasic();
    this->parts = 0;
    this->pos = NULL;
    return OK;
}
status_t CStatusBar::Init()
{
    this->InitBasic();
    CCtrl::Init();
    
    return OK;
}
status_t  CStatusBar::Destroy()
{
    FREE(this->pos);
    CCtrl::Destroy();
    this->InitBasic();
    return OK;
}
status_t CStatusBar::Create()
{
    CWnd::CreateCtrl(L"msctls_statusbar32");
    this->RefreshUI();
    
    return OK;
}
status_t CStatusBar::SetParts(int32_t parts)
{
    ASSERT(parts > 0 && parts < 256);
    
    if(this->parts == parts)
        return OK;
    
    this->parts = parts;
    
    FREE(this->pos);
    MALLOC(this->pos,int32_t,parts);
    
    this->SetAveragePos();
    
    return OK;
}
status_t CStatusBar::RefreshUI()
{
    if(this->hwnd == NULL)
        return ERROR;
    
    if(this->parts > 0)
    {
        ::SendMessageW(hwnd,SB_SETPARTS,(WPARAM)parts,(LPARAM)pos);
    }
    
    return OK;
}
status_t CStatusBar::SetPartPos(int32_t i, int32_t pos)
{
    if(i < 0 || i >= this->parts)
        return ERROR;
    
    this->pos[i] = pos;
    
    return OK;
}
status_t CStatusBar::SetAveragePos()
{
    if(this->hwnd)
    {
        RECT r;
        
        ::GetWindowRect(this->hwnd,&r);
        
        for(int32_t i=0;i<parts;i++)
            pos[i]=(i+1)*(r.right - r.left)/parts;
        
        this->RefreshUI();
    }
    
    return OK;
}
status_t CStatusBar::OnSize()
{
    return (status_t)::SendMessageW(hwnd,WM_SIZE,0,0);
}
status_t CStatusBar::SetPartText(int32_t i, const wchar_t *str)
{
    ASSERT(this->hwnd);
    
    if(i > parts)
        return ERROR;
    
    return (status_t)::SendMessageW(hwnd,SB_SETTEXTW,(WPARAM)i,(LPARAM)str);
}
/***********************************************************/
/*                          ProgressBar                    */
/***********************************************************/
CProgressBar::CProgressBar()
{
    this->InitBasic();
}
CProgressBar::~CProgressBar()
{
    this->Destroy();
}
status_t CProgressBar::InitBasic()
{
    CCtrl::InitBasic();
    this->min = 0;
    this->max = 100;
    this->step = 1;
    
    return OK;
}
status_t CProgressBar::Init()
{
    this->InitBasic();
    CCtrl::Init();
    
    return OK;
}
status_t  CProgressBar::Destroy()
{
    CCtrl::Destroy();
    this->InitBasic();
    return OK;
}
status_t CProgressBar::Create()
{
    this->CreateCtrl(PROGRESS_CLASSW);
    ASSERT(this->hwnd);
    
    ::SendMessageW(hwnd,PBM_SETRANGE32,(WPARAM)min,(LPARAM)max);
    ::SendMessageW(hwnd,PBM_SETSTEP,(WPARAM)step,(LPARAM)0);
    return OK;
}
status_t CProgressBar::SetRange(int32_t min, int32_t max)
{
    this->max = max;
    this->min = min;
    
    if(this->hwnd)
    {
        ::SendMessageW(hwnd,PBM_SETRANGE32,(WPARAM)min,(LPARAM)max);
    }
    
    return OK;   
}
status_t CProgressBar::SetStep(int32_t step)
{
    this->step = step;
    if(this->hwnd)
    {
        ::SendMessageW(hwnd,PBM_SETSTEP,(WPARAM)step,(LPARAM)0);
    }
    
    return OK;
}
status_t CProgressBar::GoAStep()
{
    return (status_t)::SendMessageW(hwnd,PBM_STEPIT,(WPARAM)0,(LPARAM)0); 
}
status_t CProgressBar::SetPos(int32_t pos)
{
    return (status_t)::SendMessageW(hwnd,PBM_SETPOS,(WPARAM)pos,(LPARAM)0);         
}
status_t CProgressBar::SetColor(COLORREF fc, COLORREF bc)
{
#if _WIN32_ || _WIN64_
    ::SendMessageW(hwnd,PBM_SETBARCOLOR,(WPARAM)0,(LPARAM)bc); 
    ::SendMessageW(hwnd,PBM_SETBKCOLOR,(WPARAM)0,(LPARAM)fc);
#endif
    return OK;
}
/***********************************************************/
/*                          ImageList                      */
/***********************************************************/
CImageList::CImageList()
{
    this->InitBasic();
}
CImageList::~CImageList()
{
    this->Destroy();
}
status_t CImageList::InitBasic()
{
    this->hBitmap = NULL;
    this->hImages = NULL;
    
    return OK;
}
status_t CImageList::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CImageList::Destroy()
{
    if(this->hImages)
    {
        ::DeleteObject(this->hImages);
        this->hImages = NULL;
    }
    
    if(this->hBitmap)
    {
        ::DeleteObject(this->hBitmap);
        this->hBitmap = NULL;
    }
    
    this->InitBasic();
    
    return OK;
}
status_t CImageList::CreateImageList(DWORD bmp_id,int32_t img_width,int32_t img_height,int32_t color_depth,COLORREF mask_color) 
{
    DWORD cd = ILC_COLOR;
    
    ASSERT(this->hBitmap == NULL && this->hImages == NULL);
    
#if _WIN32_ || _WIN64_
    switch(color_depth)
    {
    case 4: cd = ILC_COLOR4; break;
    case 8: cd = ILC_COLOR8; break;
    case 16: cd = ILC_COLOR16; break;
    case 24: cd = ILC_COLOR24; break;
    case 32: cd = ILC_COLOR32; break;
    }
#endif
    
    this->hBitmap = LoadBitmapW(GetSelfInstance(), MAKEINTRESOURCEW (bmp_id));
    ASSERT(this->hBitmap);
    this->hImages = ImageList_Create( img_width , img_height , ILC_MASK|cd, 0 , 10 );
    ASSERT(this->hImages);
    ImageList_AddMasked( this->hImages , this->hBitmap , mask_color);   
    
    return OK;
}
status_t CImageList::GetImageCount()
{
    return ImageList_GetImageCount(hImages);    
}
/***********************************************************/
/*                          TreeView                       */
/***********************************************************/
CTreeView::CTreeView()
{
    this->InitBasic();
}
CTreeView::~CTreeView()
{
    this->Destroy();
}
status_t CTreeView::InitBasic()
{
    CCtrl::InitBasic();
    this->image_list = NULL;
    return OK;
}
status_t CTreeView::Init()
{
    this->InitBasic();
    CCtrl::Init();
    this->wnd_create->style = WS_CHILD|WS_VISIBLE|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_SHOWSELALWAYS;
    NEW(this->image_list,CImageList);
    this->image_list->Init();   
    return OK;
}
status_t  CTreeView::Destroy()
{
    DEL(this->image_list);
    CCtrl::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTreeView::Create()
{
    return this->CreateCtrl(L"SysTreeView32");
}
status_t CTreeView::CreateImageList(DWORD bmp_id,int32_t img_width,int32_t img_height,int32_t color_depth,COLORREF mask_color) 
{
    this->image_list->Destroy();
    this->image_list->Init();
    this->image_list->CreateImageList(bmp_id,img_width,img_height,color_depth,mask_color);
    ::SendMessageW(this->hwnd, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM) this->image_list->hImages); 
    return OK;
}
HTREEITEM CTreeView::AddRoot(wchar_t *str)
{
    TVINSERTSTRUCTW tvis = { 0 };// 定义一个父根
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvis.item.pszText = str;
    return (HTREEITEM)(status_t)::SendMessageW(hwnd, TVM_INSERTITEMW, 0, (LPARAM)(&tvis));
}
HTREEITEM CTreeView::AddItem(HTREEITEM hparent,wchar_t *str)
{
    TVINSERTSTRUCTW tvis;
    tvis.hParent=hparent;
    tvis.hInsertAfter=TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvis.item.pszText = str;
    
    return (HTREEITEM) (status_t)::SendMessageW(hwnd, TVM_INSERTITEMW, 0, (LPARAM)(LPTV_INSERTSTRUCT)(&tvis));
}
status_t CTreeView::GetItemText(HTREEITEM hitem,wchar_t *buf,int32_t max)
{
    TVITEMW item;
    
    item.mask=TVIF_TEXT|TVIF_HANDLE;
    item.hItem=hitem;
    item.cchTextMax=max;
    item.pszText=buf;
    
    return (status_t)::SendMessageW((hwnd), TVM_GETITEMW, 0, (LPARAM)(TV_ITEM FAR*)(&item));
}
status_t CTreeView::GetItemText(HTREEITEM hitem, CMem *mem)
{
    ASSERT(hitem && mem);
    mem->SetSize(0);
    this->GetItemText(hitem,(wchar_t*)mem->GetRawBuf(),(int_ptr_t)(mem->GetMaxSize()/sizeof(wchar_t)));
    mem->SetSize(crt_strlen_w((wchar_t*)mem->GetRawBuf())*sizeof(wchar_t));
    return OK;
}
HTREEITEM CTreeView::GetNextItem(HTREEITEM hti, int32_t code)
{
    return (HTREEITEM)(status_t)::SendMessageW(hwnd, TVM_GETNEXTITEM, code, (LPARAM)(hti));
}
HTREEITEM CTreeView::GetParent(HTREEITEM hitem)
{    
    return this->GetNextItem(hitem,TVGN_PARENT);
}
status_t CTreeView::GetParentText(HTREEITEM hitem,wchar_t *buf,int32_t max)
{
    
    HTREEITEM h = GetParent(hitem);
    
    if(h==NULL)return ERROR;
    
    if(GetItemText(h,buf,max))
        return OK;
    
    return ERROR;
}
HTREEITEM CTreeView::GetSelectItem(LPARAM lparam)
{
    LPNMTREEVIEW pNmtv=(LPNMTREEVIEW)lparam;
    
    return pNmtv->itemNew.hItem;
}
HTREEITEM CTreeView::GetSelectItem()
{
    return this->GetNextItem(NULL,TVGN_CARET);
}
status_t CTreeView::SetSelect(HTREEITEM hitem)
{
    return (status_t)::SendMessageW(hwnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hitem);
}
HTREEITEM CTreeView::ItemFromPoint(int32_t x,int32_t y)
{
    TVHITTESTINFO hts;
    
    hts.flags=TVHT_ONITEMLABEL ;
    hts.pt.x=x;
    hts.pt.y=y;
    
    (status_t)::SendMessageW(hwnd, TVM_HITTEST, 0, (LPARAM)(&hts));
    
    return hts.hItem;
}
status_t CTreeView::OnRClick(WPARAM wparam,LPARAM lparam)
{
    if(!this->IsMyNotify(lparam))
        return ERROR;
    
    if(this->GetNotifyCode(lparam)!=NM_RCLICK)
        return ERROR;
    
    POINT p;
    
    GetCursorPos(&p);
    ScreenToClient(hwnd,&p);
    SetSelect(ItemFromPoint(p.x,p.y));
    
    return OK;
}
status_t CTreeView::DeleteItem(HTREEITEM hItem)
{    
    return (status_t)::SendMessageW(hwnd, TVM_DELETEITEM, 0, (LPARAM)(HTREEITEM)(hItem));
}
status_t CTreeView::GetDepth(HTREEITEM hItem)
{
    int32_t n=0;
    HTREEITEM htemp=hItem;
    while(htemp!=NULL)
    {
        htemp = GetParent(htemp);
        n++;
    }
    return n;
}
HTREEITEM CTreeView::SearchChild(HTREEITEM hParent,wchar_t *buf)
{
    HTREEITEM hChild;
    if(hParent!=NULL)
        hChild = this->GetNextItem(hParent,TVGN_CHILD);
    else
        hChild=GetRoot();
    
    if(hChild==NULL)
        return NULL;
    
    LOCAL_MEM(mem);
    
    do
    {
        GetItemText(hChild,&mem);
        if(mem.StrCmpW(buf) == 0)
            return hChild;
        hChild= this->GetNextItem(hChild,TVGN_NEXT);
    }
    while(hChild!=NULL);
    
    return NULL;
}
HTREEITEM CTreeView::GetRoot()
{
    return this->GetNextItem(NULL,TVGN_ROOT);
}
HTREEITEM CTreeView::GetChild(HTREEITEM hParent)
{
    return this->GetNextItem(hParent,TVGN_CHILD);
}
HTREEITEM CTreeView::GetSibling(HTREEITEM htv)
{
    return this->GetNextItem(htv,TVGN_NEXT);
}
status_t CTreeView::SetItemText(HTREEITEM htv,wchar_t *str)
{    
    TVITEMW item;
    
    item.mask=TVIF_TEXT|TVIF_HANDLE;
    item.hItem=htv;
    item.pszText=str;
    return (status_t)::SendMessageW(hwnd, TVM_SETITEM, 0, (LPARAM)(&item));
}
status_t CTreeView::SetItemImg(HTREEITEM htv,int32_t img_index)
{    
    TVITEMW item;
    
    item.mask= TVIF_IMAGE|TVIF_HANDLE|TVIF_SELECTEDIMAGE;
    item.hItem=htv;
    item.iImage = img_index;    
    item.iSelectedImage = img_index;
    
    return (status_t)::SendMessageW(hwnd, TVM_SETITEM, 0, (LPARAM)(&item));
}
status_t CTreeView::IsLeaf(HTREEITEM htv)
{
    HTREEITEM h;
    
    if(htv == NULL)
        return FALSE;
    
    h = this->GetChild(htv);
    
    if(h == NULL)
        return TRUE;
    
    return FALSE;
}
status_t CTreeView::Expand(HTREEITEM htv,UINT flag,int32_t recursive)
{
    HTREEITEM hchild;
    
    if(htv == NULL)
        return TRUE;
    
    ::SendMessageW(hwnd, TVM_EXPAND, (WPARAM)flag, (LPARAM)(htv));
    
    if(!recursive)
        return OK;
    
    hchild = this->GetChild(htv);
    while(hchild)
    {
        this->Expand(hchild,flag,recursive);
        hchild = this->GetSibling(hchild);
    }
    
    return OK;
}
status_t CTreeView::GetPath(HTREEITEM hitem, CFileBase *file)
{
    ASSERT(file);
    
    HTREEITEM hs[256],h;
    int32_t i,max;
    
    LOCAL_MEM(mem);
    file->SetSize(0);
    
    for(i = 0; i < 256; i++)
        hs[i] = NULL;
    
    i = 0;
    h = hitem;
    while(h && (i < 256))
    {
        hs[i++] = h;
        h = this->GetParent(h);
    }
    
    max = i;
    for(i = max-1; i >= 0; i--)
    {
        this->GetItemText(hs[i],&mem);      
        file->PutsW(L"/");
        file->PutsW(mem.CStrW());
    }
        
    return OK;
}
/***********************************************************/
/*                          ToolBar Button                 */
/***********************************************************/
CTBButton::CTBButton()
{
    this->InitBasic();
}
CTBButton::~CTBButton()
{
    this->Destroy();
}
status_t CTBButton::InitBasic()
{
    this->tbb = NULL;
    return OK;
}
status_t CTBButton::Init()
{
    this->InitBasic();    
    
    NEW(this->tbb,TBBUTTON);
    memset(this->tbb,0,sizeof(TBBUTTON));
    
    this->tbb->idCommand = CWnd::GetUniqueID(); 
    this->SetState(TBSTATE_ENABLED);
    
    return OK;
}
status_t CTBButton::Destroy()
{
    DEL(this->tbb);
    
    this->InitBasic();
    return OK;
}
status_t CTBButton::SetStyle(BYTE style)
{
    this->tbb->fsStyle = style;
    return OK;
}
status_t CTBButton::SetState(BYTE state)
{
    this->tbb->fsState = state;
    return OK;
}
status_t CTBButton::SetToolTip(const wchar_t *caption)
{
    this->tbb->dwData = (DWORD)caption;
    return OK;
}
status_t CTBButton::SetImgIndex(int32_t index)
{
    this->tbb->iBitmap = index;
    return OK;
}
status_t CTBButton::IsMyCommand(WPARAM wparam)
{
    if((int32_t)LOWORD(wparam) == this->tbb->idCommand)
        return TRUE;
    return FALSE;
}
/***********************************************************/
/*                          ToolBar                        */
/***********************************************************/
CToolBar::CToolBar()
{
    this->InitBasic();
}
CToolBar::~CToolBar()
{
    this->Destroy();
}
status_t CToolBar::InitBasic()
{
    CCtrl::InitBasic();
    this->image_list = NULL;
    
    return OK;
}
status_t CToolBar::Init()
{
    this->InitBasic();
    CCtrl::Init();
    
    this->wnd_create->style |= (WS_VISIBLE|WS_CHILD|TBSTYLE_TOOLTIPS|TBSTYLE_FLAT);
    
    NEW(this->image_list,CImageList);
    this->image_list->Init();
    
    return OK;
}
status_t CToolBar::Destroy()
{
    DEL(this->image_list);
    CCtrl::Destroy();
    this->InitBasic();
    return OK;
}
status_t CToolBar::Create()
{
    return this->CreateCtrl(TOOLBARCLASSNAMEW);
}
status_t CToolBar::SetAutoSize()
{
    return (status_t)::SendMessageW(hwnd, TB_AUTOSIZE, 0, 0); 
}
status_t CToolBar::SetButtonSize(int32_t cx, int32_t cy)
{
    return (status_t)::SendMessageW(hwnd,TB_SETBUTTONSIZE, 0,(LPARAM) MAKELONG(cx, cy));
}
status_t CToolBar::CreateImageList(DWORD bmp_id,int32_t img_width,int32_t img_height,int32_t color_depth,COLORREF mask_color) 
{
    this->image_list->Destroy();
    this->image_list->Init();
    this->image_list->CreateImageList(bmp_id,img_width,img_height,color_depth,mask_color);
    return (status_t)::SendMessageW(this->hwnd, TB_SETIMAGELIST, 0, (LPARAM) this->image_list->hImages); 
}
status_t CToolBar::AddButton(CTBButton *tbb)
{
    ASSERT(tbb);
    return (status_t)::SendMessageW(this->hwnd, TB_ADDBUTTONS, (WPARAM)1, (LPARAM) (LPTBBUTTON)(tbb->tbb)); 
}
status_t CToolBar::OnNotify(WPARAM wparam,LPARAM lparam)
{
#if _WIN32_ || _WIN64_
    int32_t idButton;
    
    if((uint32_t)(CWnd::GetNotifyCode(lparam)) == TTN_NEEDTEXTW)
    {
        CTBButton btn;
        btn.Init();     
        LPTOOLTIPTEXTW lpttt;  
        lpttt = (LPTOOLTIPTEXTW) lparam; 
        ASSERT(lpttt);
        idButton = (int32_t)lpttt->hdr.idFrom;
        
        if(this->GetButtonByCmd(idButton,&btn))
        {
            lpttt->lpszText = (wchar_t*)btn.tbb->dwData;
        }
    }   
#endif
    return OK;
}
status_t CToolBar::GetButtonByCmd(int32_t cmd,CTBButton *ptbb)
{
    ASSERT(ptbb);
    
    int32_t i = 0,ret = TRUE;
    
    while(ret)
    {
        ret = (int32_t)::SendMessageW(hwnd,TB_GETBUTTON,i++,(LPARAM)ptbb->tbb);
        if(ret)
        {
            if(ptbb->tbb->idCommand == cmd)
                break;
        }
    }
    
    return ret;
}
status_t CToolBar::AddSeparator()
{
    static TBBUTTON tbb;
    
    memset(&tbb,0,sizeof(TBBUTTON));
    
    tbb.fsStyle = TBSTYLE_SEP;
    
    return (status_t)::SendMessageW(this->hwnd, TB_ADDBUTTONS, (WPARAM)1, (LPARAM) (LPTBBUTTON)&(tbb));
}
status_t CToolBar::OnSize()
{
    return (status_t)::SendMessageW(hwnd,WM_SIZE,0,0);
}
/***********************************************************/
/*                          ListView                       */
/***********************************************************/
CListView::CListView()
{
    this->InitBasic();
}
CListView::~CListView()
{
    this->Destroy();
}
status_t CListView::InitBasic()
{
    CCtrl::InitBasic();
    this->_column_count = 0;
    return OK;
}
status_t CListView::Init()
{
    this->InitBasic();
    CCtrl::Init();
    
    this->wnd_create->style |= (WS_TABSTOP | WS_CHILD | WS_VISIBLE | LVS_AUTOARRANGE | LVS_REPORT | LVS_SHOWSELALWAYS); 
    this->wnd_create->ex_style |= WS_EX_CLIENTEDGE;
    
    return OK;
}
status_t CListView::Destroy()
{
    //add your code
    CCtrl::Destroy();
    this->InitBasic();
    return OK;
}
status_t CListView::Create()
{
    return this->CreateCtrl(L"SysListView32");
}
status_t CListView::InsertColumn(int32_t index, const wchar_t *str,int32_t width)
{
    ASSERT(index>=0 && index<LISTVIEW_MAX_COLUMNS);
    ASSERT(this->_column_count < LISTVIEW_MAX_COLUMNS);

    LV_COLUMNW   lvColumn;
    int32_t ret;
    
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = width;    
    lvColumn.pszText = (wchar_t*)str;       
    
    ret = (status_t)::SendMessageW(hwnd, LVM_INSERTCOLUMNW, (WPARAM)(int32_t)(index), (LPARAM)(&lvColumn));
    
    if(ret >= 0)
    {
        for(int i=LISTVIEW_MAX_COLUMNS-1; i>=index+1; i--)
            this->_column_widths[i] = this->_column_widths[i-1];
        this->_column_widths[index] = width;
        this->_column_count ++;
    }
    
    return OK;
}
status_t CListView::InsertItem(int32_t img_index, int32_t index,const wchar_t *str)
{
    LVITEMW lvitem;
    
    memset(&lvitem,0,sizeof(lvitem));
    
    lvitem.iItem = index;
    lvitem.mask = LVIF_TEXT;
    lvitem.pszText = (wchar_t*)str;
    
    if(img_index >= 0)
    {
        lvitem.mask |= LVIF_IMAGE;
        lvitem.iImage=img_index;
    }
    
    return (status_t)::SendMessageW(hwnd, LVM_INSERTITEMW, 0, (LPARAM)(&lvitem));
}
status_t CListView::InsertSumItem(int32_t row, int32_t col, const wchar_t *str)
{
    LVITEMW lvitem;
    
    memset(&lvitem,0,sizeof(lvitem));
    
    lvitem.iItem = row;
    lvitem.iSubItem = col;
    lvitem.mask = LVIF_TEXT;
    lvitem.pszText = (wchar_t*)str;    
    
    return (status_t)::SendMessageW(hwnd, LVM_SETITEMW, 0, (LPARAM)&lvitem);
}
status_t CListView::Clear()
{
    return (status_t)::SendMessageW((hwnd), LVM_DELETEALLITEMS, 0, 0);
}
status_t CListView::DeleteColumn(int32_t index)
{   
    int32_t ret = (status_t)::SendMessageW((hwnd), LVM_DELETECOLUMN, (WPARAM)(int32_t)(index), 0);
    
    if(ret == TRUE)
        this->_column_count --;
    
    return ret;
}
status_t CListView::DeleteItem(int32_t index)
{
    return (status_t)::SendMessageW((hwnd), LVM_DELETEITEM, (WPARAM)(int32_t)(index), 0);
}
status_t CListView::SetColumnWidth(int32_t index, int32_t width)
{
    return (status_t)::SendMessageW((hwnd), LVM_SETCOLUMNWIDTH, (WPARAM)(int32_t)(index), MAKELPARAM((width), 0));
}
status_t CListView::SetItemText(int32_t row, int32_t col,const wchar_t *str)
{
    LV_ITEMW _ms_lvi;
    
    _ms_lvi.iSubItem = col;
    _ms_lvi.pszText = (wchar_t*)str;
    
    return (status_t)::SendMessageW((hwnd), LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)(LV_ITEM FAR *)&_ms_lvi);
}
status_t CListView::SetItemText(int32_t row, const wchar_t *str)
{
    return this->SetItemText(row,0,str);
}
status_t CListView::SetBkColor(COLORREF color)
{
    return (status_t)::SendMessageW((hwnd), LVM_SETBKCOLOR, 0, (LPARAM)(COLORREF)(color));
}
status_t CListView::SetTextBkColor(COLORREF color)
{
    return (status_t)::SendMessageW((hwnd), LVM_SETTEXTBKCOLOR, 0, (LPARAM)(COLORREF)(color));
}
status_t CListView::SetTextColor(COLORREF color)
{
    return (status_t)::SendMessageW((hwnd), LVM_SETTEXTCOLOR, 0, (LPARAM)(COLORREF)(color));
}
status_t CListView::GetSelectedCount()
{
    return (UINT)(status_t)::SendMessageW((hwnd), LVM_GETSELECTEDCOUNT, 0, 0L);
}
status_t CListView::GetItemCount()
{
    return (int32_t)::SendMessageW((hwnd), LVM_GETITEMCOUNT, 0, 0L);
}

int CListView::GetFirstSelection()
{
    return (int)::SendMessageW((hwnd), LVM_GETSELECTIONMARK, 0, 0);
}
status_t CListView::SetSelectionMark(int32_t index)
{
    return (int32_t)::SendMessageW((hwnd), LVM_SETSELECTIONMARK, 0, (LPARAM)index);
}
status_t CListView::GetItemText(int32_t row, int32_t col, CMem *mem)
{
    ASSERT(mem);
    LV_ITEMW _ms_lvi;
    int32_t len;
    _ms_lvi.iSubItem = col;
    _ms_lvi.cchTextMax = (int32_t)(mem->GetMaxSize()*sizeof(wchar_t));
    _ms_lvi.pszText = (wchar_t*)mem->GetRawBuf();
    len = (int32_t)::SendMessageW((hwnd), LVM_GETITEMTEXTW, (WPARAM)row, (LPARAM)(LV_ITEM FAR *)&_ms_lvi);
    mem->SetSize(len * sizeof(wchar_t));
    return OK;
}

int CListView::GetImgIndex(int row)
{
    LV_ITEMW _ms_lvi;
    _ms_lvi.mask = LVIF_IMAGE;
    _ms_lvi.iItem = row;
    _ms_lvi.iSubItem = 0;
    ::SendMessageW((hwnd), LVM_GETITEMW, (WPARAM)0, (LPARAM)(LV_ITEM FAR *)&_ms_lvi);
    return _ms_lvi.iImage;
}

status_t CListView::EnsureVisible(int32_t index)
{
    return (BOOL)(status_t)::SendMessageW((hwnd), LVM_ENSUREVISIBLE, (WPARAM)(int32_t)(index), MAKELPARAM((TRUE), 0));
}
status_t CListView::FindItem(int32_t start,const wchar_t *str)
{
    LVFINDINFOW lvf;
    
    lvf.flags = LVFI_STRING;
    lvf.psz = (wchar_t*)str;
    
    return (int32_t)::SendMessageW((hwnd), LVM_FINDITEMW, (WPARAM)(int32_t)(start), (LPARAM)(const LV_FINDINFO FAR*)(&lvf));
}
status_t CListView::AddColumn(wchar_t *str,int32_t width)
{
    return this->InsertColumn(this->_column_count,str,width);
}
status_t CListView::OnSize()
{
    int32_t i,total_len = 0;
    int32_t new_width;
    
    for(i = 0; i < this->_column_count; i++)
    {
        total_len += this->_column_widths[i];
    }
    if(total_len <= 0)
        return ERROR;
    
    RECT r;
    
    ::GetClientRect(this->hwnd,&r);

    new_width = r.right - r.left-2;
    if(this->GetStyle() & WS_VSCROLL)
    {
        new_width -= GetSystemMetrics(SM_CYVSCROLL);
    }

    for(i = 0; i < this->_column_count; i++)
    {
        this->SetColumnWidth(i, this->_column_widths[i]*new_width/total_len);
    }
    
    return OK;
}
status_t CListView::GetColumnWidth(int32_t index)
{
    return (int32_t)::SendMessageW((hwnd), LVM_GETCOLUMNWIDTH, (WPARAM)(int32_t)(index), 0);
}
status_t CListView::SetListViewExStyle(DWORD style)
{
    return (DWORD)(status_t)::SendMessageW((hwnd), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, style);
}
status_t CListView::AddItem(int32_t img_index, const wchar_t *str)
{
    int32_t index = this->GetItemCount();
    
    if(index < 0) index = 0;
    
    return this->InsertItem(img_index,index,str);
}
status_t CListView::IsChecked(int32_t index)
{
    return ((((UINT)((status_t)::SendMessageW((hwnd), LVM_GETITEMSTATE, (WPARAM)index, LVIS_STATEIMAGEMASK))) >> 12) -1);
}
status_t CListView::AddSubItem(int32_t col, const wchar_t *str)
{
    return this->InsertSumItem(this->GetItemCount() - 1,col,str);
}
status_t CListView::SetCheck(int32_t index, BOOL flag)
{
    LV_ITEMW _ms_lvi;
    
    _ms_lvi.stateMask = LVIS_STATEIMAGEMASK;
    _ms_lvi.state = INDEXTOSTATEIMAGEMASK(flag?2:1);
    
    return (status_t)::SendMessageW((hwnd), LVM_SETITEMSTATE, (WPARAM)index, (LPARAM)(LV_ITEM FAR *)&_ms_lvi);
}
status_t CListView::CheckAll(BOOL flag)
{
    int32_t i,max;
    
    max = this->GetItemCount();
    
    for(i = 0; i < max; i++)
    {
        this->SetCheck(i,flag);
    }
    
    return OK;
}
status_t CListView::IsSelected(int32_t index)
{
    DWORD state = (status_t)::SendMessageW((hwnd), LVM_GETITEMSTATE, (WPARAM)index, LVIS_SELECTED );
    return state & LVIS_SELECTED;
}

status_t CListView::SetImageList(CImageList *list, int32_t type)
{
    ASSERT(list && hwnd);
    return (status_t)SendMessageW(hwnd,LVM_SETIMAGELIST,type, (LPARAM)list->hImages);
}

status_t CListView::SeleteItem(int32_t index)
{
    LVITEMW v;

    v.mask = LVIF_STATE;
    v.iItem = index;
    v.stateMask = LVIS_SELECTED;
    v.state = LVIS_SELECTED;
    
    return (status_t)SendMessageW(hwnd,LVM_SETITEMSTATE,index,(LPARAM)&v);
}
/***********************************************************/
/*                         SpinBox                         */
/***********************************************************/
status_t CSpinBox::Create()
{
    return this->CreateCtrl(UPDOWN_CLASSW);
}
status_t CSpinBox::Init()
{
    CCtrl::Init();
    this->AddStyle(UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS);
    return OK;
}
status_t CSpinBox::SetBuddy(HWND hwnd)
{
    return (status_t)::SendMessageW(this->hwnd,UDM_SETBUDDY , (WPARAM)hwnd,0);
}
status_t CSpinBox::SetRange(int32_t min, int32_t max)
{
    return (status_t)::SendMessageW(this->hwnd,UDM_SETRANGE32 , (WPARAM)min,(LPARAM)max);
}
/***********************************************************/
/*                          CRichEdit                      */
/***********************************************************/
#if HAVE_RICH_EDIT
HMODULE CRichEdit::hmodule = NULL;
CRichEdit::CRichEdit()
{
    this->InitBasic();
}
CRichEdit::~CRichEdit()
{
    this->Destroy();
}
status_t CRichEdit::InitBasic()
{
    CEditBox::InitBasic();
    this->menu_mouse = NULL;
    this->mitem_copy = NULL;
    this->mitem_paste = NULL;
    this->mitem_cut = NULL;
    this->mitem_sel_all = NULL;
    this->old_proc = NULL;
    return OK;
}
status_t CRichEdit::Init()
{
    this->InitBasic();
    CEditBox::Init();
    return OK;
}
status_t CRichEdit::Create()
{
    this->CreateCtrl(L"RICHEDIT20A");
    ASSERT(this->hwnd);
    
    this->old_proc=(WNDPROC)CWnd::GetWndLong(hwnd,GWL_WNDPROC);
    ASSERT(this->old_proc);
    CWnd::SetWndLong(hwnd,GWL_WNDPROC,(int_ptr_t)CRichEdit::NewEditProc);
    NEW(this->menu_mouse,CPopupMenu);
    this->menu_mouse->Init();
    this->menu_mouse->SetParent(hwnd);
    this->menu_mouse->SetName(L"Menu");
    this->menu_mouse->Create();
    NEW(this->mitem_copy,CMenu);
    this->mitem_copy->Init();
    this->mitem_copy->SetName(L"Copy");
    this->mitem_copy->Create();
    
    NEW(this->mitem_paste,CMenu);
    this->mitem_paste->Init();
    this->mitem_paste->SetName(L"Paste");
    this->mitem_paste->Create();
    NEW(this->mitem_cut,CMenu);
    this->mitem_cut->Init();
    this->mitem_cut->SetName(L"Cut");
    this->mitem_cut->Create();
    NEW(this->mitem_sel_all,CMenu);
    this->mitem_sel_all->Init();
    this->mitem_sel_all->SetName(L"Select All");
    this->mitem_sel_all->Create();
    this->menu_mouse->AddItem(mitem_copy);
    this->menu_mouse->AddItem(mitem_paste);
    this->menu_mouse->AddItem(mitem_cut);
    this->menu_mouse->AddSeparator();
    this->menu_mouse->AddItem(mitem_sel_all);
    return OK;
}
status_t CRichEdit::Destroy()
{
    CEditBox::Destroy();
    
    DEL(this->menu_mouse);
    DEL(this->mitem_copy);
    DEL(this->mitem_paste);
    DEL(this->mitem_cut);
    DEL(this->mitem_sel_all);
    return OK;
}
status_t CRichEdit::SetBkColor(COLORREF color)
{
    return (status_t)::SendMessageW(hwnd,EM_SETBKGNDCOLOR,0,(LPARAM)color);
}
status_t CRichEdit::SetSelectTextColor(COLORREF color)
{
    CHARFORMAT2W charf;
    charf.cbSize = sizeof(CHARFORMAT2W);
    charf.dwMask = CFM_COLOR;
    charf.crTextColor = color;
    
    return (status_t)::SendMessageW(hwnd,EM_SETCHARFORMAT,(WPARAM)SCF_SELECTION,(LPARAM)&charf);
}
status_t CRichEdit::SetAllTextColor(COLORREF color)
{
    CHARFORMAT2W charf;
    
    charf.cbSize = sizeof(CHARFORMAT2W);
    charf.dwMask = CFM_COLOR;
    charf.crTextColor = color;
    
    return (status_t)::SendMessageW(hwnd,EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&charf);
}
status_t CRichEdit::SetSelectTextHeight(int32_t height)
{
    CHARFORMAT2W charf;
    
    charf.cbSize = sizeof(CHARFORMAT2W);
    charf.dwMask = CFM_SIZE;
    charf.yHeight = height*20;
    return (status_t)::SendMessageW(hwnd,EM_SETCHARFORMAT,(WPARAM)SCF_SELECTION,(LPARAM)&charf);
}
status_t CRichEdit::SetSelectTextWeight(WORD wWeight)
{
#ifndef CFM_WEIGHT
#define CFM_WEIGHT  0x00400000
#endif
    CHARFORMAT2W charf;
    charf.cbSize = sizeof(CHARFORMAT2W);
    charf.dwMask = CFM_WEIGHT;
    charf.wWeight = wWeight ;
    return (status_t)::SendMessageW(hwnd,EM_SETCHARFORMAT,(WPARAM)SCF_SELECTION,(LPARAM)&charf);
}
status_t CRichEdit::SetCharSet(BYTE char_set)
{
    CHARFORMAT2W charf;
    
    charf.cbSize = sizeof(CHARFORMAT2W);
    charf.dwMask = CFM_CHARSET;
    charf.bCharSet = char_set;
    return (status_t)::SendMessageW(hwnd,EM_SETCHARFORMAT,(WPARAM)SCF_SELECTION,(LPARAM)&charf);
}
status_t CRichEdit::SetFontName(const wchar_t *name)
{
    CHARFORMAT2W charf;
    
    charf.cbSize = sizeof(CHARFORMAT2W);
    charf.dwMask = CFM_FACE;
    crt_strcpy_w(charf.szFaceName,name);
    return (status_t)::SendMessageW(hwnd,EM_SETCHARFORMAT,(WPARAM)SCF_SELECTION,(LPARAM)&charf);
}
status_t CRichEdit::GetSelectTextRange(int32_t *min, int32_t *max)
{
    ASSERT(min && max);
    CHARRANGE range;
    (status_t)::SendMessageW(hwnd,EM_EXGETSEL,0,(LPARAM)&range);
    *min=range.cpMin;
    *max=range.cpMax;
    return OK;
}
status_t CRichEdit::LoadModule()
{
    CRichEdit::hmodule = LoadLibraryW(L"RichEd20.dll");
    ASSERT(CRichEdit::hmodule);
    return OK;
 }
status_t CRichEdit::UnloadModule()
{
    FreeLibrary(CRichEdit::hmodule);
    CRichEdit::hmodule = NULL;
    return OK;
}
status_t CRichEdit::SetTextMode(DWORD mode)
{
    return (status_t)::SendMessageW(hwnd,EM_SETTEXTMODE,(WPARAM)mode,0);
}
status_t CRichEdit::UpdateMenuStatus()
{
    int32_t sel_min,sel_max;
    this->GetSelectTextRange(&sel_min,&sel_max);
    BOOL b = (sel_min!=sel_max);
    this->mitem_copy->Enable(b);
    this->mitem_cut->Enable(b);
    return OK;
}
LRESULT CRichEdit::NewEditProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    int_ptr_t data;
    
    if(!CWnd::GetUserData(hwnd,0,&data))
        return ERROR;
    CRichEdit *tp=(CRichEdit *)data;
    ASSERT(tp);
    if(message==WM_RBUTTONUP)
    {
        tp->UpdateMenuStatus();
        tp->menu_mouse->Show();
        return TRUE;
    }    
    if(message==WM_COMMAND)
    {
        if(tp->mitem_copy->IsMyCommand(wparam))
            ::SendMessageW(tp->hwnd,WM_COPY,0,0);
        if(tp->mitem_cut->IsMyCommand(wparam))
            ::SendMessageW(tp->hwnd,WM_CUT,0,0);
        if(tp->mitem_paste->IsMyCommand(wparam))
            ::SendMessageW(tp->hwnd,WM_PASTE,0,0);
        if(tp->mitem_sel_all->IsMyCommand(wparam))
            tp->SetSel(0,tp->GetTextLen());
    
        return TRUE;
    }
    return CallWindowProc(tp->old_proc,hwnd,message,wparam,lparam);
}
status_t CRichEdit::SetRtfText(CFileBase *file)
{
    ASSERT(file);
    file->Seek(0);
    EDITSTREAM es;
    
    es.dwError = 0;
    es.dwCookie = (DWORD)file;
    es.pfnCallback = (EDITSTREAMCALLBACK )CRichEdit::callback_set_rtf;
    return (status_t)::SendMessage(hwnd,EM_STREAMIN,SF_RTF,(LPARAM)&es);
}
DWORD CALLBACK CRichEdit::callback_set_rtf(int_ptr_t dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    CFileBase *file = (CFileBase *)dwCookie;
    ASSERT(file);
    
    LOCAL_MEM(mem);
    
    fsize_t read_size = mem.GetMaxSize();
    if(read_size > cb)
        read_size = cb;
    *pcb = 0;
    int32_t rs = file->Read(mem.GetRawBuf(),cb);
    if(rs > 0)
    {
        *pcb = rs;
        memcpy(pbBuff,mem.GetRawBuf(),rs);
    }
    return 0;
}
status_t CRichEdit::GetRtfText(CFileBase *file)
{
    ASSERT(file);
    file->SetSize(0);
    EDITSTREAM es;
    
    es.dwError = 0;
    es.dwCookie = (DWORD)file;
    es.pfnCallback = (EDITSTREAMCALLBACK )CRichEdit::callback_get_rtf;
    return (status_t)::SendMessage(hwnd,EM_STREAMOUT,SF_RTF,(LPARAM)&es);
}
DWORD CALLBACK CRichEdit::callback_get_rtf(int_ptr_t dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    CFileBase *file = (CFileBase *)dwCookie;
    ASSERT(file);
    *pcb = file->Write(pbBuff,cb);
    return 0;
}
#endif
/***********************************************************/
/*                          ToolTip                        */
/***********************************************************/
#if HAVE_TOOL_TIP
CToolTip::CToolTip()
{
}
CToolTip::~CToolTip()
{
    this->Destroy();
}
status_t CToolTip::Init()
{
    CCtrl::Init();
    this->wnd_create->style = TTS_ALWAYSTIP ;
    this->wnd_create->ex_style =  WS_EX_TOOLWINDOW ;
    this->wnd_create->id = 0;
    return OK;
}
status_t CToolTip::Destroy()
{
    CCtrl::Destroy();
    return OK;
}
status_t CToolTip::Create()
{
    return this->CreateCtrl(TOOLTIPS_CLASSW);
}
status_t CToolTip::AddTool(TOOLINFOW *ti)
{
    ASSERT(ti);
    ti->cbSize = sizeof(TOOLINFOW);
    int32_t ret = (status_t)::SendMessageW(hwnd, TTM_ADDTOOLW, 0, (LPARAM)ti);
    if(ret == 0)
    {
        ti->cbSize = sizeof(TOOLINFOW) - sizeof(void*);
        ret = (status_t)::SendMessageW(hwnd, TTM_ADDTOOLW, 0, (LPARAM)ti);
    }
    return ret;
}
status_t CToolTip::GetToolCount()
{
    return (status_t)::SendMessageW(hwnd,TTM_GETTOOLCOUNT,0,0);
}
status_t CToolTip::GetToolById(DWORD uid, TOOLINFOW *ti)
{
    ASSERT(ti);
    
    ti->cbSize = sizeof(TOOLINFOW);
    ti->uId = uid;
    int32_t ret = (status_t)::SendMessageW(hwnd,TTM_GETTOOLINFOW,0,(LPARAM)ti);
    if(ret == 0)
    {
        ti->cbSize = sizeof(TOOLINFOW) - sizeof(void*);
        ret = (status_t)::SendMessageW(hwnd,TTM_GETTOOLINFOW,0,(LPARAM)ti);
    }
    return ret;
}
status_t CToolTip::GetToolByIndex(int32_t index, TOOLINFOW *ti)
{
    ASSERT(ti);
    ti->cbSize = sizeof(TOOLINFOW);
    int32_t ret = (status_t)::SendMessageW(hwnd,TTM_ENUMTOOLSW,index,(LPARAM)ti);
    if(ret == 0)
    {
        ti->cbSize = sizeof(TOOLINFOW) - sizeof(void*);
        ret = (status_t)::SendMessageW(hwnd,TTM_ENUMTOOLSW,index,(LPARAM)ti);
    }
    return ret;
}
status_t CToolTip::AddTool(RECT *r, const wchar_t *text)
{
    ASSERT(r && text);
    TOOLINFOW ti;
    ti.cbSize = sizeof(TOOLINFOW); 
    ti.uFlags = TTF_SUBCLASS; 
    ti.hwnd = this->wnd_create->hparent; 
    ti.hinst = GetSelfInstance(); 
    ti.uId = CWnd::GetUniqueID(); 
    ti.lpszText = (wchar_t*)text;
    CopyRect(&ti.rect,r);
    
    return this->AddTool(&ti);
}
status_t CToolTip::AddTool(HWND hwnd, const wchar_t *text)
{
    ASSERT(text);
    TOOLINFOW ti;
    ti.cbSize = sizeof(TOOLINFOW); 
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND; 
    ti.hwnd = this->wnd_create->hparent; 
    ti.hinst = GetSelfInstance(); 
    ti.uId = (UINT)hwnd;
    ti.lpszText = (wchar_t*)text;
    return this->AddTool(&ti);
}
status_t CToolTip::SetToolRect(TOOLINFOW *ti)
{
    ASSERT(ti);
    return (status_t)::SendMessageW(hwnd,TTM_NEWTOOLRECTW,0,(LPARAM)ti);
}
status_t CToolTip::SetToolRectByIndex(int32_t index, RECT *r)
{
    ASSERT(r);
    TOOLINFOW ti;
    memset(&ti,0,sizeof(ti));
    this->GetToolByIndex(index,&ti);
    CopyRect(&ti.rect,r);
    
    return this->SetToolRect(&ti);
}
status_t CToolTip::SetToolInfo(TOOLINFOW *ti)
{
    ASSERT(ti);
    return (status_t)::SendMessageW(hwnd,TTM_SETTOOLINFOW,0,(LPARAM)ti);
}
status_t CToolTip::SetToolTextByIndex(int32_t index, const wchar_t *text)
{
    ASSERT(text);
    TOOLINFOW ti;
    
    this->GetToolByIndex(index,&ti);
    ti.lpszText = (wchar_t*)text;
    return this->SetToolInfo(&ti);
}
status_t CToolTip::SetDelayTime(int32_t ms)
{
    return (status_t)::SendMessageW(this->hwnd,TTM_SETDELAYTIME,TTDT_INITIAL,MAKELONG(ms,0));
}
#endif
/***********************************************************/
/*                          TrayIcon                       */
/***********************************************************/
UINT CTrayIcon::s_uTaskbarRestart = ::RegisterWindowMessageW(L"TaskbarCreated");
CTrayIcon::CTrayIcon()
{
    this->InitBasic();
}
CTrayIcon::~CTrayIcon()
{
    this->Destroy();
}
status_t CTrayIcon::InitBasic()
{
    this->hico = NULL;
    memset(&notify_data,0,sizeof(notify_data));
    this->notify_data.cbSize = sizeof(notify_data);
    this->notify_data.uFlags = NIF_ICON |NIF_MESSAGE |NIF_TIP;
    this->notify_data.uID = CWnd::GetUniqueID();
    return OK;
}
status_t CTrayIcon::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CTrayIcon::Destroy()
{
    if(this->hico)
    {
        this->DeleteTrayIcon();
        ::DestroyIcon(hico);
        this->hico = NULL;
    }
    this->InitBasic();
    return OK;
}
status_t CTrayIcon::SetIcon(UINT res_id)
{
   ASSERT(this->hico == NULL);
   hico=::LoadIcon(GetSelfInstance(),MAKEINTRESOURCE(res_id));
   ASSERT(hico);
   this->notify_data.hIcon = this->hico;
   return OK;
}
status_t CTrayIcon::SetToolTip(const wchar_t *str)
{
    ASSERT(str);
    ASSERT(crt_strlen_w(str) < 64);
    crt_strcpy_w(this->notify_data.szTip,str);
    return OK;
}
status_t CTrayIcon::SetParent(HWND hwnd)
{
    this->notify_data.hWnd = hwnd;
    return OK;
}
status_t CTrayIcon::SetCallBackMessage(UINT message)
{
    this->notify_data.uCallbackMessage =  message;
    return OK;
}
status_t CTrayIcon::AddTrayIcon()
{
    return Shell_NotifyIconW(NIM_ADD,&this->notify_data);
}
status_t CTrayIcon::DeleteTrayIcon()
{
    return Shell_NotifyIconW(NIM_DELETE,&this->notify_data);
}
UINT CTrayIcon::GetTrayMessage(WPARAM wparam, LPARAM lparam)
{
    if(this->notify_data.uID != wparam)
        return 0;
    return (UINT)lparam;
}
/***********************************************************/
/*                          CommandBar                     */
/***********************************************************/
#if HAVE_COMMAND_BAR
CCommandBar::CCommandBar()
{
    this->InitBasic();
}
CCommandBar::~CCommandBar()
{
    this->Destroy();
}
status_t CCommandBar::InitBasic()
{
    this->hwndCB = NULL;
    return OK;
}
status_t CCommandBar::Init()
{
    this->InitBasic();
    return OK;
}
status_t CCommandBar::Destroy()
{
    if(this->hwndCB)
    {
        CommandBar_Destroy(hwndCB);
    }
    this->InitBasic();
    return OK;
}

status_t CCommandBar::Create(HWND hparent)
{
    hwndCB = CommandBar_Create(GetSelfInstance(), hparent, 1);  
    ASSERT(this->hwndCB);
    return OK;
}

status_t CCommandBar::InsertMenu(CMenu *menu)
{
    ASSERT(menu);
    CommandBar_InsertMenubarEx(hwndCB,NULL,(LPTSTR)menu->hmenu,0);
    return OK;
}

status_t CCommandBar::AddAdornments(uint32_t flag)
{   
    CommandBar_AddAdornments(hwndCB, flag, 0);
    return OK;
}
#endif
