#include "taskftpdownload.h"
#include "mem.h"
#include "dirmgr.h"
#include "taskcreatebigfile.h"
#include "taskftpthread.h"
#include "tasktimer.h"

#define STEP_LOGIN                      1
#define STEP_RETRY                      2
#define STEP_PREPARE_NEXT_TASK          3
#define STEP_START_DOWNLOAD             4
#define STEP_FILE_DOWNLOAD_COMPLETE     5
#define STEP_QUERY_SIZE                 6
#define STEP_MD5_SUM                    7
#define STEP_PREPARE_DST_FILE           8
#define STEP_BIG_FILE_CREATED           9

int CTaskFtpDownload::EVENT_ALL_DONE = 1;
int CTaskFtpDownload::EVENT_SINGLE_FILE_DONE = 2;

status_t CTaskFtpDownload::ERROR_NONE = 0;
status_t CTaskFtpDownload::ERROR_LOGIN_ERROR = 1;
status_t CTaskFtpDownload::ERROR_OPEN_FILE_ERROR = 2;
status_t CTaskFtpDownload::ERROR_FTP_CLIENT_ERROR = 3;
status_t CTaskFtpDownload::ERROR_EXCEED_MAX_RETRIES = 4;
status_t CTaskFtpDownload::ERROR_RENAME_FILE_ERROR = 5;
status_t CTaskFtpDownload::ERROR_SIZE_NOT_MATCH = 6;
status_t CTaskFtpDownload::ERROR_MD5_CHECK_FAIL = 7;
status_t CTaskFtpDownload::ERROR_CREATE_BIG_FILE_ERROR = 8;
status_t CTaskFtpDownload::ERROR_TASK_MD5_SUM_ERROR = 9;

CTaskFtpDownload::CTaskFtpDownload()
{
    this->InitBasic();
}
CTaskFtpDownload::~CTaskFtpDownload()
{
    this->Destroy();
}
status_t CTaskFtpDownload::InitBasic()
{
    CTask::InitBasic();
    this->mTaskFtpClientForSize = 0;
    this->mLoginInfo = NULL;
    this->iDstFile = NULL;
    this->mFileInfoList = NULL;
    this->mDstFileInMem = NULL;
    this->mDstPath = NULL;
    this->mRetry = 0;
    this->mMaxRetry = 10;
    this->mStep = 0;
    this->mDownloadToMem = false;
    this->mKeepFolder = false;
    this->mCheckVersion = true;
    this->mTmpDstFile = NULL;
    this->mTaskMd5Sum = 0;
    this->mCurFileInfo = NULL;
    this->mCurPartFileInfo = NULL;
    this->mTaskCreateBigFile = 0;
    this->mMaxConcurrency = 0;
    this->mAllThreads = NULL;
    this->mTimerAutoSave = 0;
    return OK;
}
 status_t CTaskFtpDownload::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);

    NEW(this->mLoginInfo,CFtpLoginInfo);
    this->mLoginInfo->Init();

    NEW(this->mFileInfoList,CFtpFileInfoStk);
    this->mFileInfoList->Init(1024);

    NEW(this->mDstFileInMem,CMem);
    this->mDstFileInMem->Init();
    
    NEW(this->mDstPath,CMem);
    this->mDstPath->Init();
    this->mDstPath->Malloc(1024);

    NEW(this->mTmpDstFile,CFile);
    this->mTmpDstFile->Init();

    NEW(this->mCurFileInfo,CFtpFileInfo);
    this->mCurFileInfo->Init();

    NEW(this->mCurPartFileInfo,CFtpPartFileInfo);
    this->mCurPartFileInfo->Init();
    this->mCurPartFileInfo->Clear();
    
    return OK;
}
status_t CTaskFtpDownload::Destroy()
{
    if(this->mCurPartFileInfo)
        this->SaveThreadInfo();
    DEL(this->mCurPartFileInfo);
    DEL(this->mCurFileInfo);
    DEL(this->mTmpDstFile);
    DEL(this->mDstFileInMem);
    DEL(this->mDstPath);
    DEL(this->mFileInfoList);
    DEL(this->mLoginInfo);
    DEL_ARRAY(this->mAllThreads);
    QuitTask(&this->mTaskFtpClientForSize);
    QuitTask(&this->mTaskMd5Sum);
    QuitTask(&this->mTaskCreateBigFile);
    QuitTask(&this->mTimerAutoSave);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskFtpDownload::Copy(CTaskFtpDownload *p)
{
    if(this == p)
        return OK;
    CTask::Copy(p);
    //add your code
    return OK;
}
status_t CTaskFtpDownload::Comp(CTaskFtpDownload *p)
{
    return 0;
}
status_t CTaskFtpDownload::Print()
{
    CTask::Print();
    //add your code
    return TRUE;
}
status_t CTaskFtpDownload::OnTimer(int interval)
{
    if(this->mStep == STEP_RETRY)
    {
        this->mRetry ++;
        if(this->mRetry > this->mMaxRetry)
        {
            this->Stop(ERROR_EXCEED_MAX_RETRIES);
            return ERROR;
        }

        QuitTask(&this->mTaskFtpClientForSize);
        this->mStep = STEP_PREPARE_NEXT_TASK;
    }
    else if(this->mStep == STEP_PREPARE_NEXT_TASK)
    {
        if(this->mFileInfoList->GetLen() <= 0)
        {
            this->Stop(ERROR_NONE);
            return OK;
        }
        
        this->ResetAllThreads();
        CFtpFileInfo *pinfo = this->mFileInfoList->GetElem(0);
        ASSERT(pinfo);
        this->mCurFileInfo->Copy(pinfo);

        if(!this->mDownloadToMem)
        {
            this->LoadPartFileInfo();
            if(this->mCurPartFileInfo->IsComplete())
            {
                LOCAL_MEM(mem);
                this->GetFullDstFileName(&mem);
                if(CDirMgr::IsFileExist(&mem))
                {
                    if(this->IsLatestVersion())
                    {
                        LOG("file \"%s\" is the latest version, no need to download.\n",mCurFileInfo->GetFileName());
                        this->mCurFileInfo->SetFileSize(this->mCurPartFileInfo->size); //update cur file size
                        this->mFileInfoList->DelElem(0);
                        this->mStep = STEP_PREPARE_NEXT_TASK;
                        return OK;
                    }
                }
            }
        }
        this->mStep = STEP_LOGIN;
    }
    else if(this->mStep == STEP_LOGIN)
    {
        CTaskFtpClient *pt = this->GetTaskFtpClientForSize(true);
        ASSERT(pt);
        pt->AddRequest("USER %s",this->mLoginInfo->GetUser());
        pt->AddRequest("PASS %s",this->mLoginInfo->GetPass());
        pt->AddRequest("SIZE %s",this->mCurFileInfo->GetFileName());        
        pt->AddRequest("QUIT");
        pt->Start();
        this->Suspend();
        this->mStep = STEP_PREPARE_DST_FILE;
    }
    else if(this->mStep == STEP_PREPARE_DST_FILE)
    {
        if(this->mDownloadToMem)
        {
            this->mDstFileInMem->Destroy();
            this->mDstFileInMem->Init();
            this->mDstFileInMem->Malloc((int_ptr_t)(this->mCurFileInfo->GetFileSize()));
            this->mDstFileInMem->SetSize(this->mDstFileInMem->GetMaxSize());
            this->iDstFile = this->mDstFileInMem;
            this->mStep = STEP_BIG_FILE_CREATED;
        }
        else
        {
            LOCAL_MEM(mem);

            this->GetFullDstPartFileName(&mem);
            if(CDirMgr::IsFileExist(&mem))
            {
                if(CDirMgr::GetFileSize(&mem) == this->mCurFileInfo->GetFileSize())
                {
                    if(this->IsLatestVersion() || (!this->mCheckVersion))
                    {
                        LOG("file \"%s\" is the latest version, will download from break point.\n",mem.CStr());
                        this->mStep = STEP_BIG_FILE_CREATED;
                        return OK;
                    }
                }               
            }
            this->DeletePartInfoFile(); //delete old .part.info file
            LOG("empty file \'%s\' will be created\n",mem.CStr());
            ASSERT(!IsTask(this->mTaskCreateBigFile));
            CTaskCreateBigFile *pt;
            NEW(pt,CTaskCreateBigFile);
            pt->Init(this->GetTaskMgr());
            pt->SetFileName(mem.CStr(),this->mCurFileInfo->GetFileSize());
            pt->SetInterval(1);
            pt->Start();
            pt->SetCallbackParam(10,this);
            pt->SetCallbackFunc(on_big_file_created);
            this->mTaskCreateBigFile = pt->GetID();
            this->Suspend();            
            this->mStep = STEP_BIG_FILE_CREATED;
        }
    }
    else if(this->mStep == STEP_BIG_FILE_CREATED)
    {
        if(!this->mDownloadToMem)
        {
            LOCAL_MEM(mem);

            this->GetFullDstPartFileName(&mem);
            this->mTmpDstFile->Destroy();
            this->mTmpDstFile->Init();
            if(!this->mTmpDstFile->OpenFile(mem.CStr(),"rb+"))
            {
                this->Stop(ERROR_OPEN_FILE_ERROR);
                return OK;
            }
            this->iDstFile = this->mTmpDstFile;
        }
        this->mStep = STEP_START_DOWNLOAD;
    }

    else if(this->mStep == STEP_START_DOWNLOAD)
    {
        this->LoadThreadInfos();
        this->CreateAutoSaveTimer();
        if(!this->IsAllThreadComplete())
            this->Suspend();
        if(this->mCurFileInfo->HasMd5Check())
            this->mStep = STEP_MD5_SUM;
        else
            this->mStep = STEP_FILE_DOWNLOAD_COMPLETE;
    }
    else if(this->mStep == STEP_MD5_SUM)
    {
        LOG("start md5 check\n");
        CTaskMd5Sum *pt = this->GetTaskMd5Sum(true);
        ASSERT(pt);
        pt->SetSrcFile(this->iDstFile);
        pt->Start();        
        this->Suspend();
        this->mStep = STEP_FILE_DOWNLOAD_COMPLETE;
    }
    else if(this->mStep == STEP_FILE_DOWNLOAD_COMPLETE)
    {
        if(this->mDownloadToMem)
        {
            this->SetCallbackParam(1,this->mDstFileInMem);
            this->RunCallback(EVENT_SINGLE_FILE_DONE);
            this->mFileInfoList->DelElem(0);
            this->mStep = STEP_PREPARE_NEXT_TASK;
        }
        else
        {
            if(this->Rename())
            {
                this->mFileInfoList->DelElem(0);
                this->mStep = STEP_PREPARE_NEXT_TASK;
            }
            else
            {
                this->Stop(ERROR_RENAME_FILE_ERROR);
            }
        }
    }

    return OK;
}
const char * CTaskFtpDownload::ErrorToString(int err)
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
    if(err == ERROR_RENAME_FILE_ERROR)
        return "rename file error";
    if(err == ERROR_SIZE_NOT_MATCH)
        return "size not match";
    if(err == ERROR_MD5_CHECK_FAIL)
        return "md5 check fail";
    if(err == ERROR_CREATE_BIG_FILE_ERROR)
        return "create big file error";
    if(err == ERROR_TASK_MD5_SUM_ERROR)
        return "task md5 sum error";
    return "unknown error";
}
status_t CTaskFtpDownload::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskFtpDownload ");
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
status_t CTaskFtpDownload::Start()
{
    this->mStep = STEP_RETRY;
    this->Resume();
    return OK;
}
status_t CTaskFtpDownload::Stop(status_t err)
{
    this->ReportError(err);
    this->Quit();
    this->mTmpDstFile->Destroy();
    this->SetCallbackParam(1,(void*)err);
    this->RunCallback(EVENT_ALL_DONE);
    return OK;
}

status_t CTaskFtpDownload::SetFtpServer(const char *serverName, int port, const char *user, const char *pass)
{
    ASSERT(serverName && user && pass);
    this->mLoginInfo->SetServer(serverName);
    this->mLoginInfo->SetPort(port);
    this->mLoginInfo->SetUser(user);
    this->mLoginInfo->SetPass(pass);
    return OK;
}

status_t CTaskFtpDownload::SetFtpServer(CFtpLoginInfo *info)
{
    ASSERT(info);
    this->mLoginInfo->Copy(info);
    return OK;
}

status_t CTaskFtpDownload::AddFileToDownload(const char *fileName, const char *md5, double version)
{
    ASSERT(fileName && md5);
    
    CFtpFileInfo info;
    info.Init();
    info.SetVersion(version);
    info.SetFileName(fileName);
    info.SetMd5Check(md5);

    return this->AddFileToDownload(&info);
}

status_t CTaskFtpDownload::AddFileToDownload(CFtpFileInfo *info)
{
    ASSERT(info);
    this->mFileInfoList->Push(info);
    return OK;
}

status_t CTaskFtpDownload::GetInfoFileName(CFileBase *out)
{
    ASSERT(out);

    LOCAL_MEM(path);
    LOCAL_MEM(single);
    LOCAL_MEM(mem);

    const char *fileName = this->mCurFileInfo->GetFileName();
    ASSERT(fileName);

    if(this->mKeepFolder)
    {
        mem.StrCpy(fileName);
    }
    else
    {
        CDirMgr::GetFileName(fileName,&mem,FN_FILENAME);
    }

    CDirMgr::GetFileName(&mem,&path,FN_PATH);
    CDirMgr::GetFileName(&mem,&single,FN_SINGLE);

    out->SetSize(0);
    out->Puts(this->mDstPath);
    out->Putc(CDirMgr::GetPathSplitor());
    
    if(path.StrLen() > 0)
    {
        out->Puts(&path);
        out->Putc(CDirMgr::GetPathSplitor());
    }

    out->Puts(&single);
    out->Puts(".part.info");

    return OK;
}
status_t CTaskFtpDownload::GetFullDstFileName(CFileBase *out)
{
    ASSERT(out);

    LOCAL_MEM(mem);

    const char *fileName = this->mCurFileInfo->GetFileName();
    ASSERT(fileName);

    if(this->mKeepFolder)
    {
        mem.StrCpy(fileName);
    }
    else
    {
        CDirMgr::GetFileName(fileName,&mem,FN_FILENAME);
    }

    out->SetSize(0);
    out->Puts(this->mDstPath);
    out->Putc(CDirMgr::GetPathSplitor());
    out->Puts(&mem);
    
    return OK;
}

status_t CTaskFtpDownload::GetFullDstPartFileName(CFileBase *out)
{
    this->GetFullDstFileName(out);
    out->StrCat(".part");
    return OK;
}

status_t CTaskFtpDownload::on_ftp_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpDownload *self = (CTaskFtpDownload*)p[10];
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
                LOG("ftp command (retr) error\n");
                goto retry;
            }
        }
        
        if(req_code == FTP_REQ_SIZE)
        {
            if(res_code == 213)
            {
                self->mCurFileInfo->SetFileSize(crt_str_to_fsize(res_body));
                self->Resume();
            }
            else
            {
                LOG("ftp command (size) error\n");
                goto retry;
            }
        }
        
        return OK;
        
retry:
        self->Retry();
        return ERROR;
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

    return OK;
}

CTaskFtpClient * CTaskFtpDownload::GetTaskFtpClientForSize(bool renew)
{
    CTaskFtpClient *pt = (CTaskFtpClient*)GetTask(this->mTaskFtpClientForSize);
    if(pt != NULL) return pt;
    if(!renew)return NULL;

    NEW(pt,CTaskFtpClient);
    pt->Init(this->GetTaskMgr());
    pt->SetServer(this->mLoginInfo->GetServer(),this->mLoginInfo->GetPort());
    pt->SetInterval(1);
    pt->SetTimeout(30*1000);

    pt->SetCallbackFunc(on_ftp_event);
    pt->SetCallbackParam(10,this);

    this->mTaskFtpClientForSize = pt->GetID();  
    return pt;
}

status_t CTaskFtpDownload::SetMaxRetries(int max)
{
    this->mMaxRetry = max;
    return OK;
}

status_t CTaskFtpDownload::Retry()
{
    LOG("ftp, some error occur, will retry\n"); 
    this->SaveThreadInfo();
    this->mStep = STEP_RETRY;
    this->Resume();
    return OK;
}

status_t CTaskFtpDownload::SetDownloadToMem(bool inMem)
{
    this->mDownloadToMem = inMem;
    return OK;
}

status_t CTaskFtpDownload::SetDstPath(const char *path)
{
    ASSERT(path);
    this->mDstPath->StrCpy(path);
    CDirMgr::ToAbsPath("",this->mDstPath);
    return OK;
}

status_t CTaskFtpDownload::SetKeepFolder(bool keep)
{
    this->mKeepFolder = keep;
    return OK;
}

status_t CTaskFtpDownload::SetCheckVersion(bool check)
{
    this->mCheckVersion = check;
    return OK;
}

CTaskMd5Sum * CTaskFtpDownload::GetTaskMd5Sum(bool renew)
{
    CTaskMd5Sum *pt = (CTaskMd5Sum*)GetTask(this->mTaskMd5Sum);
    if(pt != NULL) return pt;
    if(!renew) return NULL;

    NEW(pt,CTaskMd5Sum);
    pt->Init(this->GetTaskMgr());
    pt->SetInterval(0);
    pt->SetCallbackParam(10,this);
    pt->SetCallbackFunc(on_md5_sum_finish);
    this->mTaskMd5Sum = pt->GetID();
    return pt;
}

status_t CTaskFtpDownload::on_md5_sum_finish(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpDownload *self = (CTaskFtpDownload*)p[10];
    ASSERT(self);
    
    if(event == CTaskMd5Sum::EVENT_FINISH)
    {
        int_ptr_t err = (int_ptr_t)p[1];
        const char *md5 = (const char *)p[2];
        
        if(err == CTaskFtpDownload::ERROR_NONE)
        {
            ASSERT(md5);
            if(crt_stricmp(self->mCurFileInfo->GetMd5Check(),md5) == 0)
            {
                LOG("md5 check ok,download success.\n");
                self->Resume();
            }
            else
            {
                if(!self->mDownloadToMem)
                    self->DeleteDstPartFile();
                self->Stop(CTaskFtpDownload::ERROR_MD5_CHECK_FAIL);
            }
        }
        else
        {
            self->Stop(CTaskFtpDownload::ERROR_TASK_MD5_SUM_ERROR);
        }
    }
    return OK;
}

bool CTaskFtpDownload::IsLatestVersion()
{
    return mCheckVersion?(this->mCurFileInfo->GetVersion() <= this->mCurPartFileInfo->version):true;
}

status_t CTaskFtpDownload::on_big_file_created(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpDownload *self = (CTaskFtpDownload*)p[10];
    ASSERT(self);

    if(event == CTaskCreateBigFile::EVENT_FINISH)
    {
        int_ptr_t err = (int_ptr_t)p[1];
        if(err != CTaskCreateBigFile::ERROR_NONE)
        {
            self->Stop(CTaskFtpDownload::ERROR_CREATE_BIG_FILE_ERROR);
            return OK;
        }
        self->Resume();
    }
    return OK;
}

status_t CTaskFtpDownload::SetMaxConcurrency(int max)
{
    if(max <= 0)return ERROR;
    DEL_ARRAY(this->mAllThreads);
    NEW_ARRAY(this->mAllThreads,CFtpThreadInfo,max);
    for(int i = 0; i < max; i++)
    {
        this->mAllThreads[i].Init(this->GetTaskMgr());
    }
    this->mMaxConcurrency = max;
    return OK;
}

status_t CTaskFtpDownload::on_ftp_thread_finish(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];  
    if(event != CTaskFtpThread::EVENT_FINISH)
        return ERROR;

    int_ptr_t err = (int_ptr_t)p[1];
    CTaskFtpThread *thread = (CTaskFtpThread*)p[2];
    ASSERT(thread);

    CTaskFtpDownload *self = (CTaskFtpDownload*)p[10];
    ASSERT(self);
    CFtpThreadInfo *info = (CFtpThreadInfo*)p[11];
    ASSERT(info);
    int_ptr_t slot = (int_ptr_t)p[12];
    
    ASSERT(self->mAllThreads);
    if(err == CTaskFtpThread::ERROR_NONE)
    {
        LOG("ftp thread %d finished successfully.\n",slot);
        int max = self->FindMaxUncompleteBlock();

        if(max >= 0)
        {
            CFtpThreadInfo *maxInfo = &self->mAllThreads[max];
            fsize_t off,size;
            maxInfo->GetUncompleteBlock(&off,&size);
            if(size > 1024*1024)
            {
                char str_off[128],str_size[128];
                crt_fsize_to_str(off,str_off);
                crt_fsize_to_str(size,str_size);
                LOG("find empty block(off=%s,size=%s), will create a new thread\n",str_off,str_size);
                maxInfo->Destroy(); // stop current downloading task
                fsize_t half_size = size/2;
                self->CreateFtpThread(slot,off,half_size);
                self->CreateFtpThread(max,off+half_size,size - half_size);
            }
        }
    }
    else
    {
        LOG("ftp thread %d finished with error %d.\n",slot,err);
        fsize_t off,size;
        info->GetUncompleteBlock(&off,&size);
        self->CreateFtpThread(slot,off,size);
    }

    if(self->IsAllThreadStopped())
    {
        self->SaveThreadInfo();
        if(self->IsAllThreadComplete())
        {
            LOG("all ftp threads complete.\n");
            self->Resume();
        }
        else
        {
            self->Retry();
        }
    }
    return OK;
}

status_t CTaskFtpDownload::CreateFtpThread(int slot, fsize_t offset, fsize_t size)
{
    ASSERT(this->mAllThreads);
    ASSERT(slot >= 0 && slot < this->mMaxConcurrency);
    ASSERT(!this->mAllThreads[slot].IsTaskRunning());
    ASSERT(this->iDstFile);

    this->mAllThreads[slot].Destroy();
    this->mAllThreads[slot].Init(this->GetTaskMgr());
    CTaskFtpThread *pt = this->mAllThreads[slot].CreateTaskFtpThread(this->iDstFile,this->mCurFileInfo->GetFileName(),offset,size,this->mLoginInfo);
    ASSERT(pt);
    
    pt->SetCallbackParam(10,this);
    pt->SetCallbackParam(11,&mAllThreads[slot]);
    pt->SetCallbackParam(12,(void*)slot);
    pt->SetCallbackFunc(on_ftp_thread_finish);
    pt->Start();
    
    char str_offset[128],str_size[128];
    crt_fsize_to_str(offset,str_offset);
    crt_fsize_to_str(size,str_size);
    LOG("ftp thread %d created, offset=%s,size=%s\n",slot,str_offset,str_size);
    return OK;
}

status_t CTaskFtpDownload::InitAllThreads()
{
    fsize_t size = this->mCurFileInfo->GetFileSize();
    int max = this->mMaxConcurrency;

    if(max == 0) //auto
    {
        int blocks = (int)(size/(2*1024*1024));
        if(blocks < 1) blocks = 1;
        if(blocks > 10) blocks = 10;
        this->SetMaxConcurrency(blocks);
        max = blocks;
    }
    
    fsize_t block_size = size/max;
    for(int i = 0; i < max; i++)
    {
        if(i == max - 1)
        {
            this->CreateFtpThread(i,i*block_size,size - i*block_size);
        }
        else
        {
            this->CreateFtpThread(i,i*block_size,block_size);
        }
    }
    return OK;
}

bool CTaskFtpDownload::IsAllThreadStopped()
{
    ASSERT(this->mAllThreads);
    int max = this->mMaxConcurrency;
    for(int i = 0; i < max; i++)
    {
        if(this->mAllThreads[i].IsTaskRunning())
            return false;
    }

    return true;
}

bool CTaskFtpDownload::IsAllThreadComplete()
{
    ASSERT(this->mAllThreads);
    int max = this->mMaxConcurrency;
    for(int i = 0; i < max; i++)
    {
        if(!this->mAllThreads[i].IsComplete())
            return false;
    }

    return true;
}

int CTaskFtpDownload::FindMaxUncompleteBlock()
{
    ASSERT(this->mAllThreads);
    fsize_t t = 0,off,size;
    int index = -1;

    int max = this->mMaxConcurrency;
    for(int i = 0; i < max; i++)
    {       
        this->mAllThreads[i].GetUncompleteBlock(&off,&size);
        if(size > t)
        {
            index = i;
            t = size;
        }
    }
    return index;
}

status_t CTaskFtpDownload::SaveThreadInfo(CFileBase *out)
{
    ASSERT(out);
    ASSERT(this->mAllThreads);

    this->mCurPartFileInfo->version = this->mCurFileInfo->GetVersion();
    this->mCurPartFileInfo->totalSize = this->mCurFileInfo->GetFileSize();
    this->mCurPartFileInfo->SetFileName(this->mCurFileInfo->GetFileName());
    this->mCurPartFileInfo->SetMd5(this->mCurFileInfo->GetMd5Check());
    this->mCurPartFileInfo->size = this->GetDownloadedSize();

    out->SetSize(0);
    out->Printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");  
    out->Printf("<FtpPartFileInfo>\r\n");
    this->mCurPartFileInfo->SaveToXml(out);
    out->Printf("<ThreadCount>%d</ThreadCount>\r\n",this->mMaxConcurrency);
    out->Printf("<AllThreads>\r\n");
    for(int i = 0; i < this->mMaxConcurrency; i++)
    {       
        out->Printf("<Thread>\r\n");
        fsize_t off,size;
        char str[128];
        this->mAllThreads[i].GetUncompleteBlock(&off,&size);
        crt_fsize_to_str(off,str);
        out->Printf("<offset>%s</offset>\r\n",str);
        crt_fsize_to_str(size,str);
        out->Printf("<size>%s</size>\r\n",str);
        out->Printf("</Thread>\r\n");
    }
    out->Printf("</AllThreads>\r\n");
    out->Printf("</FtpPartFileInfo>\r\n");
    return OK;
}

status_t CTaskFtpDownload::SaveThreadInfo()
{
    if(this->mAllThreads == NULL)
        return ERROR;
    if(this->mDownloadToMem)
        return ERROR;
    if(!this->NeedSaveInfoFile())
        return ERROR;
    CMemFile mf;
    mf.Init();
    this->SaveThreadInfo(&mf);
    LOCAL_MEM(mem);
    this->GetInfoFileName(&mem);
    mf.WriteToFile(mem.CStr());
    return OK;
}

status_t CTaskFtpDownload::LoadPartFileInfo()
{
    LOCAL_MEM(mem);
    this->mCurPartFileInfo->Clear();
    this->GetInfoFileName(&mem);
    CXml xml;
    xml.Init();
    if(xml.LoadXml(mem.CStr()))
    {
        this->mCurPartFileInfo->LoadFromXml(xml.GetRoot());
    }
    return OK;
}

status_t CTaskFtpDownload::DeletePartInfoFile()
{
    LOCAL_MEM(mem);
    this->GetInfoFileName(&mem);
    return CDirMgr::DeleteFile(&mem);
}

status_t CTaskFtpDownload::LoadThreadInfos()
{
    LOCAL_MEM(mem);

    if(this->mDownloadToMem)
    {
        this->InitAllThreads();
        return OK;
    }

    this->GetInfoFileName(&mem);
    CXml xml;
    xml.Init();
    if(!xml.LoadXml(mem.CStr()))
    {
        this->InitAllThreads();
    }
    else
    {
        CXmlNode *px = xml.GetNodeByPath("/FtpPartFileInfo/ThreadCount");
        ASSERT(px);
        px->GetStringValue(&mem);
        int count = atoi(mem.CStr());
        ASSERT(count > 0 && count <= 50);
        this->SetMaxConcurrency(count);

        px = xml.GetNodeByPath("/FtpPartFileInfo/AllThreads");
        ASSERT(px);

        int slot = 0;
        px = px->child;
        while(px)
        {
            CXmlNode *node = px->GetChildByName("size");
            ASSERT(node);
            node->GetStringValue(&mem);
            fsize_t size = crt_str_to_fsize(mem.CStr());

            node=px->GetChildByName("offset");
            ASSERT(node);
            node->GetStringValue(&mem);
            fsize_t offset = crt_str_to_fsize(mem.CStr());
            if(size > 0)
            {
                this->CreateFtpThread(slot,offset,size);
                slot ++;
            }
            px = px->next;
        }
    }

    return OK;  
}

status_t CTaskFtpDownload::ResetAllThreads()
{
    return this->SetMaxConcurrency(this->mMaxConcurrency);
}

bool CTaskFtpDownload::NeedSaveInfoFile()
{
    ASSERT(this->mAllThreads);
    for(int i = 0; i < this->mMaxConcurrency; i++)
    {
        fsize_t off,size;
        this->mAllThreads[i].GetUncompleteBlock(&off,&size);

        if(off > 0 || size > 0)
            return true;
    }
    return false;
}

fsize_t CTaskFtpDownload::GetDownloadedSize()
{   
    if(this->mAllThreads != NULL)
    {
        fsize_t max_size = this->mCurFileInfo->GetFileSize();
        fsize_t total = 0;
        for(int i = 0; i < this->mMaxConcurrency; i++)
        {
            fsize_t off,size;
            this->mAllThreads[i].GetUncompleteBlock(&off,&size);
            total += size;
        }
        this->mCurPartFileInfo->size = max_size - total;
    }
    return this->mCurPartFileInfo->size;
}

status_t CTaskFtpDownload::Rename()
{
    ASSERT(!this->mDownloadToMem);
    LOCAL_MEM(name);
    LOCAL_MEM(partName);

    this->mTmpDstFile->Destroy(); //close file
    this->GetFullDstFileName(&name);
    this->GetFullDstPartFileName(&partName);

    CDirMgr::DeleteFile(&name);
    if(rename(partName.CStr(),name.CStr()))
    {
        return ERROR;
    }
    return OK;
}

status_t CTaskFtpDownload::DeleteDstPartFile()
{
    ASSERT(!this->mDownloadToMem);
    LOCAL_MEM(mem);
    this->mTmpDstFile->Destroy();
    this->GetFullDstPartFileName(&mem);
    return CDirMgr::DeleteFile(&mem);
}

status_t CTaskFtpDownload::CreateAutoSaveTimer()
{
    QuitTask(&this->mTimerAutoSave);
    CTaskTimer *pt = CTaskTimer::NewTimer(this->GetTaskMgr(),2000, false);
    ASSERT(pt);
    pt->SetCallbackFunc(on_autosave);
    pt->SetCallbackParam(10,this);
    this->mTimerAutoSave = pt->GetID();
    return OK;
}

status_t CTaskFtpDownload::on_autosave(void **p)
{
    CTaskFtpDownload *self = (CTaskFtpDownload*)p[10];
    ASSERT(self);

    fsize_t old_size = self->mCurPartFileInfo->size;
    fsize_t now_size = self->GetDownloadedSize();
    if(now_size > old_size)
    {
        self->SaveThreadInfo();
    }
    
    return OK;
}
