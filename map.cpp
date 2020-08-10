#include "map.h"

#define MAP_ENTITY_TYPE_ERR     0
#define MAP_ENTITY_TYPE_INT     1
#define MAP_ENTITY_TYPE_FLOAT   2
#define MAP_ENTITY_TYPE_DOUBLE  3
#define MAP_ENTITY_TYPE_STRING  4
#define MAP_ENTITY_TYPE_BINARY  5
#define MAP_ENTITY_TYPE_BOOL    6
#define MAP_ENTITY_TYPE_USER    7

CMap::CMap()
{
    this->InitBasic();
}
CMap::~CMap()
{
    this->Destroy();
}
status_t CMap::InitBasic()
{
    this->__weak_ref_id = 0;    
    this->mKeyValues = NULL;
    return OK;
}
status_t CMap::Init()
{
    this->InitBasic();
    this->__weak_ref_id = crt_get_unique_id();
    
    NEW(this->mKeyValues,CMemStk);
    this->mKeyValues->Init(64);

    return OK;
}
status_t CMap::Destroy()
{
    if(this->mKeyValues)
    {
        this->mKeyValues->RunDelFunc();
        DEL(this->mKeyValues);
    }   
    this->InitBasic();
    return OK;
}
status_t CMap::Copy(CMap *p)
{
    ASSERT(p && this != p);

    this->mKeyValues->Clear();
    for(int i = 0; i < p->mKeyValues->GetLen(); i++)
    {
        int32_t size,type;
        void *value;
        
        CMem *pmem = p->mKeyValues->GetElem(i);
        ASSERT(pmem);

        p->ReadValueRaw(pmem,&value,&size,&type);
        CMem tmp;
        LOCAL_MEM(tmp);
        p->SetKeyValue(&tmp,pmem->p,value,size,type);
        this->mKeyValues->Push(&tmp);
    }

    return OK;
}
status_t CMap::Comp(CMap *p)
{
    ASSERT(p);
    return 0;
}
status_t CMap::Print()
{
    for(int i = 0; i < this->mKeyValues->GetLen(); i++)
    {
        int32_t size,type;
        void *value;
        CMem *pmem = this->mKeyValues->GetElem(i);
        ASSERT(pmem);
        this->ReadValueRaw(pmem,&value,&size,&type);
        
        if(type == MAP_ENTITY_TYPE_INT || type == MAP_ENTITY_TYPE_BOOL)
        {
            int t;
            memcpy(&t,value,sizeof(t));
            LOG("%s=%d",pmem->p,t);
        }
        else if(type == MAP_ENTITY_TYPE_FLOAT)
        {
            float t;
            memcpy(&t,value,sizeof(t));
            LOG("%s=%f",pmem->p,t);
        }
        else if(type == MAP_ENTITY_TYPE_DOUBLE)
        {
            double t;
            memcpy(&t,value,sizeof(t));
            LOG("%s=%f",pmem->p,t);
        }
        else if(type == MAP_ENTITY_TYPE_STRING)
        {
            LOG("%s=\"%s\"",pmem->p,(char*)value);
        }
        else
        {
            LOG("%s=<...>",pmem->p);
        }
    }

    return TRUE;
}

status_t CMap::SetKeyValue(CMem *out, const char *key, void *value, int32_t size, int32_t type)
{
    ASSERT(out && key && value);

    out->RunDelFunc();
    out->StrCpy(key);
    out->UseFreeFunc();
    char *p;
    MALLOC(p,char,size+sizeof(int32_t)*2);
    out->user_data = p;

    memcpy(p,&size,sizeof(size));
    memcpy(p+sizeof(int32_t),&type,sizeof(type));
    memcpy(p+sizeof(int32_t)*2,value,size);

    return OK;
}

status_t CMap::WriteKeyValue(const char *key, void *value, int32_t size, int32_t type)
{
    ASSERT(key && value);

    CMem entity;
    LOCAL_MEM(entity);
    
    this->SetKeyValue(&entity,key,value,size,type);

    CMem *pmem = this->SearchKey(key);
    if(pmem == NULL)
    {
        this->mKeyValues->InsOrdered(&entity,0,1);
    }
    else
    {
        pmem->RunDelFunc();
        pmem->Copy(&entity);
    }

    return OK;
}

CMem * CMap::SearchKey(const char *key)
{
    ASSERT(key);
    CMem mem;
    mem.Init();
    mem.SetP((char*)key);
    return this->mKeyValues->BSearch_Node(&mem,0);
}

status_t CMap::ReadValueRaw(CMem *mem, void **value, int32_t *size, int32_t *type)
{
    ASSERT(mem && value && size && type);
    char *p = (char*)mem->user_data;
    ASSERT(p);
    memcpy(size,p,sizeof(int32_t));
    memcpy(type,p+sizeof(int32_t),sizeof(int32_t));
    *value = p + sizeof(int32_t)*2;
    return OK;
}

status_t CMap::ReadKeyValueRaw(const char *key, void **value, int32_t *size, int32_t *type)
{
    ASSERT(key && value && size && type);
    CMem *pmem = this->SearchKey(key);
    if(pmem == NULL)return ERROR;
    return this->ReadValueRaw(pmem,value,size,type);
}

int CMap::ReadInt(const char *key, int defValue)
{
    void *p = NULL;
    int32_t size = 0;
    int32_t type;

    if(!this->ReadKeyValueRaw(key,&p,&size,&type))
        return defValue;
    ASSERT(type == MAP_ENTITY_TYPE_INT);

    int i;
    memcpy(&i,p,sizeof(i));
    return i;
}

status_t CMap::WriteInt(const char *key, int i)
{
    return this->WriteKeyValue(key,&i,sizeof(i),MAP_ENTITY_TYPE_INT);
}

double CMap::ReadDouble(const char *key, double defValue)
{
    void *p = NULL;
    int32_t size = 0;
    int32_t type;

    if(!this->ReadKeyValueRaw(key,&p,&size,&type))
        return defValue;
    ASSERT(type == MAP_ENTITY_TYPE_DOUBLE);

    double d;
    memcpy(&d,p,sizeof(d));
    return d;
}

status_t CMap::WriteDouble(const char *key, double d)
{
    return this->WriteKeyValue(key,&d,sizeof(d),MAP_ENTITY_TYPE_DOUBLE);
}

float CMap::ReadFloat(const char *key, float defValue)
{
    void *p = NULL;
    int32_t size = 0;
    int32_t type;

    if(!this->ReadKeyValueRaw(key,&p,&size,&type))
        return defValue;
    ASSERT(type == MAP_ENTITY_TYPE_FLOAT);

    float d;
    memcpy(&d,p,sizeof(d));
    return d;
}

status_t CMap::WriteFloat(const char *key, float d)
{
    return this->WriteKeyValue(key,&d,sizeof(d),MAP_ENTITY_TYPE_FLOAT);
}

bool CMap::ReadBool(const char *key, bool defValue)
{
    void *p = NULL;
    int32_t size = 0;
    int32_t type;

    if(!this->ReadKeyValueRaw(key,&p,&size,&type))
        return defValue;
    ASSERT(type == MAP_ENTITY_TYPE_BOOL);

    bool d;
    memcpy(&d,p,sizeof(d));
    return d;
}

status_t CMap::WriteBool(const char *key, bool d)
{
    return this->WriteKeyValue(key,&d,sizeof(d),MAP_ENTITY_TYPE_BOOL);
}

status_t CMap::ReadString(const char *key, CMem *out, const char *defValue)
{
    ASSERT(defValue);

    void *p = NULL;
    int32_t size = 0;
    int32_t type;

    if(!this->ReadKeyValueRaw(key,&p,&size,&type))
    {
        out->StrCpy(defValue);
        return ERROR;
    }

    ASSERT(type == MAP_ENTITY_TYPE_STRING);

    CMem tmp;
    tmp.Init();
    tmp.SetP((char*)p,size);

    out->Copy(&tmp);
    return OK;
}

status_t CMap::WriteString(const char *key, const char *str)
{
    return this->WriteKeyValue(key,(void*)str,strlen(str)+1,MAP_ENTITY_TYPE_STRING);
}

status_t CMap::Clear()
{
    return this->mKeyValues->Clear();
}
