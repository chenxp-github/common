#include "paxosstoragefile.h"
#include "syslog.h"
#include "memfile.h"

static status_t _hash_code(void **p)
{
    CMem *key = (CMem*)p[0];
    int max = (int)p[1];
    ASSERT(key);
    int t;
    key->Read(&t,sizeof(t));
    return t%max;
}

CPaxosStorageFile::CPaxosStorageFile()
{
    this->InitBasic();
}
CPaxosStorageFile::~CPaxosStorageFile()
{
    this->Destroy();
}
status_t CPaxosStorageFile::InitBasic()
{
    CPaxosStorage::InitBasic();
    this->mHashFile = NULL;
    this->mSaveFile = NULL;
    this->mAcceptorId = -1;
    this->mRecordBuf = NULL;

    return OK;
}
status_t CPaxosStorageFile::Init(int acceptor_id)
{
    this->Destroy();
    CPaxosStorage::Init();
    
    this->mAcceptorId = acceptor_id;

    NEW(this->mSaveFile,CFile);
    this->mSaveFile->Init();

    NEW(this->mHashFile,CHashFile);
    this->mHashFile->Init();
    this->mHashFile->callback_hashcode = _hash_code;

    NEW(this->mRecordBuf,CPaxosAcceptorRecord);
    this->mRecordBuf->Init();

    return OK;
}
status_t CPaxosStorageFile::Destroy()
{
    DEL(this->mRecordBuf);
    DEL(this->mHashFile);
    DEL(this->mSaveFile);
    CPaxosStorage::Destroy();
    this->InitBasic();
    return OK;
}

status_t CPaxosStorageFile::OpenHashFile(const char *filename,int max)
{
    ASSERT(filename);
    if(!this->mSaveFile->OpenFile(filename,"r+"))
    {
        ASSERT(this->mSaveFile->OpenFile(filename,"wb+"));
        this->mHashFile->SetDestFile(this->mSaveFile);
        this->mHashFile->AllocBuckets(max);
        return OK;
    }
    else
    {
        return this->mHashFile->LoadHashFile(this->mSaveFile);
    }
}

CPaxosAcceptorRecord* CPaxosStorageFile::GetRecord(int iid)
{
    CPartFile value;
    value.Init();

    if(this->mHashFile->Get(iid,&value))
    {
        value.Seek(0);
        this->mRecordBuf->Unserialize(&value);
        ASSERT(iid == this->mRecordBuf->GetIId());
        return mRecordBuf;
    }

    return NULL;
}

status_t CPaxosStorageFile::PutRecord(int key,CPaxosAcceptorRecord *record)
{
    ASSERT(record);
    CMemFile mf;
    mf.Init();
    record->Serialize(&mf);
    return this->mHashFile->Put(key,&mf);
}

CPaxosAcceptorRecord* CPaxosStorageFile::SaveAccept(CPaxosAcceptReq *ar)
{
    ASSERT(ar);

    mRecordBuf->SetAcceptorId(this->mAcceptorId);
    mRecordBuf->SetIId(ar->GetIId());
    mRecordBuf->SetBallot(ar->GetBallot());
    mRecordBuf->SetValueBallot(ar->GetBallot());
    mRecordBuf->SetIsFinal(0);
    mRecordBuf->SetValue(ar->GetValue());

    if(this->PutRecord(ar->GetIId(),mRecordBuf))
        return this->mRecordBuf;
    return NULL;
}

CPaxosAcceptorRecord* CPaxosStorageFile::SavePrepare(CPaxosPrepareReq *pr, CPaxosAcceptorRecord *rec)
{
    ASSERT(pr);

    if (rec == NULL) 
    {
        rec = this->mRecordBuf;
        rec->SetAcceptorId(this->mAcceptorId);
        rec->SetIId(pr->GetIId());
        rec->SetBallot(pr->GetBallot());
        rec->SetValueBallot(0);
        rec->SetIsFinal(0);
        rec->ClearValue();
    } 
    else 
    {
        rec->SetBallot(pr->GetBallot());
    }

    if(this->PutRecord(pr->GetIId(),mRecordBuf))
        return this->mRecordBuf;
    return NULL;
}

CPaxosAcceptorRecord* CPaxosStorageFile::SaveFinalValue(void *value, int size, int id, int ballot)
{
    ASSERT(value);

    this->mRecordBuf->SetIId(id);
    this->mRecordBuf->SetBallot(ballot);
    this->mRecordBuf->SetValueBallot(ballot);
    this->mRecordBuf->SetIsFinal(1);
    this->mRecordBuf->SetValue(value,size);
    
    if(this->PutRecord(id,mRecordBuf))
        return this->mRecordBuf;
    return NULL;
}

