#include "winprinter.h"
#include "syslog.h"

CWinPrinter::CWinPrinter()
{
    this->InitBasic();
}
CWinPrinter::~CWinPrinter()
{
    this->Destroy();
}
status_t CWinPrinter::InitBasic()
{
    this->mDc = NULL;
    this->mMemDc = NULL;
    this->mPaper = NULL;
    return OK;
}
status_t CWinPrinter::Init()
{
    this->InitBasic();
    
    NEW(this->mMemDc,CMemDC);
    this->mMemDc->Init();

    NEW(this->mPaper,CVirtualPaper);
    this->mPaper->Init();
    this->mPaper->SetMemDc(this->mMemDc);
    return OK;
}
status_t CWinPrinter::Destroy()
{
    DEL(this->mPaper);
    DEL(this->mMemDc);
    if(this->mDc)
    {
        DeleteDC(this->mDc);
    }
    this->InitBasic();
    return OK;
}

status_t CWinPrinter::OpenDefault()
{
    ASSERT(this->mDc == NULL);

    char szprinter[1024];

    GetProfileString("windows","device","",szprinter,sizeof(szprinter));
    
    CMem mem,driver,device,output;
    LOCAL_MEM(driver);
    LOCAL_MEM(device);
    LOCAL_MEM(output);

    mem.SetP(szprinter);
    mem.SetSplitChars(",");
    
    mem.Seek(0);
    mem.ReadString(&device);
    mem.ReadString(&driver);
    mem.ReadString(&output);

    this->mDc = CreateDC(driver.GetP(),device.GetP(),output.GetP(),NULL);

    if(this->mDc)
    {
        this->mMemDc->AttachDC(mDc);
        this->mMemDc->width = this->GetPageWidth();
        this->mMemDc->height = this->GetPageHeight();
    }

    return this->mDc != NULL;
}

status_t CWinPrinter::StartDoc(const char *docName)
{
    ASSERT(docName);

    DOCINFO di;
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = docName;
    di.lpszOutput = NULL;
    di.lpszDatatype = NULL;
    di.fwType = 0;
    
    return ::StartDoc(mDc,&di) > 0;
}

status_t CWinPrinter::EndDoc()
{
    return ::EndDoc(mDc) > 0;
}

status_t CWinPrinter::StartPage()
{
    return ::StartPage(mDc) > 0;
}

status_t CWinPrinter::EndPage()
{
    return ::EndPage(mDc) > 0;
}

int CWinPrinter::GetPageWidth()
{
    return GetDeviceCaps(mDc, HORZRES);
}

int CWinPrinter::GetPageHeight()
{
    return GetDeviceCaps(mDc, VERTRES);
}   

HDC CWinPrinter::GetDc()
{
    return this->mDc;
}

CVirtualPaper * CWinPrinter::GetVirtualPaper()
{
    return this->mPaper;
}
