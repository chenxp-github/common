#include "paxoslinstance.h"
#include "syslog.h"
#include "paxoscommon.h"

CPaxosLInstance::CPaxosLInstance()
{
    this->InitBasic();
}
CPaxosLInstance::~CPaxosLInstance()
{
    this->Destroy();
}
status_t CPaxosLInstance::InitBasic()
{
    this->mIId = 0;
    this->mLastUpdateBallot = 0;
    this->mAcks = NULL;
    this->iFinalValue = NULL;
    return OK;
}
status_t CPaxosLInstance::Init(int acceptors)
{
    this->Destroy();
    NEW(this->mAcks,CPaxosAcceptAckStk);
    this->mAcks->Init(acceptors);
    this->mAcks->mTop = acceptors;

    return OK;
}
status_t CPaxosLInstance::Destroy()
{
    DEL(this->mAcks);
    this->InitBasic();
    return OK;
}
status_t CPaxosLInstance::Copy(CPaxosLInstance *p)
{
    ASSERT(p && this != p);
    ASSERT(0);
    return OK;
}
int CPaxosLInstance::Comp(CPaxosLInstance *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosLInstance::Print()
{
    //add your code
    return OK;
}
status_t CPaxosLInstance::SetIId(int id)
{
    this->mIId = id;
    return OK;
}
status_t CPaxosLInstance::SetLastUpdateBallot(int lastupdateballot)
{
    this->mLastUpdateBallot = lastupdateballot;
    return OK;
}
int CPaxosLInstance::GetIId()
{
    return this->mIId;
}
int CPaxosLInstance::GetLastUpdateBallot()
{
    return this->mLastUpdateBallot;
}
status_t CPaxosLInstance::Update(CPaxosAcceptAck* ack,int acceptors)
{   
    if (this->GetIId() == 0) 
    {
        LOG_DEBUG("Received first message for iid: %d,ballot:%d", ack->GetIId(),ack->GetBallot());
        this->SetIId(ack->GetIId());
        this->SetLastUpdateBallot(ack->GetBallot());
    }
    
    if (this->HasQuorum(acceptors)) 
    {
        LOG_DEBUG("Dropped accept_ack iid %u. Already closed.", ack->GetIId());
        return OK;
    }

    CPaxosAcceptAck *prev_ack = this->mAcks->GetElem(ack->GetAcceptorId());
    if (prev_ack != NULL && prev_ack->GetBallot() >= ack->GetBallot())
    {
        LOG_DEBUG("Dropped accept_ack for iid %u."
            "Previous ballot is newer or equal.", ack->GetIId());
        return OK;
    }
    
    this->AddAccept(ack);
    return OK;
}

int CPaxosLInstance::HasQuorum(int acceptors)
{
    CPaxosAcceptAck* curr_ack;
    int i, a_valid_index = -1, count = 0;

    if (this->iFinalValue != NULL)
        return 1;

    for (i = 0; i < acceptors; i++) 
    {
        curr_ack = this->mAcks->GetElem(i);
        // Skip over missing acceptor acks
        if (curr_ack == NULL) continue;
        // Count the ones "agreeing" with the last added
        if (curr_ack->GetBallot() == this->GetLastUpdateBallot())
        {
            count++;
            a_valid_index = i;
            
            // Special case: an acceptor is telling that
            // this value is -final-, it can be delivered immediately.
            if (curr_ack->IsFinal()) 
            {
                count += acceptors; // For sure >= quorum...
                break;
            }
        }
    }

    if (count >= paxos_quorum(acceptors)) 
    {
        LOG_DEBUG("Reached quorum, iid: %u is closed!", this->GetIId());
        this->iFinalValue = this->mAcks->GetElem(a_valid_index);
        return 1;
    }
    return 0;
}

status_t CPaxosLInstance::AddAccept(CPaxosAcceptAck* ack)
{
    this->mAcks->SetElem(ack->GetAcceptorId(),ack);
    this->SetLastUpdateBallot(ack->GetBallot());
    return OK;
}


