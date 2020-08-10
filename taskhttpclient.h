#ifndef __TASKHTTPCLIENT_H
#define __TASKHTTPCLIENT_H

#include "taskmgr.h"
#include "mem.h"
#include "tcp.h"
#include "socketreaderwriter.h"
#include "closure.h"

class CTaskHttpClient:public CTask{
public:
    enum{
        EVENT_PREPARE_HEADER_TO_SEND = 1,
        EVENT_HEADER_SEND_OK,
        EVENT_PREPARE_DATA_TO_SEND,
        EVENT_DATA_SEND_OK,
        EVENT_PREPARE_FILE_TO_RECV_HEADER,
        EVENT_PREPARE_FILE_TO_RECV_DATA,
        EVENT_GOT_HEADER,
        EVENT_DONE,
        EVENT_STOP,
    };

    enum{
        ERROR_NONE,
        ERROR_SOCKET_RW_ERROR,
    };

    int mStep;
    CFileBase *iCurFile;
    CMem *mTmpLine;
    CSocketReaderWriter *mSocketRw;
    bool mNoContentLength;
    CClosure *mCallback;
public:
    CClosure* Callback();
    status_t PrepareFile(CFileBase *ifile);
    status_t SetSocket(CSocket *isocket);
    CTaskHttpClient();
    virtual ~CTaskHttpClient();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t Run(uint32_t interval);
    const char * ErrorToString(int err);
    status_t ReportError(int err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
