#include "taskcreatebigfile.h"
#include "mem.h"

int CTaskCreateBigFile::EVENT_FINISH = 1;

status_t CTaskCreateBigFile::ERROR_NONE = 0;
status_t CTaskCreateBigFile::ERROR_OPEN_FILE_ERROR = 1;
status_t CTaskCreateBigFile::ERROR_ERROR_SIZE = 2;

#define STEP_PREPARE_FILE       1
#define STEP_WRITE_DATA         2

CTaskCreateBigFile::CTaskCreateBigFile()
{
    this->InitBasic();
}
CTaskCreateBigFile::~CTaskCreateBigFile()
{
    this->Destroy();
}
status_t CTaskCreateBigFile::InitBasic()
{
    CTask::InitBasic();
    this->mFile = NULL;
    this->mMaxSize = 0;
    this->mFileName = NULL;
    this->mBuf = NULL;  
    return OK;
}
status_t CTaskCreateBigFile::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    
    NEW(this->mFile,CFile);
    this->mFile->Init();

    NEW(this->mFileName,CMem);
    this->mFileName->Init();
    this->mFileName->Malloc(1024);

    NEW(this->mBuf,CMem);
    this->mBuf->Init();
    this->mBuf->Malloc(1024*1024);
    this->mBuf->Zero();
    
    this->mStep = 0;
    return OK;
}
status_t CTaskCreateBigFile::Destroy()
{
    DEL(this->mBuf);
    DEL(this->mFileName);
    DEL(this->mFile);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskCreateBigFile::Copy(CTaskCreateBigFile *p)
{
    if(this == p)
        return OK;
    CTask::Copy(p);
    //add your code
    return OK;
}
status_t CTaskCreateBigFile::Comp(CTaskCreateBigFile *p)
{
    return 0;
}
status_t CTaskCreateBigFile::Print()
{
    CTask::Print();
    //add your code
    return TRUE;
}
status_t CTaskCreateBigFile::OnTimer(int32_t interval)
{
    if(this->mStep == STEP_PREPARE_FILE)
    {
        if(this->mMaxSize <= 0)
        {
            this->Stop(ERROR_ERROR_SIZE);
            return OK;
        }

        this->mFile->Destroy();
        this->mFile->Init();
        if(!this->mFile->OpenFile(this->mFileName->CStr(),"wb+"))
        {
            this->Stop(ERROR_OPEN_FILE_ERROR);
            return OK;
        }
        this->mStep = STEP_WRITE_DATA;
    }
    else if(this->mStep == STEP_WRITE_DATA)
    {
        fsize_t left_size = this->mMaxSize - this->mFile->GetSize();
        if(left_size <= 0)
        {
            this->Stop(ERROR_NONE);
            return OK;
        }
        fsize_t block_size = this->mBuf->GetMaxSize();
        if(block_size > left_size) block_size = left_size;
        this->mFile->Write(this->mBuf->GetRawBuf(),(int_ptr_t)block_size);
    }
    return OK;
}
const char * CTaskCreateBigFile::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_OPEN_FILE_ERROR)
        return "open file error";
    if(err == ERROR_ERROR_SIZE)
        return "error size";
    return "unknown error";
}
status_t CTaskCreateBigFile::ReportError(int32_t err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskCreateBigFile ");
    mem.Printf("(%d) ",this->GetID());

    if(err != ERROR_NONE)
    {
        mem.Puts("exit with error:");
        mem.Puts(this->ErrorToString(err));     
    }
    else 
    {
        mem.Puts("exit successfully!");
    }
    
    if(err != ERROR_NONE)
    {
        LOG("%s\n",mem.CStr());
    }
    return OK;
}
status_t CTaskCreateBigFile::Start()
{
    this->mStep = STEP_PREPARE_FILE;
    this->Resume();
    return OK;
}
status_t CTaskCreateBigFile::Stop(status_t err)
{
    this->Quit();
    this->ReportError(err);

    this->mFile->Destroy();
    this->SetCallbackParam(1,(void*)err);
    this->RunCallback(EVENT_FINISH);

    return OK;
}

status_t CTaskCreateBigFile::SetFileName(const char *fileName, fsize_t size)
{
    ASSERT(fileName);
    this->mFileName->StrCpy(fileName);
    this->mMaxSize = size;
    return OK;
}
