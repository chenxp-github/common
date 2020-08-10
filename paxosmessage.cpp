#include "paxosmessage.h"
#include "syslog.h"

CPaxosMessage::CPaxosMessage()
{
    this->InitBasic();
}
CPaxosMessage::~CPaxosMessage()
{
    this->Destroy();
}
status_t CPaxosMessage::InitBasic()
{
    this->mType = UNKNOWN;
    this->mData = NULL;
    return OK;
}
status_t CPaxosMessage::Init()
{
    this->Destroy();
    NEW(this->mData,CMem);
    this->mData->Init();

    return OK;
}
status_t CPaxosMessage::Destroy()
{
    DEL(this->mData);
    this->InitBasic();
    return OK;
}
status_t CPaxosMessage::Copy(CPaxosMessage *p)
{
    ASSERT(p && this != p);
    this->mType = p->mType;
    this->mData->Copy(p->mData);
    return OK;
}
int CPaxosMessage::Comp(CPaxosMessage *p)
{
    ASSERT(p);
    return 0;
}
status_t CPaxosMessage::Print()
{
    //add your code
    return OK;
}

status_t CPaxosMessage::SetData(CFileBase *data)
{
    ASSERT(data);
    this->mData->Copy(data);
    return OK;
}

status_t CPaxosMessage::SetData(const void *data, int size)
{
    ASSERT(data);
    CMem mem;
    mem.Init();
    mem.SetRawBuf((void*)data,size,true);
    return this->SetData(&mem);
}

status_t CPaxosMessage::SetType(int type)
{
    this->mType = type;
    return OK;
}

int CPaxosMessage::GetType()
{
    return this->mType;
}

status_t CPaxosMessage::Transfer(CPaxosMessage *from)
{
    ASSERT(from && from->mData);
    ASSERT(from->mData->mSelfAlloc);
    this->mType = from->mType;
    this->mData->Transfer(from->mData);
    return OK;
}

bool CPaxosMessage::HasValue()
{
    return this->mData->GetSize() > 0;
}

status_t CPaxosMessage::ClearData()
{
    this->mData->Destroy();
    this->mData->Init();
    return OK;
}

CMem * CPaxosMessage::GetData()
{
    return this->mData;
}

status_t CPaxosMessage::TransferData(CMem *data)
{
    ASSERT(data);
    return this->mData->Transfer(data);
}
