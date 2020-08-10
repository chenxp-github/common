#ifndef __PAXOSLEARNER_H
#define __PAXOSLEARNER_H

#include "paxoslinstance.h"
#include "paxoslinstancehashmap.h"

class CPaxosLearner{
public:
    int mAcceptors;
    int mLateStart;
    int mCurrentIId;
    int mHighestIIdClosed;
    CPaxosLInstanceHashMap *mInstances;
public:
    CPaxosAcceptAck * DeliverNext();
    int HasHole(int *from, int *to);
    status_t DeleteInstance(CPaxosLInstance *inst);
    CPaxosLInstance* GetInstanceOrCreate(int id);
    CPaxosLInstance* GetCureentInstance();
    CPaxosLInstance* GetInstance(int id);
    status_t ReceiveAccept(CPaxosAcceptAck* ack);
    status_t SetAcceptors(int acceptors);
    status_t SetLateStart(int latestart);
    status_t SetCurrentIId(int currentiid);
    status_t SetHighestIIdClosed(int highestidclosed);
    int GetAcceptors();
    int GetLateStart();
    int GetCurrentIId();
    int GetHighestIIdClosed();
    CPaxosLearner();
    virtual ~CPaxosLearner();
    status_t InitBasic();
    status_t Init(int acceptors);
    status_t Destroy();
    status_t Copy(CPaxosLearner *p);
    int Comp(CPaxosLearner *p);
    status_t Print();
};

#endif

