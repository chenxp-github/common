#include "paxoslinstancehashmap.h"
#include "syslog.h"

CPaxosLInstanceHashMap::CPaxosLInstanceHashMap()
{
    this->InitBasic();
}
CPaxosLInstanceHashMap::~CPaxosLInstanceHashMap()
{
    this->Destroy();
}
status_t CPaxosLInstanceHashMap::InitBasic()
{
    this->mCapacity = 0;
    this->mData = 0;
    this->mSize = 0;
    return OK;
}
status_t CPaxosLInstanceHashMap::Init(int capacity)
{
    this->InitBasic();
    this->mCapacity = capacity;
    MALLOC(this->mData,CPaxosLInstance*,capacity);
    memset(this->mData,0,capacity*sizeof(CPaxosLInstance*));
    return OK;
}
status_t CPaxosLInstanceHashMap::Destroy()
{
    if(this->mData == NULL)
        return OK;

    CPaxosLInstance *q,*p;
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

status_t CPaxosLInstanceHashMap::PutPtr(CPaxosLInstance *ptr)
{
    ASSERT(ptr);

    if(this->Get(ptr) != NULL)
        return ERROR;

    int code = this->HashCode(ptr);
    ASSERT(code >= 0 && code < this->mCapacity);
    CPaxosLInstance *p = this->mData[code];
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

status_t CPaxosLInstanceHashMap::Put(CPaxosLInstance *paxoslinstance)
{
    CPaxosLInstance *tmp = CloneNode(paxoslinstance);
    if(!this->PutPtr(tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CPaxosLInstance * CPaxosLInstanceHashMap::Get(CPaxosLInstance *key)
{
    ASSERT(key);

    int code = this->HashCode(key);
    ASSERT(code >= 0 && code < this->mCapacity);
    CPaxosLInstance *p = this->mData[code];
    while(p)
    {
        if(Equals(p,key))
            return p;
        p = p->next;
    }
    return NULL;
}

status_t CPaxosLInstanceHashMap::EnumAll(CALLBACK_FUNC func, void **params)
{
    ASSERT(params);
    for(int i = 0; i < this->mCapacity; i++)
    {
        CPaxosLInstance *p = this->mData[i];
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
status_t CPaxosLInstanceHashMap::Del(CPaxosLInstance *key)
{
    ASSERT(key);
    int code = this->HashCode(key);
    if(code < 0 || code >= this->mCapacity)
        return ERROR;

    CPaxosLInstance *pre,*p;
    p = pre = this->mData[code];

    if(p != NULL)
    {
        if(Equals(p,key))
        {
            this->mData[code] = p->next;
            DelNode(p);
            this->mSize --;
            return OK;
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
                    DelNode(p);
                    this->mSize --;
                    return OK;
                }
            }
        }
    }

    return ERROR;
}
int CPaxosLInstanceHashMap::GetSize()
{
    return this->mSize;
}
bool CPaxosLInstanceHashMap::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CPaxosLInstanceHashMap::Print()
{
    int collision = 0;
    int maxLength = 0;

    for(int i = 0; i < this->mCapacity; i++)
    {
        CPaxosLInstance *p = this->mData[i];
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
int CPaxosLInstanceHashMap::HashCode(CPaxosLInstance *paxoslinstance)
{
    ASSERT(paxoslinstance);
    return paxoslinstance->GetIId()%mCapacity;
}
bool CPaxosLInstanceHashMap::Equals(CPaxosLInstance *paxoslinstance1, CPaxosLInstance *paxoslinstance2)
{
    ASSERT(paxoslinstance1 && paxoslinstance2);
    return paxoslinstance1->Comp(paxoslinstance2) == 0;
}

CPaxosLInstance * CPaxosLInstanceHashMap::CloneNode(CPaxosLInstance *paxoslinstance)
{
    CPaxosLInstance *ptr;
    NEW(ptr,CPaxosLInstance);
    ptr->Init(1);
    ptr->Copy(paxoslinstance);
    return ptr;
}

status_t CPaxosLInstanceHashMap::DelNode(CPaxosLInstance *paxoslinstance)
{
    DEL(paxoslinstance);
    return OK;
}
CPaxosLInstance* CPaxosLInstanceHashMap::Get(int id)
{
    CPaxosLInstance tmp;
    tmp.SetIId(id);
    return this->Get(&tmp);
}


