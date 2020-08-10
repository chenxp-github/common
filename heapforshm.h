#ifndef __S_HEAPFORSHM_H
#define __S_HEAPFORSHM_H

#include "cruntime.h"
#include "filebase.h"
#include "crt_win32.h"  // Added by ClassView

union header
{
    struct
    {
        union header *ptr;
        uint32_t size;
    }s;
    uint32_t x; 
};

typedef union header Header;

class CHeapForShm{
public:
    uint8_t *mHeapStart;
    uint8_t *mHeapEnd;
    Header mBase;
    Header *mFreeP;
    bool mHasRun;
public:
    int_ptr_t ToVirtualPtr(void* rp);
    void* ToRealPtr(int_ptr_t vp);
    int_ptr_t Malloc(uint32_t nbytes);
    Header* MoreCore(uint32_t nu);
    void Free(int_ptr_t ap);
    CHeapForShm();
    virtual ~CHeapForShm();
    status_t InitBasic();
    status_t Init(void *heap_start,void *heap_end);
    status_t Destroy();
};

#endif
