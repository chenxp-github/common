#ifndef __TASKFTPTHREAD_H
#define __TASKFTPTHREAD_H

#include "taskmgr.h"
#include "ftplogininfo.h"
#include "partfile.h"
#include "taskftpclient.h"

class CTaskFtpThread:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_LOGIN_ERROR;
    static status_t ERROR_RETR_COMMAND_ERROR;
    static status_t ERROR_FTP_CLIENT_ERROR;
    static status_t ERROR_REST_COMMAND_ERROR;
    
    static int EVENT_FINISH;

    int32_t mStep;
    int32_t mTaskFtpClient;
    CFtpLoginInfo *mLoginInfo;
    CPartFile *mDstPartFile;
    CMem *mFileName;
public:
    fsize_t GetDownloadedSize();
    fsize_t GetBlockSize();
    fsize_t GetHostOffset();
    status_t SetFileName(const char *filename);
    static status_t on_ftp_event(void **p);
    CTaskFtpClient * GetTaskFtpClient(bool renew);
    status_t SetDstFile(CFileBase *hostFile, fsize_t offset, fsize_t size);
    status_t SetLoginInfo(CFtpLoginInfo *info);
    CTaskFtpThread();
    virtual ~CTaskFtpThread();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
    const char * ErrorToString(int32_t err);
    status_t ReportError(int32_t err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
