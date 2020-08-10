#ifndef __S2_SHAREDKEYVALUES_H
#define __S2_SHAREDKEYVALUES_H

#include "cruntime.h"
#include "filebase.h"
#include "buddyallocator.h"
#include "processlock.h"
#include "minibson.h"

#define USER_TYPE_UNKNOWN   0
#define USER_TYPE_BOOL      1
#define USER_TYPE_INT       2
#define USER_TYPE_STRING    3
#define USER_TYPE_FLOAT     4
#define USER_TYPE_BSON      5
#define USER_TYPE_BINARY    6

#define SKV_FLAG_FUNC(func,bit) FLAG_FUNC(this->m_Flags,func,bit)

typedef struct{
    uint32_t type;
    uint32_t offset;
}UserKey;

/**share memory layout**/
// Max Keys  ---> 4 bytes
// Data Size ---> 4 bytes
// Lock  --->sizeof(sem_t)
// Keys  ---> MaxKeys * sizeof(UserKey)
// buddy allocator ---> ShmSize();
// User Data  ---> To The End
/////////////////////////
class CSharedKeyValues{
public:
    WEAK_REF_ID_DEFINE();
public:
    int32_t *m_MaxKeys;
    int32_t *m_DataSize;
    UserKey *m_Keys;
    CBuddyAllocator m_Allocator;
    CProcessLock m_Lock;  //only used for attach
    uint8_t *m_Data;
    uint8_t *m_StartAddr;
    uint32_t m_Flags;
public:
    status_t Unlock();
    status_t Lock();
    status_t DestroyLock();
    status_t InitLock();
    int Type(int key);
    status_t GetDataSize();
    int GetMaxKeys();
    status_t Print(CFileBase *_buf);
    status_t PutBool(int key, uint8_t value);
    status_t PutInt(int key, int value);
    status_t PutFloat(int key, float value);
    status_t PutString(int key, const char *value);
    status_t PutBson(int key, CMiniBson *bson);
    status_t PutBinary(int key, uint32_t size);
    int GetDataSize(int key);
    status_t GetBool(int key,uint8_t *value);
    status_t GetBool(int key,bool *value);
    status_t GetInt(int key, int *value);
    status_t GetFloat(int key,float *value);
    status_t GetString(int key, const char **value);
    status_t GetBson(int key,CMiniBson *bson);
    status_t GetBinary(int key,CMem *bin);
    void *GetRawData(int key);
    status_t Write(int key, uint16_t type, const void *data, uint16_t size);
    status_t Delete(int key);
    UserKey* GetUserKey(int key);
    status_t Free(int offset);
    int Alloc(int size);
    status_t Attach(void *data, int data_size);
    CSharedKeyValues();
    ~CSharedKeyValues();
    status_t InitBasic();
    status_t Init(void *data, int data_size,int max_keys, int alloctor_level);
    status_t Destroy();

    SKV_FLAG_FUNC(IsLockInitiated,0x00000001);
};

#endif
