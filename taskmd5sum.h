#ifndef __TASKMD5SUM_H
#define __TASKMD5SUM_H

#include "taskmgr.h"
#include "filebase.h"
#include "mem.h"

class CTaskMd5Sum:public CTask{
public:
    static status_t ERROR_NONE;
    
    static int EVENT_FINISH;

    CFileBase *iSrcFile;
    CMem *mCache;
    uint32_t A,B,C,D;
    uint32_t buf[16];
    int32_t mStep;
public:
    status_t GetStringResult(CFileBase *out);
    status_t SetSrcFile(CFileBase *iFile);
    CTaskMd5Sum();
    virtual ~CTaskMd5Sum();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CTaskMd5Sum *p);
    status_t Comp(CTaskMd5Sum *p);
    status_t Print();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
    const char * ErrorToString(int32_t err);
    status_t ReportError(int32_t err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
