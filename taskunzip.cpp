#include "taskunzip.h"
#include "mem.h"
#include "dirmgr.h"

status_t CTaskUnzip::ERROR_NONE = 0;
status_t CTaskUnzip::ERROR_CREATE_DIR_FAIL = 1;
status_t CTaskUnzip::ERROR_OPEN_FILE_ERROR = 2;
status_t CTaskUnzip::ERROR_UNZIP_ERROR = 3;

#define STEP_PREPARE_NEXT_FILE          1
#define STEP_WAIT_FILE_UNZIP_COMPLETE   2

CTaskUnzip::CTaskUnzip()
{
    this->InitBasic();
}
CTaskUnzip::~CTaskUnzip()
{
    this->Destroy();
}
status_t CTaskUnzip::InitBasic()
{
    CTask::InitBasic();
    this->mTaskUnzipFile = 0;
    this->iZipFile = NULL;
    this->mUnzip = NULL;
    this->mTmpFile = NULL;
    this->mPath = NULL;
    this->mStep = 0;
    this->mInnerZipFile = NULL;
    this->callback_finish = NULL;
    crt_memset(param_finish,0,sizeof(param_finish));
    return OK;
}
status_t CTaskUnzip::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    NEW(this->mUnzip,CMiniUnzip);
    this->mUnzip->Init();

    NEW(this->mTmpFile,CFile);
    this->mTmpFile->Init();

    NEW(this->mPath,CMem);
    this->mPath->Init();
    this->mPath->Malloc(1024);
    this->mPath->StrCpy("");

    NEW(this->mInnerZipFile,CFile);
    this->mInnerZipFile->Init();

    return OK;
}
status_t CTaskUnzip::Destroy()
{
    DEL(this->mInnerZipFile);
    DEL(this->mPath);
    DEL(this->mTmpFile);
    DEL(this->mUnzip);
    QuitTask(&this->mTaskUnzipFile);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskUnzip::Copy(CTaskUnzip *p)
{
    if(this == p)
        return OK;
    CTask::Copy(p);
    //add your code
    return OK;
}
status_t CTaskUnzip::Comp(CTaskUnzip *p)
{
    return 0;
}
status_t CTaskUnzip::Print()
{
    CTask::Print();
    //add your code
    return TRUE;
}
status_t CTaskUnzip::OnTimer(int32_t interval)
{   
    if(this->mStep == STEP_PREPARE_NEXT_FILE)
    {
        CMem full,mem;
        LOCAL_MEM(mem);
        LOCAL_MEM(full);

        this->mTmpFile->Destroy();
        this->mTmpFile->Init();
        this->mUnzip->GetCurFileName(&mem);

        full.SetSize(0);
        full.Puts(this->mPath);
        full.Putc(CDirMgr::GetPathSplitor());
        full.Puts(&mem);
        full.Putc(0);

        CDirMgr::StdPath(&full);

        if(this->mUnzip->IsCurFileDir())
        {
            if(!CDirMgr::CreateDirSuper(full.p))
            {
                this->Stop(ERROR_CREATE_DIR_FAIL);
                return ERROR;
            }
        }
        else 
        {
            if(!CDirMgr::CreateFilePath(full.p))
            {
                this->Stop(ERROR_CREATE_DIR_FAIL);
                return ERROR;
            }
        
            if(!this->mTmpFile->OpenFile(full.p,"wb+"))
            {
                this->Stop(ERROR_OPEN_FILE_ERROR);
                return ERROR;
            }
            this->mUnzip->OpenCurFile();            
            CTaskUnzipFile *pt = this->GetTaskUnzipFile(true);
            ASSERT(pt);
            pt->SetDstFile(this->mTmpFile);
            pt->Start();
            this->Suspend();
        }
        
        this->mStep = STEP_WAIT_FILE_UNZIP_COMPLETE;

    }
    else if(this->mStep == STEP_WAIT_FILE_UNZIP_COMPLETE)
    {
        if(!this->mUnzip->GotoNextFile())
        {
            this->Stop(ERROR_NONE);
            return OK;
        }
        this->mStep = STEP_PREPARE_NEXT_FILE;
    }
    
    return OK;
}

const char * CTaskUnzip::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_CREATE_DIR_FAIL)
        return "create dir fail";
    if(err == ERROR_OPEN_FILE_ERROR)
        return "open file error";
    if(err == ERROR_UNZIP_ERROR)
        return "unzip error";
    return "unknown error";
}
status_t CTaskUnzip::ReportError(int32_t err)
{
    CMem mem;
    LOCAL_MEM(mem);

    mem.Puts("CTaskUnzip ");
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
    
    mem.Putc(0);
    if(err != ERROR_NONE)
    {
        LOG("%s\n",mem.p);
    }
    return OK;
}
status_t CTaskUnzip::Start()
{
    this->mStep = STEP_PREPARE_NEXT_FILE;
    this->mUnzip->GotoFirstFile();
    this->Resume();
    return OK;
}
status_t CTaskUnzip::Stop(status_t err)
{
    if(this->callback_finish)
    {
        this->param_finish[0] = (void*)err;
        this->callback_finish(this->param_finish);
    }
    this->ReportError(err);
    this->Quit();
    return OK;
}

CTaskUnzipFile * CTaskUnzip::GetTaskUnzipFile(bool renew)
{
    CTaskUnzipFile *pt = (CTaskUnzipFile*)GetTask(this->mTaskUnzipFile);
    if(pt != NULL) return pt;
    if(!renew) return NULL;

    NEW(pt,CTaskUnzipFile);
    pt->Init(this->GetTaskMgr());
    pt->SetInterval(0);
    pt->SetMiniUnzip(this->mUnzip);
    pt->callback_finish = on_unzip_file_finish;
    pt->param_finish[10] = this;
    this->mTaskUnzipFile = pt->GetID();
    return pt;
}

status_t CTaskUnzip::SetZipFile(CFileBase *iFile)
{
    ASSERT(iFile);
    this->iZipFile = iFile;
    this->mUnzip->Destroy();
    this->mUnzip->Init();
    this->mUnzip->SetZipFile(iFile);    
    return OK;
}

status_t CTaskUnzip::SetDstPath(const char *path)
{
    ASSERT(path);
    this->mPath->StrCpy(path);
    return OK;
}

status_t CTaskUnzip::on_unzip_file_finish(void **p)
{
    CTaskUnzip *self = (CTaskUnzip*)p[10];
    ASSERT(self);
    int_ptr_t err = (int_ptr_t)p[0];
    if(err != CTaskUnzipFile::ERROR_NONE)
    {
        self->Stop(CTaskUnzip::ERROR_UNZIP_ERROR);
        return ERROR;
    }
    self->Resume();
    return OK;
}

status_t CTaskUnzip::SetZipFile(const char *zipFile)
{
    ASSERT(zipFile);
    this->mInnerZipFile->Destroy();
    this->mInnerZipFile->Init();
    ASSERT(this->mInnerZipFile->OpenFile(zipFile,"rb"));
    return this->SetZipFile(this->mInnerZipFile);
}
