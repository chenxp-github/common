#ifndef __FTPTHREADINFO_H
#define __FTPTHREADINFO_H

#include "partfile.h"
#include "mem.h"
#include "ftplogininfo.h"
#include "taskftpthread.h"

class CFtpThreadInfo{
public:
    TASK_CONTAINER_DEFINE();
    fsize_t offset;
    fsize_t maxSize;
    fsize_t size;
    int32_t task_ftp_thread;
public:
    bool IsComplete();
    bool IsTaskRunning();
    status_t GetUncompleteBlock(fsize_t *offset, fsize_t *size);
    status_t UpdateSize();
    fsize_t GetSize();
    CTaskFtpThread * CreateTaskFtpThread(CFileBase *dstFile,const char *fileName, fsize_t offset, fsize_t size,CFtpLoginInfo *info);
    CFtpThreadInfo();
    virtual ~CFtpThreadInfo();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CFtpThreadInfo *p);
    status_t Comp(CFtpThreadInfo *p);
    status_t Print();
    status_t InitBasic();
};

#endif
