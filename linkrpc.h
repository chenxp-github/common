#ifndef __LINKRPC_H
#define __LINKRPC_H

#include "common.h"
#include "tasklinkrpcreader.h"
#include "tasklinkrpcwriter.h"
#include "taskmgr.h"


class CLinkRpc{
public:
    TASK_CONTAINER_DEFINE();

    CSocket *mSocket;
    int mTaskReader;
    int mTaskWriter;
    CTaskLinkRpcReader *mReader;
    CTaskLinkRpcWriter *mWriter;
    int mTimeout;
    int_ptr_t mRequestCountL;
    int_ptr_t mResponseCountL;
    int_ptr_t mRequestCountR;
    int_ptr_t mResponseCountR;
public:
    status_t SendRequest(CFileBase *header, CFileBase *data);
    bool IsAlive();
    status_t Stop();
    status_t SetTimeout(int to);
    status_t Start();
    status_t TransferSocket(CSocket *from);
    status_t SetSocket(int32_t fd);
    status_t SetDataBuf(CFileBase *iBuf);
    status_t SetHeaderBuf(CFileBase *iBuf);
    status_t StartSending();
    virtual status_t OnGetNextPackage();
    virtual status_t OnGotResponseData(CFileBase *header,CFileBase *data);
    virtual status_t OnGotRequestData(CFileBase *header,CFileBase *data);
    virtual status_t OnGotResponseHeader(CFileBase *header);
    virtual status_t OnGotRequestHeader(CFileBase *header);
    virtual status_t OnStop();
    status_t CreateWriter();
    status_t CreateReader();
    CLinkRpc();
    virtual ~CLinkRpc();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
};

#endif
