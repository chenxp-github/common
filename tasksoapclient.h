#ifndef __TASKSOAPCLIENT_H
#define __TASKSOAPCLIENT_H

#include "taskmgr.h"
#include "tcp.h"
#include "taskhttp.h"

class CTaskSoapClient:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_CONNECT_ERROR;
    static status_t ERROR_CONNECT_TIMEOUT;
    static status_t ERROR_CONNECT_CLOSED;
    static status_t ERROR_RESOLVE_HOST_ERROR;
    static status_t ERROR_PREPARE_TCP_CLIENT_ERROR;
    static status_t ERROR_HTTP_ERROR;   
    static status_t ERROR_EXCEED_MAX_RETRIES;

    static int EVENT_QUIT;

    CTcpClient *mTcpClient;
    int mTaskHttp;
    CMem *mUrlToPost;
    int mStep;
    int mConnectTime;
    int mTimeout;
    int_ptr_t *hostoip_context;
    CMemFile *mDataToSend;
    CMemFile *mReceivedData;
    int mRetryTime;
    int MaxRetryTime;
public:
    status_t SplitXmlOut(CFileBase *xmlOut);
    status_t SetSoapBody(CFileBase *body);
    status_t MakeHttpPostHeader(CFileBase *header);
    static status_t on_http_quit(void **p);
    CTaskHttp * GetTaskHttp(bool renew);
    status_t SetTimeout(int timeout);
    status_t PrepareTcpClient();
    status_t SetUrlToPost(const char *url);
    CTaskSoapClient();
    virtual ~CTaskSoapClient();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CTaskSoapClient *p);
    status_t Comp(CTaskSoapClient *p);
    status_t Print();
    status_t InitBasic();
    status_t OnTimer(int interval);
    const char * ErrorToString(int err);
    status_t ReportError(int err);  
    status_t Start();
    status_t Stop(status_t err);
    status_t Retry(int err);
    status_t SetRetryTime(int retryTime);
};

#endif
