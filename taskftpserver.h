#ifndef __TASKFTPSERVER_H
#define __TASKFTPSERVER_H

#include "taskmgr.h"
#include "tcp.h"
#include "tasksocketreader.h"
#include "tasksocketwriter.h"
#include "ftpconfiglist.h"
#include "file.h"

class CTaskFtpServer:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_SOCKET_CLOSED;
    static status_t ERROR_PERMISSION_DENIED;
    static status_t ERROR_CONNECT_TIMEOUT;

    int mTaskSocketReader;
    int mTaskSocketWriter;
    CMem *mRequest;
    CMem *mResponse;
    CSocket *iSocket;
    CTcpServer *mServer;
    int mStep;
    CFtpConfigList *iConfigList;
    CFtpConfig *mCurConfig;
    status_t mAfterResponseParam;
    CMemFile *mTmpFile;
    int mTaskFtpSocketPort;
    CTcpClient *mDataClientSocket;
    CTcpServer *mDataServerSocket;
    CSocket *iDataSocket;
    int mTaskDataSocketReader;
    int mTaskDataSocketWriter;
    CMem *mCurDir;
    CFile *mIoFile;
    int mTransferType;
    CMem *mClientIP;
    CMem *mFileNameToBeRenamed;
    status_t mAllowAnonymous;
    CMem *mAnonymousRootPath;
    fsize_t mRestPos;
    int mTimeout;
    int mConnectTime;
    int mTimerSyncTimeout;
public:
    static status_t on_sync_timeout(void **p);
    status_t SyncTimeout();
    const char * ErrorToString(int err);
    status_t ReportError(int err);
    static status_t on_reader_event(void **p);
    static status_t on_writer_event(void **p);
    static status_t on_data_reader_event(void **p);
    static status_t on_data_writer_event(void **p);
    status_t SetTimeout(int timeout);
    status_t SetAnonymouseRootPath(const char *str);
    status_t AllowAnonymouse(status_t enable);
    status_t SetServerSocket(int snum, const char *client_ip);
    status_t MakeDirList(const char *dir, CFileBase *list);
    status_t ChangeDir(const char *dir);
    status_t ToAbsPath(CFileBase *file);
    status_t MakeFileList(const char *dir, CFileBase *list);
    CTaskSocketReader * GetDataReader(status_t renew);
    CTaskSocketWriter * GetDataWritter(status_t renew);
    status_t AfterResponse();
    status_t SetConfigs(CFtpConfigList *i_list);
    status_t PrepareResponse(const char *response);
    status_t PrepareResponse(CFileBase *file);
    status_t HandleRequest();
    status_t UpdateCurConfig();
    status_t PrepareRequest(const char *request);
    status_t PrepareRequest(CFileBase *file);
    status_t Start();
    status_t Stop(status_t err);
    CTaskSocketWriter * GetWritter(status_t renew);
    CTaskSocketReader * GetReader(status_t renew);
    CTaskFtpServer();
    virtual ~CTaskFtpServer();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t OnTimer(int interval);
};

#endif
