#include "paxospreparereq.h"
#include "syslog.h"
#include "paxosmessage.h"

CPaxosPrepareReq::CPaxosPrepareReq()
{
    this->InitBasic();
}
CPaxosPrepareReq::~CPaxosPrepareReq()
{
    this->Destroy();
}
status_t CPaxosPrepareReq::InitBasic()
{
    this->mIId = 0;
    this->mBallot = 0;
    
    return OK;
}
status_t CPaxosPrepareReq::Init()
{
    this->Destroy();
    //add your code
    return OK;
}
status_t CPaxosPrepareReq::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
status_t CPaxosPrepareReq::Copy(CPaxosPrepareReq *p)
{
    ASSERT(p && this != p);
    this->mIId = p->mIId;
    this->mBallot = p->mBallot;
    return OK;
}
int CPaxosPrepareReq::Comp(CPaxosPrepareReq *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosPrepareReq::Print()
{
    char _str_mid[64];
    sprintf(_str_mid,"%d",mIId);
    LOG("Id = %s",_str_mid);
    char _str_mballot[64];
    sprintf(_str_mballot,"%d",mBallot);
    LOG("Ballot = %s",_str_mballot);
    return OK;
    return OK;
}
status_t CPaxosPrepareReq::SetIId(int iid)
{
    this->mIId = iid;
    return OK;
}
status_t CPaxosPrepareReq::SetBallot(int ballot)
{
    this->mBallot = ballot;
    return OK;
}
int CPaxosPrepareReq::GetIId()
{
    return this->mIId;
}
int CPaxosPrepareReq::GetBallot()
{
    return this->mBallot;
}

status_t CPaxosPrepareReq::Serialize(CFileBase *file)
{
    file->Write(&this->mBallot,sizeof(this->mBallot));
    file->Write(&this->mIId,sizeof(this->mIId));
    return OK;
}
status_t CPaxosPrepareReq::Unserialize(CFileBase *file)
{
    file->Read(&this->mBallot,sizeof(this->mBallot));
    file->Read(&this->mIId,sizeof(this->mIId));
    return OK;
}
status_t CPaxosPrepareReq::ToMessage(CPaxosMessage *msg)
{
    ASSERT(msg);
    LOCAL_MEM(mem);
    this->Serialize(&mem);
    msg->SetType(PREPARE_REQS);
    msg->SetData(&mem);
    return OK;
}

