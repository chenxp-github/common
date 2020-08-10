#ifndef __S2_BUDDYALLOCATOR_H
#define __S2_BUDDYALLOCATOR_H

#include "cruntime.h"
#include "filebase.h"

class CBuddyAllocator{
public:
    int32_t *m_Level;
    uint8_t *m_Tree;
public:
    int ShmSize(int level);
    int ShmSize();
    int GetLevel();
    status_t Attach(void *data);
    void MarkParent(int index);
    void Dump();
    void Dump( int index , int level);
    int Size(int offset);
    void Free(int offset);
    void Combine(int index);
    int Alloc(int s);
    CBuddyAllocator();
    ~CBuddyAllocator();
    status_t InitBasic();
    status_t Init(int level,void *data);
    status_t Destroy();
};

#endif

