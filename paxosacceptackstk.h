#ifndef __PAXOSACCEPTACKSTK_H
#define __PAXOSACCEPTACKSTK_H

#include "paxosacceptack.h"

class CPaxosAcceptAckStk{
public:
    CPaxosAcceptAck **mIndex;
    int mTop;
    int mSize;
public:     
    status_t SetElem(int index,CPaxosAcceptAck *ack);
    status_t SetElemPtr(int index,CPaxosAcceptAck *ack);
    int CompNode(CPaxosAcceptAck *paxosacceptack1, CPaxosAcceptAck *paxosacceptack2);
    status_t CopyNode(CPaxosAcceptAck *dst, CPaxosAcceptAck *src);
    status_t DelNode(CPaxosAcceptAck *paxosacceptack);
    CPaxosAcceptAck * CloneNode(CPaxosAcceptAck *paxosacceptack);
    status_t Copy(CPaxosAcceptAckStk *stk);
    CPaxosAcceptAck * RemoveElem(int index);
    status_t InsertElemPtr(int i, CPaxosAcceptAck *paxosacceptack);
    CPaxosAcceptAck * PopPtr();
    status_t AutoResize();
    status_t PushPtr(CPaxosAcceptAck *paxosacceptack);
    status_t InitBasic();  
    int BSearchPos(CPaxosAcceptAck *paxosacceptack,int order,int *find_flag);
    status_t InsOrderedPtr(CPaxosAcceptAck *paxosacceptack,int order,int unique);
    status_t InsOrdered(CPaxosAcceptAck *paxosacceptack,int order,int unique);
    status_t DelElem(int i);
    status_t InsertElem(int i,CPaxosAcceptAck *paxosacceptack);
    int BSearch(CPaxosAcceptAck *paxosacceptack,int order);
    CPaxosAcceptAck * BSearchNode(CPaxosAcceptAck *paxosacceptack,int order);
    status_t Sort(int order);
    CPaxosAcceptAck * GetElem(int index);
    CPaxosAcceptAck * GetTopPtr();
    CPaxosAcceptAck * Search(CPaxosAcceptAck *paxosacceptack);
    int SearchPos(CPaxosAcceptAck *paxosacceptack);
    CPaxosAcceptAckStk();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CPaxosAcceptAck *paxosacceptack);
    status_t Print();
    status_t Push(CPaxosAcceptAck *paxosacceptack);
    int GetLen();
    ~CPaxosAcceptAckStk();
};

#endif
