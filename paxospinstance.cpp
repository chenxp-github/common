#include "paxospinstance.h"
#include "syslog.h"

CPaxosPInstance::CPaxosPInstance()
{
    this->InitBasic();
}
CPaxosPInstance::~CPaxosPInstance()
{
    this->Destroy();
}
status_t CPaxosPInstance::InitBasic()
{
    this->mIId = 0;
    this->mBallot = 0;
    this->mValueBallot = 0;
    this->mCreateTime = 0;
    this->mValue = NULL;
    this->mQuorum = NULL;
    
    return OK;
}
status_t CPaxosPInstance::Init(int acceptors)
{
    this->Destroy();

    NEW(this->mValue,CPaxosMessage);
    this->mValue->Init();

    NEW(this->mQuorum,CPaxosQuorum);
    this->mQuorum->Init(acceptors);

    this->mCreateTime = crt_get_sys_timer();
    return OK;
}
status_t CPaxosPInstance::Destroy()
{
    DEL(this->mValue);
    DEL(this->mQuorum);
    this->InitBasic();
    return OK;
}
status_t CPaxosPInstance::Copy(CPaxosPInstance *p)
{
    ASSERT(p && this != p);
    this->mIId = p->mIId;
    this->mBallot = p->mBallot;
    this->mValueBallot = p->mValueBallot;
    this->mCreateTime = p->mCreateTime;
    this->mValue->Copy(p->mValue);
    this->mQuorum->Copy(p->mQuorum);
    return OK;
}
int CPaxosPInstance::Comp(CPaxosPInstance *p)
{
    ASSERT(p);
    return this->mIId - p->mIId;
}
status_t CPaxosPInstance::Print()
{
    char _str_mid[64];
    sprintf(_str_mid,"%d",mIId);
    LOG("Id = %s",_str_mid);
    char _str_mballot[64];
    sprintf(_str_mballot,"%d",mBallot);
    LOG("Ballot = %s",_str_mballot);
    char _str_mvalueballot[64];
    sprintf(_str_mvalueballot,"%d",mValueBallot);
    LOG("ValueBallot = %s",_str_mvalueballot);
    char _str_mcreatetime[64];
    sprintf(_str_mcreatetime,"%d",mCreateTime);
    LOG("CreateTime = %s",_str_mcreatetime);
    LOG("Value={");
    this->mValue->Print();
    LOG("}");
    LOG("Quorum={");
    this->mQuorum->Print();
    LOG("}");
    return OK;
}
status_t CPaxosPInstance::SetIId(uint32_t id)
{
    this->mIId = id;
    return OK;
}
status_t CPaxosPInstance::SetBallot(uint32_t ballot)
{
    this->mBallot = ballot;
    return OK;
}
status_t CPaxosPInstance::SetValueBallot(uint32_t valueballot)
{
    this->mValueBallot = valueballot;
    return OK;
}
status_t CPaxosPInstance::SetCreateTime(uint32_t createtime)
{
    this->mCreateTime = createtime;
    return OK;
}
status_t CPaxosPInstance::SetValue(CPaxosMessage *value)
{
    ASSERT(value);
    this->mValue->Copy(value);
    return OK;
}
status_t CPaxosPInstance::SetQuorum(CPaxosQuorum *quorum)
{
    ASSERT(quorum);
    this->mQuorum->Copy(quorum);
    return OK;
}
int32_t CPaxosPInstance::GetIId()
{
    return this->mIId;
}
int32_t CPaxosPInstance::GetBallot()
{
    return this->mBallot;
}
int32_t CPaxosPInstance::GetValueBallot()
{
    return this->mValueBallot;
}
int32_t CPaxosPInstance::GetCreateTime()
{
    return this->mCreateTime;
}
CPaxosMessage* CPaxosPInstance::GetValue()
{
    return this->mValue;
}
CPaxosQuorum* CPaxosPInstance::GetQuorum()
{
    return this->mQuorum;
}

bool CPaxosPInstance::HasValue()
{
    return this->mValue->HasValue();
}

status_t CPaxosPInstance::SetValue(CMem *val)
{
    ASSERT(val);
    this->mValue->SetType(SUBMIT);
    this->mValue->SetData(val);
    return OK;
}

status_t CPaxosPInstance::ClearValue()
{
    return this->mValue->ClearData();
}

status_t CPaxosPInstance::TransferValue(CPaxosMessage *value)
{
    return this->mValue->Transfer(value);
}

status_t CPaxosPInstance::ClearQuorum()
{
    return this->mQuorum->Clear();
}

status_t CPaxosPInstance::ToAcceptReq(CPaxosAcceptReq *out)
{
    out->SetIId(this->GetIId());
    out->SetBallot(this->GetBallot());
    out->SetValue(this->mValue->GetData());
    return OK;
}

bool CPaxosPInstance::HasTimedOut(uint32_t now)
{
    uint32_t diff = now - this->mCreateTime;
    return diff >= paxos_config.proposer_timeout;
}
