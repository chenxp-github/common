#ifndef __TASKUNZIPFILE_H
#define __TASKUNZIPFILE_H

#include "taskmgr.h"
#include "miniunzip.h"

class CTaskUnzipFile:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_WRITE_ERROR;

    void *param_finish[MAX_CALLBACK_PARAMS];
    status_t (*callback_finish)(void **p);

    CMiniUnzip *iMiniUnzip;
    CFileBase *iDstFile;
    CMem *mBuf;
public:
    CTaskUnzipFile();
    virtual ~CTaskUnzipFile();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CTaskUnzipFile *p);
    status_t Comp(CTaskUnzipFile *p);
    status_t Print();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
    const char * ErrorToString(int32_t err);
    status_t ReportError(int32_t err);  
    status_t Start();
    status_t Stop(status_t err);
    status_t SetMiniUnzip(CMiniUnzip * i_miniunzip);
    status_t SetDstFile(CFileBase * i_dstfile);
};

#endif
