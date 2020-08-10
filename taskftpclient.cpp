#include "taskftpclient.h"
#include "mem.h"
#include "ftputils.h"
#include "tasktimer.h"

#define STEP_ERROR                      0
#define STEP_RESOLVE_HOST               1
#define STEP_INIT_TCP_CLIENT            2
#define STEP_WAIT_CONNECT               3
#define STEP_READ_RESPONSE              4
#define STEP_WAIT_RESPONSE_COMPLETE     6
#define STEP_SEND_REQUEST               7
#define STEP_WAIT_REQUEST_COMPLETE      8
#define STEP_IDLE                       9
#define STEP_INIT_DATA_SOCKET           10
#define STEP_WAIT_DATA_CONNECT          11
#define STEP_READ_DATA                  12
#define STEP_WRITE_DATA                 13

int CTaskFtpClient::EVENT_QUIT = 1;
int CTaskFtpClient::EVENT_ON_RESPONSE = 2;

status_t CTaskFtpClient::ERROR_NONE = 0;
status_t CTaskFtpClient::ERROR_SOCKET_CLOSED = 1;
status_t CTaskFtpClient::ERROR_PERMISSION_DENIED = 2;
status_t CTaskFtpClient::ERROR_CONNECT_TIMEOUT = 3;
status_t CTaskFtpClient::ERROR_UNKNOWN_HOST = 4;
status_t CTaskFtpClient::ERROR_CONNECT = 5;
status_t CTaskFtpClient::ERROR_RESPONSE=6;
status_t CTaskFtpClient::ERROR_AUTH_FAIL=7;


CTaskFtpClient::CTaskFtpClient()
{
    this->InitBasic();
}
CTaskFtpClient::~CTaskFtpClient()
{
    this->Destroy();
}
status_t CTaskFtpClient::InitBasic()
{
    CTask::InitBasic();

    this->mTimeout = 60*1000;
    this->mTcpClient = NULL;
    this->mTaskSocketReader = 0;
    this->mTaskSocketWriter = 0;
    this->mTaskDataSocketReader = 0;
    this->mTaskDataSocketWriter = 0;
    this->mServerName = NULL;
    this->mPort = 0;
    this->mHostToIpContext = NULL;
    this->mStep = STEP_ERROR;
    this->mConnectTime = 0;
    this->mRequest = NULL;
    this->mResponse = NULL;
    this->mUserRequests = NULL;
    this->mTcpDataClient = NULL;
    this->mDataIP = NULL;
    this->mDataPort = 0;
    this->iDstFile = NULL;
    this->iSrcFile = NULL;
    this->mTimerSyncTimeout = 0;
    this->mMaxDstFileSize = MAX_FSIZE;
    return OK;
}
status_t CTaskFtpClient::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    
    NEW(this->mTcpClient,CTcpClient);
    this->mTcpClient->Init();

    NEW(this->mTcpDataClient,CTcpClient);
    this->mTcpDataClient->Init();

    NEW(this->mServerName,CMem);
    this->mServerName->Init();
    this->mServerName->Malloc(1024);
    
    NEW(this->mRequest,CMem);
    this->mRequest->Init();
    this->mRequest->Malloc(1024);
    this->mRequest->StrCpy("");

    NEW(this->mResponse,CMem);
    this->mResponse->Init();
    this->mResponse->Malloc(64*1024);

    NEW(this->mUserRequests,CMemStk);
    this->mUserRequests->Init(256);

    NEW(this->mDataIP,CMem);
    this->mDataIP->Init();
    this->mDataIP->Malloc(128);

    CTaskTimer *timer = CTaskTimer::NewTimer(mgr,1000,false);
    ASSERT(timer);
    timer->SetCallbackFunc(on_sync_timeout);
    timer->SetCallbackParam(10,this);
    this->mTimerSyncTimeout = timer->GetID();

    return OK;
}
status_t CTaskFtpClient::Destroy()
{
    crt_free_host_to_ip_context(&this->mHostToIpContext);
    QuitTask(&this->mTaskDataSocketReader);
    QuitTask(&this->mTaskDataSocketWriter);
    QuitTask(&this->mTaskSocketReader);
    QuitTask(&this->mTaskSocketWriter);
    QuitTask(&this->mTimerSyncTimeout); 
    DEL(this->mDataIP);
    DEL(this->mTcpDataClient);
    DEL(this->mUserRequests);
    DEL(this->mRequest);
    DEL(this->mResponse);
    DEL(this->mServerName);
    DEL(this->mTcpClient);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskFtpClient::OnTimer(int interval)
{
    if(this->mStep == STEP_RESOLVE_HOST)
    {
        crt_host_to_ip_async(this->GetServerName(),&this->mHostToIpContext);
        this->mStep = STEP_INIT_TCP_CLIENT;
    }
    else if(this->mStep == STEP_INIT_TCP_CLIENT)
    {
        ASSERT(this->mHostToIpContext);
        if(this->mHostToIpContext[0] == 0)
            return ERROR;
        if(this->mHostToIpContext[1] == 0)
        {
            this->Stop(ERROR_UNKNOWN_HOST);
            return ERROR;
        }
        if(!this->InitTcpClient())
        {
            this->Stop(ERROR_CONNECT);
        }
        this->mTcpClient->Connect();
        this->mStep = STEP_WAIT_CONNECT;
    }

    else if(mStep ==  STEP_WAIT_CONNECT)
    {
        if(this->mTcpClient->IsConnectComplete())
        {
            this->mConnectTime = 0;
            this->mStep = STEP_READ_RESPONSE;
        }
        else
        {
            this->mConnectTime += interval;
            if(this->mConnectTime >= this->mTimeout)
            {
                this->mConnectTime = 0;
                this->Stop(ERROR_CONNECT_TIMEOUT);
            }
        }
    }

    else if(mStep == STEP_READ_RESPONSE)
    {
        CTaskSocketReader *reader = this->GetSocketReader(true);
        ASSERT(reader);
        this->mStep = STEP_WAIT_RESPONSE_COMPLETE;
    }
    else if(mStep == STEP_WAIT_RESPONSE_COMPLETE)
    {
        CTaskSocketReader *reader = this->GetSocketReader(false);
        ASSERT(reader);
        if(reader->BufHasWholeLine())
        {
            LOCAL_MEM(mem);
            reader->mQBuf->ReadLine(&mem);
            this->mResponse->Puts(&mem);
            this->mResponse->Puts("\r\n");
            if(this->IsEndResponseLine(&mem))
            {
                this->OnResponse(this->mResponse);
                this->mResponse->SetSize(0);
            }
        }
    }

    else if(mStep == STEP_SEND_REQUEST)
    {
        CTaskSocketWriter *writer = this->GetSocketWriter(true);
        ASSERT(writer);

        this->mRequest->SetSize(0);
        if(this->mUserRequests->GetLen() <= 0)
        {
            this->mStep = STEP_IDLE;
            this->Suspend();
            return OK;
        }
        else
        {
            CMem *req = this->mUserRequests->GetElem(0);
            ASSERT(req);
            LOG("(%d)=== %s",this->GetID(),req->CStr());
            this->mRequest->SetSize(0);
            this->mRequest->Puts(req);
            this->mRequest->Puts("\r\n");           
            this->mUserRequests->DelElem(0); // remove first request            
        }
        ASSERT(this->mRequest->GetSize()>0);
        writer->SetSrcFile(this->mRequest);
        writer->Resume();
        this->Suspend();
    }

    else if(mStep == STEP_INIT_DATA_SOCKET)
    {
        if(!this->InitTcpDataClient())
        {
            this->Stop(ERROR_CONNECT);
        }
        this->mTcpDataClient->Connect();
        this->mStep = STEP_WAIT_DATA_CONNECT;
    }

    else if(mStep == STEP_WAIT_DATA_CONNECT)
    {
        if(this->mTcpDataClient->IsConnectComplete())
        {
            this->mConnectTime = 0;
            this->mStep = STEP_SEND_REQUEST;
        }
        else
        {
            this->mConnectTime += interval;
            if(this->mConnectTime >= this->mTimeout)
            {
                this->mConnectTime = 0;
                this->Stop(ERROR_CONNECT_TIMEOUT);
            }
        }
    }

    else if(mStep == STEP_READ_DATA)
    {
        CTaskSocketReader *reader = this->GetDataSocketReader(true);
        ASSERT(reader);
        ASSERT(this->iDstFile);
        reader->SetDstFile(this->iDstFile,this->mMaxDstFileSize);
        reader->Resume();
        this->Suspend();
    }

    else if(mStep == STEP_WRITE_DATA)
    {
        CTaskSocketWriter *writer = this->GetDataSocketWriter(true);
        ASSERT(writer);
        ASSERT(this->iSrcFile);
        fsize_t off = this->iSrcFile->GetOffset();
        writer->SetSrcFile(off,this->iSrcFile);
        writer->Resume();
        this->Suspend();
    }

    return OK;
}

status_t CTaskFtpClient::SetTimeout(int timeout)
{
    this->mTimeout=timeout;
    return OK;
}

const char * CTaskFtpClient::ErrorToString(int err)
{
    if(err == ERROR_SOCKET_CLOSED)
        return "socket closed";
    if(err == ERROR_PERMISSION_DENIED)
        return "permission denied";
    if(err == ERROR_CONNECT_TIMEOUT)
        return "connect timeout";
    if(err == ERROR_UNKNOWN_HOST)
        return "unknown host";
    if(err == ERROR_CONNECT)
        return "connect error";
    if(err == ERROR_RESPONSE)
        return "error response";
    if(err == ERROR_AUTH_FAIL)
        return "authorization error";
    return "unknown error";
}

status_t CTaskFtpClient::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskFtpClient ");
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

status_t CTaskFtpClient::on_socket_reader_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpClient *self = (CTaskFtpClient *)p[10];
    ASSERT(self);
    if(event == CTaskSocketReader::EVENT_QUIT)
    {
        self->Stop(ERROR_SOCKET_CLOSED);
    }
    return OK;
}

status_t CTaskFtpClient::on_socket_writer_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpClient *self = (CTaskFtpClient *)p[10];
    ASSERT(self);

    if(event == CTaskSocketWriter::EVENT_QUIT)
    {
        self->Stop(ERROR_SOCKET_CLOSED);
    }

    else if(event == CTaskSocketWriter::EVENT_FINISH)
    {
        self->Resume();
        self->mStep = STEP_READ_RESPONSE;
    }
    return OK;
}

status_t CTaskFtpClient::on_data_socket_reader_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpClient *self = (CTaskFtpClient *)p[10];
    ASSERT(self);

    if(event == CTaskSocketReader::EVENT_QUIT)
    {
        self->Resume();
        self->mStep = STEP_READ_RESPONSE;
    }
    else if(event == CTaskSocketReader::EVENT_FINISH)
    {
        self->Stop(ERROR_NONE); //if user specify the maxDstFileSize, this callback maybe called
    }
    return OK;
}

status_t CTaskFtpClient::on_data_socket_writer_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskFtpClient *self = (CTaskFtpClient *)p[10];
    ASSERT(self);

    if(event == CTaskSocketWriter::EVENT_FINISH)
    {
        self->mTcpDataClient->Destroy();
        self->Resume();
        self->mStep = STEP_READ_RESPONSE;
    }

    return OK;
}

CTaskSocketWriter * CTaskFtpClient::GetSocketWriter(bool renew)
{
    CTaskSocketWriter *pt = (CTaskSocketWriter *)GetTask(this->mTaskSocketWriter);
    if(pt == NULL && renew)
    {       
        NEW(pt,CTaskSocketWriter);
        pt->Init(this->GetTaskMgr());
        pt->SetSocket(this->mTcpClient);
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);     
        pt->SetCallbackFunc(on_socket_writer_event);
        pt->SetCallbackParam(10,this);
        pt->Start();
        this->mTaskSocketWriter = pt->GetID();
    }   
    return pt;
}

CTaskSocketReader * CTaskFtpClient::GetSocketReader(bool renew)
{
    CTaskSocketReader *pt = (CTaskSocketReader *)GetTask(this->mTaskSocketReader);
    if(pt == NULL && renew)
    {
        NEW(pt,CTaskSocketReader);
        pt->Init(this->GetTaskMgr());
        pt->SetSocket(this->mTcpClient);
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);     
        pt->SetCallbackFunc(on_socket_reader_event);
        pt->SetCallbackParam(10,this);
        pt->Start();
        this->mTaskSocketReader = pt->GetID();
    }
    return pt;
}

CTaskSocketWriter * CTaskFtpClient::GetDataSocketWriter(bool renew)
{
    CTaskSocketWriter *pt = (CTaskSocketWriter *)GetTask(this->mTaskDataSocketWriter);
    if(pt == NULL && renew)
    {       
        NEW(pt,CTaskSocketWriter);
        pt->Init(this->GetTaskMgr());
        pt->SetSocket(this->mTcpDataClient);
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);             
        pt->SetCallbackFunc(on_data_socket_writer_event);
        pt->SetCallbackParam(10,this);
        pt->Start();
        this->mTaskDataSocketWriter = pt->GetID();
    }   
    return pt;
}

CTaskSocketReader * CTaskFtpClient::GetDataSocketReader(bool renew)
{
    CTaskSocketReader *pt = (CTaskSocketReader *)GetTask(this->mTaskDataSocketReader);
    if(pt == NULL && renew)
    {
        NEW(pt,CTaskSocketReader);
        pt->Init(this->GetTaskMgr());
        pt->SetSocket(this->mTcpDataClient);
        pt->SetInterval(1);
        pt->SetTimeout(this->mTimeout);     
        pt->SetCallbackParam(10,this);
        pt->SetCallbackFunc(on_data_socket_reader_event);
        pt->Start();
        this->mTaskDataSocketReader = pt->GetID();
    }
    return pt;
}

status_t CTaskFtpClient::Start()
{
    this->mStep = STEP_RESOLVE_HOST;
    this->Resume();
    return OK;
}

status_t CTaskFtpClient::Stop(int err)
{
    if(this->IsDead())return ERROR;
    this->Quit();
    this->ReportError(err);
    this->SetCallbackParam(1,(void*)err);
    this->RunCallback(EVENT_QUIT);
    return OK;
}

status_t CTaskFtpClient::SetServer(const char *name, int port)
{
    ASSERT(name && port);
    this->mServerName->StrCpy(name);
    this->mPort = port;
    return OK;
}

const char * CTaskFtpClient::GetServerName()
{
    return this->mServerName->CStr();
}

status_t CTaskFtpClient::InitTcpClient()
{
    ASSERT(this->mHostToIpContext[0] == 1);
    ASSERT(this->mHostToIpContext[1] == 1);

    CMem mem;
    mem.Init();
    LOCAL_MEM(ip);

    mem.SetStr((char*)&mHostToIpContext[5]);
    mem.Seek(0);
    mem.ReadLine(&ip);  
    this->mTcpClient->Destroy();
    this->mTcpClient->Init();
    this->mTcpClient->SetServerIP(ip.CStr());
    this->mTcpClient->SetPort(this->mPort);
    this->mTcpClient->SetBlocking(0);
    this->mConnectTime = 0;
    return OK;
}

status_t CTaskFtpClient::AddRequest(CMem *req)
{
    ASSERT(req);
    this->mUserRequests->Push(req);
    this->Resume(); 
    if(this->mStep == STEP_IDLE)
    {
        this->mStep = STEP_SEND_REQUEST;
    }
    return OK;
}

status_t CTaskFtpClient::AddRequest(const char*szFormat,...)
{
    ASSERT(szFormat);
    char szBuffer [FILEBASE_LBUF_SIZE];
    crt_va_list pArgList ;
    crt_va_start(pArgList, szFormat) ;
    crt_vsprintf(szBuffer, szFormat, pArgList) ;
    crt_va_end (pArgList) ;
    
    return this->AddRequest(&CMem(szBuffer));
}

status_t CTaskFtpClient::ParseResponse(CFileBase *res,int *code, CFileBase *body)
{
    ASSERT(res && code && body);
    LOCAL_MEM(mem);
    res->Seek(0);
    res->ReadString(&mem);
    *code = atoi(mem.CStr());
    res->ReadLeftStr(body,true);
    return OK;
}

status_t CTaskFtpClient::OnResponse(CMem *response)
{
    ASSERT(response);

    int code;

    LOCAL_MEM(body);
    LOCAL_MEM(mem);

    LOG("(%d)>>> %s",this->GetID(),response->CStr());

    response->Seek(0);
    response->ReadLine(&mem);
    this->ParseResponse(&mem,&code,&body);
    ASSERT(code > 0);
    int req_code = this->ToRequestCode(this->mRequest);

    this->SetCallbackParam(1,(void*)req_code);
    this->SetCallbackParam(2,this->iDstFile);
    this->SetCallbackParam(3,(void*)code);
    this->SetCallbackParam(4,(void*)body.CStr());
    this->RunCallback(EVENT_ON_RESPONSE);

    this->mStep = STEP_SEND_REQUEST;
    if(code == 227)
    {       
        this->ParsePasvParams(&body,this->mDataIP,&this->mDataPort);
        this->mStep = STEP_INIT_DATA_SOCKET;
    }
    else if(code == 150)
    {
        if(req_code == FTP_REQ_RETR)
        {
            ASSERT(this->iDstFile);
            this->mStep = STEP_READ_DATA;
        }

        else if(req_code == FTP_REQ_LIST)
        {
            ASSERT(this->iDstFile);
            this->iDstFile->SetSize(0);
            this->mStep = STEP_READ_DATA;
        }

        else if(req_code == FTP_REQ_STOR)
        {
            ASSERT(this->iSrcFile);
            this->mStep = STEP_WRITE_DATA;
        }
    }
    else if(code == 221)
    {
        if(req_code == FTP_REQ_QUIT)
        {
            this->Stop(ERROR_NONE);
            return OK;
        }
    }

    return OK;
}

status_t CTaskFtpClient::ParsePasvParams(CMem *line, CMem *ip, int *port)
{
    ASSERT(line && ip && port);
    LOCAL_MEM(mem);

    line->Seek(0);
    fsize_t pos = line->SearchStr("(",0,0);
    ASSERT(pos >= 0);

    line->Seek(pos+1);
    line->ReadQuoteStr(0,')',&mem);
    CFtpUtils::ParsePortParams(&mem,ip,port);
    return OK;
}

status_t CTaskFtpClient::InitTcpDataClient()
{
    this->mTcpDataClient->Destroy();
    this->mTcpDataClient->Init();
    this->mTcpDataClient->SetServerIP(this->mDataIP->CStr());
    this->mTcpDataClient->SetPort(this->mDataPort);
    this->mTcpDataClient->SetBlocking(0);
    this->mConnectTime = 0;
    return OK;
}

status_t CTaskFtpClient::SetDstFile(CFileBase *iFile)
{
    this->iDstFile = iFile;
    return OK;
}

status_t CTaskFtpClient::SetSrcFile(CFileBase *iFile)
{
    this->iSrcFile = iFile;
    return OK;
}

int CTaskFtpClient::VerbToRequestCode(const char *verb)
{
    if(crt_stricmp(verb,"list") == 0)
        return FTP_REQ_LIST;
    if(crt_stricmp(verb,"stor") == 0)
        return FTP_REQ_STOR;
    if(crt_stricmp(verb,"retr") == 0)
        return FTP_REQ_RETR;
    if(crt_stricmp(verb,"pass") == 0)
        return FTP_REQ_PASS;
    if(crt_stricmp(verb,"user") == 0)
        return FTP_REQ_USER;
    if(crt_stricmp(verb,"size") == 0)
        return FTP_REQ_SIZE;
    if(crt_stricmp(verb,"quit") == 0)
        return FTP_REQ_QUIT;
    if(crt_stricmp(verb,"rest") == 0)
        return FTP_REQ_REST;
    if(crt_stricmp(verb,"pasv") == 0)
        return FTP_REQ_PASV;
    if(crt_stricmp(verb,"type") == 0)
        return FTP_REQ_TYPE;
    return FTP_REQ_ERROR;
}

int CTaskFtpClient::ToRequestCode(CMem *req)
{
    LOCAL_MEM(verb);
    req->Seek(0);
    req->ReadString(&verb);
    return this->VerbToRequestCode(verb.CStr());
}

bool CTaskFtpClient::IsEndResponseLine(CMem *line)
{
    ASSERT(line);
    const char *p = line->CStr();
    if(p[0] <'0'||p[0] >'9')return false;
    if(p[1] <'0'||p[1] >'9')return false;
    if(p[2] <'0'||p[2] >'9')return false;
    if(p[3] == '-')return false;
    return true;
}

status_t CTaskFtpClient::SyncTimeout()
{
    if(mTcpClient == NULL)return ERROR;
    if(mTcpDataClient == NULL)  return ERROR;
    if(!mTcpClient->IsConnected())return ERROR;
    if(!mTcpDataClient->IsConnected())return ERROR;
    
    int base = -1;
    CTaskSocketReader *data_reader = this->GetDataSocketReader(false);
    if(data_reader != NULL)
    {
        base = data_reader->mReadTime;      
    }
    else
    {
        CTaskSocketWriter *data_writer = this->GetDataSocketWriter(false);
        if(data_writer != NULL)
        {
            base = data_writer->mWriteTime;
        }
    }
    if(base >= 0)
    {
        CTaskSocketReader *reader = this->GetSocketReader(false);
        if(reader != NULL)
            reader->mReadTime = base;
        CTaskSocketWriter *writer = this->GetSocketWriter(false);
        if(writer != NULL)
            writer->mWriteTime = base;
    }
    return OK;
}

status_t CTaskFtpClient::on_sync_timeout(void **p)
{
    CTaskFtpClient *self = (CTaskFtpClient*)p[10];
    ASSERT(self);
    self->SyncTimeout();
    return OK;
}

status_t CTaskFtpClient::SetMaxDstFileSize(fsize_t max)
{
    this->mMaxDstFileSize = max;
    return OK;
}
