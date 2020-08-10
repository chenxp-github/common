#include "paxosacceptor.h"
#include "syslog.h"

CPaxosAcceptor::CPaxosAcceptor()
{
    this->InitBasic();
}
CPaxosAcceptor::~CPaxosAcceptor()
{
    this->Destroy();
}
status_t CPaxosAcceptor::InitBasic()
{
    this->iStore = NULL;
    return OK;
}
status_t CPaxosAcceptor::Init()
{
    this->Destroy();
    return OK;
}
status_t CPaxosAcceptor::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}

CPaxosAcceptorRecord* CPaxosAcceptor::ReceivePrepare(CPaxosPrepareReq *req)
{
    ASSERT(req && iStore);
    CPaxosAcceptorRecord *rec;
    rec = this->iStore->GetRecord(req->GetIId());
    rec = this->ApplyPrepare(req,rec);
    return rec;
}

CPaxosAcceptorRecord* CPaxosAcceptor::ApplyPrepare(CPaxosPrepareReq *pr, CPaxosAcceptorRecord *rec)
{
    ASSERT(this->iStore && pr);

    // We already have a more recent ballot
    if (rec != NULL && rec->GetBallot() >= pr->GetBallot()) 
    {
        LOG_DEBUG("Prepare iid: %u dropped (ballots curr:%u recv:%u)",
            pr->GetIId(), rec->GetBallot(), pr->GetBallot());
        return rec;
    }

    // Stored value is final, the instance is closed already
    if (rec != NULL && rec->IsFinal()) 
    {
        LOG_DEBUG("Prepare request for iid: %u dropped \
            (stored value is final)", pr->GetIId());
        return rec;
    }
    
    // Record not found or smaller ballot, in both cases overwrite and store
    LOG_DEBUG("Preparing iid: %u, ballot: %u", pr->GetIId(), pr->GetBallot());
    
    return this->iStore->SavePrepare(pr,rec);
}

CPaxosAcceptorRecord* CPaxosAcceptor::ReceiveAccept(CPaxosAcceptReq* req)
{
    ASSERT(req && iStore);

    CPaxosAcceptorRecord* rec;
    rec = this->iStore->GetRecord(req->GetIId());
    rec = this->ApplyAccept(req,rec);
    return rec;
}

CPaxosAcceptorRecord* CPaxosAcceptor::ApplyAccept(CPaxosAcceptReq* ar, CPaxosAcceptorRecord* rec)
{
    ASSERT(ar && rec && iStore);
    // We already have a more recent ballot
    if (rec != NULL && rec->GetBallot() > ar->GetBallot()) 
    {
        LOG_DEBUG("Accept for iid:%u dropped (ballots curr:%u recv:%u)",ar->GetIId(), rec->GetBallot(), ar->GetBallot());
        return rec;
    }
    
    // Record not found or smaller ballot, in both cases overwrite and store
    LOG_DEBUG("Accepting iid: %u, ballot: %u", ar->GetIId(), ar->GetBallot());
    
    // Store the updated record
    return this->iStore->SaveAccept(ar);
}

CPaxosAcceptorRecord* CPaxosAcceptor::ReceiveRepeat(int iid)
{
    CPaxosAcceptorRecord* rec;
    rec = this->iStore->GetRecord(iid);
    return rec;
}

status_t CPaxosAcceptor::SetStorage(CPaxosStorage *storage)
{
    this->iStore = storage;
    return OK;
}
