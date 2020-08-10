// Sms.h: interface for the CSms class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMS_H__F20EBBA4_0E02_4962_937F_613EEADAA891__INCLUDED_)
#define AFX_SMS_H__F20EBBA4_0E02_4962_937F_613EEADAA891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mem.h"
#include "memstk.h"

class CSms{
public:
    CMemStk *addr_list;
    CMem *sms_body;
public:
    int SendSms();
    static int SendSms(WORD* lpszMessage, WORD* lpszRecipient);
    int SetSmsBody(CFileBase *file);
    int AddAddr(WORD *addr);
    CSms();
    virtual ~CSms();
    int Init();
    int Destroy();
    int InitBasic();
};

#endif // !defined(AFX_SMS_H__F20EBBA4_0E02_4962_937F_613EEADAA891__INCLUDED_)
