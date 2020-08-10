#include "paxosacceptackstk.h"
#include "mem_tool.h"
#include "syslog.h"

CPaxosAcceptAckStk::CPaxosAcceptAckStk()
{
    this->InitBasic();  
}
CPaxosAcceptAckStk::~CPaxosAcceptAckStk()
{
    this->Destroy();
}
status_t CPaxosAcceptAckStk::InitBasic()
{
    this->mIndex = NULL;
    this->mTop = 0;;
    this->mSize = 0;    
    return OK;
}
status_t CPaxosAcceptAckStk::Init(int init_size)
{
    int i;
    this->InitBasic();  
    this->mSize = init_size;
    MALLOC(this->mIndex,CPaxosAcceptAck * ,this->mSize);
    for(i = 0; i < this->mSize; i++)
        this->mIndex[i] = NULL;
    return OK;
}
status_t CPaxosAcceptAckStk::Destroy()
{
    int i;
    if(this->mIndex == NULL)
        return ERROR;
    for(i = 0; i < this->mTop; i++)
    {
        DelNode(this->mIndex[i]);
    }
    FREE(this->mIndex);
    this->InitBasic();  
    return OK;
}
bool CPaxosAcceptAckStk::IsEmpty()
{
    return this->mTop <= 0;
}
bool CPaxosAcceptAckStk::IsFull()
{
    return this->mTop >= this->mSize;
}

status_t CPaxosAcceptAckStk::AutoResize()
{
    if(this->IsFull())
    {
        REALLOC(this->mIndex,CPaxosAcceptAck*,this->mSize,this->mSize*2);
        this->mSize *= 2;
        for(int i = this->mTop; i < this->mSize; i++)
        {
            this->mIndex[i] = NULL;
        }
    }
    return OK;
}

status_t CPaxosAcceptAckStk::Push(CPaxosAcceptAck *paxosacceptack)
{
    ASSERT(paxosacceptack);
    CPaxosAcceptAck *tmp = this->CloneNode(paxosacceptack);
    if(!this->PushPtr(tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CPaxosAcceptAckStk::PushPtr(CPaxosAcceptAck *paxosacceptack)
{
    ASSERT(paxosacceptack); 
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mIndex[this->mTop] = paxosacceptack;
    this->mTop++;
    return OK;
}

CPaxosAcceptAck * CPaxosAcceptAckStk::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->mTop--;
    return this->mIndex[this->mTop];
}

status_t CPaxosAcceptAckStk::Pop(CPaxosAcceptAck *paxosacceptack)
{
    ASSERT(!this->IsEmpty());   
    this->mTop--;
    CopyNode(paxosacceptack,mIndex[mTop]);
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPaxosAcceptAckStk::Print()
{
    int i;
    for(i = 0; i <this->mTop; i++)
    {
        this->mIndex[i]->Print();
        LOG(",");
    }
    LOG("mSize=%d",this->mSize);
    LOG("mTop=%d",this->mTop);
    LOG("bottom=%d",0);
    return OK;
}
int CPaxosAcceptAckStk::GetLen()
{
    return this->mTop;
}

int CPaxosAcceptAckStk::SearchPos(CPaxosAcceptAck *paxosacceptack)
{
    int i;
    for(i=0;i<this->mTop;i++)
    {
        if(CompNode(mIndex[i],paxosacceptack) == 0)
            return i;
    }
    return -1;
}

CPaxosAcceptAck * CPaxosAcceptAckStk::Search(CPaxosAcceptAck *paxosacceptack)
{
    int pos = this->SearchPos(paxosacceptack);
    if(pos >= 0 && pos < this->mTop)
        return this->mIndex[pos];;
    return NULL;
}
CPaxosAcceptAck * CPaxosAcceptAckStk::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->mIndex[this->mTop - 1];
}
status_t CPaxosAcceptAckStk::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->mTop--;
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPaxosAcceptAckStk::Clear()
{
    while(this->DelTop());
    return OK;
}

CPaxosAcceptAck * CPaxosAcceptAckStk::GetElem(int mIndex)
{
    if(mIndex < 0 || mIndex >= this->mTop)
        return NULL;
    return this->mIndex[mIndex];
}

CPaxosAcceptAck * CPaxosAcceptAckStk::BSearchNode(CPaxosAcceptAck *paxosacceptack,int order)
{
    return this->GetElem(this->BSearch(paxosacceptack,order));
}
int CPaxosAcceptAckStk::BSearch(CPaxosAcceptAck *paxosacceptack,int order)
{
    int find,pos;
    pos = this->BSearchPos(paxosacceptack,order,&find);
    if(find) return pos;
    return -1;
}

status_t CPaxosAcceptAckStk::InsertElemPtr(int i, CPaxosAcceptAck *paxosacceptack)
{
    ASSERT(paxosacceptack);
    ASSERT(i >= 0 && i <= this->mTop);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(int k = this->mTop; k > i; k--)
    {
        this->mIndex[k] = this->mIndex[k - 1];
    }
    this->mIndex[i] = paxosacceptack;
    this->mTop++;
    return OK;
}

status_t CPaxosAcceptAckStk::InsertElem(int i, CPaxosAcceptAck *paxosacceptack)
{
    ASSERT(paxosacceptack);
    CPaxosAcceptAck *tmp = this->CloneNode(paxosacceptack);
    if(!this->InsertElemPtr(i,tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CPaxosAcceptAck * CPaxosAcceptAckStk::RemoveElem(int index)
{
    CPaxosAcceptAck *p;
    
    ASSERT(index >= 0 && index < this->mTop);
    p = this->GetElem(index);

    for(int k = index; k < this->mTop-1; k++)
    {
        this->mIndex[k] = this->mIndex[k + 1];
    }
    this->mTop --;
    this->mIndex[mTop] = NULL;
    return p;
}

status_t CPaxosAcceptAckStk::DelElem(int i)
{
    CPaxosAcceptAck *p = this->RemoveElem(i);
    if(p != NULL)
    {
        DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CPaxosAcceptAckStk::InsOrderedPtr(CPaxosAcceptAck *paxosacceptack, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(paxosacceptack,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElemPtr(pos,paxosacceptack);
}

status_t CPaxosAcceptAckStk::InsOrdered(CPaxosAcceptAck *paxosacceptack, int order,int unique)
{
    ASSERT(paxosacceptack);
    int pos,find;
    pos = this->BSearchPos(paxosacceptack,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElem(pos,paxosacceptack);
}

int CPaxosAcceptAckStk::BSearchPos(CPaxosAcceptAck *paxosacceptack, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = CompNode(mIndex[mid],paxosacceptack);
        if(comp == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
    
        if(order != 0) comp = -comp;

        if(comp > 0)
            high = mid-1;
        else
            low = mid+1;
    }
    *find_flag = FALSE;
    return low;
}

status_t CPaxosAcceptAckStk::Sort(int order)
{
    int i,len;
    CPaxosAcceptAckStk tmp;

    len = this->GetLen();
    tmp.Init(len);

    for(i = 0; i < len; i++)
    {
        tmp.PushPtr(this->GetElem(i));
    }

    this->mTop = 0;
    for(i = 0; i < len; i++)
    {
        this->InsOrderedPtr(tmp.PopPtr(),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.mIndex[i] = NULL;
    }

    return OK;
}
status_t CPaxosAcceptAckStk::Copy(CPaxosAcceptAckStk *stk)
{
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(int i = 0; i < stk->GetLen(); i++)
    {
        CPaxosAcceptAck *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CPaxosAcceptAckStk::CopyNode(CPaxosAcceptAck *dst, CPaxosAcceptAck *src)
{
    return dst->Copy(src);
}

CPaxosAcceptAck * CPaxosAcceptAckStk::CloneNode(CPaxosAcceptAck *paxosacceptack)
{
    CPaxosAcceptAck *tmp;
    NEW(tmp,CPaxosAcceptAck);
    tmp->Init();
    CopyNode(tmp,paxosacceptack);
    return tmp;
}

status_t CPaxosAcceptAckStk::DelNode(CPaxosAcceptAck *paxosacceptack)
{
    DEL(paxosacceptack);
    return OK;
}

int CPaxosAcceptAckStk::CompNode(CPaxosAcceptAck *paxosacceptack1, CPaxosAcceptAck *paxosacceptack2)
{
    ASSERT(paxosacceptack1 && paxosacceptack2);
    return paxosacceptack1->Comp(paxosacceptack2);
}

status_t CPaxosAcceptAckStk::SetElemPtr(int index, CPaxosAcceptAck *ack)
{
    ASSERT(index >= 0 && index <= this->mTop);
    CPaxosAcceptAck *old = this->GetElem(index);
    if(old != NULL)DEL(old);
    this->mIndex[index] = ack;
    return OK;
}

status_t CPaxosAcceptAckStk::SetElem(int index, CPaxosAcceptAck *ack)
{
    CPaxosAcceptAck *dup = this->CloneNode(ack);

    if(!this->SetElemPtr(index,dup))
    {
        DelNode(dup);
        return ERROR;
    }

    return OK;
}
