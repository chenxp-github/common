#ifndef __TASKPEERSERVER_H
#define __TASKPEERSERVER_H

#include "cruntime.h"
#include "tasklinkrpc.h"
#include "peerproxy.h"
#include "peercommon.h"

class CTaskPeerServer:public CTaskLinkRpc{
public:
    PEER_GLOBAL_CONTEXT_DEFINE();
    CMem *mRecvDataBuf;
    CMem *mRecvHeadBuf;
    CMem *mSendHeadBuf;
    CWeakPointer<CPeerProxy> iHostProxy;
    bool quit_after_send;
public:
    status_t QuitAfterSend();
    CPeerMessageStk * GetSendingQueue();
    CTaskPeerServer();
    virtual ~CTaskPeerServer();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr, const void *peer_globals);
    status_t Destroy();
	status_t OnInitNameMessage(CPeerMessage *msg);

    //connection
    status_t OnGetSocket();
    virtual status_t OnSocketError();

    //reading package
    bool CanReadNextPackage();
    status_t OnGotPackageHeader(LINKRPC_HEADER *header,CMem *header_data);
    status_t OnGotPackageData(LINKRPC_HEADER *header,CMem *header_data,CFileBase *data);

    //writing package
    status_t OnPreparePackageToSend();
    status_t OnPackageSendOk();
};

#endif
