#include "taskunzipfile.h"
#include "mem.h"

status_t CTaskUnzipFile::ERROR_NONE = 0;
status_t CTaskUnzipFile::ERROR_WRITE_ERROR = 1;

CTaskUnzipFile::CTaskUnzipFile()
{
    this->InitBasic();
}
CTaskUnzipFile::~CTaskUnzipFile()
{
    this->Destroy();
}
status_t CTaskUnzipFile::InitBasic()
{
    CTask::InitBasic();
    this->iMiniUnzip = NULL;
    this->iDstFile = NULL;
    this->mBuf = NULL;
    this->callback_finish = NULL;
    crt_memset(param_finish,0,sizeof(param_finish));
    return OK;
}
status_t CTaskUnzipFile::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    NEW(this->mBuf,CMem);
    this->mBuf->Init();
    this->mBuf->Malloc(1024*1024);
    return OK;
}
status_t CTaskUnzipFile::Destroy()
{
    DEL(this->mBuf);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskUnzipFile::Copy(CTaskUnzipFile *p)
{
    if(this == p)
        return OK;
    CTask::Copy(p);
    //add your code
    return OK;
}
status_t CTaskUnzipFile::Comp(CTaskUnzipFile *p)
{
    return 0;
}
status_t CTaskUnzipFile::Print()
{
    CTask::Print();
    //add your code
    return TRUE;
}

status_t CTaskUnzipFile::OnTimer(int32_t interval)
{
    ASSERT(this->iDstFile && this->iMiniUnzip);

    char *buf = this->mBuf->GetP();
    int32_t buf_size = this->mBuf->GetMaxSize();
    int32_t read_size = this->iMiniUnzip->ReadCurFile(buf,buf_size);

    if(read_size <= 0)
    {
        this->Stop(ERROR_NONE);
    }
    else
    {
        if(this->iDstFile->Write(buf,read_size) <= 0)
        {
            this->Stop(ERROR_WRITE_ERROR);
        }
    }

    return OK;
}

const char * CTaskUnzipFile::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_WRITE_ERROR)
        return "write error";
    return "unknown error";
}
status_t CTaskUnzipFile::ReportError(int32_t err)
{
    CMem mem;
    LOCAL_MEM(mem);

    mem.Puts("CTaskUnzipFile ");
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
status_t CTaskUnzipFile::Start()
{
    this->Resume();
    return OK;
}
status_t CTaskUnzipFile::Stop(status_t err)
{
    ASSERT(this->iMiniUnzip);
    this->iMiniUnzip->CloseCurFile();

    if(this->callback_finish)
    {
        this->param_finish[0] = (void*)err;
        this->callback_finish(this->param_finish);
    }

    this->ReportError(err);
    this->Quit();
    return OK;
}

status_t CTaskUnzipFile::SetMiniUnzip(CMiniUnzip * i_miniunzip)
{
    this->iMiniUnzip=i_miniunzip;
    return OK;
}
status_t CTaskUnzipFile::SetDstFile(CFileBase * i_dstfile)
{
    this->iDstFile=i_dstfile;
    return OK;
}
