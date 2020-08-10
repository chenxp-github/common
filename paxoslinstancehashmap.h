#ifndef __PAXOSLINSTANCEHASHMAP_H
#define __PAXOSLINSTANCEHASHMAP_H

#include "cruntime.h"
#include "paxoslinstance.h"

class CPaxosLInstanceHashMap{
public:
    CPaxosLInstance **mData;
    int mCapacity;
    int mSize;
public: 
    bool IsEmpty();
    int GetSize();
    status_t Del(CPaxosLInstance *key);
    status_t EnumAll(CALLBACK_FUNC func, void **params);        
    CPaxosLInstance * Get(CPaxosLInstance *key);    
    CPaxosLInstance* Get(int id);
    status_t Put(CPaxosLInstance *paxoslinstance);
    status_t PutPtr(CPaxosLInstance *ptr);
    int HashCode(CPaxosLInstance *paxoslinstance);
    static bool Equals(CPaxosLInstance *paxoslinstance1, CPaxosLInstance *paxoslinstance2); 
    static CPaxosLInstance * CloneNode(CPaxosLInstance *paxoslinstance);
    status_t DelNode(CPaxosLInstance *paxoslinstance);
    CPaxosLInstanceHashMap();
    virtual ~CPaxosLInstanceHashMap();
    status_t InitBasic();
    status_t Init(int capacity);
    status_t Destroy();
    status_t Print();
};

#endif

