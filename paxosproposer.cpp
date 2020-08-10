#include "paxosproposer.h"
#include "syslog.h"

CPaxosProposer::CPaxosProposer()
{
    this->InitBasic();
}
CPaxosProposer::~CPaxosProposer()
{
    this->Destroy();
}
status_t CPaxosProposer::InitBasic()
{
    this->mIId = 0;
    this->mAcceptors = 0;
    this->mNextPrepareIId = 0;

    this->mAcceptInstances = NULL;
    this->mPrepareInstances = NULL;
    this->mValues = NULL;

    return OK;
}
status_t CPaxosProposer::Init(int acceptors)
{
    this->Destroy();
    
    this->mAcceptors = acceptors;

    NEW(this->mAcceptInstances,CPaxosPInstanceHashMap);
    this->mAcceptInstances->Init(128);

    NEW(this->mPrepareInstances,CPaxosPInstanceHashMap);
    this->mPrepareInstances->Init(128);

    NEW(this->mValues,CPaxosMessageStk);
    this->mValues->Init(128);

    return OK;
}
status_t CPaxosProposer::Destroy()
{
    DEL(this->mAcceptInstances);
    DEL(this->mPrepareInstances);
    DEL(this->mValues);

    this->InitBasic();
    return OK;
}
status_t CPaxosProposer::Copy(CPaxosProposer *p)
{
    ASSERT(p && this != p);
    //add your code
    return OK;
}
int CPaxosProposer::Comp(CPaxosProposer *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosProposer::Print()
{
    //add your code
    return OK;
}
status_t CPaxosProposer::SetIId(int iid)
{
    this->mIId = iid;
    return OK;
}
status_t CPaxosProposer::SetAcceptors(int acceptors)
{
    this->mAcceptors = acceptors;
    return OK;
}
status_t CPaxosProposer::SetNextPrepareId(int nextprepareid)
{
    this->mNextPrepareIId = nextprepareid;
    return OK;
}
int CPaxosProposer::GetIId()
{
    return this->mIId;
}
int CPaxosProposer::GetAcceptors()
{
    return this->mAcceptors;
}
int CPaxosProposer::GetNextPrepareId()
{
    return this->mNextPrepareIId;
}

status_t CPaxosProposer::Propose(CPaxosMessage *msg)
{
    ASSERT(msg);

    CPaxosMessage *tmp;
    NEW(tmp,CPaxosMessage);
    tmp->Init();
    tmp->Transfer(msg);

    return this->mValues->PushPtr(tmp);
}

status_t CPaxosProposer::Propose(void *data, int size)
{
    ASSERT(data);
    CPaxosMessage tmp;
    tmp.Init();
    tmp.SetData(data,size);
    return this->Propose(&tmp);
}

status_t CPaxosProposer::Propose(CMem *data)
{
    ASSERT(data);
    CPaxosMessage tmp;
    tmp.Init();
    tmp.SetData(data);
    return this->Propose(&tmp);
}

int CPaxosProposer::PreparedCount()
{
    return this->mPrepareInstances->GetSize();
}

status_t CPaxosProposer::Prepare(CPaxosPrepareReq *out)
{
    ASSERT(out);

    this->mNextPrepareIId++;
    int iid = this->mNextPrepareIId;
    int bal = this->NextBallot(0);

    CPaxosPInstance *inst;
    NEW(inst,CPaxosPInstance);
    inst->Init(this->mAcceptors);
    inst->SetIId(iid);
    inst->SetBallot(bal);

    status_t rv = this->mPrepareInstances->PutPtr(inst);
    ASSERT(rv);

    out->SetIId(iid);
    out->SetBallot(bal);

    return OK;
}

int CPaxosProposer::NextBallot(int b)
{
    if (b > 0)
        return MAX_N_OF_PROPOSERS + b;
    else
        return MAX_N_OF_PROPOSERS + this->mIId;
}

status_t CPaxosProposer::Preempt(CPaxosPInstance *inst, CPaxosPrepareReq *out)
{
    ASSERT(inst && out);

    inst->SetBallot(this->NextBallot(inst->GetBallot()));
    inst->SetValueBallot(0);
    CPaxosQuorum *quorum = inst->GetQuorum();
    ASSERT(quorum);
    quorum->Clear();
    out->SetIId(inst->GetIId());
    out->SetBallot(inst->GetBallot());
    inst->SetCreateTime(crt_get_sys_timer());
    return OK;
}

status_t CPaxosProposer::ReceivePrepareAck(CPaxosPrepareAck *ack ,CPaxosPrepareReq *out)
{
    ASSERT(ack && out);

    CPaxosPInstance *inst = this->mPrepareInstances->Get(ack->GetIId());
    if(inst == NULL)
    {
        LOG_DEBUG("Promise dropped, instance %d not pending", ack->GetIId());
        return ERROR;
    }

    if (ack->GetBallot() < inst->GetBallot()) 
    {
        LOG_DEBUG("Promise dropped, too old");
        return ERROR;
    }

    if (ack->GetBallot() > inst->GetBallot())
    {
        LOG_DEBUG("Instance %d preempted: ballot %d ack ballot %d",
            inst->GetIId(), inst->GetBallot(), ack->GetBallot());
        this->Preempt(inst, out);
        return OK;
    }
    
    ASSERT(inst->GetQuorum());
    if (!inst->GetQuorum()->Add(ack->GetAcceptorId()))
    {
        LOG_DEBUG("Duplicate promise dropped from: %d, iid: %d",
            ack->GetAcceptorId(), inst->GetIId());
        return 0;
    }

    LOG_DEBUG("Received valid promise from: %d, iid: %d",
        ack->GetAcceptorId(), inst->GetIId());

    if (ack->HasValue())
    {
        LOG_DEBUG("Promise has value");
        if (!inst->HasValue()) 
        {
            inst->SetValueBallot(ack->GetValueBallot());
            inst->SetValue(ack->GetValue());
        }
        else if (ack->GetValueBallot() > inst->GetValueBallot()) 
        {
            inst->ClearValue();
            inst->SetValueBallot(ack->GetValueBallot());
            inst->SetValue(ack->GetValue());
            LOG_DEBUG("Value in promise saved, removed older value");
        } else
        {
            LOG_DEBUG("Value in promise ignored");
        }
    }

    return ERROR;
}

status_t CPaxosProposer::Accept(CPaxosAcceptReq *out)
{   
    ASSERT(out);

    CPaxosPInstance *inst = this->mPrepareInstances->FindSmallestId();
    if(inst == NULL || (!inst->GetQuorum()->Reached()))
        return ERROR;

    if(!inst->HasValue())
    {
        CPaxosMessage *front = this->mValues->GetElem(0);
        if(front != NULL)
        {
            inst->TransferValue(front);
            this->mValues->DelElem(0);
        }
    }
    if (!inst->HasValue()) 
    {
        LOG_DEBUG("No value to accept for iid %d",inst->GetIId());
        return ERROR;
    }

    LOG_DEBUG("Trying to accept iid %u", inst->GetIId());

    this->MoveInstance(this->mPrepareInstances,this->mAcceptInstances,inst);
    inst->ToAcceptReq(out);
    return OK;
}

status_t CPaxosProposer::MoveInstance(CPaxosPInstanceHashMap *from, CPaxosPInstanceHashMap *to, CPaxosPInstance *inst)
{
    ASSERT(from && to && inst);
    ASSERT(from->Remove(inst->GetIId()));
    inst->ClearQuorum();
    ASSERT(to->PutPtr(inst));
    return OK;
}

status_t CPaxosProposer::ReceiveAcceptAck(CPaxosAcceptAck *ack,CPaxosPrepareReq* out)
{
    ASSERT(ack && out);

    CPaxosPInstance *inst = this->mAcceptInstances->Get(ack->GetIId());
    if(inst == NULL)
    {
        LOG_DEBUG("Accept ack dropped, iid: %d not pending", ack->GetIId());
        return ERROR;
    }

    if(ack->GetBallot() == inst->GetBallot())
    {
        ASSERT(ack->GetValueBallot() == inst->GetBallot());
        if(!inst->GetQuorum()->Add(ack->GetAcceptorId()))
        {
            LOG_DEBUG("Duplicate accept dropped from: %d, iid: %u", 
                ack->GetAcceptorId(), inst->GetIId());
            return ERROR;
        }

        if(inst->GetQuorum()->Reached())
        {
            LOG_DEBUG("Quorum reached for instance %d", inst->GetIId());
            this->mAcceptInstances->Del(inst);
        }

        return ERROR;
    }
    else
    {
        LOG_DEBUG("Instance %d preempted: ballot %d ack ballot %d",
            inst->GetIId(), inst->GetBallot(), ack->GetBallot());
        
        if(inst->GetValueBallot() == 0)
            this->mValues->PushTransfer(inst->GetValue());
        inst->ClearValue();

        this->MoveInstance(this->mAcceptInstances,this->mPrepareInstances,inst);
        this->Preempt(inst,out);
        return OK;
    }
}

static status_t check_timedout(void **p)
{
    CPaxosPInstance *inst = (CPaxosPInstance*)p[0];
    ASSERT(inst);
    CALLBACK_FUNC func = (CALLBACK_FUNC)p[5];
    ASSERT(func);
    uint32_t now = (uint32_t)p[6];
    if(!inst->GetQuorum()->Reached())
    {
        if(inst->HasTimedOut(now))
        {
            func(p);
        }
    }
    return OK;
}
status_t CPaxosProposer::EnumAllTimeout(CPaxosPInstanceHashMap *map, uint32_t now, CALLBACK_FUNC func, void **params)
{
    ASSERT(map && params);
    params[5] = func;
    params[6] = (void*)now;
    map->EnumAll(check_timedout,params);
    return OK;
}

status_t CPaxosProposer::EnumAllPrepareTimeout(uint32_t now, CALLBACK_FUNC func, void **params)
{
    return this->EnumAllTimeout(this->mPrepareInstances,now,func,params);
}
status_t CPaxosProposer::EnumAllAcceptTimeout(uint32_t now, CALLBACK_FUNC func, void **params)
{
    return this->EnumAllTimeout(this->mAcceptInstances,now,func,params);
}
