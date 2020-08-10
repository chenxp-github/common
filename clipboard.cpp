// ClipBoard.cpp: implementation of the CClipBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClipBoard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CClipBoard::CClipBoard()
{
    this->InitBasic();
}
CClipBoard::~CClipBoard()
{
    this->Destroy();
}
int CClipBoard::InitBasic()
{
    this->hmem = NULL;
    this->size = 0;
    this->data = NULL;
    return OK;
}

int CClipBoard::Init()
{
    this->InitBasic();
    return OK;
}

int CClipBoard::Destroy()
{
    if(hmem)
    {
        ::GlobalUnlock(hmem);
        hmem = NULL;
    }   
    ::CloseClipboard(); 
    this->InitBasic();
    return OK;
}
int CClipBoard::Print()
{
    //add your code
    return TRUE;
}

int CClipBoard::IsTextContent()
{
    return ::IsClipboardFormatAvailable(CF_TEXT);
}

int CClipBoard::Open()
{
    ASSERT(::OpenClipboard(NULL));
        
    hmem = ::GetClipboardData(CF_TEXT);
    if(hmem)
    {       
        data = (char*)GlobalLock(hmem);
        ASSERT(data);
        size = strlen(data)+1;
    }
    
    return OK;
}

int CClipBoard::Empty()
{
    return ::EmptyClipboard();
}

int CClipBoard::SetData(DWORD format, CFileBase *file)
{
    ASSERT(file);
    
    this->Empty();

    HANDLE  hmem = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,file->GetSize()+1);
    ASSERT(hmem);

    void *p = ::GlobalLock(hmem);
    ASSERT(p);

    file->Seek(0);
    file->Read(p,file->GetSize());
    ::GlobalUnlock(hmem);

    return ::SetClipboardData(format,hmem) != NULL;
}
