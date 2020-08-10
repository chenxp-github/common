#ifndef __PAXOSACCEPTREQ_H
#define __PAXOSACCEPTREQ_H

#include "mem.h"

class CPaxosMessage;
class CPaxosAcceptReq{
public:
    int mIId;
    int mBallot;
    CMem *mValue;
public:
    status_t Unserialize(CFileBase *file);
    status_t Serialize(CFileBase *file);
    status_t ToMessage(CPaxosMessage *msg);
    status_t SetIId(int iid);
    status_t SetBallot(int ballot);
    status_t SetValue(CMem *value);
    status_t SetValue(const void *value, int_ptr_t size);
    int GetIId();
    int GetBallot();
    CMem* GetValue();
    const char* GetValueStr();
    CPaxosAcceptReq();
    virtual ~CPaxosAcceptReq();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CPaxosAcceptReq *p);
    int Comp(CPaxosAcceptReq *p);
    status_t Print();
    status_t Clear();
};

#endif
