#ifndef __WINPRINTER_H
#define __WINPRINTER_H

#include "common.h"
#include "mem.h"
#include "memdc.h"
#include "virtualpaper.h"

class CWinPrinter{
public:
    HDC mDc;
    CMemDC *mMemDc;
    CVirtualPaper *mPaper;
public:
    CVirtualPaper * GetVirtualPaper();
    HDC GetDc();
    int GetPageHeight();
    int GetPageWidth();
    status_t EndPage();
    status_t StartPage();
    status_t EndDoc();
    status_t StartDoc(const char *docName);
    status_t OpenDefault();
    CWinPrinter();
    virtual ~CWinPrinter();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
};

#endif

