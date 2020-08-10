#include "paxosmessagestk.h"
#include "mem_tool.h"
#include "syslog.h"

CPaxosMessageStk::CPaxosMessageStk()
{
    this->InitBasic();  
}
CPaxosMessageStk::~CPaxosMessageStk()
{
    this->Destroy();
}
status_t CPaxosMessageStk::InitBasic()
{
    this->mIndex = NULL;
    this->mTop = 0;;
    this->mSize = 0;    
    return OK;
}
status_t CPaxosMessageStk::Init(int init_size)
{
    int i;
    this->InitBasic();  
    this->mSize = init_size;
    MALLOC(this->mIndex,CPaxosMessage * ,this->mSize);
    for(i = 0; i < this->mSize; i++)
        this->mIndex[i] = NULL;
    return OK;
}
status_t CPaxosMessageStk::Destroy()
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
bool CPaxosMessageStk::IsEmpty()
{
    return this->mTop <= 0;
}
bool CPaxosMessageStk::IsFull()
{
    return this->mTop >= this->mSize;
}

status_t CPaxosMessageStk::AutoResize()
{
    if(this->IsFull())
    {
        REALLOC(this->mIndex,CPaxosMessage*,this->mSize,this->mSize*2);
        this->mSize *= 2;
        for(int i = this->mTop; i < this->mSize; i++)
        {
            this->mIndex[i] = NULL;
        }
    }
    return OK;
}

status_t CPaxosMessageStk::Push(CPaxosMessage *paxosmessage)
{
    ASSERT(paxosmessage);
    CPaxosMessage *tmp = this->CloneNode(paxosmessage);
    if(!this->PushPtr(tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CPaxosMessageStk::PushPtr(CPaxosMessage *paxosmessage)
{
    ASSERT(paxosmessage);   
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mIndex[this->mTop] = paxosmessage;
    this->mTop++;
    return OK;
}

CPaxosMessage * CPaxosMessageStk::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->mTop--;
    return this->mIndex[this->mTop];
}

status_t CPaxosMessageStk::Pop(CPaxosMessage *paxosmessage)
{
    ASSERT(!this->IsEmpty());   
    this->mTop--;
    CopyNode(paxosmessage,mIndex[mTop]);
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPaxosMessageStk::Print()
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
int CPaxosMessageStk::GetLen()
{
    return this->mTop;
}

int CPaxosMessageStk::SearchPos(CPaxosMessage *paxosmessage)
{
    int i;
    for(i=0;i<this->mTop;i++)
    {
        if(CompNode(mIndex[i],paxosmessage) == 0)
            return i;
    }
    return -1;
}

CPaxosMessage * CPaxosMessageStk::Search(CPaxosMessage *paxosmessage)
{
    int pos = this->SearchPos(paxosmessage);
    if(pos >= 0 && pos < this->mTop)
        return this->mIndex[pos];;
    return NULL;
}
CPaxosMessage * CPaxosMessageStk::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->mIndex[this->mTop - 1];
}
status_t CPaxosMessageStk::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->mTop--;
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPaxosMessageStk::Clear()
{
    while(this->DelTop());
    return OK;
}

CPaxosMessage * CPaxosMessageStk::GetElem(int mIndex)
{
    if(mIndex < 0 || mIndex >= this->mTop)
        return NULL;
    return this->mIndex[mIndex];
}

CPaxosMessage * CPaxosMessageStk::BSearchNode(CPaxosMessage *paxosmessage,int order)
{
    return this->GetElem(this->BSearch(paxosmessage,order));
}
int CPaxosMessageStk::BSearch(CPaxosMessage *paxosmessage,int order)
{
    int find,pos;
    pos = this->BSearchPos(paxosmessage,order,&find);
    if(find) return pos;
    return -1;
}

status_t CPaxosMessageStk::InsertElemPtr(int i, CPaxosMessage *paxosmessage)
{
    ASSERT(paxosmessage);
    ASSERT(i >= 0 && i <= this->mTop);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(int k = this->mTop; k > i; k--)
    {
        this->mIndex[k] = this->mIndex[k - 1];
    }
    this->mIndex[i] = paxosmessage;
    this->mTop++;
    return OK;
}

status_t CPaxosMessageStk::InsertElem(int i, CPaxosMessage *paxosmessage)
{
    ASSERT(paxosmessage);
    CPaxosMessage *tmp = this->CloneNode(paxosmessage);
    if(!this->InsertElemPtr(i,tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CPaxosMessage * CPaxosMessageStk::RemoveElem(int index)
{
    CPaxosMessage *p;
    
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

status_t CPaxosMessageStk::DelElem(int i)
{
    CPaxosMessage *p = this->RemoveElem(i);
    if(p != NULL)
    {
        DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CPaxosMessageStk::InsOrderedPtr(CPaxosMessage *paxosmessage, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(paxosmessage,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElemPtr(pos,paxosmessage);
}

status_t CPaxosMessageStk::InsOrdered(CPaxosMessage *paxosmessage, int order,int unique)
{
    ASSERT(paxosmessage);
    int pos,find;
    pos = this->BSearchPos(paxosmessage,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElem(pos,paxosmessage);
}

int CPaxosMessageStk::BSearchPos(CPaxosMessage *paxosmessage, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = CompNode(mIndex[mid],paxosmessage);
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

status_t CPaxosMessageStk::Sort(int order)
{
    int i,len;
    CPaxosMessageStk tmp;

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
status_t CPaxosMessageStk::Copy(CPaxosMessageStk *stk)
{
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(int i = 0; i < stk->GetLen(); i++)
    {
        CPaxosMessage *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CPaxosMessageStk::CopyNode(CPaxosMessage *dst, CPaxosMessage *src)
{
    return dst->Copy(src);
}

CPaxosMessage * CPaxosMessageStk::CloneNode(CPaxosMessage *paxosmessage)
{
    CPaxosMessage *tmp;
    NEW(tmp,CPaxosMessage);
    tmp->Init();
    CopyNode(tmp,paxosmessage);
    return tmp;
}

status_t CPaxosMessageStk::DelNode(CPaxosMessage *paxosmessage)
{
    DEL(paxosmessage);
    return OK;
}

int CPaxosMessageStk::CompNode(CPaxosMessage *paxosmessage1, CPaxosMessage *paxosmessage2)
{
    ASSERT(paxosmessage1 && paxosmessage2);
    return paxosmessage1->Comp(paxosmessage2);
}

status_t CPaxosMessageStk::PushTransfer(CPaxosMessage *msg)
{
    CPaxosMessage *tmp;
    NEW(tmp,CPaxosMessage);
    tmp->Init();
    tmp->Transfer(msg);
    return this->PushPtr(tmp);
}
