#include "taskftpserver.h"
#include "ftputils.h"
#include "dirmgr.h"
#include "fileinfolist.h"
#include "tasktimer.h"

status_t CTaskFtpServer::ERROR_NONE = 0;
status_t CTaskFtpServer::ERROR_SOCKET_CLOSED = 1;
status_t CTaskFtpServer::ERROR_PERMISSION_DENIED = 2;
status_t CTaskFtpServer::ERROR_CONNECT_TIMEOUT = 3;

#define STEP_ERROR                      0
#define STEP_HANDLE_REQUEST             1
#define STEP_READ_REQUEST               2
#define STEP_SEND_REQUEST               3
#define STEP_SEND_RESPONSE              4
#define STEP_WAIT_REQUEST_COMPLETE      5
#define STEP_WAIT_RESPONSE_COMPLETE     6
#define STEP_WAIT_CONNECT_COMPLETE      7
#define STEP_PORT_CONNECTED             8
#define STEP_WAIT_NLST_DATA_COMPLETE    9
#define STEP_WAIT_DATA_SEND_COMPLETE    10
#define STEP_WAIT_ACCEPT                11
#define STEP_WAIT_DATA_RECEIVE_COMPLETE 12


#define AFTER_RESPONSE_PARAM_NONE               0
#define AFTER_RESPONSE_PARAM_PERMISSION_DENIED  1
#define AFTER_RESPONSE_PARAM_QUIT               2
#define AFTER_RESPONSE_PARAM_WAIT_DATA          3
#define AFTER_RESPONSE_PARAM_WAIT_ACCEPT        4
#define AFTER_RESPONSE_PARAM_RECEIVE_FILE       5

#define TRANSFER_TYPE_ASCII     1
#define TRANSFER_TYPE_BINARY    2

CTaskFtpServer::CTaskFtpServer()
{
    this->InitBasic();
}
CTaskFtpServer::~CTaskFtpServer()
{
    this->Destroy();
}
status_t CTaskFtpServer::InitBasic()
{
    CTask::InitBasic();
    this->mTaskSocketReader = 0;
    this->mTaskSocketWriter = 0;
    this->mRequest = NULL;
    this->mResponse = NULL;
    this->iSocket = NULL;
    this->mStep = 0;
    this->iConfigList = NULL;
    this->mCurConfig = NULL;
    this->mAfterResponseParam = 0;
    this->mTmpFile = NULL;
    this->mTaskFtpSocketPort = 0;
    this->iDataSocket = NULL;
    this->mDataClientSocket = NULL;
    this->mTaskDataSocketReader = 0;
    this->mTaskDataSocketWriter = 0;
    this->mCurDir = NULL;
    this->mIoFile = NULL;
    this->mServer = NULL;
    this->mTransferType = TRANSFER_TYPE_BINARY;
    this->mDataServerSocket = NULL;
    this->mClientIP = NULL;
    this->mFileNameToBeRenamed = NULL;
    this->mAllowAnonymous = FALSE;
    this->mAnonymousRootPath = NULL;
    this->mRestPos = 0;
    this->mTimeout = 60*1000;
    this->mConnectTime = 0;
    this->mTimerSyncTimeout = 0;
    return OK;
}
status_t CTaskFtpServer::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    
    NEW(this->mRequest,CMem);
    this->mRequest->Init();
    this->mRequest->Malloc(1024);

    NEW(this->mResponse,CMem);
    this->mResponse->Init();
    this->mResponse->Malloc(1024);
    
    NEW(this->mCurConfig,CFtpConfig);
    this->mCurConfig->Init();

    NEW(this->mTmpFile,CMemFile);
    this->mTmpFile->Init();

    NEW(this->mDataClientSocket,CTcpClient);
    this->mDataClientSocket->Init();

    NEW(this->mCurDir,CMem);
    this->mCurDir->Init();
    this->mCurDir->Malloc(1024);
    this->mCurDir->StrCpy("");

    NEW(this->mIoFile,CFile);
    this->mIoFile->Init();

    NEW(this->mServer,CTcpServer);
    this->mServer->Init();

    NEW(this->mDataServerSocket,CTcpServer);
    this->mDataServerSocket->Init();

    NEW(this->mClientIP,CMem);
    this->mClientIP->Init();
    this->mClientIP->Malloc(128);

    NEW(this->mFileNameToBeRenamed,CMem);
    this->mFileNameToBeRenamed->Init();
    this->mFileNameToBeRenamed->Malloc(1024);

    NEW(this->mAnonymousRootPath,CMem);
    this->mAnonymousRootPath->Init();
    this->mAnonymousRootPath->Malloc(1024);
    this->mAnonymousRootPath->StrCpy("");

    CTaskTimer *timer = CTaskTimer::NewTimer(this->GetTaskMgr(),1000,false);
    ASSERT(timer);
    timer->SetCallbackParam(10,this);
    timer->SetCallbackFunc(on_sync_timeout);
    this->mTimerSyncTimeout = timer->GetID();

    return OK;
}
status_t CTaskFtpServer::Destroy()
{
    LOG("ftp server %d exit",this->id);
    DEL(this->mAnonymousRootPath);
    DEL(this->mFileNameToBeRenamed);
    DEL(this->mClientIP);
    DEL(this->mDataServerSocket);
    DEL(this->mServer);
    DEL(this->mIoFile);
    DEL(this->mCurDir);
    DEL(this->mDataClientSocket);
    DEL(this->mTmpFile);
    DEL(this->mCurConfig);
    DEL(this->mRequest);
    DEL(this->mResponse);
    QuitTask(&this->mTaskSocketReader);
    QuitTask(&this->mTaskSocketWriter);
    QuitTask(&this->mTaskDataSocketReader);
    QuitTask(&this->mTaskDataSocketWriter);
    QuitTask(&this->mTimerSyncTimeout);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskFtpServer::OnTimer(int interval)
{
    ASSERT(this->iSocket);

    CTaskSocketReader *reader;
    CTaskSocketWriter *writter;
    if(this->mStep == STEP_SEND_RESPONSE)
    {
        writter = this->GetWritter(true);
        ASSERT(writter);
        ASSERT(this->mResponse->GetSize() > 0);
        writter->SetSrcFile(this->mResponse);
        writter->Start();
        this->mStep = STEP_WAIT_RESPONSE_COMPLETE;
    }

    else if(this->mStep == STEP_WAIT_RESPONSE_COMPLETE)
    {
        this->Suspend();
    }
    
    else if(this->mStep == STEP_READ_REQUEST)
    {
        reader = this->GetReader(true);
        ASSERT(reader);
        reader->SetDstFile(NULL,0);
        this->mStep = STEP_WAIT_REQUEST_COMPLETE;
    }

    else if(this->mStep == STEP_WAIT_REQUEST_COMPLETE)
    {
        reader = this->GetReader(false);
        ASSERT(reader);
        if(reader->mQBuf->HasWholeLine())
        {
            reader->mQBuf->ReadLine(this->mRequest);
            ASSERT(this->mRequest->StrLen() > 0);
            this->mStep = STEP_HANDLE_REQUEST;
        }
    }

    else if(this->mStep == STEP_HANDLE_REQUEST)
    {
        this->mStep = STEP_SEND_RESPONSE;
        this->HandleRequest();      
    }

    else if(this->mStep == STEP_WAIT_CONNECT_COMPLETE)
    {
        if(!this->mDataClientSocket->IsConnectComplete())
        {
            this->mConnectTime += interval;
            if(this->mConnectTime >= this->mTimeout)
            {
                this->Stop(ERROR_CONNECT_TIMEOUT);
            }
        }
        else
        {
            this->mConnectTime = 0;
            this->mStep = STEP_PORT_CONNECTED;  
        }
    }

    else if(this->mStep == STEP_PORT_CONNECTED)
    {
        this->PrepareResponse("200 PORT command successful");
        this->mStep = STEP_SEND_RESPONSE;
    }

    else if(this->mStep == STEP_WAIT_DATA_SEND_COMPLETE)
    {
        this->Suspend();
    }

    else if(this->mStep == STEP_WAIT_ACCEPT)
    {
        int snum = this->mDataServerSocket->Accept();
        if(snum > 0)
        {
            this->mDataServerSocket->Destroy();
            this->mDataServerSocket->Init();
            this->mDataServerSocket->AttachSocketFd(snum);
            this->mDataServerSocket->SetBlocking(0);
            this->iDataSocket = this->mDataServerSocket;
            this->mStep = STEP_READ_REQUEST;
        }       
    }

    else if(this->mStep == STEP_WAIT_DATA_RECEIVE_COMPLETE)
    {
        this->Suspend();
    }

    return OK;
}

CTaskSocketReader * CTaskFtpServer::GetReader(status_t renew)
{
    ASSERT(this->iSocket);

    CTaskSocketReader *pt = (CTaskSocketReader *)GetTask(this->mTaskSocketReader);
    if(pt == NULL && renew)
    {
        NEW(pt,CTaskSocketReader);
        pt->Init(this->GetTaskMgr());
        pt->SetSocket(this->iSocket);
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);
        this->mTaskSocketReader = pt->id;
        pt->SetCallbackParam(10,this);
        pt->SetCallbackFunc(on_reader_event);

        pt->Start();
    }
    return pt;
}

CTaskSocketWriter * CTaskFtpServer::GetWritter(status_t renew)
{
    ASSERT(this->iSocket);
    
    CTaskSocketWriter *pt = (CTaskSocketWriter *)GetTask(this->mTaskSocketWriter);
    if(pt == NULL && renew)
    {       
        NEW(pt,CTaskSocketWriter);
        pt->Init(this->GetTaskMgr());
        pt->SetSocket(this->iSocket);
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);
        pt->SetCallbackFunc(on_writer_event);
        pt->SetCallbackParam(10,this);
        this->mTaskSocketWriter = pt->id;
        pt->Start();
    }   
    return pt;
}

status_t CTaskFtpServer::Stop(status_t err)
{
    this->ReportError(err);
    this->Quit();
    return OK;
}

status_t CTaskFtpServer::Start()
{
    this->mStep = STEP_SEND_RESPONSE;
    this->Resume();
    return OK;
}

status_t CTaskFtpServer::PrepareRequest(CFileBase *file)
{
    ASSERT(file);

    this->mRequest->SetSize(0);
    this->mRequest->WriteFile(file);
    return OK;
}

status_t CTaskFtpServer::PrepareRequest(const char *request)
{
    ASSERT(request);

    this->mRequest->SetSize(0);
    this->mRequest->Puts(request);
    this->mRequest->Puts("\r\n");
    return OK;
}

status_t CTaskFtpServer::PrepareResponse(CFileBase *file)
{
    ASSERT(file);

    this->mResponse->SetSize(0);
    this->mResponse->WriteFile(file);
    return OK;
}

status_t CTaskFtpServer::PrepareResponse(const char *response)
{
    ASSERT(response);

    this->mResponse->SetSize(0);
    this->mResponse->Puts(response);
    this->mResponse->Puts("\r\n");
    return OK;
}

status_t CTaskFtpServer::UpdateCurConfig()
{
    ASSERT(this->iConfigList);
    CFtpConfig *p = this->iConfigList->SearchByUserNameAndPassword(this->mCurConfig);

    if(p == NULL)
    {
        this->mCurConfig->Empty();
        if(this->mAllowAnonymous)
        {
            this->mCurConfig->SetUserName("anonymouse");
            this->mCurConfig->SetRootPath(this->mAnonymousRootPath->CStr());
        }
    }
    else
    {
        this->mCurConfig->Copy(p);
    }

    return OK;
}

status_t CTaskFtpServer::HandleRequest()
{
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);
    LOCAL_MEM(verb);
    LOCAL_MEM(params);

    CFtpUtils::ParseRequest(this->mRequest,&verb,&params);

LOG("---%s",this->mRequest->CStr());

    if(verb.StrICmp("user") == 0)
    {
        this->mCurConfig->Empty();
        this->mCurConfig->SetUserName(params.CStr());
        mem.SetSize(0);
        mem.Puts("331 Password required for ");
        mem.Puts(params.CStr());
        mem.Putc(0);
        this->PrepareResponse(mem.CStr());
    }

    else if(verb.StrICmp("pass") == 0)
    {
        this->mCurConfig->SetPassword(params.CStr());
        this->UpdateCurConfig();

        if(this->mCurConfig->IsEmpty())
        {
            this->PrepareResponse("530 Permission denied");
            this->mAfterResponseParam = AFTER_RESPONSE_PARAM_PERMISSION_DENIED;
        }
        else
        {
            this->PrepareResponse("230 Login OK");
        }
    }
    
    else if(verb.StrICmp("port") == 0)
    {
        int port;   
        CFtpUtils::ParsePortParams(&params,&mem,&port);
        this->mDataClientSocket->Destroy();
        this->mDataClientSocket->Init();
        this->mDataClientSocket->SetServerIP(mem.CStr());
        this->mDataClientSocket->SetPort(port);
        this->mDataClientSocket->SetBlocking(0);
        this->mDataClientSocket->Connect();
        this->iDataSocket = this->mDataClientSocket;
        this->mConnectTime = 0;
        this->mStep = STEP_WAIT_CONNECT_COMPLETE;
    }

    else if(verb.StrICmp("nlst") == 0)
    {
        if(params.StrCmp("*") == 0)
            params.StrCpy("");
        this->ToAbsPath(&params);
        this->MakeFileList(params.CStr(),this->mTmpFile);
        CTaskSocketWriter *writer = this->GetDataWritter(true);
        ASSERT(writer);
        writer->SetSrcFile(this->mTmpFile);
        writer->Suspend();
        this->PrepareResponse("150 Opening ASCII mode data connection for NLST.");
        this->mAfterResponseParam = AFTER_RESPONSE_PARAM_WAIT_DATA;
    }

    else if(verb.StrICmp("retr") == 0)
    {
        mem.StrCpy(&params);

        this->ToAbsPath(&mem);
        this->mIoFile->Destroy();
        this->mIoFile->Init();
        if(!this->mIoFile->OpenFile(mem.CStr(),"rb"))
        {
            mem.SetSize(0);
            mem.Puts("550 Can't find file\"");
            mem.Puts(params.CStr());
            mem.Puts("\"\r\n");
            this->PrepareResponse(&mem);
        }
        else
        {
            CTaskSocketWriter *writer = this->GetDataWritter(true);
            ASSERT(writer);
            writer->SetSrcFile(this->mRestPos,this->mIoFile);           
            writer->Suspend();
            this->PrepareResponse("150 Opening Binary mode data connection for RETR.");
            this->mAfterResponseParam = AFTER_RESPONSE_PARAM_WAIT_DATA;
        }
    }

    else if(verb.StrICmp("quit") == 0)
    {
        this->PrepareResponse("221 Good-Bye");
        this->mAfterResponseParam = AFTER_RESPONSE_PARAM_QUIT;
    }

    else if(verb.StrICmp("cwd") == 0)
    {
        this->ChangeDir(params.CStr());
    }

    else if(verb.StrICmp("xpwd") == 0 || verb.StrICmp("pwd") == 0)
    {
        mem.StrCpy(this->mCurDir);
        CFtpUtils::ToUnixPath(&mem);
        
        buf.SetSize(0);
        buf.Puts("257 \"");
        buf.Puts(&mem);
        buf.Puts("\"\r\n");

        this->PrepareResponse(&buf);
    }

    else if(verb.StrICmp("list") == 0)
    {
        mem.StrCpy(&params);

        if(params.StartWith("-a",0,1))
        {
            params.Seek(0);
            params.ReadString(&mem);
            params.ReadLeftStr(&mem,1);
        }
        this->ToAbsPath(&mem);
        this->MakeDirList(mem.CStr(),this->mTmpFile);
        CTaskSocketWriter *writer = this->GetDataWritter(true);
        ASSERT(writer);
        writer->SetSrcFile(this->mTmpFile);
        writer->Suspend();
        this->PrepareResponse("150 Opening ASCII mode data connection for LIST.");
        this->mAfterResponseParam = AFTER_RESPONSE_PARAM_WAIT_DATA;
    }

    else if(verb.StrICmp("type") == 0)
    {
        if(params.StrICmp("ascii") == 0 || params.StrICmp("A") == 0)
        {
            this->PrepareResponse("200 Type set to A");
            this->mTransferType = TRANSFER_TYPE_ASCII;
        }
        else
        {
            this->PrepareResponse("200 Type set to I");
            this->mTransferType = TRANSFER_TYPE_BINARY;
        }
    }

    else if(verb.StrICmp("noop") == 0)
    {
        this->PrepareResponse("200 OK");
    }

    else if(verb.StrICmp("pasv") == 0)
    {       
        int port = 1982,ret;
        do{
            port++;
            this->mDataServerSocket->Destroy();
            this->mDataServerSocket->Init();
            this->mDataServerSocket->InitServer();
            ret = this->mDataServerSocket->SetPort(port);
        }while(ret == ERROR);

        this->mDataServerSocket->SetBlocking(0);

        LOCAL_MEM(tmp);
        tmp.Puts("227 Entering Passive Mode (");

        CSocket::GetAllIP(&mem);
        CFtpUtils::FindNearestIp(&mem,this->mClientIP->CStr(),&buf);

        buf.Seek(0);
        buf.ReplaceStr(".",",",0,0,&mem);
        tmp.Puts(&mem);
        tmp.Printf(",%d,",port / 256);
        tmp.Printf("%d)\r\n",port % 256);

        this->PrepareResponse(&tmp);
        this->mAfterResponseParam = AFTER_RESPONSE_PARAM_WAIT_ACCEPT;
    }

    else if(verb.StrICmp("opts") == 0)
    {
        this->PrepareResponse("502 This function is disabled");
    }

    else if(verb.StrICmp("mkd") == 0 || verb.StrICmp("xmkd") == 0)
    {
        buf.StrCpy(&params);
        this->ToAbsPath(&buf);
        status_t success = 0;

        if(!CDirMgr::IsDirExist(&buf))
        {
            if(CDirMgr::CreateDirSuper(&buf))
            {
                success = 1;
                mem.SetSize(0);
                mem.Puts("257 \"");
                mem.Puts(params.CStr());
                mem.Puts("\" directory created\r\n");

                this->PrepareResponse(&mem);
            }
        }

        if(!success)
        {
            this->PrepareResponse("521 Can't create directory");
        }
    }

    else if(verb.StrICmp("rmd") == 0 || verb.StrICmp("xrmd") == 0)
    {
        this->ToAbsPath(&params);
        
        if(CDirMgr::DeleteDir(params.CStr()))
            this->PrepareResponse("250 Directory removed");
        else
            this->PrepareResponse("550 Can't remove directory");
    }
    
    else if(verb.StrICmp("dele") == 0)
    {
        this->ToAbsPath(&params);
        if(CDirMgr::DeleteFile(&params))
            this->PrepareResponse("200 Deleted file");
        else
            this->PrepareResponse("550 Can't delete file");         
    }

    else if(verb.StrICmp("stor") == 0)
    {
        this->ToAbsPath(&params);
    
        this->mIoFile->Destroy();
        this->mIoFile->Init();
        if(!this->mIoFile->OpenFile(params.CStr(),"wb+"))
        {
            this->PrepareResponse("451 Can't create file");
        }
        else
        {
            CTaskSocketReader *reader = this->GetDataReader(true);
            ASSERT(reader);
            reader->SetDstFile(this->mIoFile,MAX_FSIZE);
            this->PrepareResponse("150 Opening BINARY mode for file.");
            this->mAfterResponseParam = AFTER_RESPONSE_PARAM_RECEIVE_FILE;
        }
    }

    else if(verb.StrICmp("rnfr") == 0)
    {
        this->mFileNameToBeRenamed->StrCpy(&params);
        this->PrepareResponse("350 RNFR OK, ready for destination name");
    }

    else if(verb.StrICmp("rnto") == 0)
    {
        mem.StrCpy(this->mFileNameToBeRenamed);
        this->ToAbsPath(&mem);
        buf.StrCpy(&params);
        this->ToAbsPath(&buf);

        if(rename(mem.CStr(),buf.CStr()) == 0)
            this->PrepareResponse("200 File renamed");
        else
            this->PrepareResponse("550 rename fail");
    }

    else if(verb.StrICmp("syst") == 0)
    {
        this->PrepareResponse("215 UNIX Type: L8");
    }
    
    else if(verb.StrICmp("cdup") == 0)
    {
        this->ChangeDir("..");
    }
    
    else if(verb.StrICmp("size") == 0)
    {
        this->ToAbsPath(&params);
        fsize_t size = CDirMgr::GetFileSize(&params);
        mem.SetSize(0);
        char str[128];
        crt_fsize_to_str(size,str);
        mem.Printf("213 %s\r\n",str);
        this->PrepareResponse(&mem);            
    }
                
    else if(verb.StrICmp("rest") == 0)
    {
        this->mRestPos = crt_str_to_fsize(params.CStr());
        this->PrepareResponse("350 reset postion ok");
    }

    else 
    {
        this->PrepareResponse("550 Permission denied");
        LOG("unknown request: %s",this->mRequest->CStr());
    }

    return OK;
}

status_t CTaskFtpServer::SetConfigs(CFtpConfigList *i_list)
{
    ASSERT(i_list);
    this->iConfigList = i_list;
    return OK;
}

status_t CTaskFtpServer::AfterResponse()
{
    if(this->mAfterResponseParam == AFTER_RESPONSE_PARAM_PERMISSION_DENIED)
    {
        ASSERT(this->iSocket);
        this->iSocket->Destroy();
        this->Stop(ERROR_PERMISSION_DENIED);
    }

    if(this->mAfterResponseParam == AFTER_RESPONSE_PARAM_QUIT)
    {
        this->iSocket->Destroy();
        this->Stop(ERROR_NONE);
    }

    if(this->mAfterResponseParam == AFTER_RESPONSE_PARAM_WAIT_DATA)
    {
        CTaskSocketWriter *writer = this->GetDataWritter(false);
        ASSERT(writer);
        writer->Resume();
        this->mStep = STEP_WAIT_DATA_SEND_COMPLETE;
    }

    if(this->mAfterResponseParam == AFTER_RESPONSE_PARAM_WAIT_ACCEPT)
    {
        this->mStep = STEP_WAIT_ACCEPT;
    }

    if(this->mAfterResponseParam == AFTER_RESPONSE_PARAM_RECEIVE_FILE)
    {
        this->mStep = STEP_WAIT_DATA_RECEIVE_COMPLETE;
    }
    this->mAfterResponseParam = AFTER_RESPONSE_PARAM_NONE;
    return OK;
}

CTaskSocketReader * CTaskFtpServer::GetDataReader(status_t renew)
{
    ASSERT(this->iDataSocket);
    ASSERT(this->iDataSocket->IsConnected());

    CTaskSocketReader *pt = (CTaskSocketReader *)GetTask(this->mTaskDataSocketReader);
    if(pt == NULL && renew)
    {
        NEW(pt,CTaskSocketReader);
        pt->Init(this->GetTaskMgr());
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);
        this->mTaskDataSocketReader = pt->id;
        pt->SetCallbackFunc(on_data_reader_event);
        pt->SetCallbackParam(10,this);
        pt->Start();
    }
    if(pt)pt->SetSocket(this->iDataSocket);
    return pt;
}

CTaskSocketWriter * CTaskFtpServer::GetDataWritter(status_t renew)
{
    ASSERT(this->iDataSocket);
    ASSERT(this->iDataSocket->IsConnected());

    CTaskSocketWriter *pt = (CTaskSocketWriter *)GetTask(this->mTaskDataSocketWriter);
    if(pt == NULL && renew)
    {       
        NEW(pt,CTaskSocketWriter);
        pt->Init(this->GetTaskMgr());
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);
        this->mTaskDataSocketWriter = pt->id;
        pt->SetCallbackFunc(on_data_writer_event);
        pt->SetCallbackParam(10,this);
        pt->Start();
    }       
    if(pt)pt->SetSocket(this->iDataSocket);
    return pt;
}

status_t CTaskFtpServer::MakeFileList(const char *dir, CFileBase *list)
{
    ASSERT(dir && list);
        
    CFileInfoList tmp;
    tmp.Init(1024*10);
    tmp.GetFileList(dir,false);

    list->SetSize(0);
    for(int i = 0; i < tmp.GetLen(); i++)
    {
        CFileInfo *pinfo = tmp.GetElem(i);
        ASSERT(pinfo);
        if(!pinfo->mIsDir)
        {
            list->Puts(pinfo->GetName());
            list->Puts("\r\n");
        }
    }

    return OK;
}

status_t CTaskFtpServer::ToAbsPath(CFileBase *file)
{
    ASSERT(file);

    LOCAL_MEM(mem);

    if(!CDirMgr::IsPathSplitor(file->CharAt(0)))
    {
        mem.Puts(this->mCurDir);
        mem.Putc(CDirMgr::GetPathSplitor());
    }
    mem.Puts(file);
    mem.Putc(0);

    CDirMgr::ToAbsPath(this->mCurConfig->mRootPath->CStr(),&mem);
    if(!mem.StartWith(this->mCurConfig->mRootPath->CStr(),0,1))
    {
        mem.StrCpy(this->mCurConfig->mRootPath);
    }

    file->SetSize(0);
    file->WriteFile(&mem);
    return OK;
}

status_t CTaskFtpServer::ChangeDir(const char *dir)
{
    ASSERT(dir);

    LOCAL_MEM(buf);
    LOCAL_MEM(mem);

    if(!CDirMgr::IsPathSplitor(dir[0]))
    {
        mem.Puts(this->mCurDir);
        mem.Putc(CDirMgr::GetPathSplitor());
    }
    mem.Puts(dir);
    mem.Putc(0);
    CDirMgr::ToAbsPath("",&mem);

    buf.StrCpy(dir);
    this->ToAbsPath(&buf);
    if(CDirMgr::IsDirExist(&buf))
    {
        this->mCurDir->StrCpy(&mem);
        this->PrepareResponse("250 Directory successfully changed.");
    }
    else
    {
        buf.SetSize(0);
        buf.Puts("450 Can't change directory to \"");
        buf.Puts(mem.CStr());
        buf.Puts("\"\r\n");
        this->PrepareResponse(&buf);
    }

    return OK;
}

status_t CTaskFtpServer::MakeDirList(const char *dir, CFileBase *list)
{
    ASSERT(dir && list);
    
    LOCAL_MEM(mem);

    CFileInfoList tmp;
    tmp.Init(1024*10);
    tmp.GetFileList(dir,0);
    
    list->SetSize(0);
    for(int i = 0; i < tmp.GetLen(); i++)
    {
        CFileInfo *pinfo = tmp.GetElem(i);
        ASSERT(pinfo);

        if(pinfo->mIsDir)
            list->Putc('d');
        else
            list->Putc('-');

        list->Puts("rw-rw-rw- 1 ftp ftp");
        
        mem.SetSize(0);
        char str[128];
        crt_fsize_to_str(pinfo->mSize,str);
        mem.Printf("%s",str);
    
        list->FillBlock(17-mem.StrLen(),' ');
        list->Puts(&mem);
        list->Puts(" ");    
        CFtpUtils::TimeToFtpTimeStr(time(NULL),&mem);
        list->Puts(&mem);
        list->Puts(" ");
        list->Puts(pinfo->GetName());
        list->Puts("\r\n");
    }

    return OK;
}

status_t CTaskFtpServer::SetServerSocket(int snum,const char *client_ip)
{
    ASSERT(client_ip);
    this->mClientIP->StrCpy(client_ip);
    this->mServer->Destroy();
    this->mServer->Init();
    this->mServer->AttachSocketFd(snum);
    this->mServer->SetBlocking(0);
    this->iSocket = this->mServer;
    return OK;
}
status_t CTaskFtpServer::AllowAnonymouse(status_t enable)
{
    this->mAllowAnonymous = enable;
    return true;
}

status_t CTaskFtpServer::SetAnonymouseRootPath(const char *str)
{
    ASSERT(str);
    this->mAnonymousRootPath->StrCpy(str);
    return OK;
}

status_t CTaskFtpServer::on_reader_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpServer *self = (CTaskFtpServer *)p[10];
    ASSERT(self);

    if(event == CTaskSocketReader::EVENT_QUIT)
    {
        self->Stop(ERROR_SOCKET_CLOSED);
    }

    return OK;
}
status_t CTaskFtpServer::on_writer_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpServer *self = (CTaskFtpServer *)p[10];
    ASSERT(self);

    if(event == CTaskSocketWriter::EVENT_FINISH)
    {
        if(self->mStep == STEP_WAIT_RESPONSE_COMPLETE)
        {
            _syslog_puts(">>>");
            self->mResponse->Dump();

            self->Resume();
            self->mStep = STEP_READ_REQUEST;
            self->AfterResponse();
        }
    }

    else if(event == CTaskSocketWriter::EVENT_QUIT)
    {
        self->Stop(ERROR_SOCKET_CLOSED);
    }
    return OK;
}

status_t CTaskFtpServer::on_data_reader_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpServer *self = (CTaskFtpServer *)p[10];
    ASSERT(self);

    if(event == CTaskSocketReader::EVENT_QUIT)
    {
        ASSERT(self->iDataSocket);
        ASSERT(!self->iDataSocket->IsConnected());
        self->Resume();
        self->mIoFile->Destroy();
        self->mRestPos = 0;
        self->PrepareResponse("226 Transfer complete.");
        self->mStep = STEP_SEND_RESPONSE;
    }
    return OK;
}

status_t CTaskFtpServer::on_data_writer_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpServer *self = (CTaskFtpServer *)p[10];
    ASSERT(self);
    
    if(event == CTaskSocketWriter::EVENT_FINISH)
    {
        if(self->mStep == STEP_WAIT_DATA_SEND_COMPLETE)
        {
            self->Resume();
            ASSERT(self->iDataSocket);
            self->iDataSocket->Destroy();
            self->mIoFile->Destroy();
            self->mRestPos = 0;
            self->PrepareResponse("226 Transfer complete.");
            self->mStep = STEP_SEND_RESPONSE;
        }
    }
    return OK;
}
status_t CTaskFtpServer::SetTimeout(int timeout)
{
    this->mTimeout=timeout;
    return OK;
}

const char * CTaskFtpServer::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "ERROR_NONE";
    if(err == ERROR_SOCKET_CLOSED)
        return "ERROR_SOCKET_CLOSED";
    if(err == ERROR_PERMISSION_DENIED)
        return "ERROR_PERMISSION_DENIED";
    if(err == ERROR_CONNECT_TIMEOUT)
        return "ERROR_CONNECT_TIMEOUT";
    return "unknown error";
}

status_t CTaskFtpServer::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskFtpServer stopped with error:");
    mem.Puts(this->ErrorToString(err));
    mem.Putc(0);

    if(err != ERROR_NONE)
    {
        LOG("%s",mem.CStr());
    }
    return OK;
}

status_t CTaskFtpServer::SyncTimeout()
{
    if(iDataSocket == NULL) return ERROR;
    if(iSocket == NULL) return ERROR;
    if(!iDataSocket->IsConnected())return ERROR;
    if(!iSocket->IsConnected())return ERROR;
    
    int base = -1;
    CTaskSocketReader *data_reader = this->GetDataReader(false);
    if(data_reader != NULL)
    {
        base = data_reader->mReadTime;      
    }
    else
    {
        CTaskSocketWriter *data_writer = this->GetDataWritter(false);
        if(data_writer != NULL)
        {
            base = data_writer->mWriteTime;
        }
    }
    if(base >= 0)
    {
        CTaskSocketReader *reader = this->GetReader(false);
        if(reader != NULL)
            reader->mReadTime = base;
        CTaskSocketWriter *writer = this->GetWritter(false);
        if(writer != NULL)
            writer->mWriteTime = base;
    }
    return OK;
}

status_t CTaskFtpServer::on_sync_timeout(void **p)
{
    CTaskFtpServer *self = (CTaskFtpServer*)p[10];
    ASSERT(self);
    self->SyncTimeout();
    return OK;
}

