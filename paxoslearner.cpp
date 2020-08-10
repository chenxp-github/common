#include "paxoslearner.h"
#include "paxoscommon.h"

CPaxosLearner::CPaxosLearner()
{
    this->InitBasic();
}
CPaxosLearner::~CPaxosLearner()
{
    this->Destroy();
}
status_t CPaxosLearner::InitBasic()
{
    this->mAcceptors = 0;
    this->mLateStart = 0;
    this->mCurrentIId = 0;
    this->mHighestIIdClosed = 0;
    this->mInstances = NULL;
    return OK;
}
status_t CPaxosLearner::Init(int acceptors)
{
    this->InitBasic();

    this->mAcceptors = acceptors;
    this->mCurrentIId = 1;
    this->mHighestIIdClosed = 1;
    this->mLateStart = !paxos_config.learner_catch_up;

    NEW(this->mInstances,CPaxosLInstanceHashMap);
    this->mInstances->Init(128);

    return OK;
}
status_t CPaxosLearner::Destroy()
{
    DEL(this->mInstances);
    this->InitBasic();
    return OK;
}
status_t CPaxosLearner::Copy(CPaxosLearner *p)
{
    ASSERT(p && this != p);
    ASSERT(0);
    return OK;
}
int CPaxosLearner::Comp(CPaxosLearner *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosLearner::Print()
{
    char _str_macceptors[64];
    sprintf(_str_macceptors,"%d",mAcceptors);
    LOG("Acceptors = %s",_str_macceptors);
    char _str_mlatestart[64];
    sprintf(_str_mlatestart,"%d",mLateStart);
    LOG("LateStart = %s",_str_mlatestart);
    char _str_mcurrentid[64];
    sprintf(_str_mcurrentid,"%d",mCurrentIId);
    LOG("CurrentId = %s",_str_mcurrentid);
    char _str_mhighestidclosed[64];
    sprintf(_str_mhighestidclosed,"%d",mHighestIIdClosed);
    LOG("HighestIdClosed = %s",_str_mhighestidclosed);
    return OK;
}
status_t CPaxosLearner::SetAcceptors(int acceptors)
{
    this->mAcceptors = acceptors;
    return OK;
}
status_t CPaxosLearner::SetLateStart(int latestart)
{
    this->mLateStart = latestart;
    return OK;
}
status_t CPaxosLearner::SetCurrentIId(int currentid)
{
    this->mCurrentIId = currentid;
    return OK;
}
status_t CPaxosLearner::SetHighestIIdClosed(int highestidclosed)
{
    this->mHighestIIdClosed = highestidclosed;
    return OK;
}
int CPaxosLearner::GetAcceptors()
{
    return this->mAcceptors;
}
int CPaxosLearner::GetLateStart()
{
    return this->mLateStart;
}
int CPaxosLearner::GetCurrentIId()
{
    return this->mCurrentIId;
}
int CPaxosLearner::GetHighestIIdClosed()
{
    return this->mHighestIIdClosed;
}

status_t CPaxosLearner::ReceiveAccept(CPaxosAcceptAck* ack)
{   
    ASSERT(ack);

    if (this->GetLateStart()) 
    {
        this->SetLateStart(0);
        this->SetCurrentIId(ack->GetIId());
    }
    
    if (ack->GetIId() < this->GetCurrentIId())
    {
        LOG_DEBUG("Dropped accept_ack for iid %u. Already delivered.",ack->GetIId());
        return ERROR;
    }
    
    CPaxosLInstance *inst = this->GetInstanceOrCreate(ack->GetIId());
    ASSERT(inst);
    inst->Update(ack,this->GetAcceptors());

    if(inst->HasQuorum(this->GetAcceptors()) && (inst->GetIId() > this->GetHighestIIdClosed()))
    {
        this->SetHighestIIdClosed(inst->GetIId());
    }
    return OK;
}

CPaxosLInstance* CPaxosLearner::GetInstance(int id)
{
    return this->mInstances->Get(id);
}

CPaxosLInstance* CPaxosLearner::GetCureentInstance()
{
    return this->GetInstance(this->GetCurrentIId());
}

CPaxosLInstance* CPaxosLearner::GetInstanceOrCreate(int id)
{
    CPaxosLInstance *inst = this->GetInstance(id);
    if(inst == NULL)
    {
        NEW(inst,CPaxosLInstance);
        inst->Init(this->GetAcceptors());
        inst->SetIId(id);
        this->mInstances->PutPtr(inst);
    }
    return inst;
}

status_t CPaxosLearner::DeleteInstance(CPaxosLInstance *inst)
{
    return this->mInstances->Del(inst);
}

int CPaxosLearner::HasHole(int *from, int *to)
{
    ASSERT(from && to);

    if(this->GetHighestIIdClosed() > this->GetCurrentIId())
    {
        *from = this->GetCurrentIId();
        *to = this->GetHighestIIdClosed();
        return 1;
    }

    return 0;
}

CPaxosAcceptAck * CPaxosLearner::DeliverNext()
{
    CPaxosLInstance* inst = this->GetCureentInstance();
    if (inst == NULL)
        return NULL;
    if (inst->HasQuorum(this->GetAcceptors())) 
    {
        CPaxosAcceptAck* ack;
        NEW(ack,CPaxosAcceptAck);
        ack->Init();
        ASSERT(inst->iFinalValue);
        ack->Copy(inst->iFinalValue);
        this->DeleteInstance(inst);
        this->mCurrentIId ++;
        return ack;
    }
    return NULL;
}
