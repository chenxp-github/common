#ifndef __PAXOSPREPAREACK_H
#define __PAXOSPREPAREACK_H

#include "mem.h"

class CPaxosMessage;
class CPaxosPrepareAck{
public:
    int mAcceptorId;
    int mIId;
    int mBallot;
    int mValueBallot;

    CMem *mValue;
public:
    status_t TransferValue(CMem *val);
    status_t ToMessage(CPaxosMessage *msg);
    status_t Unserialize(CFileBase *file);
    status_t Serialize(CFileBase *file);
    bool HasValue();
    status_t SetValue(CMem *value);
    status_t SetValue(const void *value, int_ptr_t size);
    status_t SetAcceptorId(int acceptorid);
    status_t SetIId(int iid);
    status_t SetBallot(int ballot);
    status_t SetValueBallot(int valueballot);
    int GetAcceptorId();
    int GetIId();
    int GetBallot();
    int GetValueBallot();
    CMem* GetValue();
    const char* GetValueStr();

    CPaxosPrepareAck();
    virtual ~CPaxosPrepareAck();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CPaxosPrepareAck *p);
    int Comp(CPaxosPrepareAck *p);
    status_t Print();
    status_t Clear();
};

#endif

