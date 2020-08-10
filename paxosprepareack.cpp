#include "paxosprepareack.h"
#include "memfile.h"
#include "paxosmessage.h"

CPaxosPrepareAck::CPaxosPrepareAck()
{
    this->InitBasic();
}
CPaxosPrepareAck::~CPaxosPrepareAck()
{
    this->Destroy();
}
status_t CPaxosPrepareAck::InitBasic()
{
    this->mAcceptorId = 0;
    this->mIId = 0;
    this->mBallot = 0;
    this->mValueBallot = 0;
    this->mValue = NULL;
    return OK;
}
status_t CPaxosPrepareAck::Init()
{
    this->InitBasic();

    NEW(this->mValue,CMem);
    this->mValue->Init();

    return OK;
}
status_t CPaxosPrepareAck::Destroy()
{
    DEL(this->mValue);
    this->InitBasic();
    return OK;
}
status_t CPaxosPrepareAck::Copy(CPaxosPrepareAck *p)
{
    ASSERT(p && this != p);
    this->mAcceptorId = p->mAcceptorId;
    this->mIId = p->mIId;
    this->mBallot = p->mBallot;
    this->mValueBallot = p->mValueBallot;
    this->mValue->Copy(p->mValue);
    
    return OK;
}
int CPaxosPrepareAck::Comp(CPaxosPrepareAck *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosPrepareAck::Print()
{
    char _str_macceptorid[64];
    sprintf(_str_macceptorid,"%d",mAcceptorId);
    LOG("AcceptorId = %s",_str_macceptorid);
    char _str_mid[64];
    sprintf(_str_mid,"%d",mIId);
    LOG("Id = %s",_str_mid);
    char _str_mballot[64];
    sprintf(_str_mballot,"%d",mBallot);
    LOG("Ballot = %s",_str_mballot);
    char _str_mvalueballot[64];
    sprintf(_str_mvalueballot,"%d",mValueBallot);
    LOG("ValueBallot = %s",_str_mvalueballot);
    LOG("Value = <binary %d bytes>",(int)this->mValue->GetSize());
    return OK;
}
status_t CPaxosPrepareAck::Clear()
{
    this->Destroy();
    this->Init();
    return OK;
}
status_t CPaxosPrepareAck::SetAcceptorId(int acceptorid)
{
    this->mAcceptorId = acceptorid;
    return OK;
}
status_t CPaxosPrepareAck::SetIId(int iid)
{
    this->mIId = iid;
    return OK;
}
status_t CPaxosPrepareAck::SetBallot(int ballot)
{
    this->mBallot = ballot;
    return OK;
}
status_t CPaxosPrepareAck::SetValueBallot(int valueballot)
{
    this->mValueBallot = valueballot;
    return OK;
}
int CPaxosPrepareAck::GetAcceptorId()
{
    return this->mAcceptorId;
}
int CPaxosPrepareAck::GetIId()
{
    return this->mIId;
}
int CPaxosPrepareAck::GetBallot()
{
    return this->mBallot;
}
int CPaxosPrepareAck::GetValueBallot()
{
    return this->mValueBallot;
}
CMem* CPaxosPrepareAck::GetValue()
{
    return this->mValue;
}
const char* CPaxosPrepareAck::GetValueStr()
{
    return this->mValue->CStr();
}
status_t CPaxosPrepareAck::SetValue(CMem *value)
{
    ASSERT(value);
    this->mValue->Copy(value);
    return OK;
}
status_t CPaxosPrepareAck::SetValue(const void *value, int_ptr_t size)
{
    ASSERT(value);
    CMem mem_value;
    mem_value.Init();
    mem_value.SetRawBuf((void*)value,size,true);
    return this->SetValue(&mem_value);
}

bool CPaxosPrepareAck::HasValue()
{
    return this->mValue->GetSize() > 0;
}

status_t CPaxosPrepareAck::Serialize(CFileBase *file)
{
    ASSERT(file);
    file->Write(&this->mAcceptorId,sizeof(this->mAcceptorId));
    file->Write(&this->mBallot,sizeof(this->mBallot));
    file->Write(&this->mIId,sizeof(this->mIId));
    file->Write(&this->mValueBallot,sizeof(this->mValueBallot));
    int32_t size = (int32_t)this->mValue->GetSize();
    file->Write(&size,sizeof(size));
    if(size > 0)
    {
        file->Write(this->mValue->GetRawBuf(),size);
    }
    return OK;
}

status_t CPaxosPrepareAck::Unserialize(CFileBase *file)
{
    ASSERT(file);
    file->Read(&this->mAcceptorId,sizeof(this->mAcceptorId));
    file->Read(&this->mBallot,sizeof(this->mBallot));
    file->Read(&this->mIId,sizeof(this->mIId));
    file->Read(&this->mValueBallot,sizeof(this->mValueBallot));
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

status_t CPaxosPrepareAck::ToMessage(CPaxosMessage *msg)
{
    ASSERT(msg);
    CMemFile mf;
    mf.Init();
    this->Serialize(&mf);
    msg->SetType(PREPARE_ACKS);
    msg->SetData(&mf);
    return OK;
}

status_t CPaxosPrepareAck::TransferValue(CMem *val)
{
    ASSERT(val);
    return this->mValue->Transfer(val);
}
