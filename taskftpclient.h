#ifndef __TASKFTPCLIENT_H
#define __TASKFTPCLIENT_H

#include "taskmgr.h"
#include "tcp.h"
#include "tasksocketreader.h"
#include "tasksocketwriter.h"
#include "memstk.h"

#define FTP_REQ_ERROR   0
#define FTP_REQ_LIST    1
#define FTP_REQ_STOR    2
#define FTP_REQ_RETR    3
#define FTP_REQ_PASS    4
#define FTP_REQ_USER    5
#define FTP_REQ_SIZE    6
#define FTP_REQ_QUIT    7
#define FTP_REQ_REST    8
#define FTP_REQ_PASV    9
#define FTP_REQ_TYPE    10

class CTaskFtpClient:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_SOCKET_CLOSED;
    static status_t ERROR_PERMISSION_DENIED;
    static status_t ERROR_CONNECT_TIMEOUT;
    static status_t ERROR_UNKNOWN_HOST;
    static status_t ERROR_CONNECT;
    static status_t ERROR_RESPONSE;
    static status_t ERROR_AUTH_FAIL;    

    static int EVENT_QUIT;
    static int EVENT_ON_RESPONSE;

    int mStep;
    int mTimeout;   
    CTcpClient *mTcpClient;
    CTcpClient *mTcpDataClient;
    int mTaskSocketReader;
    int mTaskSocketWriter;
    int mTaskDataSocketReader;
    int mTaskDataSocketWriter;
    CMem *mServerName;
    int mPort;
    CMem *mDataIP;
    int mDataPort;
    int_ptr_t *mHostToIpContext;
    int mConnectTime;
    CMem *mRequest;
    CMem *mResponse;
    CMemStk *mUserRequests;
    CFileBase *iDstFile;
    CFileBase *iSrcFile;
    int mTimerSyncTimeout;
    fsize_t mMaxDstFileSize;
public:
    status_t SetMaxDstFileSize(fsize_t max);
    static status_t on_sync_timeout(void **p);
    status_t SyncTimeout();
    bool IsEndResponseLine(CMem *line);
    int ToRequestCode(CMem *req);
    int VerbToRequestCode(const char *verb);
    status_t SetSrcFile(CFileBase *iFile);
    status_t SetDstFile(CFileBase *iFile);
    status_t InitTcpDataClient();
    status_t ParsePasvParams(CMem *line, CMem *ip, int *port);
    status_t OnResponse(CMem *response);
    status_t ParseResponse(CFileBase *res,int *code, CFileBase *body);  
    status_t AddRequest(const char*szFormat,...);
    status_t AddRequest(CMem *req);
    status_t InitTcpClient();
    const char * GetServerName();
    status_t SetServer(const char *name , int port);
    status_t Stop(int err);
    status_t Start();
    static status_t on_socket_reader_event(void **p);
    static status_t on_socket_writer_event(void **p);
    static status_t on_data_socket_reader_event(void **p);
    static status_t on_data_socket_writer_event(void **p);
    CTaskSocketWriter * GetSocketWriter(bool renew);
    CTaskSocketReader * GetSocketReader(bool renew);
    CTaskSocketWriter * GetDataSocketWriter(bool renew);
    CTaskSocketReader * GetDataSocketReader(bool renew);
    status_t SetTimeout(int timeout);
    const char * ErrorToString(int err);
    status_t ReportError(int err);
    CTaskFtpClient();
    virtual ~CTaskFtpClient();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t OnTimer(int interval);
};

#endif
