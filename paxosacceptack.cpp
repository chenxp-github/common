#include "paxosacceptack.h"
#include "paxosmessage.h"
#include "memfile.h"

CPaxosAcceptAck::CPaxosAcceptAck()
{
    this->InitBasic();
}
CPaxosAcceptAck::~CPaxosAcceptAck()
{
    this->Destroy();
}
status_t CPaxosAcceptAck::InitBasic()
{
    this->mAcceptorId = 0;
    this->mIId = 0;
    this->mBallot = 0;
    this->mValueBallot = 0;
    this->mIsFinal = 0;
    this->mValue = NULL;
    return OK;
}
status_t CPaxosAcceptAck::Init()
{
    this->InitBasic();
    NEW(this->mValue,CMem);
    this->mValue->Init();

    return OK;
}
status_t CPaxosAcceptAck::Destroy()
{
    DEL(this->mValue);
    this->InitBasic();
    return OK;
}
status_t CPaxosAcceptAck::Copy(CPaxosAcceptAck *p)
{
    ASSERT(p && this != p);
    this->mAcceptorId = p->mAcceptorId;
    this->mIId = p->mIId;
    this->mBallot = p->mBallot;
    this->mValueBallot = p->mValueBallot;
    this->mIsFinal = p->mIsFinal;
    this->mValue->Copy(p->mValue);
    return OK;
}
int CPaxosAcceptAck::Comp(CPaxosAcceptAck *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosAcceptAck::Print()
{
    char _str_macceptorid[64];
    sprintf(_str_macceptorid,"%d",mAcceptorId);
    LOG("AcceptorId = %s",_str_macceptorid);
    char _str_mid[64];
    sprintf(_str_mid,"%d",mIId);
    LOG("IId = %s",_str_mid);
    char _str_mballot[64];
    sprintf(_str_mballot,"%d",mBallot);
    LOG("Ballot = %s",_str_mballot);
    char _str_mvalueballot[64];
    sprintf(_str_mvalueballot,"%d",mValueBallot);
    LOG("ValueBallot = %s",_str_mvalueballot);
    char _str_misfinal[64];
    sprintf(_str_misfinal,"%d",mIsFinal);
    LOG("IsFinal = %s",_str_misfinal);
    LOG("Value = <binary %d bytes>",(int)this->mValue->GetSize());
    return OK;
}
status_t CPaxosAcceptAck::ClearValue()
{
    this->mValue->Destroy();
    this->mValue->Init();
    return OK;
}
status_t CPaxosAcceptAck::SetAcceptorId(int acceptorid)
{
    this->mAcceptorId = acceptorid;
    return OK;
}
status_t CPaxosAcceptAck::SetIId(int id)
{
    this->mIId = id;
    return OK;
}
status_t CPaxosAcceptAck::SetBallot(int ballot)
{
    this->mBallot = ballot;
    return OK;
}
status_t CPaxosAcceptAck::SetValueBallot(int valueballot)
{
    this->mValueBallot = valueballot;
    return OK;
}
status_t CPaxosAcceptAck::SetIsFinal(int isfinal)
{
    this->mIsFinal = isfinal;
    return OK;
}
status_t CPaxosAcceptAck::SetValue(CMem *value)
{
    ASSERT(value);
    this->mValue->Copy(value);
    return OK;
}
status_t CPaxosAcceptAck::SetValue(const void *value, int size)
{
    ASSERT(value);
    CMem mem_value;
    mem_value.Init();
    mem_value.SetRawBuf((void*)value,size,true);
    return this->SetValue(&mem_value);
}
int CPaxosAcceptAck::GetAcceptorId()
{
    return this->mAcceptorId;
}
int CPaxosAcceptAck::GetIId()
{
    return this->mIId;
}
int CPaxosAcceptAck::GetBallot()
{
    return this->mBallot;
}
int CPaxosAcceptAck::GetValueBallot()
{
    return this->mValueBallot;
}
int CPaxosAcceptAck::IsFinal()
{
    return this->mIsFinal;
}
CMem* CPaxosAcceptAck::GetValue()
{
    return this->mValue;
}
const char* CPaxosAcceptAck::GetValueStr()
{
    return this->mValue->CStr();
}

status_t CPaxosAcceptAck::Serialize(CFileBase *file)
{
    ASSERT(file);
    file->Write(&mAcceptorId,sizeof(mAcceptorId));
    file->Write(&mIId,sizeof(mIId));
    file->Write(&mBallot,sizeof(mBallot));
    file->Write(&mValueBallot,sizeof(mValueBallot));
    file->Write(&mIsFinal,sizeof(mIsFinal));
    int32_t size = (int32_t)this->mValue->GetSize();
    file->Write(&size,sizeof(size));
    if(size > 0)
    {
        file->Write(this->mValue->GetRawBuf(),size);
    }
    return OK;
}

status_t CPaxosAcceptAck::Unserialize(CFileBase *file)
{
    ASSERT(file);

    file->Read(&mAcceptorId,sizeof(mAcceptorId));
    file->Read(&mIId,sizeof(mIId));
    file->Read(&mBallot,sizeof(mBallot));
    file->Read(&mValueBallot,sizeof(mValueBallot));
    file->Read(&mIsFinal,sizeof(mIsFinal));
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

status_t CPaxosAcceptAck::ToMessage(CPaxosMessage *msg)
{
    ASSERT(msg);
    CMemFile mf;
    mf.Init();
    this->Serialize(&mf);
    msg->SetType(ACCEPT_ACKS);
    msg->SetData(&mf);
    return OK;
}

bool CPaxosAcceptAck::HasValue()
{
    return this->mValue->GetSize() > 0;
}
