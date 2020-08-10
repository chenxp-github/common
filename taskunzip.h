#ifndef __TASKUNZIP_H
#define __TASKUNZIP_H

#include "taskmgr.h"
#include "taskunzipfile.h"
#include "file.h"

class CTaskUnzip:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_CREATE_DIR_FAIL;
    static status_t ERROR_OPEN_FILE_ERROR;
    static status_t ERROR_UNZIP_ERROR;

    void *param_finish[MAX_CALLBACK_PARAMS];
    status_t (*callback_finish)(void **p);

    CMiniUnzip *mUnzip;
    CFileBase *iZipFile;
    int32_t mTaskUnzipFile;
    CFile *mTmpFile;
    CMem *mPath;
    int32_t mStep;
    CFile *mInnerZipFile;
public:
    status_t SetZipFile(const char *zipFile);
    static status_t on_unzip_file_finish(void **p);
    status_t SetDstPath(const char *path);
    status_t SetZipFile(CFileBase *iFile);
    CTaskUnzipFile * GetTaskUnzipFile(bool renew);
    CTaskUnzip();
    virtual ~CTaskUnzip();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CTaskUnzip *p);
    status_t Comp(CTaskUnzip *p);
    status_t Print();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
    const char * ErrorToString(int32_t err);
    status_t ReportError(int32_t err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
