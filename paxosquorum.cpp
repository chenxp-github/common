#include "paxosquorum.h"
#include "syslog.h"
#include "mem_tool.h"

CPaxosQuorum::CPaxosQuorum()
{
    this->InitBasic();
}
CPaxosQuorum::~CPaxosQuorum()
{
    this->Destroy();
}
status_t CPaxosQuorum::InitBasic()
{
    this->mAcceptorIds = NULL;
    this->mAcceptors = 0;
    this->mCount = 0;
    this->mQuorum = 0;
    return OK;
}
status_t CPaxosQuorum::Init(int acceptors)
{
    this->Destroy();
    //add your code

    MALLOC(this->mAcceptorIds,int,acceptors);
    this->mAcceptors = acceptors;

    this->Clear();
    return OK;
}
status_t CPaxosQuorum::Destroy()
{
    FREE(this->mAcceptorIds);
    this->InitBasic();
    return OK;
}
status_t CPaxosQuorum::Copy(CPaxosQuorum *p)
{
    ASSERT(p && this != p);
    
    this->Destroy();
    this->Init(p->mAcceptors);

    this->mCount = p->mCount;
    this->mQuorum = p->mQuorum;

    memcpy(this->mAcceptorIds,p->mAcceptorIds,sizeof(int)*this->mAcceptors);

    return OK;
}
status_t CPaxosQuorum::Clear()
{
    this->mCount = 0;
    memset(this->mAcceptorIds,0,sizeof(int)*this->mAcceptors);
    return OK;
}

status_t CPaxosQuorum::Add(int id)
{
    ASSERT(id >= 0 && id < this->mAcceptors);

    if (this->mAcceptorIds[id] == 0)
    {
        this->mCount++;
        this->mAcceptorIds[id] = 1;
        return OK;
    }
    return ERROR;
}

status_t CPaxosQuorum::Print()
{
    LOG("acceptors=%d",this->mAcceptors);
    LOG("count=%d",this->mCount);
    LOG("quorum=%d",this->mQuorum);
    return OK;
}

bool CPaxosQuorum::Reached()
{
    return this->mCount >= this->mQuorum;
}
