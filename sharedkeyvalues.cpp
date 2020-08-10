#include "sharedkeyvalues.h"
#include "syslog.h"
#include "mem_tool.h"

#define BUDDY_UNIT 8
#define CHECK_KEY_RANGE(key)  ASSERT(key >=0 && key < (*m_MaxKeys))
#define TO_POINTER(o) (m_Data + o)
#define LOCK_OFFSET 8
#define WARNNING_DATA_SIZE (64*1024)

#define SEM_SIZE sizeof(sem_t)

CSharedKeyValues::CSharedKeyValues()
{
    this->InitBasic();
}
CSharedKeyValues::~CSharedKeyValues()
{
    this->Destroy();
}
status_t CSharedKeyValues::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    m_Flags = 0;
    m_Keys = NULL;
    m_MaxKeys = NULL;
    m_Data = NULL;
    m_DataSize = NULL;
    m_StartAddr = NULL;
    m_Allocator.InitBasic();
    m_Lock.InitBasic();
    return OK;
}
status_t CSharedKeyValues::Init(void *data, int data_size,int max_keys, int alloctor_level)
{
    this->InitBasic();
    WEAK_REF_ID_INIT();

    ASSERT(sizeof(UserKey) == 8);

    int need_size = sizeof(int32_t)*2 + max_keys*sizeof(UserKey)
        + m_Allocator.ShmSize(alloctor_level)
        + (1<<alloctor_level)*BUDDY_UNIT;

    if(need_size > data_size)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "not enough init shared data size, need %d bytes more",need_size-data_size
        );
        return ERROR;
    }

    int8_t *p = (int8_t*)data;
    int offset = 0;

    m_StartAddr = (uint8_t*)data;

    m_MaxKeys = (int32_t*)(p + offset);
    *m_MaxKeys = max_keys;

    offset += sizeof(int32_t);
    m_DataSize = (int32_t*)(p + offset);

    offset += sizeof(int32_t);
    ASSERT(LOCK_OFFSET == offset);
    m_Lock.Attach(p+offset,SEM_SIZE);
    offset += SEM_SIZE;

    m_Keys = (UserKey *)(p+offset);
    int key_size = max_keys*sizeof(UserKey);
    offset += key_size;

    memset(m_Keys,0,key_size);

    m_Allocator.Init(alloctor_level,p+offset);
    int allocator_size = m_Allocator.ShmSize();
    offset += allocator_size;

    *m_DataSize = data_size - offset;
    int left_size = (*m_DataSize) - (1<<alloctor_level)*BUDDY_UNIT;
    ASSERT(left_size >= 0);

    m_Data = (uint8_t*)(p+offset);
    return OK;
}
status_t CSharedKeyValues::Destroy()
{
    m_Lock.Destroy();
    m_Allocator.Destroy();
    this->InitBasic();
    return OK;
}

status_t CSharedKeyValues::Attach(void *data, int data_size)
{
    WEAK_REF_ID_INIT();

    int8_t *p = (int8_t*)data;
    int offset = 0;

    ASSERT(sizeof(UserKey) == 8);

    m_StartAddr = (uint8_t*)data;

    m_MaxKeys = (int32_t*)(p + offset);

    offset += sizeof(int32_t);
    m_DataSize = (int32_t*)(p + offset);

    offset += sizeof(int32_t);
    ASSERT(LOCK_OFFSET == offset);
    m_Lock.Attach(p+offset,SEM_SIZE);

    offset += SEM_SIZE;
    m_Keys = (UserKey *)(p+offset);

    int key_size = (*m_MaxKeys)*sizeof(UserKey);
    offset += key_size;

    m_Allocator.Attach(p+offset);

    int allocator_size = m_Allocator.ShmSize();
    offset += allocator_size;

    m_Data = (uint8_t*)(p+offset);

    return OK;
}

int CSharedKeyValues::Alloc(int size)
{
    if(size > WARNNING_DATA_SIZE)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_WARNING,
            "Warning: SharedKeyValues alloc a large block with size %d bytes",size
        );
    }

    int s = size/BUDDY_UNIT;
    int m = size%BUDDY_UNIT;
    if(m > 0) s++;
    if(s < 1) s = 1;
    return m_Allocator.Alloc(s) * BUDDY_UNIT;
}

status_t CSharedKeyValues::Free(int offset)
{
    int o = offset / BUDDY_UNIT;
    m_Allocator.Free(o);
    return OK;
}

UserKey* CSharedKeyValues::GetUserKey(int key)
{
    CHECK_KEY_RANGE(key);
    return &m_Keys[key];
}

status_t CSharedKeyValues::Delete(int key)
{
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    if(uk->type == USER_TYPE_UNKNOWN)
        return OK;
    this->Free(uk->offset);
    uk->type = USER_TYPE_UNKNOWN;
    return OK;
}
status_t CSharedKeyValues::GetBool(int key,uint8_t *value)
{
    ASSERT(value);
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    ASSERT(uk->type == USER_TYPE_BOOL);
    uint8_t *p = TO_POINTER(uk->offset);
    memcpy(value,p,sizeof(uint8_t));
    return OK;
}
status_t CSharedKeyValues::GetBool(int key,bool *value)
{
    ASSERT(value);
    uint8_t t;
    status_t ret = this->GetBool(key,&t);
    *value = (t!=0);
    return ret;
}
status_t CSharedKeyValues::GetInt(int key, int *value)
{
    ASSERT(value);
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    ASSERT(uk->type == USER_TYPE_INT);
    uint8_t *p = TO_POINTER(uk->offset);
    memcpy(value,p,sizeof(int32_t));
    return OK;
}

status_t CSharedKeyValues::GetFloat(int key,float *value)
{
    ASSERT(value);
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    ASSERT(uk->type == USER_TYPE_FLOAT);
    uint8_t *p = TO_POINTER(uk->offset);
    memcpy(value,p,sizeof(float));
    return OK;
}

status_t CSharedKeyValues::GetString(int key, const char **value)
{
    ASSERT(value);
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    ASSERT(uk->type == USER_TYPE_STRING);
    *value = (char*)TO_POINTER(uk->offset);
    return OK;
}
status_t CSharedKeyValues::GetBson(int key,CMiniBson *bson)
{
    ASSERT(bson);
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    ASSERT(uk->type == USER_TYPE_BSON);
    uint32_t *data = (uint32_t*)TO_POINTER(uk->offset);
    ASSERT(data);
    uint32_t size = data[0];
    ASSERT(size > 0);

    CMem tmp;
    tmp.Init();
    tmp.SetRawBuf(data,size,true);

    bson->Destroy();
    bson->Init();
    bson->LoadRawBuf(&tmp);
    return OK;
}

status_t CSharedKeyValues::GetBinary(int key,CMem *bin)
{
    ASSERT(bin);
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    ASSERT(uk->type == USER_TYPE_BINARY);
    uint32_t *data = (uint32_t*)TO_POINTER(uk->offset);
    ASSERT(data);
    uint32_t size = data[0];
    
    bin->Destroy();
    bin->Init();

    if(size > 0)
    {
        bin->SetRawBuf(data+1,size,false);
    }
    return OK;
}

void *CSharedKeyValues::GetRawData(int key)
{
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    return TO_POINTER(uk->offset);
}

//data can be NULL
status_t CSharedKeyValues::Write(int key, uint16_t type, const void *data, uint16_t size)
{
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);

    this->Lock();

    int o = uk->offset;
    bool need_alloc = false;

    if(uk->type == USER_TYPE_UNKNOWN)
    {
        need_alloc = true;
    }
    else if(GetDataSize(key) != size)
    {
        this->Free(uk->offset);
        need_alloc = true;
    }

    if(need_alloc)
    {
        o = this->Alloc(size);
        if(o < 0)
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "buddy memory alloc fail,size=%d",size
            );
            this->Unlock();
            return ERROR;
        }
    }

    void *p = TO_POINTER(o);

    if(data)
        memcpy(p,data,size);
    else
        memset(p,0,size);

    uk->type = type;
    uk->offset = o;

    this->Unlock();
    return OK;
}

int CSharedKeyValues::GetDataSize(int key)
{
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);

    switch(uk->type)
    {
        case USER_TYPE_BOOL:
        {
            return sizeof(uint8_t);
        }

        case USER_TYPE_INT:
        {
            return sizeof(int);
        }

        case USER_TYPE_FLOAT:
        {
            return sizeof(float);
        }

        case USER_TYPE_STRING:
        {
            const char *str;
            this->GetString(key,&str);
            ASSERT(str);
            return strlen((const char*)str) + 1;
        }

        case USER_TYPE_BSON:
        {
            CMiniBson bson;
            bson.Init();
            this->GetBson(key,&bson);
            return bson.GetDocumentSize();
        }

        case USER_TYPE_BINARY:
        {
            CMem bin;
            this->GetBinary(key,&bin);
            return bin.GetSize();
        }
    }

    return 0;
}
status_t CSharedKeyValues::PutBool(int key, uint8_t value)
{
    return this->Write(key,USER_TYPE_BOOL,&value,sizeof(value));
}
status_t CSharedKeyValues::PutInt(int key, int value)
{
    return this->Write(key,USER_TYPE_INT,&value,sizeof(value));
}
status_t CSharedKeyValues::PutFloat(int key, float value)
{
    return this->Write(key,USER_TYPE_FLOAT,&value,sizeof(value));
}
status_t CSharedKeyValues::PutString(int key, const char *value)
{
    return this->Write(key,USER_TYPE_STRING,value,strlen(value)+1);
}

status_t CSharedKeyValues::PutBson(int key, CMiniBson *bson)
{
    ASSERT(bson);

    CMem *data = bson->GetRawData();
    ASSERT(data);

    if(bson->GetDocumentSize() != data->GetSize())
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "wrong bson format,PutBson failed"
        );
        return ERROR;
    }
    return this->Write(key,USER_TYPE_BSON,data->GetRawBuf(),data->GetSize());
}

status_t CSharedKeyValues::PutBinary(int key, uint32_t size)
{
    status_t ret = this->Write(key,USER_TYPE_BINARY,NULL,size+sizeof(uint32_t));
    uint32_t *p = (uint32_t*)this->GetRawData(key);
    ASSERT(p);
    p[0] = size; //write size
    return ret;
}

status_t CSharedKeyValues::Print(CFileBase *_buf)
{
    for(int i=0;i < this->GetMaxKeys(); i++)
    {
        UserKey *uk = this->GetUserKey(i);
        ASSERT(uk);

        if(uk->type != USER_TYPE_UNKNOWN)
            _buf->Printf("[%d]=",i);

        if(uk->type == USER_TYPE_BOOL)
        {
            uint8_t b;
            GetBool(i,&b);
            _buf->Log("%s",b?"true":"false");
        }
        else if(uk->type == USER_TYPE_INT)
        {
            int v;
            GetInt(i,&v);
            _buf->Log("%d",v);
        }
        else if(uk->type == USER_TYPE_FLOAT)
        {
            float v;
            GetFloat(i,&v);
            _buf->Log("%f",v);
        }

        else if(uk->type == USER_TYPE_STRING)
        {
            const char *str;
            GetString(i,&str);
            _buf->Log("%s",str);
        }

        else if(uk->type == USER_TYPE_BSON)
        {
            CMiniBson bson;
            bson.Init();
            GetBson(i,&bson);
            _buf->Log("{");
            bson.Print(_buf);
            _buf->Log("}");
        }
        else if(uk->type == USER_TYPE_BINARY)
        {
            CMem bin;
            this->GetBinary(i,&bin);
            _buf->Log("<binary %d bytes>",bin.GetSize());
        }
    }
    return OK;
}

int CSharedKeyValues::GetMaxKeys()
{
    return *m_MaxKeys;
}

status_t CSharedKeyValues::GetDataSize()
{
    return (1<<m_Allocator.GetLevel())*BUDDY_UNIT;
}

int CSharedKeyValues::Type(int key)
{
    UserKey *uk = this->GetUserKey(key);
    ASSERT(uk);
    return uk->type;
}

status_t CSharedKeyValues::InitLock()
{
    int8_t *p = (int8_t*)m_StartAddr;
    if(CProcessLock::InitLock(p+LOCK_OFFSET,SEM_SIZE))
    {
        this->SetIsLockInitiated(true);
        return OK;
    }
    return ERROR;
}

status_t CSharedKeyValues::DestroyLock()
{
    int8_t *p = (int8_t*)m_StartAddr;
    this->SetIsLockInitiated(false);
    return CProcessLock::DestroyLock(p+LOCK_OFFSET);
}

status_t CSharedKeyValues::Lock()
{
    if(IsLockInitiated())
    {
        return m_Lock.Lock();
    }
    return OK;
}

status_t CSharedKeyValues::Unlock()
{
    if(IsLockInitiated())
    {
        return m_Lock.UnLock();
    }
    return OK;  
}
