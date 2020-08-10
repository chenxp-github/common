#ifndef __PAXOSPREPAREREQ_H
#define __PAXOSPREPAREREQ_H

#include "cruntime.h"
#include "mem.h"

class CPaxosMessage;
class CPaxosPrepareReq{
public:
    int mIId;
    int mBallot;
public:
    status_t ToMessage(CPaxosMessage *msg);
    status_t Serialize(CFileBase *file);
    status_t Unserialize(CFileBase *file);
    status_t SetIId(int iid);
    status_t SetBallot(int ballot);
    int GetIId();
    int GetBallot();

    CPaxosPrepareReq();
    virtual ~CPaxosPrepareReq();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CPaxosPrepareReq *p);
    int Comp(CPaxosPrepareReq *p);
    status_t Print();
};

#endif
