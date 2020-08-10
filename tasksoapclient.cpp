#include "tasksoapclient.h"
#include "mem.h"
#include "httputils.h"
#include "encoder.h"
#include "xml.h"

#define STEP_RESOLVE_HOST       1
#define STEP_WAIT_RESOLVE_HOST  2
#define STEP_WAIT_CONNECT       3
#define STEP_START_TASK_HTTP    4
#define STEP_WAIT_TASK_HTTP     5
#define STEP_RETRY              6

status_t CTaskSoapClient::ERROR_NONE = 0;
status_t CTaskSoapClient::ERROR_CONNECT_ERROR = 1;
status_t CTaskSoapClient::ERROR_CONNECT_TIMEOUT = 2;
status_t CTaskSoapClient::ERROR_CONNECT_CLOSED = 3;
status_t CTaskSoapClient::ERROR_RESOLVE_HOST_ERROR = 4;
status_t CTaskSoapClient::ERROR_PREPARE_TCP_CLIENT_ERROR = 5;
status_t CTaskSoapClient::ERROR_HTTP_ERROR = 6;
status_t CTaskSoapClient::ERROR_EXCEED_MAX_RETRIES = 7;

int CTaskSoapClient::EVENT_QUIT = 1;

CTaskSoapClient::CTaskSoapClient()
{
    this->InitBasic();
}
CTaskSoapClient::~CTaskSoapClient()
{
    this->Destroy();
}
status_t CTaskSoapClient::InitBasic()
{
    CTask::InitBasic();

    this->mTaskHttp = 0;
    this->mTcpClient = NULL;
    this->mUrlToPost = NULL;
    this->mStep = 0;
    this->hostoip_context = NULL;
    this->mConnectTime = 0;
    this->mTimeout = 15*1000;
    this->mDataToSend = NULL;
    this->mReceivedData = NULL;
    this->mRetryTime = 0;
    this->MaxRetryTime = 5;
    return OK;
}
status_t CTaskSoapClient::SetRetryTime(int retryTime)
{
    this->MaxRetryTime = retryTime;
    return OK;
}
status_t CTaskSoapClient::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    
    NEW(this->mTcpClient,CTcpClient);
    this->mTcpClient->Init();

    NEW(this->mUrlToPost,CMem);
    this->mUrlToPost->Init();
    this->mUrlToPost->Malloc(1024);

    NEW(this->mDataToSend,CMemFile);
    this->mDataToSend->Init();

    NEW(this->mReceivedData,CMemFile);
    this->mReceivedData->Init();

    return OK;
}
status_t CTaskSoapClient::Destroy()
{
    crt_free_host_to_ip_context(&this->hostoip_context);
    QuitTask(&this->mTaskHttp);
    DEL(this->mTcpClient);
    DEL(this->mUrlToPost);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskSoapClient::Copy(CTaskSoapClient *p)
{
    if(this == p)
        return OK;
    CTask::Copy(p);
    //add your code
    return OK;
}
status_t CTaskSoapClient::Comp(CTaskSoapClient *p)
{
    return 0;
}
status_t CTaskSoapClient::Print()
{
    CTask::Print();
    //add your code
    return TRUE;
}
status_t CTaskSoapClient::OnTimer(int interval)
{
    if(this->mStep == STEP_RETRY)
    {
        if(this->mRetryTime > this->MaxRetryTime) 
        {
            this->Stop(ERROR_EXCEED_MAX_RETRIES);
            return ERROR;
        }
        this->mRetryTime ++;
        this->mStep = STEP_RESOLVE_HOST;
    }
    
    else if(this->mStep == STEP_RESOLVE_HOST)
    {
        int port;
        CMem host,path;
        LOCAL_MEM(host);
        LOCAL_MEM(path);

        CHttpUtils::ParseUrl(this->mUrlToPost,&host,&port,&path);
        crt_host_to_ip_async(host.CStr(),&this->hostoip_context);
        this->mStep = STEP_WAIT_RESOLVE_HOST;
    }
    else if(this->mStep == STEP_WAIT_RESOLVE_HOST)
    {
        if(this->hostoip_context[0] == 0)
            return ERROR;
        if(this->hostoip_context[1] == 0)
        {
            this->Retry(ERROR_RESOLVE_HOST_ERROR);
            return ERROR;
        }
        
        if(!this->PrepareTcpClient())
        {
            this->Retry(ERROR_PREPARE_TCP_CLIENT_ERROR);
            return ERROR;
        }
        if(this->mTcpClient->Connect() < 0)
        {
            this->Retry(ERROR_CONNECT_ERROR);
            return ERROR;
        }
        this->mStep = STEP_WAIT_CONNECT;
    }
    
    else if(this->mStep == STEP_WAIT_CONNECT)
    {
        if(this->mTcpClient->IsConnectComplete())
        {
            this->mConnectTime = 0;
            this->mStep = STEP_START_TASK_HTTP;     
        }
        else
        {
            this->mConnectTime+=interval;
            if(this->mConnectTime>= this->mTimeout)
            {
                this->mConnectTime = 0;
                this->Retry(ERROR_CONNECT_TIMEOUT);
            }
        }
    }
    else if(this->mStep == STEP_START_TASK_HTTP)
    {
        CTaskHttp *pt = this->GetTaskHttp(true);
        ASSERT(pt);
        CMem header;
        LOCAL_MEM(header);
        this->MakeHttpPostHeader(&header);
        pt->PrepareHttpHeader(&header);
        pt->PrepareDataFileToSend(this->mDataToSend);
        this->mReceivedData->SetSize(0);
        pt->PrepareFileToReceiveData(this->mReceivedData);
        this->mStep = STEP_WAIT_TASK_HTTP;
    }
    else if(this->mStep == STEP_WAIT_TASK_HTTP)
    {
        this->Suspend(); //wait callback
    }
    return OK;
}


const char * CTaskSoapClient::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_CONNECT_ERROR)
        return "connect error";
    if(err == ERROR_CONNECT_TIMEOUT)
        return "connect timeout";
    if(err == ERROR_CONNECT_CLOSED)
        return "connect closed";
    if(err == ERROR_RESOLVE_HOST_ERROR)
        return "resolve host error";
    if(err == ERROR_PREPARE_TCP_CLIENT_ERROR)
        return "prepare tcp client error";
    if(err == ERROR_HTTP_ERROR)
        return "http error";    
    if(err == ERROR_EXCEED_MAX_RETRIES)
        return "exceed max retries";
    return "unknown error";
}
status_t CTaskSoapClient::ReportError(int err)
{
    CMem mem;
    LOCAL_MEM(mem);

    mem.Puts("CTaskSoapClient ");
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
        LOG("%s\n",mem.CStr());
    }
    return OK;
}
status_t CTaskSoapClient::Start()
{
    return this->Retry(ERROR_NONE);
}
status_t CTaskSoapClient::Retry(int err)
{
    if(err != ERROR_NONE)
    {
        LOG("CTaskSoapClient(%d):retry with error %s",this->GetID(),this->ErrorToString(err));
    }
    QuitTask(&this->mTaskHttp);
    this->mStep = STEP_RETRY;
    this->Resume();
    return OK;
}
status_t CTaskSoapClient::Stop(status_t err)
{
    if(this->IsDead())return ERROR;
    this->Quit();
    this->ReportError(err);
    
    this->SetCallbackParam(1,(void*)err);
    this->SetCallbackParam(2,NULL);
    
    CMemFile mf;
    mf.Init();
    CMem status;
    LOCAL_MEM(status);
    
    if(err == ERROR_NONE)
    {
        if(this->SplitXmlOut(&mf))
        {
            this->SetCallbackParam(2,&mf);
            CEncoder::UnEscapeXmlStr(&mf);
            CXml xml;
            xml.Init();
            ASSERT(xml.LoadXml(&mf));
            CXmlNode *px = xml.GetNodeByPath("/info/status");
            ASSERT(px);
            px->GetStringValue(&status);
        }
        
        this->SetCallbackParam(3,(void*)status.CStr());
        this->SetCallbackParam(4,(void*)this->mReceivedData);
        
    }

    this->RunCallback(EVENT_QUIT);
    return OK;
}

status_t CTaskSoapClient::SetUrlToPost(const char *url)
{
    ASSERT(url);
    this->mUrlToPost->StrCpy(url);
    return OK;
}

status_t CTaskSoapClient::PrepareTcpClient()
{
    CMem host,path;
    int port;
    
    LOCAL_MEM(host);
    LOCAL_MEM(path);
    this->mTcpClient->Destroy();
    this->mTcpClient->Init();

    ASSERT(this->hostoip_context[0]==1);
    ASSERT(this->hostoip_context[1]==1);

    CMem mem,ip;
    mem.Init();
    mem.SetStr((const char*)&hostoip_context[5]);
    LOCAL_MEM(ip);

    mem.Seek(0);
    mem.ReadLine(&ip);
        
    CHttpUtils::ParseUrl(this->mUrlToPost,&host,&port,&path);
    ASSERT(host.StrLen() > 0);
    this->mTcpClient->SetServerIP(ip.CStr());
    this->mTcpClient->SetPort(port);
    
    if(!this->mTcpClient->SetBlocking(0))
    {
        return ERROR;
    }       
    return OK;
}
status_t CTaskSoapClient::SetTimeout(int timeout)
{
    this->mTimeout=timeout;
    return OK;
}

CTaskHttp * CTaskSoapClient::GetTaskHttp(bool renew)
{
    CTaskHttp *pt = (CTaskHttp*)GetTask(this->mTaskHttp);
    if(pt == NULL && renew)
    {       
        NEW(pt,CTaskHttp);
        pt->Init(this->GetTaskMgr());
        pt->SetInterval(100);
        pt->SetTimeout(this->mTimeout);
        pt->SetAsClient();
        pt->SetSocket(this->mTcpClient);
        pt->SetCallbackFunc(this->on_http_quit);
        pt->SetCallbackParam(10,this);
        pt->StartTask();
        this->mTaskHttp = pt->GetID();
    }

    return pt;
}

status_t CTaskSoapClient::on_http_quit(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CTaskSoapClient *self = (CTaskSoapClient*)p[10];
    ASSERT(self);

    if(event == CTaskHttp::EVENT_QUIT)
    {
        int_ptr_t err = (int_ptr_t)p[1];
        if(err != CTaskHttp::ERROR_NONE)
        {
            self->Retry(ERROR_HTTP_ERROR);
            return ERROR;
        }
        else
        {
            self->Stop(ERROR_NONE);
        }
    }
    return OK;
}

status_t CTaskSoapClient::MakeHttpPostHeader(CFileBase *header)
{
    ASSERT(header);

    CMem host,path;
    int port;
    
    LOCAL_MEM(host);
    LOCAL_MEM(path);

    CHttpUtils::ParseUrl(this->mUrlToPost,&host,&port,&path);
    ASSERT(host.StrLen() > 0);

    header->SetSize(0);
    header->Printf("POST %s HTTP/1.1\r\n",path.CStr());
    header->Printf("Host: %s\r\n",host.CStr());
    header->Printf("User-Agent: SoapTest 1.0 (iPad Simulator; iPhone OS 4.3.2; en_US)\r\n");
    header->Printf("Content-Type: text/xml; charset=utf-8\r\n");    
    header->Printf("Content-Length: %d\r\n",this->mDataToSend->GetSize());
    header->Printf("Connection: close\r\n");
    return OK;
}

status_t CTaskSoapClient::SetSoapBody(CFileBase *body)
{
    ASSERT(body);
    this->mDataToSend->SetSize(0);
    this->mDataToSend->Puts("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    this->mDataToSend->Puts("<soap:Envelope  xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    this->mDataToSend->Puts(" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
    this->mDataToSend->Puts(" xmlns:xsc=\"http://schemas.xmlsoap.org/soap/encoding/\"");
    this->mDataToSend->Puts(" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n");
    this->mDataToSend->Puts("<soap:Header>\r\n");
    this->mDataToSend->Puts("</soap:Header>\r\n");
    this->mDataToSend->Puts("<soap:Body>\r\n"); 
    this->mDataToSend->Puts(body);
    this->mDataToSend->Puts("</soap:Body>\r\n");
    this->mDataToSend->Puts("</soap:Envelope>\r\n");
    
    mDataToSend->Seek(0);
    return OK;
}

status_t CTaskSoapClient::SplitXmlOut(CFileBase *xmlOut)
{
    ASSERT(xmlOut);

    this->mReceivedData->Seek(0);

    fsize_t pos1 = this->mReceivedData->SearchStr("&lt;?xml",0,0);
    ASSERT(pos1 > 0);

    fsize_t pos2 = this->mReceivedData->SearchStr("</ns1:out>",0,0);
    ASSERT(pos2 > 0);

    xmlOut->SetSize(0);
    xmlOut->WriteFile(this->mReceivedData,pos1,pos2-pos1);

    return OK;
}
