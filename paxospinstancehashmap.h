#ifndef __PAXOSPINSTANCEHASHMAP_H
#define __PAXOSPINSTANCEHASHMAP_H

#include "cruntime.h"
#include "paxospinstance.h"

class CPaxosPInstanceHashMap{
public:
    CPaxosPInstance **mData;
    int mCapacity;
    int mSize;
public: 
    CPaxosPInstance * Remove(int id);
    CPaxosPInstance * Remove(CPaxosPInstance *key);
    status_t Del(int id);
    CPaxosPInstance* FindSmallestId();
    CPaxosPInstance* Get(int id);
    bool IsEmpty();
    int GetSize();
    status_t Del(CPaxosPInstance *key);
    status_t EnumAll(CALLBACK_FUNC func, void **params);        
    CPaxosPInstance * Get(CPaxosPInstance *key);    
    status_t Put(CPaxosPInstance *paxospinstance);
    status_t PutPtr(CPaxosPInstance *ptr);
    int HashCode(CPaxosPInstance *paxospinstance);
    static bool Equals(CPaxosPInstance *paxospinstance1, CPaxosPInstance *paxospinstance2); 
    static CPaxosPInstance * CloneNode(CPaxosPInstance *paxospinstance);
    status_t DelNode(CPaxosPInstance *paxospinstance);
    CPaxosPInstanceHashMap();
    virtual ~CPaxosPInstanceHashMap();
    status_t InitBasic();
    status_t Init(int capacity);
    status_t Destroy();
    status_t Print();
};

#endif

