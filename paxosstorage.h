#ifndef __PAXOSSTORAGE_H
#define __PAXOSSTORAGE_H

#include "cruntime.h"
#include "paxosacceptreq.h"
#include "paxosacceptack.h"
#include "paxospreparereq.h"

class CPaxosStorage{
public:
public:
    virtual CPaxosAcceptorRecord* SaveFinalValue(void *value, int size, int id, int ballot) = 0;
    virtual CPaxosAcceptorRecord* SavePrepare(CPaxosPrepareReq *pr, CPaxosAcceptorRecord *rec) = 0;
    virtual CPaxosAcceptorRecord* SaveAccept(CPaxosAcceptReq *ar) = 0;
    virtual CPaxosAcceptorRecord* GetRecord(int iid) = 0;

    CPaxosStorage();
    virtual ~CPaxosStorage();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
};

#endif
