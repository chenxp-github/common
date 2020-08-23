// Misc.cpp: implementation of the CWinMisc class.
//
//////////////////////////////////////////////////////////////////////
#include "winmisc.h"
#include "mem.h"
#include "syslog.h"

#if _WINCE_ || _WM6X_
#include <aygshell.h>
#include <sipapi.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
status_t CWinMisc::GetAFileName(HWND hp,CFileBase *file,const wchar_t *filter, const wchar_t *init_dir)
{
    ASSERT(file && filter);
    
    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH];

    file->SetSize(0);
    szFile[0] = 0;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hp;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = init_dir;
    ofn.Flags = 0 ;
    
	wchar_t oldCd[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH,oldCd);
    int32_t ret = GetOpenFileNameW(&ofn);
	::SetCurrentDirectoryW(oldCd);

    file->PutsW(szFile);
    return ret;
}

status_t CWinMisc::GetMultiFileNames(HWND hwnd, CFileBase *file, const wchar_t *filter)
{
    OPENFILENAMEW ofn;
    
    CMem mem;
    
    LOCAL_MEM(tmp);
    
    mem.Init();
    mem.Malloc(1024 * 128); //太小了不够用，因为可能选择多个文件
    mem.SetCharW(0,0);
    
    file->SetSize(0);
    
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= (wchar_t*)mem.CStrW();
    ofn.nMaxFile = (uint32_t)(mem.GetMaxSize() - 16);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_ALLOWMULTISELECT | OFN_LONGNAMES |OFN_EXPLORER ;
    
	wchar_t oldCd[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH,oldCd);
	int32_t ret = GetOpenFileNameW(&ofn);
	::SetCurrentDirectoryW(oldCd);
    
    if(ret == FALSE)
        return ERROR;
    
    const wchar_t *p = mem.CStrW();
    int32_t num = 0;
    int32_t len = crt_strlen_w(p);
    
    ASSERT(len <= mem.GetMaxSize() - 16);
    
    //这是只要一个文件被选中的情况
    if(p[len] == 0 && p[len+1] == 0)
    {
        num = 1;
        file->PutsW(p);
    }
    else
    {       
        while(true)
        {
            if(*p != 0)
            {
                tmp.PutcW(*p);
            }
            else
            {
                if(num > 0)
                {
                    file->PutsW(mem.CStrW());
                    file->PutcW(L'\\');
                    file->PutsW(&tmp);
                    file->PutcW(L'\n');
                }

                tmp.SetSize(0);                 
                num++;
                if(*(p+1) == 0)             
                    break;
            }
            p++;
        }

        num --;
    }

    return num;
}

int CWinMisc::FolderSelect(HWND hp,CFileBase *file)
{
    ASSERT(file);

#if _WINCE_ || _WM6X_
    ASSERT(file);

    OPENFILENAMEW ofn;
    wchar_t szFile[MAX_PATH];

    file->SetSize(0);
    szFile[0] = 0;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hp;
    ofn.lpstrFilter = L"All file(*.*)\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PROJECT;
    
    int32_t ret = GetOpenFileName(&ofn);

    file->PutsW(szFile);

    return ret;
#endif

#if _WIN32_ || _WIN64_
    BROWSEINFOW sInfo;

    wchar_t dispname[MAX_PATH];

    file->SetSize(0);   

    LPITEMIDLIST    lpidlBrowse;
    sInfo.hwndOwner = hp;
    sInfo.pidlRoot      = NULL;
    sInfo.pszDisplayName = dispname;
    sInfo.lpszTitle     = L"Select folder";
    sInfo.ulFlags       = BIF_RETURNONLYFSDIRS;
    sInfo.lpfn          = NULL;
    sInfo.lParam        = 0;

	wchar_t oldCd[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH,oldCd);
	
    lpidlBrowse = SHBrowseForFolderW(&sInfo);
    
    if(lpidlBrowse != NULL)
    {       
        SHGetPathFromIDListW(lpidlBrowse,dispname);  //取得文件夹名
        file->PutsW(dispname);
    }
    
    if(lpidlBrowse != NULL)
    {
        CoTaskMemFree(lpidlBrowse);
    }
	
	::SetCurrentDirectoryW(oldCd);

    return (lpidlBrowse != NULL);
#endif

}


int CWinMisc::ChooseColor(HWND hp,COLORREF *color)
{
    static COLORREF cus[16]={0};
   
#if _WINCE_ || _WM6X_
#define CHOOSECOLORW  CHOOSECOLOR
#define ChooseColorW  ChooseColor
#endif

    CHOOSECOLORW cc;

    cc.lStructSize=sizeof(CHOOSECOLORW);
    cc.hwndOwner=hp;
    cc.hInstance=NULL;
    cc.rgbResult=0;
    cc.lpCustColors=cus;
    cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_SOLIDCOLOR;
    cc.lCustData=NULL;
    cc.lpfnHook=NULL;
    cc.lpTemplateName=NULL;

    int ret = ::ChooseColorW(&cc);

    if(color)
        *color = cc.rgbResult;

    return ret;
}

status_t CWinMisc::DeleteFolder(const wchar_t * pszFolder)
{
    SHFILEOPSTRUCTW fos ;
    
    ZeroMemory( &fos, sizeof( fos)) ;

    fos.hwnd = HWND_DESKTOP;
    fos.wFunc = FO_DELETE ;
    fos.fFlags = FOF_SILENT | FOF_NOCONFIRMATION ;
    fos.pFrom = pszFolder ;

    // 删除文件夹及其内容
    if( SHFileOperationW( &fos)!=0)
        return FALSE ;

    return TRUE;
}
void * CWinMisc::GetRecourceAddr(uint32_t id,const wchar_t *type,int32_t *size)
{
  void *p;
  
  ASSERT(size);

  HRSRC hrsrc=FindResourceW(GetSelfInstance(),MAKEINTRESOURCEW(id),type);

  if(hrsrc == NULL)
      return NULL;

  p=LockResource(LoadResource(GetSelfInstance(),hrsrc)); 
  *size = SizeofResource(GetSelfInstance(),hrsrc); 

  return p;
}
int CWinMisc::IsKeyDown(int nVirtKey)
{
    return (GetAsyncKeyState(nVirtKey)&0xFF00)>>15;
}
int CWinMisc::SetTopMost(HWND hwnd,bool flag)
{
    if(!flag)
        SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED) ;
    else
        SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE |SWP_FRAMECHANGED) ;
    return OK;
}
int CWinMisc::PlaySound(void *buf)
{
    ::PlaySoundW((const wchar_t *)buf,NULL,SND_ASYNC|SND_MEMORY);
    return OK;
}
int CWinMisc::MsgLoop()
{
    MSG msg;

    while (PeekMessageW(&msg,NULL,0,0,PM_REMOVE))
    {                
        TranslateMessage(&msg);
        DispatchMessageW(&msg);   
    }
    return OK;
}

int CWinMisc::GetSelfPath(CMem *mem)
{
    return CWinMisc::GetModulePath(NULL,mem);
}

#if _WIN32_ || _WIN64_
status_t CWinMisc::ChooseFont(HWND hp,LOGFONTW *lf,COLORREF *color)
{
    ASSERT(lf && color);

    
    CHOOSEFONTW cf;

    cf.lStructSize = sizeof(CHOOSEFONTW);
    cf.hwndOwner = hp;
    cf.hDC = NULL;
    cf.lpLogFont = lf;
    cf.iPointSize = 240;
    cf.Flags = CF_BOTH|CF_TTONLY|CF_EFFECTS;
    cf.rgbColors = *color;
    cf.lCustData = 0;
    cf.lpfnHook = NULL;
    cf.lpTemplateName = NULL;
    cf.hInstance = ::GetSelfInstance();

    int32_t ret = ::ChooseFontW(&cf);
    *color = cf.rgbColors;
    return ret;
}

HRESULT CWinMisc::CreateLink(const wchar_t *lpszPathObj,const wchar_t* lpszPathLink,const wchar_t* WorkDir)
{   
    HRESULT hres;
    IShellLinkW* psl;
    CoInitialize(NULL);

    hres = CoCreateInstance(CLSID_ShellLink, NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void **)&psl);     
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;
        
        psl->SetPath(lpszPathObj);    
        psl->SetWorkingDirectory(WorkDir); 

        hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
        if (SUCCEEDED(hres))
        {             
            hres = ppf->Save(lpszPathLink, TRUE);
            ppf->Release();
        } 

        psl->Release();
    }
    return hres;
} 

#endif


status_t CWinMisc::GetScreenRes(int32_t *width, int32_t *height)
{
    ASSERT(width && height);

    *width = GetSystemMetrics(SM_CXSCREEN);
    *height = GetSystemMetrics(SM_CYSCREEN);
    return OK;
}

status_t CWinMisc::GetSaveFileName(HWND hwnd, CFileBase *file,const wchar_t *filter,const wchar_t *init_dir)
{
    ASSERT(file);
    
    OPENFILENAMEW ofn;
    CMem mem;
    wchar_t szFile[MAX_PATH];
    szFile[0] = 0;
    
    mem.Init();
    mem.SetRawBuf(szFile,sizeof(szFile),false);
    mem.PutsW(file);
    mem.PutcW(0);   

    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile= szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = init_dir;
    ofn.Flags = OFN_OVERWRITEPROMPT;

	wchar_t oldCd[MAX_PATH];
	::GetCurrentDirectoryW(MAX_PATH,oldCd);	
    int ret = ::GetSaveFileNameW(&ofn);
	::SetCurrentDirectoryW((oldCd));

    file->SetSize(0);
    file->PutsW(szFile);
    return ret;
}

status_t CWinMisc::CreateMenuBar(HWND hwnd, CMenu *menu)
{
    ASSERT(menu);

#if _WINCE_
    SHMENUBARINFO mbi;
    RECT rc;
    RECT rcMenuBar;
    HWND hwnd_cb;
    
    memset(&mbi, 0, sizeof(SHMENUBARINFO));
    mbi.cbSize     = sizeof(SHMENUBARINFO);
    mbi.hwndParent = hwnd;
    if(menu)
        mbi.nToolBarId = (uint32_t)menu->hmenu;
    else
        mbi.nToolBarId = 0;

    mbi.hInstRes   = ::GetSelfInstance();
    mbi.nBmpId     = 0;
    mbi.cBmpImages = 0;
    mbi.dwFlags = SHCMBF_HMENU;

    SHCreateMenuBar(&mbi);
    
    hwnd_cb = mbi.hwndMB;
    ASSERT(hwnd_cb);

    GetWindowRect(hwnd, &rc);
    GetWindowRect(hwnd_cb, &rcMenuBar);

    if(rc.bottom > rcMenuBar.top)
        rc.bottom = rcMenuBar.top;

    if(rc.top < GetSystemMetrics(SM_CYCAPTION))
        rc.top = GetSystemMetrics(SM_CYCAPTION);

    ::MoveWindow(hwnd,rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top,1);
#endif

#if _WIN32_ || _WIN64_
    if(menu)
        menu->SetToWnd(hwnd);
#endif
    
    return OK;
}

status_t CWinMisc::IsButton(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"BUTTON") != 0)
        return FALSE;

    uint32_t style = ::GetWindowLongW(hwnd,GWL_STYLE);
    
    style &= 0x0f;

    if(style != BS_PUSHBUTTON && style != BS_DEFPUSHBUTTON)
        return FALSE;
    
    return TRUE;
}

status_t CWinMisc::IsMultiLineEditBox(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"EDIT") != 0)
        return FALSE;
    uint32_t style = ::GetWindowLong(hwnd,GWL_STYLE);
    
    if(style & ES_MULTILINE)
        return TRUE;

    return FALSE;   
}

status_t CWinMisc::IsEditBox(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"EDIT") != 0)
        return FALSE;
    return TRUE;   
}


status_t CWinMisc::IsListBox(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"LISTBOX") != 0)
        return FALSE;
    return TRUE;
}
status_t CWinMisc::IsListView(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"SysListView32") != 0)
        return FALSE;
    return TRUE;
}

status_t CWinMisc::IsSingleLineEditBox(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"EDIT") != 0)
        return FALSE;
    uint32_t style = ::GetWindowLong(hwnd,GWL_STYLE);
    
    if(style & ES_MULTILINE)
        return FALSE;

    return TRUE;    
}

status_t CWinMisc::IsCombBox(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"COMBOBOX") != 0)
        return FALSE;
    return TRUE;    
}

status_t CWinMisc::IsTreeView(HWND hwnd)
{
    wchar_t buf[100];
    ::GetClassNameW(hwnd,buf,100);
    if(crt_stricmp_w(buf,L"SysTreeView32") != 0)
        return FALSE;
    return TRUE;
}
status_t CWinMisc::TabFocus(HWND hwnd_parent,HWND *tab_list, int32_t max, MSG *msg)
{
    ASSERT(msg && tab_list);
    
    int32_t dir = -1,i,j;
    HWND focus_wnd,htemp;
    uint32_t style;

    if(::GetActiveWindow() != hwnd_parent)
        return FALSE;
    if(msg->message != WM_KEYDOWN)
        return FALSE;
    
    WPARAM key = msg->wParam;
    focus_wnd = ::GetFocus();

    switch(key)
    {
        case VK_UP:         
            dir = 0; 
            if(CWinMisc::IsMultiLineEditBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsListBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsListView(focus_wnd))
                return FALSE;
            if(CWinMisc::IsCombBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsTreeView(focus_wnd))
                return FALSE;
        break;
        
        case VK_TAB: 
            dir = 1; 
        break;

        case VK_DOWN:           
            dir = 1; 
            if(CWinMisc::IsMultiLineEditBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsListBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsListView(focus_wnd))
                return FALSE;
            if(CWinMisc::IsTreeView(focus_wnd))
                return FALSE;
            if(CWinMisc::IsCombBox(focus_wnd))
                return FALSE;
        break;

        case VK_LEFT:
            dir = 0;
            if(CWinMisc::IsMultiLineEditBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsSingleLineEditBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsTreeView(focus_wnd))
                return FALSE;
        break;

        case VK_RIGHT:
            dir = 1;
            if(CWinMisc::IsSingleLineEditBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsMultiLineEditBox(focus_wnd))
                return FALSE;
            if(CWinMisc::IsTreeView(focus_wnd))
                return FALSE;
        break;

        default: 
            return FALSE;
    }

    if(focus_wnd == hwnd_parent)
    {
        ::SetFocus(tab_list[0]);
        if(CWinMisc::IsButton(tab_list[0]))
        {
            style = ::GetWindowLong(tab_list[0],GWL_STYLE);         
            style &= 0xfffffff0;
            style += BS_DEFPUSHBUTTON;  
            ::SetWindowLongW(tab_list[0],GWL_STYLE,style);
            ::InvalidateRect(tab_list[0],NULL,1);
        }

        return TRUE;
    }
    
    for(i = 0; i < max; i++)
    {
        if(focus_wnd == tab_list[i])
        {
            for(j = 0; j < max; j++)
            {
                if(dir == 1)
                    htemp = tab_list[(i+j+1) % max];
                else if(dir == 0)
                    htemp = tab_list[(i-j-1) % max];
                
                uint32_t style = ::GetWindowLong(htemp,GWL_STYLE);
                if(!(style & WS_DISABLED))
                {
                    ::SetFocus(htemp);
                    break;
                }
            }
            
            if(CWinMisc::IsButton(htemp))
            {
                style = ::GetWindowLong(htemp,GWL_STYLE);
                style &= 0xfffffff0;
                style += BS_DEFPUSHBUTTON;  
                ::SetWindowLongW(htemp,GWL_STYLE,style);
                ::InvalidateRect(htemp,NULL,1);
            }

            if(htemp != focus_wnd)
            {
                if(CWinMisc::IsButton(focus_wnd))
                {
                    style = ::GetWindowLong(focus_wnd,GWL_STYLE);
                    style &= 0xfffffff0;
                    ::SetWindowLongW(focus_wnd,GWL_STYLE,style);
                    ::InvalidateRect(focus_wnd,NULL,1);
                }
            }
            
            return TRUE;
        }
    }
    return FALSE;
}

status_t CWinMisc::GetModulePath(HINSTANCE hinstance,CMem *mem)
{
    ASSERT(mem);
    int32_t ret,i;

    mem->SetSize(0);
    ret = GetModuleFileNameW(hinstance,(wchar_t*)mem->GetRawBuf(),(int)(mem->GetMaxSize()/sizeof(wchar_t)));

    if(ret)
    {
        mem->SetSize(ret*sizeof(wchar_t));
        for(i = ret; i >= 0; i--)
        {
            if(mem->CharAtW(i) == L'\\')
            {
                mem->SetCharW(i,0);
                break;
            }
        }
        mem->SetSize(i * sizeof(wchar_t));
    }
    return OK;
}

typedef BOOL (WINAPI *FuncSetLayeredWindowAttributes) (HWND hWnd, COLORREF cr, BYTE bAlpha, DWORD dwFlags);
#define WS_EX_LAYERED 0x00080000
status_t CWinMisc::SetWndTransparent(HWND hwnd, BYTE alpha)
{
    HMODULE hUser32 = GetModuleHandle("USER32.DLL");
    FuncSetLayeredWindowAttributes SetLayeredWindowAttributes;
    SetLayeredWindowAttributes = (FuncSetLayeredWindowAttributes)GetProcAddress(hUser32,"SetLayeredWindowAttributes");
    if(SetLayeredWindowAttributes)
    {
        DWORD ex_style = ::GetWindowLong(hwnd,GWL_EXSTYLE);
        ex_style |= WS_EX_LAYERED;
        ::SetWindowLong(hwnd,GWL_EXSTYLE,ex_style);
        SetLayeredWindowAttributes(hwnd,RGB(255,255,255),alpha,2);
    }
    ::FreeLibrary(hUser32);
    
    return OK;
}
