#ifndef __PAXOSQUORUM_H
#define __PAXOSQUORUM_H

#include "cruntime.h"

class CPaxosQuorum{
public:
    int mCount;
    int mQuorum;
    int mAcceptors;
    int* mAcceptorIds;
public:
    bool Reached();
    status_t Print();
    status_t Copy(CPaxosQuorum *p);
    status_t Add(int id);
    status_t Clear();
    CPaxosQuorum();
    virtual ~CPaxosQuorum();
    status_t InitBasic();
    status_t Init(int acceptors);
    status_t Destroy();
};

#endif

