#include "taskftpupload.h"
#include "mem.h"
#include "dirmgr.h"

#define STEP_LOGIN                      1
#define STEP_RETRY                      2
#define STEP_PREPARE_NEXT_FILE          3
#define STEP_UPLOAD_COMPLETE            4

int CTaskFtpUpload::EVENT_ALL_DONE = 1;

status_t CTaskFtpUpload::ERROR_NONE = 0;
status_t CTaskFtpUpload::ERROR_LOGIN_ERROR = 1;
status_t CTaskFtpUpload::ERROR_OPEN_FILE_ERROR = 2;
status_t CTaskFtpUpload::ERROR_FTP_CLIENT_ERROR = 3;
status_t CTaskFtpUpload::ERROR_EXCEED_MAX_RETRIES = 4;

CTaskFtpUpload::CTaskFtpUpload()
{
    this->InitBasic();
}
CTaskFtpUpload::~CTaskFtpUpload()
{
    this->Destroy();
}
status_t CTaskFtpUpload::InitBasic()
{
    CTask::InitBasic();

    this->mLoginInfo = NULL;
    this->mRetry = 0;
    this->mMaxRetry = 10;
    this->mStep = 0;
    this->mTaskFtpClient = 0;
    this->iSrcFile = NULL;
    this->mTmpSrcFile = NULL;
    this->mSrcFileList = NULL;
    this->mDstFileName = NULL;
    this->mUploadDirectly = false;
    this->mDstFileList = NULL;
    return OK;
}
 status_t CTaskFtpUpload::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);

    NEW(this->mLoginInfo,CFtpLoginInfo);
    this->mLoginInfo->Init();   
    
    NEW(this->mDstFileName,CMem);
    this->mDstFileName->Init();
    this->mDstFileName->Malloc(1024);

    NEW(this->mTmpSrcFile,CFile);
    this->mTmpSrcFile->Init();

    NEW(this->mSrcFileList,CMemStk);
    this->mSrcFileList->Init(1024);

    NEW(this->mDstFileList,CMemStk);
    this->mDstFileList->Init(1024);

    this->SetDstFileName("");

    return OK;
}
status_t CTaskFtpUpload::Destroy()
{
    QuitTask(&this->mTaskFtpClient);    
    DEL(this->mDstFileList);
    DEL(this->mDstFileName);
    DEL(this->mTmpSrcFile);
    DEL(this->mSrcFileList);    
    DEL(this->mLoginInfo);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskFtpUpload::Copy(CTaskFtpUpload *p)
{
    if(this == p)
        return OK;
    CTask::Copy(p);
    //add your code
    return OK;
}
status_t CTaskFtpUpload::Comp(CTaskFtpUpload *p)
{
    return 0;
}
status_t CTaskFtpUpload::Print()
{
    CTask::Print();
    //add your code
    return TRUE;
}
status_t CTaskFtpUpload::OnTimer(int interval)
{
    if(this->mStep == STEP_RETRY)
    {
        this->mRetry ++;
        if(this->mRetry > this->mMaxRetry)
        {
            this->Stop(ERROR_EXCEED_MAX_RETRIES);
            return ERROR;
        }
        this->mStep = STEP_LOGIN;
    }
    else if(this->mStep == STEP_LOGIN)
    {
        CTaskFtpClient *ftp = this->GetTaskFtpClient(true);
        ASSERT(ftp);
        ftp->AddRequest("USER %s",this->mLoginInfo->GetUser());
        ftp->AddRequest("PASS %s",this->mLoginInfo->GetPass());
        ftp->AddRequest("TYPE I");
        ftp->Start();
        this->mStep = STEP_PREPARE_NEXT_FILE;
        this->Suspend();
    }
    
    else if(this->mStep == STEP_PREPARE_NEXT_FILE)
    {       
        if(!this->mUploadDirectly)
        {
            this->mTmpSrcFile->Destroy();
            this->mTmpSrcFile->Init();

            const char *fn = this->GetCurSrcFileName();
            ASSERT(fn);

            if(!this->mTmpSrcFile->OpenFile(fn,"rb"))
            {
                this->Stop(ERROR_OPEN_FILE_ERROR);
                return ERROR;
            }
            this->iSrcFile = this->mTmpSrcFile;
        }

        ASSERT(this->iSrcFile);

        CTaskFtpClient *ftp = this->GetTaskFtpClient(false);
        ASSERT(ftp);

        ftp->AddRequest("PASV");
        ftp->SetSrcFile(this->iSrcFile);
        ftp->AddRequest("REST 0");
        
        LOCAL_MEM(full);
        LOCAL_MEM(path);
        this->MakeFullDstFileName(&full);
        CDirMgr::GetFileName(&full,&path,FN_PATH);

        if(path.StrLen() > 0)
        {
            this->CreateMultiLevelFtpPath(ftp,&path);
        }

        ftp->AddRequest("STOR %s",full.CStr());

        this->Suspend();
        this->mStep = STEP_UPLOAD_COMPLETE;
    }

    else if(this->mStep == STEP_UPLOAD_COMPLETE)
    {
        if(!this->mUploadDirectly)
        {
            this->mSrcFileList->DelElem(0);
            this->mDstFileList->DelElem(0);
            this->mStep = STEP_PREPARE_NEXT_FILE;
            if(this->mSrcFileList->GetLen() <= 0)
            {
                this->Stop(ERROR_NONE);
                return OK;
            }
        }
        else
        {
            this->Stop(ERROR_NONE);
        }
    }

    return OK;
}
const char * CTaskFtpUpload::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_LOGIN_ERROR)
        return "login error";
    if(err == ERROR_OPEN_FILE_ERROR)
        return "open file error";
    if(err == ERROR_FTP_CLIENT_ERROR)
        return "ftp client error";
    if(err == ERROR_EXCEED_MAX_RETRIES)
        return "exceed max retries";
    return "unknown error";
}
status_t CTaskFtpUpload::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskFtpUpload ");
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
        LOG("%s",mem.CStr());
    }
    return OK;
}
status_t CTaskFtpUpload::Start()
{
    this->mStep = STEP_RETRY;
    this->Resume();
    return OK;
}
status_t CTaskFtpUpload::Stop(status_t err)
{
    if(this->IsDead())return ERROR;
    this->ReportError(err);
    this->Quit();
    this->SetCallbackParam(1,(void*)err);
    this->RunCallback(EVENT_ALL_DONE);
    return OK;
}

status_t CTaskFtpUpload::SetFtpServer(const char *serverName, int port, const char *user, const char *pass)
{
    ASSERT(serverName && user && pass);
    this->mLoginInfo->SetServer(serverName);
    this->mLoginInfo->SetPort(port);
    this->mLoginInfo->SetUser(user);
    this->mLoginInfo->SetPass(pass);
    return OK;
}

status_t CTaskFtpUpload::SetFtpServer(CFtpLoginInfo *info)
{
    ASSERT(info);
    this->mLoginInfo->Copy(info);
    return OK;
}

status_t CTaskFtpUpload::on_ftp_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpUpload *self = (CTaskFtpUpload*)p[10];
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
        else if(req_code == FTP_REQ_TYPE)
        {
            if(res_code != 200)
                goto retry;
            else
                self->Resume();
        }
        else if(req_code == FTP_REQ_PASV)
        {
            if(res_code != 227)
                goto retry;
        }
        else if(req_code == FTP_REQ_STOR)
        {
            if(res_code != 226 && res_code != 150)
                goto retry;
            if(res_code == 226) //stor ok
            {
                self->mRetry = 0;
                self->Resume();
            }
        }
        
        return OK;
        
retry:
        self->Retry();
    }

    else if(event == CTaskFtpClient::EVENT_QUIT)
    {
        int_ptr_t err = (int_ptr_t)p[1];
        if(err != CTaskFtpClient::ERROR_NONE)
        {
            self->Retry();
            return OK;
        }
    }
    return ERROR;
}

CTaskFtpClient * CTaskFtpUpload::GetTaskFtpClient(bool renew)
{
    CTaskFtpClient *pt = (CTaskFtpClient*)GetTask(this->mTaskFtpClient);
    if(pt != NULL) return pt;
    if(!renew)return NULL;

    NEW(pt,CTaskFtpClient);
    pt->Init(this->GetTaskMgr());
    pt->SetServer(this->mLoginInfo->GetServer(),this->mLoginInfo->GetPort());
    pt->SetInterval(1);
    pt->SetTimeout(30*1000);
    pt->SetCallbackFunc(on_ftp_event);
    pt->SetCallbackParam(10,this);
    this->mTaskFtpClient = pt->GetID();
    return pt;
}

status_t CTaskFtpUpload::SetMaxRetries(int max)
{
    this->mMaxRetry = max;
    return OK;
}

status_t CTaskFtpUpload::Retry()
{
    LOG("ftp, some error occur, will retry\n"); 
    this->mStep = STEP_RETRY;
    this->Resume();
    return OK;
}

status_t CTaskFtpUpload::SetDstFileName(const char *fn)
{
    ASSERT(fn);
    this->mDstFileName->StrCpy(fn);
    return OK;
}

status_t CTaskFtpUpload::AddFileToUpload(const char *srcFileName,const char *ftpFileName)
{
    ASSERT(srcFileName && ftpFileName);
    this->mSrcFileList->Push(srcFileName);
    this->mDstFileList->Push(ftpFileName);
    return OK;
}

const char * CTaskFtpUpload::GetCurSrcFileName()
{
    CMem *pmem = this->mSrcFileList->GetElem(0);
    if(pmem)
    {
        return pmem->CStr();
    }
    return NULL;
}

const char * CTaskFtpUpload::GetCurDstFileName()
{
    CMem *pmem = this->mDstFileList->GetElem(0);
    if(pmem)
    {
        return pmem->CStr();
    }
    return NULL;
}

status_t CTaskFtpUpload::StartDirectly(CFileBase *iFile)
{
    ASSERT(iFile);
    iFile->Seek(0);
    this->iSrcFile = iFile;
    this->mUploadDirectly = true;
    this->Start();
    return OK;
}

status_t CTaskFtpUpload::MakeFullDstFileName(CMem *out)
{
    ASSERT(out);

    out->SetSize(0);
    out->Putc('/');

    if(this->mUploadDirectly)
    {
        ASSERT(this->mDstFileName->StrLen() > 0);
        out->Puts(this->mDstFileName);
    }
    else
    {
        if(this->mDstFileName->StrLen() > 0)
        {
            out->Puts(this->mDstFileName);
        }
        else
        {
            const char *curDstFn = this->GetCurDstFileName();
            ASSERT(curDstFn);
            out->Puts(curDstFn);
        }
    }

    CDirMgr::StdPath(out);
    CDirMgr::ToUnixPath(out);
    return OK;
}

status_t CTaskFtpUpload::CreateMultiLevelFtpPath(CTaskFtpClient *ftp, CMem *path)
{
    ASSERT(ftp && path);

    LOCAL_MEM(tmp);
    LOCAL_MEM(buf);

    path->SetSplitChars("\\/");
    path->Seek(0);

    while(path->ReadString(&buf))
    {
        char str[2];
        str[0] = '/';
        str[1] = 0;
        tmp.StrCat(str);
        tmp.StrCat(&buf);   
        ftp->AddRequest("MKD %s",tmp.CStr());
    }

    return OK;
}
