#include "main_loop_context_manager.h"
#include "syslog.h"
#include "mem_tool.h"
#include "misc.h"


MainLoopContextManager_HashEntry::MainLoopContextManager_HashEntry()
{

    m_RawPtr = NULL;
    next = NULL;
}

MainLoopContextManager_HashEntry::~MainLoopContextManager_HashEntry()
{

    DEL(m_RawPtr);
    next = NULL;
}
    
CMainLoopContext* MainLoopContextManager_HashEntry::get()
{
    return m_RawPtr;
}

status_t MainLoopContextManager_HashEntry::set(CMainLoopContext *node)
{
    ASSERT(m_RawPtr == NULL);
    m_RawPtr = node;
    return OK;
}

/*********************************************/

CMainLoopContextManager::CMainLoopContextManager()
{
    this->InitBasic();
}
CMainLoopContextManager::~CMainLoopContextManager()
{
    this->Destroy();
}
status_t CMainLoopContextManager::InitBasic()
{

    this->m_Capacity = 0;
    this->m_Data = 0;
    this->m_Size = 0;
    return OK;
}
status_t CMainLoopContextManager::Init(int capacity)
{
    this->InitBasic();

    this->m_Capacity = capacity;
    MALLOC(this->m_Data,MainLoopContextManager_HashEntry*,capacity);
    memset(this->m_Data,0,capacity*sizeof(MainLoopContextManager_HashEntry*));
    return OK;
}
status_t CMainLoopContextManager::Destroy()
{
    int i;
    MainLoopContextManager_HashEntry *q,*p;


    if(this->m_Data == NULL)
        return OK;

    for(i = 0; i < this->m_Capacity; i++)
    {
        p  = this->m_Data[i];
        while(p)
        {
            q = p->next;
            DEL(p);
            p = q;            
        }            
    }
    FREE(this->m_Data);
    this->InitBasic();
    return OK;
}

status_t CMainLoopContextManager::PutPtr(CMainLoopContext *ptr)
{
    int code;
    MainLoopContextManager_HashEntry *p;

    ASSERT(ptr);

    if(this->Get(ptr) != NULL)
        return ERROR;

    code = this->HashCode(ptr);
    ASSERT(code >= 0 && code < this->m_Capacity);
    p = this->m_Data[code];

    MainLoopContextManager_HashEntry *ptr_entry;
    NEW(ptr_entry,MainLoopContextManager_HashEntry);
    ptr_entry->set(ptr);

    if (p == NULL)
    {
        this->m_Data[code] = ptr_entry;
        ptr_entry->next = NULL;
    }
    else
    {
        while(p->next)
        {
            p = p->next;
        }
        p->next = ptr_entry;
        ptr_entry->next = NULL;
    }
    this->m_Size++;
    return OK;
}

CMainLoopContext* CMainLoopContextManager::Get(CMainLoopContext *key)
{
    int code;
    MainLoopContextManager_HashEntry *p;

    ASSERT(key);

    code = this->HashCode(key);
    ASSERT(code >= 0 && code < this->m_Capacity);
    p = this->m_Data[code];
    while(p)
    {
        if(this->Equals(p->get(),key))
            return p->get();
        p = p->next;
    }
    return NULL;
}

status_t CMainLoopContextManager::EnumAll(CClosure *closure)
{
    int i,_contine;
    MainLoopContextManager_HashEntry *pre,*p,*next;
    
    ASSERT(closure);
    
    for(i = 0; i < this->m_Capacity; i++)
    {
        p = this->m_Data[i];
        if(p == NULL)continue;
        
        pre = p;
        p = p->next;
        while(p)
        {            
            next = p->next;
            closure->SetParamPointer(0,p->get());
            closure->SetParamInt(1,p==m_Data[i]); //is first
            _contine = closure->Run();
            //mark param 0 to NULL mean to delete the entry
            if(closure->GetParamPointer(0) == NULL) 
            {
                p->m_RawPtr = NULL;
                DEL(p);
                pre->next = next;
                p = next;
                this->m_Size --;
            }
            else
            {
                pre = p;
                p = next;
            }
            
            if(!_contine)
                goto end;
        }
        
        p = this->m_Data[i];
        next = p->next;
        closure->SetParamPointer(0,p->get());
        closure->SetParamInt(1,p==m_Data[i]);
        _contine = closure->Run();
        if(closure->GetParamPointer(0) == NULL)
        {
            p->m_RawPtr = NULL;
            DEL(p);
            this->m_Data[i] = next;
            this->m_Size --;
        }
        if(!_contine)
            goto end;
    }
    
end:
    return OK;
}

MainLoopContextManager_HashEntry* CMainLoopContextManager::Remove(CMainLoopContext *key)
{
    int code;
    MainLoopContextManager_HashEntry *pre,*p;

    ASSERT(key);
    code = this->HashCode(key);
    if(code < 0 || code >= this->m_Capacity)
        return NULL;
    p = this->m_Data[code];
    if(p == NULL) return NULL;

    if(this->Equals(p->get(),key))
    {
        this->m_Data[code] = p->next;
        this->m_Size --;
        return p;
    }
    else
    {
        while(p)
        {
            pre = p;
            p = p->next;
            if(p && this->Equals(p->get(),key))
            {
                pre->next = p->next;
                this->m_Size --;
                return p;
            }
        }
    }

    return NULL;
}
status_t CMainLoopContextManager::Del(CMainLoopContext *key)
{
    MainLoopContextManager_HashEntry *p = this->Remove(key);
    if(p != NULL)
    {
        DEL(p);
        return OK;
    }
    return ERROR;
}
int CMainLoopContextManager::GetSize()
{
    return this->m_Size;
}
int CMainLoopContextManager::GetCapacity()
{
    return this->m_Capacity;
}
bool CMainLoopContextManager::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CMainLoopContextManager::DiscardAll()
{
    m_Data = NULL;
    return OK;
}
int CMainLoopContextManager::HashCode(CMainLoopContext *hashentry)
{
    return HashCode(hashentry,m_Capacity);
}

status_t CMainLoopContextManager::Clear()
{
    int capacity = m_Capacity;
    this->Destroy();
    this->Init(capacity);
    return OK;
}

status_t CMainLoopContextManager::ToArray(CMainLoopContext *arr[], int *len)
{    
    ASSERT(arr && len);   
    int max_len = *len;    
    *len = 0;    
       
    BEGIN_CLOSURE(on_enum)        
    {        
        CLOSURE_PARAM_PTR(CMainLoopContext*,node,0);        
        CLOSURE_PARAM_PTR(CMainLoopContextManager*,self,10);        
        CLOSURE_PARAM_PTR(CMainLoopContext**,arr,11);        
        CLOSURE_PARAM_PTR(int*,len,12);        
        CLOSURE_PARAM_INT(max_len,13);        
        ASSERT(*len < max_len);        
        arr[(*len)++] = node;           
        return OK;        
    }    
    END_CLOSURE(on_enum);
            
    on_enum.SetParamPointer(10,this);    
    on_enum.SetParamPointer(11,arr);    
    on_enum.SetParamPointer(12,len);    
    on_enum.SetParamInt(13,max_len);        
    this->EnumAll(&on_enum);    
    return OK;
}
/*********************************************/
/*********************************************/
int CMainLoopContextManager::HashCode(CMainLoopContext *hashentry,int capacity)
{
    ASSERT(hashentry);
    return bob_32bit_integer_hash((int_ptr_t)hashentry->m_Env) % capacity;
}

bool CMainLoopContextManager::Equals(CMainLoopContext *hashentry1, CMainLoopContext *hashentry2)
{
    ASSERT(hashentry1 && hashentry2);
    return hashentry1->m_Env == hashentry2->m_Env;
}

status_t CMainLoopContextManager::Del(JNIEnv *env)
{
    ASSERT(env);

    CMainLoopContext tmp;
    tmp.Init();
    tmp.SetJniEnv(env);
    return this->Del(&tmp);
}

CMainLoopContext *CMainLoopContextManager::Get(JNIEnv *env)
{
    if(env == NULL) return NULL;    
    CMainLoopContext tmp;
    tmp.Init();
    tmp.SetJniEnv(env);
    return this->Get(&tmp);
}

