#ifndef __TREE_CACHE_H
#define __TREE_CACHE_H

#include "filebase.h"
#include "syslog.h"
#include "mem_tool.h"
#include "weak_pointer.h"

template <class T>
class CTreeCache{
    typedef status_t (*HOW_TO_COMP)(T* t1,T* t2);
public:
    WEAK_REF_DEFINE();
private:
    CWeakPointer<T> **m_Index;
    int m_Top;
    int m_Size;
    bool m_IsWeakTable;
    HOW_TO_COMP how_to_comp;
public:     
CTreeCache()
{
    this->InitBasic();  
}
~CTreeCache()
{
    this->Destroy();
}
status_t InitBasic()
{
    WEAK_REF_CLEAR();
    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;   
    this->how_to_comp = NULL;
    return OK;
}
status_t Init(int init_size=256)
{
    int i;

    this->InitBasic();      
    
    this->m_Size = init_size;
    MALLOC(this->m_Index,CWeakPointer<T>*,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}
status_t Destroy()
{
    int i;    
    
    WEAK_REF_DESTROY();
    if(this->m_Index == NULL)
        return ERROR;
    for(i = 0; i < this->m_Top; i++)
    {        
        this->DelNode(this->m_Index[i]);
    }
    FREE(this->m_Index);
    this->InitBasic();  
    return OK;
}
bool IsEmpty()
{
    return this->m_Top <= 0;
}
bool IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,CWeakPointer<T>*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t PushPtr(CWeakPointer<T> *t)
{
    ASSERT(t);  
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = t;
    this->m_Top++;
    return OK;
}

CWeakPointer<T> * PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

int GetLen()
{
    return this->m_Top;
}

int SearchPos(CWeakPointer<T> *t)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],t) == 0)
            return i;
    }
    return -1;
}

CWeakPointer<T>* Search(CWeakPointer<T> *t)
{
    int pos = this->SearchPos(t);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}

CWeakPointer<T>* GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}

status_t DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t Clear()
{
    while(this->DelTop());
    return OK;
}

CWeakPointer<T>* GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

CWeakPointer<T>* BSearchNode(CWeakPointer<T> *t,int order)
{
    return this->GetElem(this->BSearch(t,order));
}

int BSearch(CWeakPointer<T> *t,int order)
{
    int find,pos;
    pos = this->BSearchPos(t,order,&find);
    if(find) return pos;
    return -1;
}

status_t InsElemPtr(int i, CWeakPointer<T> *t)
{
    int k;

    ASSERT(t);
    ASSERT(i >= 0 && i <= this->m_Top);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(k = this->m_Top; k > i; k--)
    {
        this->m_Index[k] = this->m_Index[k - 1];
    }
    this->m_Index[i] = t;
    this->m_Top++;
    return OK;
}

CWeakPointer<T>* RemoveElem(int index)
{
    CWeakPointer<T> *p;
    int k;
    
    ASSERT(index >= 0 && index < this->m_Top);
    p = this->GetElem(index);

    for(k = index; k < this->m_Top-1; k++)
    {
        this->m_Index[k] = this->m_Index[k + 1];
    }
    this->m_Top --;
    this->m_Index[this->m_Top] = NULL;
    return p;
}

status_t DelElem(int i)
{
    CWeakPointer<T> *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t InsOrderedPtr(CWeakPointer<T> *t, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(t,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,t);
}

int BSearchPos(CWeakPointer<T> *t, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = this->CompNode(this->m_Index[mid],t);
        if(comp == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
    
        if(order != 0) comp = -comp;
        if(comp>0)high=mid-1;else low=mid+1;
    }
    *find_flag = FALSE;
    return low;
}

status_t Sort(int order=0)
{
    int i,len;
    CTreeCache tmp;

    len = this->GetLen();
    tmp.Init(len);

    for(i = 0; i < len; i++)
    {
        tmp.PushPtr(this->GetElem(i));
    }

    this->m_Top = 0;
    for(i = 0; i < len; i++)
    {
        this->InsOrderedPtr(tmp.PopPtr(),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.m_Index[i] = NULL;
    }

    return OK;
}

status_t Print(CFileBase *_buf)
{
    return OK;
}

bool IsWeakTable()
{
    return m_IsWeakTable;
}

status_t DelNode(CWeakPointer<T> *t)
{
    DEL(t);
    return OK;
}

int CompNode(CWeakPointer<T> *t1, CWeakPointer<T> *t2)
{
    ASSERT(t1 && t2);
    ASSERT(t1->get() && t2->get());
    if(how_to_comp)
    {
        return how_to_comp(t1->get(),t2->get());
    }
    else
    {
        return t1->get()->Comp(t2->get());
    }
}

void SetCompFunc(HOW_TO_COMP func)
{
    how_to_comp = func;
}

HOW_TO_COMP GetCompFunc()
{
    return how_to_comp;
}
////////////////////////////////////////
status_t AddEntry(T *p)
{
    CWeakPointer<T> *wp;
    NEW(wp,CWeakPointer<T>);
    wp->Init();
    wp->WeakRef(p);
    return this->InsOrderedPtr(wp,0,1);
}

T *Find(T *key)
{
    ASSERT(key);
    CWeakPointer<T> wp;
    wp.Init();
    wp.WeakRef(key);
    CWeakPointer<T> *r =  this->BSearchNode(&wp,0);
    if(!r)return NULL;
    return r->get();
}

status_t AutoClear()
{
    for(int i = 0; i < GetLen(); i++)
    {
        if(!m_Index[i]->get())
        {
            this->DelElem(i);
            i--;
        }
    }
    return OK;
}


};

#endif
