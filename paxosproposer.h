#ifndef __PAXOSPROPOSER_H
#define __PAXOSPROPOSER_H

#include "cruntime.h"
#include "paxosmessagestk.h"
#include "paxospinstancehashmap.h"
#include "paxospreparereq.h"
#include "paxosprepareack.h"
#include "paxosacceptreq.h"
#include "paxosacceptack.h"

class CPaxosProposer{
public:
    int mIId;
    int mAcceptors;
    int mNextPrepareIId;
    
    CPaxosMessageStk *mValues;
    CPaxosPInstanceHashMap *mPrepareInstances;
    CPaxosPInstanceHashMap *mAcceptInstances;
public:
    status_t EnumAllAcceptTimeout(uint32_t now, CALLBACK_FUNC func, void **params);
    status_t EnumAllPrepareTimeout(uint32_t now, CALLBACK_FUNC func, void **params);
    status_t EnumAllTimeout(CPaxosPInstanceHashMap *map, uint32_t now, CALLBACK_FUNC func,void **params);
    status_t ReceiveAcceptAck(CPaxosAcceptAck *ack,CPaxosPrepareReq *out);
    status_t MoveInstance(CPaxosPInstanceHashMap *from, CPaxosPInstanceHashMap *to, CPaxosPInstance* inst);
    status_t Accept(CPaxosAcceptReq *out);
    status_t ReceivePrepareAck(CPaxosPrepareAck *ack,CPaxosPrepareReq *out);
    status_t Preempt(CPaxosPInstance *inst,CPaxosPrepareReq *out);
    int NextBallot(int b);
    status_t Prepare(CPaxosPrepareReq *out);
    int PreparedCount();
    status_t Propose(CMem *data);
    status_t Propose(void *data, int size);
    status_t Propose(CPaxosMessage *msg);
    status_t SetIId(int id);
    status_t SetAcceptors(int acceptors);
    status_t SetNextPrepareId(int nextprepareid);
    int GetIId();
    int GetAcceptors();
    int GetNextPrepareId();

    CPaxosProposer();
    virtual ~CPaxosProposer();
    status_t InitBasic();
    status_t Init(int acceptors);
    status_t Destroy();
    status_t Copy(CPaxosProposer *p);
    int Comp(CPaxosProposer *p);
    status_t Print();
};

#endif

