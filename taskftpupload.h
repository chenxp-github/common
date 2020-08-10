#ifndef __TASKFTPUPLOAD_H
#define __TASKFTPUPLOAD_H

#include "taskmgr.h"
#include "taskftpclient.h"
#include "ftppartfileinfo.h"
#include "ftpfileinfostk.h"
#include "taskmd5sum.h"
#include "ftplogininfo.h"
#include "ftpthreadinfo.h"

class CTaskFtpUpload:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_LOGIN_ERROR;
    static status_t ERROR_OPEN_FILE_ERROR;
    static status_t ERROR_FTP_CLIENT_ERROR;
    static status_t ERROR_EXCEED_MAX_RETRIES;
    static status_t ERROR_SIZE_NOT_MATCH;

    static int EVENT_ALL_DONE;

    int mStep;
    CFileBase *iSrcFile;    
    int mTaskFtpClient;
    CFtpLoginInfo *mLoginInfo;
    CMem *mDstFileName;
    int mRetry;
    int mMaxRetry;
    CMemStk *mSrcFileList;
    CMemStk *mDstFileList;
    CFile *mTmpSrcFile;
    bool mUploadDirectly;
public: 
    status_t CreateMultiLevelFtpPath(CTaskFtpClient *ftp, CMem *path);
    const char * GetCurDstFileName();
    status_t MakeFullDstFileName(CMem *out);
    status_t StartDirectly(CFileBase *iFile);
    const char * GetCurSrcFileName();
    status_t AddFileToUpload(const char *srcFileName,const char *ftpFileName);
    status_t SetDstFileName(const char *fn);
    CTaskFtpClient* GetTaskFtpClient(bool renew);
    status_t Retry();
    status_t SetMaxRetries(int max);
    static status_t on_ftp_event(void **p);
    status_t SetFtpServer(const char *serverName, int port, const char *user, const char *pass);
    status_t SetFtpServer(CFtpLoginInfo *info);
    CTaskFtpUpload();
    virtual ~CTaskFtpUpload();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CTaskFtpUpload *p);
    status_t Comp(CTaskFtpUpload *p);
    status_t Print();
    status_t InitBasic();
    status_t OnTimer(int interval);
    const char * ErrorToString(int err);
    status_t ReportError(int err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
