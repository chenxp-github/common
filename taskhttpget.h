#ifndef __TASKHTTPGET_H
#define __TASKHTTPGET_H

#include "tcp.h"
#include "taskmgr.h"
#include "taskhttpclient.h"
#include "httpheader.h"
#include "memfile.h"
#include "file.h"
#include "closure.h"

class CTaskHttpGet:public CTask{
public:

    enum{
        EVENT_STOP = 1,
        EVENT_DONE,
    };

    enum{
        ERROR_NONE = 0,
        ERROR_EXCEED_MAX_RETRIES,
        ERROR_HTTP_CLIENT_ERROR,
        ERROR_OPEN_FILE_ERROR,
        ERROR_ERROR_HOST_NAME,
        ERROR_CONNECT_ERROR,
        ERROR_TOO_MANY_RELOCATE,
        ERROR_ERROR_HTTP_RESPONSE,
        ERROR_REMOTE_FILE_NOT_FOUND,
    };

    CHttpHeader *mSendHeader;
    CHttpHeader *mRecvHeader;
    int mTaskTcpConnector;
    int mTaskHttpClient;
    CTcpClient *mTcpClient;
    CMemFile *mHttpHeaderFile;
    CMem *mDestFileName;
    CFile *mDestFile;
    CMemFile *mTmpFileInMem;
    CMem *mProxtHost;
    int mProxyPort;
    int mRelocateTimes;
    int mState;
    int mRetry;
    int mMaxRetries;
    uint32_t mFlags;
    CClosure *mCallback;
public:
    status_t SetMaxRetries(int max);
    status_t OpenOrCreateDestFile();
    status_t SetDumpHeader(bool tf);
    status_t SetResumeDownload(bool r);
    bool UseProxy();
    status_t SetProxyAuth(const char *auth);
    status_t SetProxy(const char *host, int port);
    status_t Done();
    status_t CreateDestFile();
    status_t PrepareRange();
    status_t OpenDestFile();
    bool IsDownloadInMem();
    status_t SetDestFileName(const char *destfilename);
    status_t SetUserAgent(const char *user_agent);
    status_t SetHost(const char *host);
    status_t CreateTaskHttpClient();
    status_t CreateTaskTcpConnector();
    status_t Retry(int err);
    status_t GetHostAndPort(CMem *host, int *port);
    status_t SetRequestUrl(const char *url);
    CHttpHeader* GetSendHeader();
    CTaskHttpGet();
    virtual ~CTaskHttpGet();
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

