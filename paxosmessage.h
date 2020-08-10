#ifndef __PAXOSMESSAGE_H
#define __PAXOSMESSAGE_H

#include "cruntime.h"
#include "paxoscommon.h"
#include "mem.h"

class CPaxosMessage{
public:
    int mType;
    CMem *mData;
public:
    status_t TransferData(CMem *data);
    CMem * GetData();
    status_t ClearData();
    bool HasValue();
    status_t Transfer(CPaxosMessage *from);
    int GetType();
    status_t SetType(int type);
    status_t SetData(const void *data, int size);
    status_t SetData(CFileBase *data);
    CPaxosMessage();
    virtual ~CPaxosMessage();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CPaxosMessage *p);
    int Comp(CPaxosMessage *p);
    status_t Print();
};

#endif

