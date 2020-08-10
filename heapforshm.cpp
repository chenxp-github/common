#include "heapforshm.h"

CHeapForShm::CHeapForShm()
{
    this->InitBasic();
}
CHeapForShm::~CHeapForShm()
{
    this->Destroy();
}
status_t CHeapForShm::InitBasic()
{
    this->mHeapEnd = NULL;
    this->mHeapStart = NULL;
    this->mFreeP = NULL;
    this->mBase.x = 0;
    this->mHasRun = false;
    return OK;
}
status_t CHeapForShm::Init(void *heap_start,void *heap_end)
{
    this->Destroy();

    this->mHeapStart = (uint8_t*)heap_start;
    this->mHeapEnd = (uint8_t*)heap_end;

    return OK;
}
status_t CHeapForShm::Destroy()
{
    this->InitBasic();
    return OK;
}

void CHeapForShm::Free(int_ptr_t ap)
{
    Header *bp, *p;

    bp = (Header*)this->ToRealPtr(ap)-1;
    for(p=this->mFreeP;!(bp > p && bp < p->s.ptr); p=p->s.ptr)
    {
        if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;
    }
    if(bp + bp->s.size == p->s.ptr)
    {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
    {
        bp->s.ptr = p->s.ptr;
    }
    
    if(p + p->s.size == bp)
    {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;   
    }
    else
    {
        p->s.ptr = bp;
    }
    this->mFreeP = p;
}

Header * CHeapForShm::MoreCore(uint32_t nu)
{
    char *cp;
    Header *up;

    if(this->mHasRun)
        return NULL;
    this->mHasRun = true;
        
    nu = (this->mHeapEnd - this->mHeapStart)/sizeof(Header) - 1;

    cp = (char *)this->mHeapStart;
    up = (Header *)cp;
    up->s.size = nu;
    
    this->Free(this->ToVirtualPtr(up+1));
    
    return this->mFreeP;
}

int_ptr_t CHeapForShm::Malloc(uint32_t nbytes)
{
    Header *p, *prevp;
    uint32_t nunits;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;

    prevp = this->mFreeP;
    if(prevp == NULL)
    {
        this->mBase.s.ptr = this->mFreeP = prevp = &this->mBase;
        this->mBase.s.size=0;
    }
    for(p=prevp->s.ptr ; ;prevp = p,p=p->s.ptr)
    {
        if( p->s.size >= nunits)
        {
            if(p->s.size == nunits)
            {
                prevp->s.ptr = p->s.ptr;
            }
            else
            {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits; 
            }
            this->mFreeP = prevp;
            return this->ToVirtualPtr(p+1);
        }
        if(p == this->mFreeP)
        {
            p = this->MoreCore(nunits);
            if(p==NULL)
                return NULL;
        }
    }
}

void* CHeapForShm::ToRealPtr(int_ptr_t vp)
{
    return (void*)(this->mHeapStart + vp);
}

int_ptr_t CHeapForShm::ToVirtualPtr(void* rp)
{
    return (int_ptr_t)((uint8_t*)rp - this->mHeapStart);
}

