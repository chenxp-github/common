#ifndef __PAXOSPINSTANCE_H
#define __PAXOSPINSTANCE_H

#include "cruntime.h"
#include "paxosquorum.h"
#include "paxosmessage.h"
#include "paxosacceptreq.h"

class CPaxosPInstance{
public:
    int32_t mIId;
    int32_t mBallot;
    int32_t mValueBallot;
    CPaxosMessage* mValue;
    CPaxosQuorum *mQuorum;
    uint32_t mCreateTime;

    CPaxosPInstance *next;
public:
    bool HasTimedOut(uint32_t now);
    status_t ToAcceptReq(CPaxosAcceptReq *out);
    status_t ClearQuorum();
    status_t TransferValue(CPaxosMessage *value);
    status_t ClearValue();
    status_t SetValue(CMem *val);
    bool HasValue();
    CPaxosPInstance();
    virtual ~CPaxosPInstance();
    status_t InitBasic();
    status_t Init(int acceptors);
    status_t Destroy();
    status_t Copy(CPaxosPInstance *p);
    int Comp(CPaxosPInstance *p);
    status_t Print();

    status_t SetIId(uint32_t id);
    status_t SetBallot(uint32_t ballot);
    status_t SetValueBallot(uint32_t valueballot);
    status_t SetCreateTime(uint32_t createtime);
    status_t SetValue(CPaxosMessage *value);
    status_t SetQuorum(CPaxosQuorum *quorum);
    int32_t GetIId();
    int32_t GetBallot();
    int32_t GetValueBallot();
    int32_t GetCreateTime();
    CPaxosMessage* GetValue();
    CPaxosQuorum* GetQuorum();

};

#endif
