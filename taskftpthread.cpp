#include "taskftpthread.h"
#include "mem.h"

#define STEP_LOGIN      1
#define STEP_FINISH     2

int CTaskFtpThread::EVENT_FINISH = 1;

status_t CTaskFtpThread::ERROR_NONE = 0;
status_t CTaskFtpThread::ERROR_LOGIN_ERROR = 1;
status_t CTaskFtpThread::ERROR_RETR_COMMAND_ERROR = 2;
status_t CTaskFtpThread::ERROR_FTP_CLIENT_ERROR = 3;
status_t CTaskFtpThread::ERROR_REST_COMMAND_ERROR = 4;

CTaskFtpThread::CTaskFtpThread()
{
    this->InitBasic();
}
CTaskFtpThread::~CTaskFtpThread()
{
    this->Destroy();
}
status_t CTaskFtpThread::InitBasic()
{
    CTask::InitBasic();
    this->mTaskFtpClient = 0;
    this->mDstPartFile = NULL;
    this->mLoginInfo = NULL;
    this->mFileName = NULL; 
    return OK;
}
status_t CTaskFtpThread::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);

    NEW(this->mDstPartFile,CPartFile);
    this->mDstPartFile->Init();

    NEW(this->mLoginInfo,CFtpLoginInfo);
    this->mLoginInfo->Init();

    NEW(this->mFileName,CMem);
    this->mFileName->Init();
    this->mFileName->Malloc(1024);

    return OK;
}
status_t CTaskFtpThread::Destroy()
{
    QuitTask(&this->mTaskFtpClient);
    DEL(this->mDstPartFile);
    DEL(this->mLoginInfo);
    DEL(this->mFileName);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskFtpThread::OnTimer(int32_t interval)
{
    if(this->mStep == STEP_LOGIN)
    {
        CTaskFtpClient *pt = this->GetTaskFtpClient(true);
        ASSERT(pt);
        pt->AddRequest("USER %s",this->mLoginInfo->GetUser());
        pt->AddRequest("PASS %s",this->mLoginInfo->GetPass());
        pt->AddRequest("PASV");

        char str[128];
        crt_fsize_to_str(this->GetHostOffset(),str);
        pt->AddRequest("REST %s",str);

        pt->AddRequest("RETR %s",this->mFileName->CStr());
        this->Suspend();
        this->mStep = STEP_FINISH;
    }
    else if(this->mStep == STEP_FINISH)
    {
        this->Stop(ERROR_NONE);
    }
    return OK;
}
const char * CTaskFtpThread::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_LOGIN_ERROR)
        return "login error";
    if(err == ERROR_RETR_COMMAND_ERROR)
        return "retr command error";
    if(err == ERROR_FTP_CLIENT_ERROR)
        return "ftp client error";
    if(err == ERROR_REST_COMMAND_ERROR)
        return "rest command error";
    return "unknown error";
}
status_t CTaskFtpThread::ReportError(int32_t err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskFtpThread ");
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
status_t CTaskFtpThread::Start()
{
    this->mStep = STEP_LOGIN;
    this->Resume();
    return OK;
}
status_t CTaskFtpThread::Stop(status_t err)
{
    this->Quit();
    this->ReportError(err);

    this->SetCallbackParam(1,(void*)err);
    this->SetCallbackParam(2,this);
    this->RunCallback(EVENT_FINISH);
        
    return OK;
}

status_t CTaskFtpThread::SetLoginInfo(CFtpLoginInfo *info)
{
    this->mLoginInfo->Copy(info);
    return OK;
}

status_t CTaskFtpThread::SetDstFile(CFileBase *hostFile, fsize_t offset, fsize_t size)
{
    ASSERT(hostFile);
    this->mDstPartFile->SetHostFile(hostFile,offset,size);
    this->mDstPartFile->Seek(0);
    this->mDstPartFile->SetSize(0);
    return OK;
}

CTaskFtpClient * CTaskFtpThread::GetTaskFtpClient(bool renew)
{
    CTaskFtpClient *pt = (CTaskFtpClient*)GetTask(this->mTaskFtpClient);
    if(pt != NULL)return pt;
    if(!renew) return NULL;

    NEW(pt,CTaskFtpClient);
    pt->Init(this->GetTaskMgr());
    pt->SetTimeout(10*1000);
    pt->SetInterval(1);
    pt->SetServer(this->mLoginInfo->GetServer(),this->mLoginInfo->GetPort());
    pt->SetDstFile(this->mDstPartFile);
    pt->SetMaxDstFileSize(this->GetBlockSize());
    pt->SetCallbackFunc(on_ftp_event);
    pt->SetCallbackParam(10,this);

    pt->Start();

    this->mTaskFtpClient = pt->GetID();
    return pt;
}

status_t CTaskFtpThread::on_ftp_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpThread *self = (CTaskFtpThread*)p[10];
    ASSERT(self);
    
    if(event == CTaskFtpClient::EVENT_ON_RESPONSE)
    {
        int_ptr_t req_code = (int_ptr_t)p[1];
        int_ptr_t res_code = (int_ptr_t)p[3];
        const char *res_body = (const char*)p[4];
        ASSERT(res_body);
        
        if(req_code == FTP_REQ_PASS)
        {
            if(res_code != 230)
            {
                self->Stop(ERROR_LOGIN_ERROR);
            }
        }
        
        if(req_code == FTP_REQ_RETR)
        {
            if(res_code == 226)
            {
                self->Resume();
            }
            else if(res_code != 150)
            {
                self->Stop(ERROR_RETR_COMMAND_ERROR);
            }
        }
        
        if(req_code == FTP_REQ_REST)
        {
            if(res_code != 350)
            {
                self->Stop(ERROR_REST_COMMAND_ERROR);
            }
        }
    }

    else if(event == CTaskFtpClient::EVENT_QUIT)
    {
        int_ptr_t err = (int_ptr_t)p[1];

        if(err != CTaskFtpClient::ERROR_NONE)
            self->Stop(ERROR_FTP_CLIENT_ERROR);
        else
            self->Stop(ERROR_NONE);
    }
    return OK;
}

status_t CTaskFtpThread::SetFileName(const char *filename)
{
    ASSERT(filename);
    this->mFileName->StrCpy(filename);
    return OK;
}

fsize_t CTaskFtpThread::GetHostOffset()
{
    return this->mDstPartFile->GetHostOffset();
}

fsize_t CTaskFtpThread::GetBlockSize()
{
    return this->mDstPartFile->GetMaxSize();
}

fsize_t CTaskFtpThread::GetDownloadedSize()
{
    return this->mDstPartFile->GetSize();
}
