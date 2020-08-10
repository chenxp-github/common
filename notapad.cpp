#include "notepad.h"

CNotePad note_pad;

CNotePad::CNotePad()
{
    this->handle = NULL;
}
int CNotePad::FindNotePad()
{
    HWND hwnd;
    hwnd=FindWindowW(L"DEF_WND_CLASS",L"[LOG window]"); 
    if(hwnd==NULL)return ERROR; 
    handle=GetWindow(hwnd,GW_CHILD);
    return OK;
}
int CNotePad::Puts(const WCHAR *str)
{
    this->FindNotePad();
    if(this->handle == NULL)
        return ERROR;
    SendMessageW(handle,EM_SETSEL,(WPARAM)-1,(LPARAM)-1);
    SendMessageW(handle,EM_REPLACESEL,0,(LPARAM)str);
    return OK;
}

int CNotePad::Printf (const WCHAR * szFormat, ...)
{   
    WCHAR   szBuffer [2048];
    va_list pArgList;
    va_start (pArgList, szFormat);
    wvsprintfW (szBuffer, szFormat, pArgList);
    va_end (pArgList);
    return this->Puts(szBuffer);
}
