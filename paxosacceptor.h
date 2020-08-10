#ifndef __PAXOSACCEPTOR_H
#define __PAXOSACCEPTOR_H

#include "paxoscommon.h"
#include "paxosstorage.h"

class CPaxosAcceptor{
public:
    CPaxosStorage *iStore;
public:
    status_t SetStorage(CPaxosStorage *storage);
    CPaxosAcceptorRecord* ReceiveRepeat(int iid);
    CPaxosAcceptorRecord* ApplyAccept(CPaxosAcceptReq* ar, CPaxosAcceptorRecord* rec);
    CPaxosAcceptorRecord* ReceiveAccept(CPaxosAcceptReq* req);
    CPaxosAcceptorRecord* ApplyPrepare(CPaxosPrepareReq *pr, CPaxosAcceptorRecord *rec);
    CPaxosAcceptorRecord* ReceivePrepare(CPaxosPrepareReq *req);
    CPaxosAcceptor();
    virtual ~CPaxosAcceptor();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
};

#endif
