#ifndef __PAXOSMESSAGESTK_H
#define __PAXOSMESSAGESTK_H

#include "paxosmessage.h"

class CPaxosMessageStk{
public:
    CPaxosMessage **mIndex;
    int mTop;
    int mSize;
public:     
    status_t PushTransfer(CPaxosMessage *msg);
    int CompNode(CPaxosMessage *paxosmessage1, CPaxosMessage *paxosmessage2);
    status_t CopyNode(CPaxosMessage *dst, CPaxosMessage *src);
    status_t DelNode(CPaxosMessage *paxosmessage);
    CPaxosMessage * CloneNode(CPaxosMessage *paxosmessage);
    status_t Copy(CPaxosMessageStk *stk);
    CPaxosMessage * RemoveElem(int index);
    status_t InsertElemPtr(int i, CPaxosMessage *paxosmessage);
    CPaxosMessage * PopPtr();
    status_t AutoResize();
    status_t PushPtr(CPaxosMessage *paxosmessage);
    status_t InitBasic();  
    int BSearchPos(CPaxosMessage *paxosmessage,int order,int *find_flag);
    status_t InsOrderedPtr(CPaxosMessage *paxosmessage,int order,int unique);
    status_t InsOrdered(CPaxosMessage *paxosmessage,int order,int unique);
    status_t DelElem(int i);
    status_t InsertElem(int i,CPaxosMessage *paxosmessage);
    int BSearch(CPaxosMessage *paxosmessage,int order);
    CPaxosMessage * BSearchNode(CPaxosMessage *paxosmessage,int order);
    status_t Sort(int order);
    CPaxosMessage * GetElem(int index);
    CPaxosMessage * GetTopPtr();
    CPaxosMessage * Search(CPaxosMessage *paxosmessage);
    int SearchPos(CPaxosMessage *paxosmessage);
    CPaxosMessageStk();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CPaxosMessage *paxosmessage);
    status_t Print();
    status_t Push(CPaxosMessage *paxosmessage);
    int GetLen();
    ~CPaxosMessageStk();
};

#endif

