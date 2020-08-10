#ifndef __PAXOSACCEPTACK_H
#define __PAXOSACCEPTACK_H

#include "mem.h"

class CPaxosMessage;
class CPaxosAcceptAck{
public:
    int mAcceptorId;
    int mIId;
    int mBallot;
    int mValueBallot;
    int mIsFinal;
    CMem *mValue;
public:
    bool HasValue();
    status_t ToMessage(CPaxosMessage *msg);
    status_t Unserialize(CFileBase *file);
    status_t Serialize(CFileBase *file);
    status_t SetAcceptorId(int acceptorid);
    status_t SetIId(int iid);
    status_t SetBallot(int ballot);
    status_t SetValueBallot(int valueballot);
    status_t SetIsFinal(int isfinal);
    status_t SetValue(CMem *value);
    status_t SetValue(const void *value, int_ptr_t size);
    int GetAcceptorId();
    int GetIId();
    int GetBallot();
    int GetValueBallot();
    int IsFinal();
    CMem* GetValue();
    const char* GetValueStr();
    CPaxosAcceptAck();
    virtual ~CPaxosAcceptAck();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CPaxosAcceptAck *p);
    int Comp(CPaxosAcceptAck *p);
    status_t Print();
    status_t ClearValue();
};

typedef CPaxosAcceptAck CPaxosAcceptorRecord;

#endif

