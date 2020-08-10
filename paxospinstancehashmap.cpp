#include "paxospinstancehashmap.h"
#include "syslog.h"

CPaxosPInstanceHashMap::CPaxosPInstanceHashMap()
{
    this->InitBasic();
}
CPaxosPInstanceHashMap::~CPaxosPInstanceHashMap()
{
    this->Destroy();
}
status_t CPaxosPInstanceHashMap::InitBasic()
{
    this->mCapacity = 0;
    this->mData = 0;
    this->mSize = 0;
    return OK;
}
status_t CPaxosPInstanceHashMap::Init(int capacity)
{
    this->InitBasic();
    this->mCapacity = capacity;
    MALLOC(this->mData,CPaxosPInstance*,capacity);
    memset(this->mData,0,capacity*sizeof(CPaxosPInstance*));
    return OK;
}
status_t CPaxosPInstanceHashMap::Destroy()
{
    if(this->mData == NULL)
        return OK;

    CPaxosPInstance *q,*p;
    for(int i = 0; i < this->mCapacity; i++)
    {
        p  = this->mData[i];
        while(p)
        {
            q = p->next;
            DelNode(p);
            p = q;          
        }           
    }
    FREE(this->mData);
    this->InitBasic();
    return OK;
}

status_t CPaxosPInstanceHashMap::PutPtr(CPaxosPInstance *ptr)
{
    ASSERT(ptr);

    if(this->Get(ptr) != NULL)
        return ERROR;

    int code = this->HashCode(ptr);
    ASSERT(code >= 0 && code < this->mCapacity);
    CPaxosPInstance *p = this->mData[code];
    if (p == NULL)
    {
        this->mData[code] = ptr;
        ptr->next = NULL;
    }
    else
    {
        while(p->next)
        {
            p = p->next;
        }
        p->next = ptr;
        ptr->next = NULL;
    }
    this->mSize++;
    return OK;
}

status_t CPaxosPInstanceHashMap::Put(CPaxosPInstance *paxospinstance)
{
    CPaxosPInstance *tmp = CloneNode(paxospinstance);
    if(!this->PutPtr(tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CPaxosPInstance * CPaxosPInstanceHashMap::Get(CPaxosPInstance *key)
{
    ASSERT(key);

    int code = this->HashCode(key);
    ASSERT(code >= 0 && code < this->mCapacity);
    CPaxosPInstance *p = this->mData[code];
    while(p)
    {
        if(Equals(p,key))
            return p;
        p = p->next;
    }
    return NULL;
}

status_t CPaxosPInstanceHashMap::EnumAll(CALLBACK_FUNC func, void **params)
{
    ASSERT(params);
    for(int i = 0; i < this->mCapacity; i++)
    {
        CPaxosPInstance *p = this->mData[i];
        if(p != NULL)
        {
            while(p)
            {   
                params[0] = p;
                func(params);
                p = p->next;
            }
        }
    }
    return OK;
}
status_t CPaxosPInstanceHashMap::Del(CPaxosPInstance *key)
{
    CPaxosPInstance *p = this->Remove(key);
    if(p != NULL)
    {
        DelNode(p);
        return OK;
    }
    return ERROR;
}   
int CPaxosPInstanceHashMap::GetSize()
{
    return this->mSize;
}
bool CPaxosPInstanceHashMap::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CPaxosPInstanceHashMap::Print()
{
    int collision = 0;
    int maxLength = 0;

    for(int i = 0; i < this->mCapacity; i++)
    {
        CPaxosPInstance *p = this->mData[i];
        if(p != NULL)
        {
            int len = 0;
            collision--;
            while(p)
            {               
                p = p->next;
                len ++;
                collision++;
            }
            if(len > maxLength)
                maxLength = len;
        }
    }

    LOG("capacity is %d", mCapacity);
    LOG("total size is %d",mSize);
    LOG("maximum linked list length is %d",maxLength);
    LOG("total collison is %d",collision);
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
int CPaxosPInstanceHashMap::HashCode(CPaxosPInstance *paxospinstance)
{
    ASSERT(paxospinstance);
    return paxospinstance->GetIId()%mCapacity;
}
bool CPaxosPInstanceHashMap::Equals(CPaxosPInstance *paxospinstance1, CPaxosPInstance *paxospinstance2)
{
    ASSERT(paxospinstance1 && paxospinstance2);
    return paxospinstance1->Comp(paxospinstance2) == 0;
}

CPaxosPInstance * CPaxosPInstanceHashMap::CloneNode(CPaxosPInstance *paxospinstance)
{
    CPaxosPInstance *ptr;
    NEW(ptr,CPaxosPInstance);
    ptr->Init(1);
    ptr->Copy(paxospinstance);
    return ptr;
}

status_t CPaxosPInstanceHashMap::DelNode(CPaxosPInstance *paxospinstance)
{
    DEL(paxospinstance);
    return OK;
}

CPaxosPInstance* CPaxosPInstanceHashMap::Get(int iid)
{
    CPaxosPInstance tmp;
    tmp.SetIId(iid);
    return this->Get(&tmp);
}

static status_t _smallest(void **p)
{
    CPaxosPInstance *smallest = (CPaxosPInstance*)p[10];
    CPaxosPInstance *cur = (CPaxosPInstance*)p[0];

    if(smallest == NULL || cur->GetIId() < smallest->GetIId())
    {
        p[10] = (void*)cur;
    }
    return OK;
}

CPaxosPInstance* CPaxosPInstanceHashMap::FindSmallestId()
{
    CALLBACK_PARAMS(p);
    p[10] = NULL;
    this->EnumAll(_smallest,p);
    return (CPaxosPInstance*)p[10];
}

status_t CPaxosPInstanceHashMap::Del(int id)
{
    CPaxosPInstance tmp;
    tmp.SetIId(id);
    return this->Del(&tmp);
}

CPaxosPInstance * CPaxosPInstanceHashMap::Remove(CPaxosPInstance *key)
{
    ASSERT(key);
    int code = this->HashCode(key);
    if(code < 0 || code >= this->mCapacity)
        return NULL;

    CPaxosPInstance *pre,*p;
    p = pre = this->mData[code];

    if(p != NULL)
    {
        if(Equals(p,key))
        {
            this->mData[code] = p->next;
            this->mSize --;
            return p;
        }
        else
        {
            while(p)
            {
                pre = p;
                p = p->next;
                if(Equals(p,key))
                {
                    pre->next = p->next;                    
                    this->mSize --;
                    return p;
                }
            }
        }
    }

    return NULL;
}

CPaxosPInstance * CPaxosPInstanceHashMap::Remove(int id)
{
    CPaxosPInstance tmp;
    tmp.SetIId(id);
    return this->Remove(&tmp);
}
