#include "taskhttpget.h"
#include "mem.h"
#include "uri.h"
#include "tasktcpconnector.h"
#include "taskhttpclient.h"
#include "encoder.h"
#include "mem_tool.h"
#include "syslog.h"

#define FLAG_RESUME_DOWNLOAD    0x00000001
#define FLAG_DUMP_HEADER        0x00000002

enum{
    STATE_RETRY = 1,
    STATE_CONNECT_TO_HOST,
    STATE_START_HTTP_GET,
};

#define TOGGLE_FLAG(tf,flag)  do{tf?(this->mFlags |= flag):(this->mFlags&=(~flag));}while(0)

CTaskHttpGet::CTaskHttpGet()
{
    this->InitBasic();
}
CTaskHttpGet::~CTaskHttpGet()
{
    this->Destroy();
}
status_t CTaskHttpGet::InitBasic()
{
    CTask::InitBasic();
    this->mSendHeader = NULL;
    this->mRecvHeader = NULL;

    this->mTaskHttpClient = 0;
    this->mTaskTcpConnector = 0;
    this->mState = 0;

    this->mDestFileName = NULL;
    this->mMaxRetries = 5;
    this->mRetry = 0;
    this->mRelocateTimes = 0;
    this->mTcpClient = NULL;

    this->mHttpHeaderFile = NULL;
    this->mDestFile = NULL;
    this->mTmpFileInMem = NULL;
    this->mProxtHost = NULL;
    this->mProxyPort = 0;
    this->mFlags = 0;
    this->mCallback = NULL;

    return OK;
}
status_t CTaskHttpGet::Init(CTaskMgr *mgr)
{
    this->Destroy();
    CTask::Init(mgr);
    
    NEW(mCallback,CClosure);
    mCallback->Init();

    NEW(this->mSendHeader,CHttpHeader);
    this->mSendHeader->Init();

    NEW(this->mRecvHeader,CHttpHeader);
    this->mRecvHeader->Init();

    NEW(this->mTcpClient,CTcpClient);
    this->mTcpClient->Init();
    
    NEW(this->mHttpHeaderFile,CMemFile);
    this->mHttpHeaderFile->Init();

    NEW(this->mDestFileName,CMem);
    this->mDestFileName->Init();

    NEW(this->mTmpFileInMem,CMemFile);
    this->mTmpFileInMem->Init();

    NEW(this->mDestFile,CFile);
    this->mDestFile->Init();

    NEW(this->mProxtHost,CMem);
    this->mProxtHost->Init();

    this->mSendHeader->SetMethod("GET");
    
    this->SetUserAgent("Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:28.0) Gecko/20100101 Firefox/28.0");
    this->mSendHeader->SetPair("Accept","*/*");
    this->mSendHeader->SetPair("Connection","close");
    return OK;
}
status_t CTaskHttpGet::Destroy()
{
    DEL(this->mProxtHost);
    DEL(this->mRecvHeader);
    DEL(this->mDestFile);
    DEL(this->mTmpFileInMem);
    DEL(this->mDestFileName);
    DEL(this->mHttpHeaderFile);
    DEL(this->mTcpClient);
    QuitTask(&this->mTaskHttpClient);
    QuitTask(&this->mTaskTcpConnector);
    DEL(this->mSendHeader);
    DEL(mCallback);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskHttpGet::Run(uint32_t interval)
{
    if(this->mState == STATE_RETRY)
    {
        this->mRetry++;
        if(this->mMaxRetries > 0 && this->mRetry > this->mMaxRetries)
        {
            this->Stop(ERROR_EXCEED_MAX_RETRIES);
            return ERROR;
        }
        this->mState = STATE_CONNECT_TO_HOST;
    }

    if(this->mState == STATE_CONNECT_TO_HOST)
    {
        this->CreateTaskTcpConnector();
        this->Suspend();
    }

    else if(this->mState == STATE_START_HTTP_GET)
    {
        this->CreateTaskHttpClient();
        this->Suspend();
    }
    return OK;

}

const char * CTaskHttpGet::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_EXCEED_MAX_RETRIES)
        return "exceed max retries";
    if(err == ERROR_HTTP_CLIENT_ERROR)
        return "http client error";
    if(err == ERROR_OPEN_FILE_ERROR)
        return "open file error";
    if(err == ERROR_ERROR_HOST_NAME)
        return "error host name";
    if(err == ERROR_CONNECT_ERROR)
        return "connect error";
    if(err == ERROR_TOO_MANY_RELOCATE)
        return "too many relocate";
    if(err == ERROR_ERROR_HTTP_RESPONSE)
        return "error http response";
    if(err == ERROR_REMOTE_FILE_NOT_FOUND)
        return "remote file not found";
    return "unknown error";
}
status_t CTaskHttpGet::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskHttpGet ");
    mem.Printf("(%d) ",this->GetId());

    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));     
    LOG("%s",mem.CStr());
    return OK;
}
status_t CTaskHttpGet::Start()
{
    QuitTask(&this->mTaskHttpClient);
    QuitTask(&this->mTaskTcpConnector);
    this->mTmpFileInMem->Destroy();
    this->mTmpFileInMem->Init();
    this->mState = STATE_RETRY;
    this->Resume();
    return OK;
}
status_t CTaskHttpGet::Stop(status_t err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);

    this->mCallback->SetParamInt(1,err);
    this->mCallback->Run(EVENT_STOP);

    return OK;
}
CHttpHeader* CTaskHttpGet::GetSendHeader()
{
    return this->mSendHeader;
}

status_t CTaskHttpGet::SetRequestUrl(const char *url)
{
    return this->mSendHeader->SetRequestUrl(url);
}

status_t CTaskHttpGet::GetHostAndPort(CMem *host, int *port)
{
    ASSERT(host && port);
    host->SetSize(0);

    if(this->UseProxy())
    {
        host->StrCpy(this->mProxtHost);
        *port = this->mProxyPort;
    }

    CUri uri;
    uri.Init();
    uri.ParseString(this->mSendHeader->GetRequestUrl());
    if(uri.HasHostName())
    {
        if(!this->UseProxy())host->StrCpy(uri.GetHostName());
        this->SetHost(uri.GetHostNameStr());
    }
    else
    {
        this->mSendHeader->GetPair("Host",host);
    }
    
    if(!this->UseProxy())
    {
        if(uri.GetPort() > 0)
            *port = uri.GetPort();
        else
            *port = 80;
    }
    return OK;
}
status_t CTaskHttpGet::Retry(int err)
{
    this->mRelocateTimes = 0;
    LOG("retry with error: %s",this->ErrorToString(err));
    this->Start();
    return OK;
}

static status_t on_connect_event(CClosure *closure)
{
    CLOSURE_PARAM_OBJ(CTaskHttpGet*,self,10);
    CLOSURE_PARAM_INT(event,0);

    if(event == CTaskTcpConnector::EVENT_STOP)
    {
        CLOSURE_PARAM_INT(err,1);
        if(err != CTaskTcpConnector::ERROR_NONE)
        {
            self->Retry(self->ERROR_CONNECT_ERROR);
            return ERROR;
        }
    }
    
    if(event == CTaskTcpConnector::EVENT_CONNECTED)
    {
        CLOSURE_PARAM_OBJ(CTcpClient*,client,2);
        ASSERT(self->mState == STATE_CONNECT_TO_HOST);
        self->mTcpClient->TransferSocketFd(client);
        self->Resume();
        self->mState = STATE_START_HTTP_GET;
    }

    return OK;
}

status_t CTaskHttpGet::CreateTaskTcpConnector()
{
    ASSERT(!IsTask(this->mTaskTcpConnector));
        
    LOCAL_MEM(host);
    int port;

    this->GetHostAndPort(&host,&port);
    if(host.StrLen() <= 0)
    {
        this->Stop(ERROR_ERROR_HOST_NAME);
        return ERROR;
    }

    CTaskTcpConnector *connector;
    NEW(connector,CTaskTcpConnector);
    connector->Init(this->GetTaskMgr());
    connector->SetServerName(&host);
    connector->SetPort(port);
    connector->Start();
    connector->Callback()->SetFunc(on_connect_event);
    connector->Callback()->SetParamPointer(10,this);
    this->mTaskTcpConnector = connector->GetId();

    return OK;
}

static status_t on_http_event(CClosure *closure)
{
    CLOSURE_PARAM_INT(event,0);
    CLOSURE_PARAM_OBJ(CTaskHttpGet*,self,10);
    CLOSURE_PARAM_OBJ(CTaskHttpClient*,http,11);

    ASSERT(self && http);

    if(event == CTaskHttpClient::EVENT_PREPARE_HEADER_TO_SEND)
    {
        if(self->mFlags & FLAG_RESUME_DOWNLOAD)
            self->PrepareRange();

        self->mSendHeader->Save(self->mHttpHeaderFile);
        http->PrepareFile(self->mHttpHeaderFile);

        if(self->mFlags & FLAG_DUMP_HEADER)
            self->mHttpHeaderFile->Dump();
        return TRUE;
    }
    else if(event == CTaskHttpClient::EVENT_PREPARE_DATA_TO_SEND)
    {       
        return TRUE;
    }

    else if(event == CTaskHttpClient::EVENT_PREPARE_FILE_TO_RECV_HEADER)
    {
        self->mHttpHeaderFile->Destroy();
        self->mHttpHeaderFile->Init();
        http->PrepareFile(self->mHttpHeaderFile);
        return TRUE;
    }

    else if(event == CTaskHttpClient::EVENT_GOT_HEADER)
    {
        self->mRecvHeader->Clear();
        self->mRecvHeader->Load(self->mHttpHeaderFile);

        if(self->mFlags & FLAG_DUMP_HEADER)
            self->mHttpHeaderFile->Dump();

        return TRUE;
    }

    else if(event == CTaskHttpClient::EVENT_PREPARE_FILE_TO_RECV_DATA)
    {
        if(self->mRecvHeader->GetRetCode() >= 300 && self->mRecvHeader->GetRetCode() < 400)
        {
            LOCAL_MEM(location);
            if(self->mRecvHeader->GetLocation(&location))
            {
                http->PrepareFile(self->mTmpFileInMem);
                return TRUE;
            }
            return FALSE;
        }

        else if(self->mRecvHeader->GetRetCode() == 200)
        {
            if(self->IsDownloadInMem())
            {
                http->PrepareFile(self->mTmpFileInMem);
                return TRUE;
            }
            else
            {
                if(!self->CreateDestFile())
                {
                    self->Stop(self->ERROR_OPEN_FILE_ERROR);
                    return FALSE;
                }
                http->PrepareFile(self->mDestFile);
                return TRUE;
            }
        }

        else if(self->mRecvHeader->GetRetCode() == 206)
        {
            fsize_t start = 0,end = 0;
            if(self->mRecvHeader->GetContentRange(&start,&end))
            {
                if(start >= end)
                {
                    self->Done();
                    return TRUE;
                }

                if(!self->IsDownloadInMem())
                {
                    if(!self->OpenOrCreateDestFile())
                    {
                        self->Stop(self->ERROR_OPEN_FILE_ERROR);
                        return FALSE;
                    }
                    self->mDestFile->Seek(start);
                    http->PrepareFile(self->mDestFile);
                    return TRUE;
                }
                else
                {
                    self->mTmpFileInMem->Seek(start);
                    http->PrepareFile(self->mTmpFileInMem);
                    return TRUE;
                }
            }
            return FALSE;
        }
        else if(self->mRecvHeader->GetRetCode() == 416)
        {
            self->Done();
        }
        else if(self->mRecvHeader->GetRetCode() == 404)
        {
            self->Stop(self->ERROR_REMOTE_FILE_NOT_FOUND);
        }
        else
        {
            self->Stop(self->ERROR_ERROR_HTTP_RESPONSE);
        }
        return FALSE;
    }
    
    else if(event == CTaskHttpClient::EVENT_DONE)
    {       
        if(self->mRecvHeader->GetRetCode() >= 300 && self->mRecvHeader->GetRetCode() < 400)
        {
            LOCAL_MEM(location);
            if(self->mRecvHeader->GetLocation(&location))
            {
                self->mRelocateTimes++;
                if(self->mRelocateTimes > 5)
                {
                    self->Stop(self->ERROR_TOO_MANY_RELOCATE);
                    return ERROR;
                }
                LOG_INFO("relocate to %s",location.CStr());
                self->mSendHeader->SetPair("Referer",self->mSendHeader->GetRequestUrlStr());
                self->mRetry = 0;
                self->SetRequestUrl(location.CStr());
                self->Start();
            }
            else
            {
                self->mHttpHeaderFile->Dump();
                self->Stop(self->ERROR_ERROR_HTTP_RESPONSE);
            }

            return OK;
        }

        if(self->mRecvHeader->GetRetCode() == 200 || self->mRecvHeader->GetRetCode() == 206)
        {
            self->Done();
        }
    }

    else if(event == CTaskHttpClient::EVENT_STOP)
    {
        CLOSURE_PARAM_INT(err,1);
        if(err != CTaskHttpClient::ERROR_NONE)
            self->Retry(self->ERROR_HTTP_CLIENT_ERROR);
    }
    return ERROR;
}

status_t CTaskHttpGet::CreateTaskHttpClient()
{
    ASSERT(!IsTask(this->mTaskHttpClient));
    CTaskHttpClient *pt;
    NEW(pt,CTaskHttpClient);
    pt->Init(this->GetTaskMgr());
    pt->SetSocket(this->mTcpClient);
    pt->Callback()->SetFunc(on_http_event);
    pt->Callback()->SetParamPointer(10,this);
    pt->Callback()->SetParamPointer(11,pt);
    pt->Start();
    this->mTaskHttpClient = pt->GetId();
    return OK;
}

status_t CTaskHttpGet::SetHost(const char *host)
{
    return this->mSendHeader->SetPair("Host",host);
}

status_t CTaskHttpGet::SetUserAgent(const char *user_agent)
{
    return this->mSendHeader->SetPair("User-Agent",user_agent);
}

status_t CTaskHttpGet::SetDestFileName(const char *destfilename)
{
    ASSERT(destfilename);
    CMem mem_destfilename(destfilename);
    return this->mDestFileName->Copy(&mem_destfilename);
}

bool CTaskHttpGet::IsDownloadInMem()
{
    return this->mDestFileName->StrLen() <= 0;
}

status_t CTaskHttpGet::OpenDestFile()
{
    if(this->mDestFile->IsOpened())
        return OK;
    this->mDestFile->Destroy();
    this->mDestFile->Init();
    if(this->mDestFile->OpenFile(this->mDestFileName->CStr(),"r+"))
    {
        return OK;
    }
    return ERROR;
}

status_t CTaskHttpGet::CreateDestFile()
{
    this->mDestFile->Destroy();
    this->mDestFile->Init();
    if(this->mDestFile->OpenFile(this->mDestFileName->CStr(),"wb+"))
    {
        return OK;
    }
    return ERROR;
}

status_t CTaskHttpGet::PrepareRange()
{
    this->mSendHeader->DelPair("Range");
    if(this->IsDownloadInMem())
        return OK;

    this->OpenDestFile();

    LOCAL_MEM(value);
    fsize_t start = this->mDestFile->GetSize();
    fsize_t end = -1;
    if(start <= 0)return ERROR;

    char str[128];
    crt_fsize_to_str(start,str);
    value.Printf("bytes=%s-",str);
    if(end > start)
    {
        crt_fsize_to_str(end,str);
        value.Printf("%s",str);
    }
    this->mSendHeader->SetPair("Range",value.CStr());
    return OK;
}

status_t CTaskHttpGet::Done()
{
    mCallback->SetParamPointer(1,this->mDestFile);
    mCallback->SetParamPointer(2,this->mTmpFileInMem);
    mCallback->SetParamInt(3,this->IsDownloadInMem());
    this->mCallback->Run(EVENT_DONE);

    this->mDestFile->Destroy();
    this->Stop(ERROR_NONE);
    return OK;
}

status_t CTaskHttpGet::SetProxy(const char *host, int port)
{
    ASSERT(host);
    CMem mem_host(host);
    this->mProxtHost->Copy(&mem_host);
    this->mProxyPort = port;
    return OK;
}

status_t CTaskHttpGet::SetProxyAuth(const char *auth)
{
    ASSERT(auth);

    LOCAL_MEM(mem);
    LOCAL_MEM(buf);

    mem.StrCpy(auth);
    CEncoder::Base64_Encode(&mem,&buf);

    mem.SetSize(0);
    mem.Printf("Basic %s",buf.CStr());
    this->mSendHeader->SetPair("Proxy-Authorization",mem.CStr());
    return OK;
}

bool CTaskHttpGet::UseProxy()
{
    return this->mProxtHost->StrLen() > 0;
}

status_t CTaskHttpGet::SetResumeDownload(bool r)
{
    TOGGLE_FLAG(r,FLAG_RESUME_DOWNLOAD);
    return OK;
}

status_t CTaskHttpGet::SetDumpHeader(bool tf)
{
    TOGGLE_FLAG(tf,FLAG_DUMP_HEADER);
    return OK;
}

status_t CTaskHttpGet::OpenOrCreateDestFile()
{
    if(this->OpenDestFile())
        return OK;
    if(this->CreateDestFile())
        return OK;
    return ERROR;
}

status_t CTaskHttpGet::SetMaxRetries(int max)
{
    this->mMaxRetries = max;
    return OK;
}
