#include "linkrpc.h"
#include "syslog.h"

CLinkRpc::CLinkRpc()
{
    this->InitBasic();
}
CLinkRpc::~CLinkRpc()
{
    this->Destroy();
}
status_t CLinkRpc::InitBasic()
{
    TASK_CONTAINER_INIT();
    this->mTaskReader = 0;
    this->mTaskWriter = 0;
    this->mSocket = NULL;
    this->mTimeout = 30*1000;
    this->mRequestCountL = 0;
    this->mResponseCountL = 0;
    this->mRequestCountR = 0;
    this->mResponseCountR = 0;
    this->mReader = NULL;
    this->mWriter = NULL;
    return OK;  
}
status_t CLinkRpc::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    
    NEW(this->mSocket,CSocket);
    this->mSocket->Init();

    this->SetTaskMgr(mgr);
    return OK;
}
status_t CLinkRpc::Destroy()
{
    QuitTask(&this->mTaskReader);
    QuitTask(&this->mTaskWriter);
    DEL(this->mSocket);
    this->InitBasic();
    return OK;
}

static int on_reader_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CLinkRpc *self = (CLinkRpc*)p[10];
    ASSERT(self && self->IsAlive());

    if(event == CTaskLinkRpcReader::EVENT_GOT_HEADER)
    {
        LINKRPC_HEADER *header = (LINKRPC_HEADER*)p[1];
        CFileBase *data = (CFileBase*)p[2];
        ASSERT(data);
        if(header->cmd == LINKRPC_CMD_REQUEST)
            self->OnGotRequestHeader(data);
        else if(header->cmd == LINKRPC_CMD_RESPONSE)
            self->OnGotResponseHeader(data);
    }
    else if(event == CTaskLinkRpcReader::EVENT_GOT_DATA)
    {
        LINKRPC_HEADER *header = (LINKRPC_HEADER*)p[1];
        CFileBase *header_data = (CFileBase*)p[2];
        CFileBase *data = (CFileBase*)p[3];
        ASSERT(data && header);
        if(header->cmd == LINKRPC_CMD_REQUEST)
        {
            ASSERT(self->mRequestCountL == self->mResponseCountL);
            self->mRequestCountL++;
            self->OnGotRequestData(header_data,data);
        }
        else if(header->cmd == LINKRPC_CMD_RESPONSE)
        {
            self->mResponseCountR++;
            ASSERT(self->mRequestCountR == self->mResponseCountR);
            self->OnGotResponseData(header_data,data);
        }
    }
    else if(event == CTaskLinkRpcReader::EVENT_STOP)
    {
        self->mReader = NULL;
        self->OnStop();
    }
    return OK;
}

status_t CLinkRpc::CreateReader()
{
    ASSERT(!IsTask(this->mTaskReader));
    
    CTaskLinkRpcReader *reader;
    NEW(reader,CTaskLinkRpcReader);
    reader->Init(this->GetTaskMgr());
    reader->SetInterval(10);
    reader->SetSocket(this->mSocket);
    reader->SetCallbackFunc(on_reader_event);
    reader->SetCallbackParam(10,this);
    reader->SetTimeout(this->mTimeout);
    reader->Start();

    this->mTaskReader = reader->GetID();
    this->mReader = reader;
    
    return OK;
}

static int on_writer_event(void **p)
{
    int_ptr_t event = (int_ptr_t)p[0];
    CLinkRpc *self = (CLinkRpc*)p[10];
    ASSERT(self && self->IsAlive());
    
    if(event == CTaskLinkRpcWriter::EVENT_GET_NEXT_PACKAGE)
    {
        int ret = self->OnGetNextPackage();
        if(ret)
        {
            ASSERT(self->mWriter);
            LINKRPC_HEADER *header = self->mWriter->Header();
            if(header->cmd == LINKRPC_CMD_REQUEST)
            {
                ASSERT(self->mRequestCountR == self->mResponseCountR);
                self->mRequestCountR++;
            }
            else if(header->cmd == LINKRPC_CMD_RESPONSE)
            {
                self->mResponseCountL++;
                ASSERT(self->mRequestCountL == self->mResponseCountL);
            }
        }
        return ret;
    }
    else if(event == CTaskLinkRpcWriter::EVENT_STOP)
    {
        self->mWriter = NULL;
        self->OnStop();
    }
    return OK;
}

status_t CLinkRpc::CreateWriter()
{
    ASSERT(!IsTask(this->mTaskWriter));

    CTaskLinkRpcWriter *writer;
    NEW(writer,CTaskLinkRpcWriter);
    writer->Init(this->GetTaskMgr());
    writer->SetInterval(1);
    writer->SetSocket(this->mSocket);
    writer->SetCallbackFunc(on_writer_event);
    writer->SetCallbackParam(10,this);
    writer->SetTimeout(this->mTimeout);
    writer->Start();
    this->mTaskWriter = writer->GetID();
    this->mWriter = writer;
    return OK;
}

status_t CLinkRpc::StartSending()
{   
    return this->ResumeTask(this->mTaskWriter);
}

status_t CLinkRpc::SetHeaderBuf(CFileBase *iBuf)
{
    ASSERT(this->mReader);
    return mReader->SetHeaderBuf(iBuf);
}

status_t CLinkRpc::SetDataBuf(CFileBase *iBuf)
{
    ASSERT(this->mReader);
    return mReader->SetDataBuf(iBuf);
}
status_t CLinkRpc::SetSocket(int32_t fd)
{
    this->mSocket->AttachSocketFd(fd);
    return OK;
}

status_t CLinkRpc::TransferSocket(CSocket *from)
{
    return this->mSocket->TransferSocketFd(from);
}

status_t CLinkRpc::Start()
{
    ASSERT(!this->IsAlive());
    ASSERT(this->mSocket->IsConnected());
    this->CreateReader();
    this->CreateWriter();
    return OK;
}
status_t CLinkRpc::SetTimeout(int to)
{
    this->mTimeout = to;
    return OK;
}

status_t CLinkRpc::Stop()
{
    this->mSocket->CloseConnect();
    QuitTask(&this->mTaskReader);
    QuitTask(&this->mTaskWriter);
    this->mReader = NULL;
    this->mWriter = NULL;
    return OK;
}

bool CLinkRpc::IsAlive()
{
    return (this->mReader!=NULL && this->mWriter!=NULL);
}

status_t CLinkRpc::SendRequest(CFileBase *header, CFileBase *data)
{
    ASSERT(this->mWriter);
    return this->mWriter->SendRequest(header,data);
}

///////////////////////////////////////////////////////////////////////////////
//override
///////////////////////////////////////////////////////////////////////////////
status_t CLinkRpc::OnStop()
{
    return OK;
}

status_t CLinkRpc::OnGotRequestHeader(CFileBase *header)
{
    return OK;
}

status_t CLinkRpc::OnGotResponseHeader(CFileBase *header)
{
    return OK;
}

status_t CLinkRpc::OnGotRequestData(CFileBase *header,CFileBase *data)
{
    return OK;
}

status_t CLinkRpc::OnGotResponseData(CFileBase *header,CFileBase *data)
{
    return OK;
}
status_t CLinkRpc::OnGetNextPackage()
{
    return OK;
}
