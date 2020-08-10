#ifndef __PAXOSSTORAGEFILE_H
#define __PAXOSSTORAGEFILE_H

#include "cruntime.h"
#include "filebase.h"
#include "paxosstorage.h"
#include "hashfile.h"
#include "file.h"
#include "paxosacceptack.h"
#include "paxosacceptreq.h"
#include "paxospreparereq.h"

class CPaxosStorageFile:public CPaxosStorage{
public:
    int mAcceptorId;
    CHashFile *mHashFile;
    CFile *mSaveFile;
    CPaxosAcceptorRecord *mRecordBuf;
public:
    status_t PutRecord(int key,CPaxosAcceptorRecord *record);
    CPaxosAcceptorRecord* SaveFinalValue(void *value, int size, int id, int ballot);
    CPaxosAcceptorRecord* SavePrepare(CPaxosPrepareReq *pr, CPaxosAcceptorRecord *rec);
    CPaxosAcceptorRecord* SaveAccept(CPaxosAcceptReq *ar);
    CPaxosAcceptorRecord* GetRecord(int iid);
    status_t OpenHashFile(const char *filename,int max);
    CPaxosStorageFile();
    virtual ~CPaxosStorageFile();
    status_t InitBasic();
    status_t Init(int acceptor_id);
    status_t Destroy();
};

#endif

