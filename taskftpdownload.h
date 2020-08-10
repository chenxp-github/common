#ifndef __TASKFTPDOWNLOAD_H
#define __TASKFTPDOWNLOAD_H

#include "taskmgr.h"
#include "taskftpclient.h"
#include "ftppartfileinfo.h"
#include "ftpfileinfostk.h"
#include "taskmd5sum.h"
#include "ftplogininfo.h"
#include "ftpthreadinfo.h"

class CTaskFtpDownload:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_LOGIN_ERROR;
    static status_t ERROR_OPEN_FILE_ERROR;
    static status_t ERROR_FTP_CLIENT_ERROR;
    static status_t ERROR_EXCEED_MAX_RETRIES;
    static status_t ERROR_RENAME_FILE_ERROR;
    static status_t ERROR_SIZE_NOT_MATCH;
    static status_t ERROR_MD5_CHECK_FAIL;
    static status_t ERROR_CREATE_BIG_FILE_ERROR;
    static status_t ERROR_TASK_MD5_SUM_ERROR;
    
    static int EVENT_ALL_DONE;
    static int EVENT_SINGLE_FILE_DONE;

    int32_t mStep;
    int32_t mTaskFtpClientForSize;
    int32_t mTaskMd5Sum;
    int32_t mTaskCreateBigFile;
    CFileBase *iDstFile;
        
    CFtpLoginInfo *mLoginInfo;
    CFtpFileInfoStk *mFileInfoList;
    CFtpFileInfo *mCurFileInfo;
    CFtpPartFileInfo *mCurPartFileInfo;
    CMem *mDstFileInMem;
    CMem *mDstPath;
    int32_t mRetry;
    int32_t mMaxRetry;
    bool mDownloadToMem;
    bool mKeepFolder;
    bool mCheckVersion;
    CFile *mTmpDstFile;
    int32_t mMaxConcurrency;
    CFtpThreadInfo *mAllThreads;
    int32_t mTimerAutoSave;
public: 
    static status_t on_autosave(void **p);
    status_t CreateAutoSaveTimer();
    status_t DeleteDstPartFile();
    status_t Rename();
    fsize_t GetDownloadedSize();
    bool NeedSaveInfoFile();
    status_t ResetAllThreads();
    status_t LoadThreadInfos();
    status_t DeletePartInfoFile();
    status_t LoadPartFileInfo();
    status_t SaveThreadInfo();
    status_t SaveThreadInfo(CFileBase *out);
    int32_t FindMaxUncompleteBlock();
    bool IsAllThreadComplete();
    bool IsAllThreadStopped();
    status_t InitAllThreads();
    status_t CreateFtpThread(int32_t slot, fsize_t offset, fsize_t size);
    static status_t on_ftp_thread_finish(void **p);
    status_t SetMaxConcurrency(int32_t max);
    static status_t on_big_file_created(void **p);
    bool IsLatestVersion();
    static status_t on_md5_sum_finish(void **p);
    CTaskMd5Sum * GetTaskMd5Sum(bool renew);
    status_t SetCheckVersion(bool check);
    status_t GetFullDstFileName(CFileBase *out);
    status_t SetKeepFolder(bool keep);
    status_t SetDstPath(const char *path);
    status_t SetDownloadToMem(bool inMem);
    status_t Retry();
    status_t SetMaxRetries(int32_t max);
    CTaskFtpClient * GetTaskFtpClientForSize(bool renew);
    static status_t on_ftp_event(void **p);
    status_t GetFullDstPartFileName(CFileBase *out);
    status_t GetInfoFileName(CFileBase *out);
    status_t AddFileToDownload(const char *fileName, const char *md5, double version);
    status_t AddFileToDownload(CFtpFileInfo *info);
    status_t SetFtpServer(const char *serverName, int32_t port, const char *user, const char *pass);
    status_t SetFtpServer(CFtpLoginInfo *info);
    CTaskFtpDownload();
    virtual ~CTaskFtpDownload();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CTaskFtpDownload *p);
    status_t Comp(CTaskFtpDownload *p);
    status_t Print();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
    const char * ErrorToString(int32_t err);
    status_t ReportError(int32_t err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
