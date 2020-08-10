// Sms.cpp: implementation of the CSms class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "sms.h"

#if _WINCE_
#include <sms.h>
#pragma comment(lib,"sms.lib")
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSms::CSms()
{
    this->InitBasic();
}
CSms::~CSms()
{
    this->Destroy();
}
int CSms::InitBasic()
{
    this->sms_body = NULL;
    this->addr_list = NULL;
    
    return OK;
}
int CSms::Init()
{
    this->InitBasic();
    NEW(this->addr_list,CMemStk);
    this->addr_list->Init(64);
    this->addr_list->unicode_mode = TRUE;

    NEW(this->sms_body,CMem);
    this->sms_body->Init();
    this->sms_body->Malloc(1024);

    return OK;
}
int CSms::Destroy()
{
    DEL(this->sms_body);
    DEL(this->addr_list);

    this->InitBasic();
    return OK;
}

int CSms::AddAddr(WORD *addr)
{
    ASSERT(addr);
    this->addr_list->PushW(addr);

    return OK;
}

int CSms::SetSmsBody(CFileBase *file)
{
    ASSERT(file);

    this->sms_body->SetSize(0);
    this->sms_body->WriteFile(file);
    this->sms_body->PutcW(0);

    return OK;
}   

int CSms::SendSms(WORD* lpszMessage, WORD* lpszRecipient)
{
#if _WINCE_
    SMS_HANDLE smshHandle; 
    SMS_ADDRESS smsaDestination;
    TEXT_PROVIDER_SPECIFIC_DATA tpsd;
    SMS_MESSAGE_ID smsmidMessageID;
    int status;
    
    memset(&tpsd, 0, sizeof(tpsd));

    status = SmsOpen(SMS_MSGTYPE_TEXT, SMS_MODE_SEND, &smshHandle, NULL);
    ASSERT(!FAILED(status));
        
    smsaDestination.smsatAddressType = SMSAT_UNKNOWN;

    _tcsncpy(smsaDestination.ptsAddress, lpszRecipient, SMS_MAX_ADDRESS_LENGTH);
        
    tpsd.dwMessageOptions =  PS_MESSAGE_OPTION_NONE;
    tpsd.psMessageClass = PS_MESSAGE_CLASSUNSPECIFIED;
    tpsd.psReplaceOption = PSRO_NONE;
        
    memset(tpsd.pbHeaderData, 0,sizeof(tpsd.pbHeaderData));

    tpsd.dwHeaderDataSize = 0; 
    tpsd.fMessageContainsEMSHeaders = FALSE; 
    tpsd.dwProtocolID = SMS_MSGPROTOCOL_UNKNOWN; 
    
    status = SmsSendMessage(smshHandle, NULL, &smsaDestination, NULL, 
                (PBYTE) lpszMessage, _tcslen(lpszMessage) * sizeof(TCHAR), 
                (PBYTE) &tpsd, sizeof(TEXT_PROVIDER_SPECIFIC_DATA), SMSDE_UCS2,
                SMS_OPTION_DELIVERY_NONE, &smsmidMessageID);

    ASSERT(SUCCEEDED(status));
    ASSERT( SUCCEEDED(SmsClose(smshHandle)) );
#else
    LOG("send sms:\n");
    PSW(lpszRecipient);
    PSW(lpszMessage);   
#endif

    return OK;  
}

int CSms::SendSms()
{
    CMem *pmem;
    int i;

    for(i = 0; i < this->addr_list->GetLen(); i++)
    {
        pmem = this->addr_list->GetElem(i);
        ASSERT(pmem);

        ASSERT( this->SendSms((WORD*)this->sms_body->p,(WORD*)pmem->p) );
    }
    
    return OK;
}
