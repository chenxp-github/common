#include "memfile.h"
#include "paxosmessage.h"
#include "paxosacceptreq.h"

CPaxosAcceptReq::CPaxosAcceptReq()
{
    this->InitBasic();
}
CPaxosAcceptReq::~CPaxosAcceptReq()
{
    this->Destroy();
}
status_t CPaxosAcceptReq::InitBasic()
{
    this->mIId = 0;
    this->mBallot = 0;
    this->mValue = NULL;
    return OK;
}
status_t CPaxosAcceptReq::Init()
{
    this->InitBasic();
    NEW(this->mValue,CMem);
    this->mValue->Init();

    return OK;
}
status_t CPaxosAcceptReq::Destroy()
{
    DEL(this->mValue);
    this->InitBasic();
    return OK;
}
status_t CPaxosAcceptReq::Copy(CPaxosAcceptReq *p)
{
    ASSERT(p && this != p);
    this->mIId = p->mIId;
    this->mBallot = p->mBallot;
    this->mValue->Copy(p->mValue);
    return OK;
}
int CPaxosAcceptReq::Comp(CPaxosAcceptReq *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosAcceptReq::Print()
{
    char _str_mid[64];
    sprintf(_str_mid,"%d",mIId);
    LOG("Id = %s",_str_mid);
    char _str_mballot[64];
    sprintf(_str_mballot,"%d",mBallot);
    LOG("Ballot = %s",_str_mballot);
    LOG("Value = <binary %d bytes>",(int)this->mValue->GetSize());
    return OK;
}
status_t CPaxosAcceptReq::Clear()
{
    this->Destroy();
    this->Init();
    return OK;
}
status_t CPaxosAcceptReq::SetIId(int iid)
{
    this->mIId = iid;
    return OK;
}
status_t CPaxosAcceptReq::SetBallot(int ballot)
{
    this->mBallot = ballot;
    return OK;
}
status_t CPaxosAcceptReq::SetValue(CMem *value)
{
    ASSERT(value);
    this->mValue->Copy(value);
    return OK;
}
status_t CPaxosAcceptReq::SetValue(const void *value, int size)
{
    ASSERT(value);
    CMem mem_value;
    mem_value.Init();
    mem_value.SetRawBuf((void*)value,size,true);
    return this->SetValue(&mem_value);
}
int CPaxosAcceptReq::GetIId()
{
    return this->mIId;
}
int CPaxosAcceptReq::GetBallot()
{
    return this->mBallot;
}
CMem* CPaxosAcceptReq::GetValue()
{
    return this->mValue;
}
const char* CPaxosAcceptReq::GetValueStr()
{
    return this->mValue->CStr();
}

status_t CPaxosAcceptReq::Serialize(CFileBase *file)
{
    ASSERT(file);
    file->Write(&mIId,sizeof(mIId));
    file->Write(&mBallot,sizeof(mBallot));
    int32_t size = (int32_t)this->mValue->GetSize();
    file->Write(&size,sizeof(size));
    if(size > 0)
    {
        file->Write(this->mValue->GetRawBuf(),size);
    }
    return OK;
}

status_t CPaxosAcceptReq::Unserialize(CFileBase *file)
{
    ASSERT(file);
    file->Read(&mIId,sizeof(mIId));
    file->Read(&mBallot,sizeof(mBallot));
    int32_t size = 0;
    file->Read(&size,sizeof(size));

    this->mValue->Destroy();
    this->mValue->Init();
    if(size > 0)
    {
        this->mValue->Malloc(size);
        this->mValue->SetSize(size);
        file->Read(this->mValue->GetRawBuf(),size);
    }
    return OK;
}

status_t CPaxosAcceptReq::ToMessage(CPaxosMessage *msg)
{
    ASSERT(msg);
    CMemFile mf;
    mf.Init();
    this->Serialize(&mf);
    msg->SetType(ACCEPT_REQS);
    msg->SetData(&mf);
    return OK;
}
