#ifndef __TASKCREATEBIGFILE_H
#define __TASKCREATEBIGFILE_H

#include "mem.h"
#include "taskmgr.h"
#include "file.h"

class CTaskCreateBigFile:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_OPEN_FILE_ERROR;
    static status_t ERROR_ERROR_SIZE;
    
    static int EVENT_FINISH;

    CFile *mFile;
    fsize_t mMaxSize;
    CMem *mFileName;
    CMem *mBuf;
    int32_t mStep;
public:
    status_t SetFileName(const char *fileName, fsize_t size);
    CTaskCreateBigFile();
    virtual ~CTaskCreateBigFile();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CTaskCreateBigFile *p);
    status_t Comp(CTaskCreateBigFile *p);
    status_t Print();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
    const char * ErrorToString(int32_t err);
    status_t ReportError(int32_t err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
