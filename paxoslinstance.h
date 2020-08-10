#ifndef __PAXOSLINSTANCE_H
#define __PAXOSLINSTANCE_H

#include "cruntime.h"
#include "paxosacceptackstk.h"

class CPaxosLInstance{
public:
    int mIId;
    int mLastUpdateBallot;
    CPaxosAcceptAckStk *mAcks;
    CPaxosAcceptAck *iFinalValue;

    CPaxosLInstance *next;
public:
    status_t AddAccept(CPaxosAcceptAck* ack);
    int HasQuorum(int acceptors);
    status_t Update(CPaxosAcceptAck* ack,int acceptors);
    status_t SetIId(int id);
    status_t SetLastUpdateBallot(int lastupdateballot);
    int GetIId();
    int GetLastUpdateBallot();
    
    CPaxosLInstance();
    virtual ~CPaxosLInstance();
    status_t InitBasic();
    status_t Init(int acceptors);
    status_t Destroy();
    status_t Copy(CPaxosLInstance *p);
    int Comp(CPaxosLInstance *p);
    status_t Print();
};

#endif
