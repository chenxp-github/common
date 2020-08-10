// TaskHttpClient.cpp: implementation of the CTaskHttp class.
//
//////////////////////////////////////////////////////////////////////
#include "taskhttp.h"
#include "misc.h"
#include "httputils.h"

int CTaskHttp::EVENT_SERVER_HANDLE_REQUEST = 1;
int CTaskHttp::EVENT_QUIT = 2;
int CTaskHttp::EVENT_GET_HEADER = 3;

#define STEP_CLIENT_ERROR                   0
#define STEP_CLIENT_PREPARE_1               1   
#define STEP_CLIENT_PREPARE_2               2
#define STEP_CLIENT_SEND_HTTP_HEADER        3
#define STEP_CLIENT_SEND_HTTP_DATA          4
#define STEP_CLIENT_READ_HTTP_HEADER        5
#define STEP_CLIENT_READ_NORM_HTTP_DATA     6
#define STEP_CLIENT_PREPARE_3               7
#define STEP_CLIENT_PREPARE_4               8
#define STEP_CLIENT_READ_CHUNKED_SIZE       9
#define STEP_CLIENT_READ_CHUNKED_DATA       10

#define STEP_SERVER_ERROR                   0
#define STEP_SERVER_PREPARE_1               1   
#define STEP_SERVER_PREPARE_2               2
#define STEP_SERVER_SEND_HTTP_HEADER        3
#define STEP_SERVER_SEND_HTTP_DATA          4
#define STEP_SERVER_READ_HTTP_HEADER        5
#define STEP_SERVER_READ_NORM_HTTP_DATA     6
#define STEP_SERVER_PREPARE_3               7
#define STEP_SERVER_PREPARE_4               8
#define STEP_SERVER_HANLDE_REQUEST          9
#define STEP_SERVER_LAST                    10

#define TASK_TYPE_ERROR     0
#define TASK_TYPE_CLIENT    1
#define TASK_TYPE_SERVER    2

status_t CTaskHttp::ERROR_NONE = 0;
status_t CTaskHttp::ERROR_CONNECTION_CLOSED = 1;
status_t CTaskHttp::ERROR_CONTENT_LENGTH = 2;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTaskHttp::CTaskHttp()
{
    this->InitBasic();
}
CTaskHttp::~CTaskHttp()
{
    this->Destroy();
}
status_t CTaskHttp::InitBasic()
{
    CTask::InitBasic();

    this->cur_step = 0;
    this->i_file_data_to_receive = NULL;
    this->i_file_data_to_send = NULL;
    this->mf_header = NULL;
    this->tid_socket_reader = -1;
    this->tid_socket_writer = -1;
    this->i_socket = NULL;
    this->task_type = TASK_TYPE_ERROR;
    this->timeout = 60*1000;
    return OK;  
}
status_t CTaskHttp::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    
    NEW(this->mf_header,CMemFile);
    this->mf_header->Init();
    SuspendTask(this->id);
    return OK;
}
status_t CTaskHttp::Destroy()
{
    QuitTask(&this->tid_socket_reader);
    QuitTask(&this->tid_socket_writer);
    DEL(this->mf_header);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskHttp::OnTimer(int32_t interval)
{
    if(task_type == TASK_TYPE_CLIENT)
        return OnTimer_Client();
    if(task_type == TASK_TYPE_SERVER)
        return OnTimer_Server();    
    ASSERT(FALSE);
    return ERROR;
}
status_t CTaskHttp::OnTimer_Server()
{
    CTaskSocketWriter *writer;
    CTaskSocketReader *reader;

    ASSERT(this->i_socket);
    if(cur_step == STEP_SERVER_PREPARE_1)
    {
        reader = this->GetSocketReader(true);
        ASSERT(reader);     
        reader->SetDstFile(NULL,0); //read http header
        reader->Start();
        cur_step = STEP_SERVER_READ_HTTP_HEADER;
    }
    else if(cur_step == STEP_SERVER_READ_HTTP_HEADER)
    {
        reader = this->GetSocketReader(false);
        ASSERT(reader);
        if(reader->mQBuf->HasEmptyLine())
        {
            LOCAL_MEM(mem);
            this->mf_header->SetSize(0);
            while(reader->mQBuf->ReadLine(&mem))
            {
                if(mem.C(0) == 0)
                {
                    if(this->mf_header->GetSize() > 0)
                        break;
                    else
                        continue;
                }
                this->mf_header->Puts(&mem);
                this->mf_header->Puts("\r\n");
            }
            cur_step = STEP_SERVER_PREPARE_4;
        }
    }
    else if(cur_step == STEP_SERVER_PREPARE_4)
    {
        ASSERT(i_file_data_to_receive);
        this->i_file_data_to_receive->SetSize(0);
        reader = this->GetSocketReader(false);
        ASSERT(reader);
        fsize_t len = CHttpUtils::GetContentLength(this->mf_header);
        if(len > 0)
        {
            reader->SetDstFile(this->i_file_data_to_receive,len);
            reader->Start();
            cur_step = STEP_SERVER_READ_NORM_HTTP_DATA;
        }
        else
        {
            this->cur_step = STEP_SERVER_HANLDE_REQUEST;
        }
    }
    else if(cur_step == STEP_SERVER_READ_NORM_HTTP_DATA)
    {       
        this->Suspend();
    }
    else if(cur_step == STEP_SERVER_HANLDE_REQUEST)
    {
        this->cur_step = STEP_SERVER_PREPARE_3;
        this->RunCallback(EVENT_SERVER_HANDLE_REQUEST);
    }
    else if(cur_step == STEP_SERVER_PREPARE_3)
    {
        writer = this->GetSocketWriter(true);
        ASSERT(writer);
        writer->SetSrcFile(this->mf_header);
        writer->Start();
        cur_step = STEP_SERVER_SEND_HTTP_HEADER;
    } 
    else if(cur_step == STEP_SERVER_SEND_HTTP_HEADER)
    {
        this->Suspend();
    }
    else if(cur_step == STEP_SERVER_PREPARE_2)
    {
        if(this->i_file_data_to_send == NULL)
        {
            cur_step = STEP_SERVER_LAST;
        }
        else
        {
            if(this->i_file_data_to_send->GetSize() <= 0)
            {
                cur_step = STEP_SERVER_LAST;
            }
            else
            {
                writer = this->GetSocketWriter(false);
                ASSERT(writer);
                writer->SetSrcFile(this->i_file_data_to_send);
                writer->Start();
                cur_step = STEP_SERVER_SEND_HTTP_DATA;
            }
        }
    }
    else if(cur_step == STEP_SERVER_SEND_HTTP_DATA)
    {
        this->Suspend();
    }
    else if(cur_step == STEP_SERVER_LAST)
    {
        this->cur_step = STEP_SERVER_PREPARE_1; //make a loop
    }
    return OK;
}

status_t CTaskHttp::OnTimer_Client()
{
    CTaskSocketWriter *writer;
    CTaskSocketReader *reader;
    ASSERT(this->i_socket);

    if(cur_step == STEP_CLIENT_PREPARE_1)
    {       
        writer = this->GetSocketWriter(true);
        ASSERT(writer);
        writer->SetSrcFile(this->mf_header);
        writer->Start();
        cur_step = STEP_CLIENT_SEND_HTTP_HEADER;
    } 
    else if(cur_step == STEP_CLIENT_SEND_HTTP_HEADER)
    {
        this->Suspend(); //wait callback
    }
    else if(cur_step == STEP_CLIENT_PREPARE_2)
    {
        if(this->i_file_data_to_send == NULL)
        {
            cur_step = STEP_CLIENT_PREPARE_3;
        }
        else
        {
            if(this->i_file_data_to_send->GetSize() <= 0)
            {
                cur_step = STEP_CLIENT_PREPARE_3;
            }
            else
            {
                writer = this->GetSocketWriter(false);
                ASSERT(writer);
                writer->SetSrcFile(this->i_file_data_to_send);
                writer->Start();
                cur_step = STEP_CLIENT_SEND_HTTP_DATA;
            }
        }
    }
    else if(cur_step == STEP_CLIENT_SEND_HTTP_DATA)
    {
        this->Suspend(); //wait callback
    }
    else if(cur_step == STEP_CLIENT_PREPARE_3)
    {
        reader = this->GetSocketReader(true);
        ASSERT(reader);
        reader->SetDstFile(NULL,0);
        reader->mQBuf->Clear();
        reader->Start();
        cur_step = STEP_CLIENT_READ_HTTP_HEADER;
    }
    else if(cur_step == STEP_CLIENT_READ_HTTP_HEADER)
    {
        reader = this->GetSocketReader(false);
        ASSERT(reader);
        if(reader->mQBuf->HasEmptyLine())
        {
            LOCAL_MEM(mem);
            this->mf_header->SetSize(0);
            while(reader->mQBuf->ReadLine(&mem))
            {
                if(mem.C(0) == 0)
                {
                    if(this->mf_header->GetSize() > 0)
                        break;
                    else
                        continue;
                }
                this->mf_header->Puts(&mem);
                this->mf_header->Puts("\r\n");
            }
            this->SetCallbackParam(1,this->mf_header);
            this->SetCallbackParam(2,this);
            this->RunCallback(EVENT_GET_HEADER);
            cur_step = STEP_CLIENT_PREPARE_4;
        }
    }
    else if(cur_step == STEP_CLIENT_PREPARE_4)
    {
        ASSERT(i_file_data_to_receive);
        reader = this->GetSocketReader(false);
        ASSERT(reader);
        if(!CHttpUtils::IsChunked(this->mf_header))
        {
            fsize_t len = CHttpUtils::GetContentLength(this->mf_header);
            if(len <= 0) len = MAX_FSIZE;
            reader->SetDstFile(this->i_file_data_to_receive,len);
            reader->Start();
            cur_step = STEP_CLIENT_READ_NORM_HTTP_DATA;
        }
        else
        {
            reader->SetDstFile(NULL,0);
            reader->Start();
            cur_step = STEP_CLIENT_READ_CHUNKED_SIZE;
        }
    }
    else if(cur_step == STEP_CLIENT_READ_NORM_HTTP_DATA)
    {       
        this->Suspend();
    }
    else if(cur_step == STEP_CLIENT_READ_CHUNKED_SIZE)
    {
        reader = this->GetSocketReader(false);
        ASSERT(reader);
        if(reader->mQBuf->HasWholeLine())
        {
            LOCAL_MEM(mem);
            while(reader->mQBuf->ReadLine(&mem))
            {
                if(mem.C(0) != 0)
                    break;
            }
            mem.Trim();
            int32_t len = hex2dec_32(mem.CStr());
            cur_step = STEP_CLIENT_READ_CHUNKED_DATA;
            if(len <= 0)
            {
                this->Stop(ERROR_NONE);
            }           
            else
            {
                ASSERT(i_file_data_to_receive);
                reader->SetDstFile(this->i_file_data_to_receive,len);
            }
        }
    }
    else if(cur_step == STEP_CLIENT_READ_CHUNKED_DATA)
    {
        this->Suspend();
    }
    return OK;
}
CTaskSocketReader * CTaskHttp::GetSocketReader(status_t renew)
{
    ASSERT(this->i_socket);
    CTaskSocketReader *ptr;

    ptr = (CTaskSocketReader*)GetTask(this->tid_socket_reader);
    if(ptr == NULL && renew)
    {
        NEW(ptr,CTaskSocketReader);
        ptr->Init(this->GetTaskMgr());
        ptr->SetSocket(this->i_socket);
        ptr->SetInterval(1);
        ptr->SetTimeout(this->timeout);
        ptr->SetCallbackParam(10,this);
        ptr->SetCallbackFunc(this->on_socket_reader_event);
        ptr->Start();

        this->tid_socket_reader = ptr->GetID();
    }
    
    return ptr;
}
CTaskSocketWriter * CTaskHttp::GetSocketWriter(status_t renew)
{
    ASSERT(this->i_socket);
    CTaskSocketWriter *ptw;
    
    ptw = (CTaskSocketWriter*)GetTask(this->tid_socket_writer);
    if(ptw == NULL && renew)
    {   
        NEW(ptw,CTaskSocketWriter);
        ptw->Init(this->GetTaskMgr());
        ptw->SetSocket(this->i_socket);
        ptw->SetInterval(1);
        ptw->SetTimeout(this->timeout);
        ptw->SetCallbackParam(10,this);
        ptw->SetCallbackFunc(this->on_socket_writer_event);     
        ptw->Start();
        this->tid_socket_writer = ptw->GetID();
    }

    return ptw;
}
status_t CTaskHttp::PrepareHttpHeader(CFileBase *header)
{
    ASSERT(header);
    
    this->mf_header->SetSize(0);
    this->mf_header->WriteFile(header);
    this->mf_header->Puts("\r\n");
    return OK;
}
status_t CTaskHttp::PrepareDataFileToSend(CFileBase *file_to_send)
{
    this->i_file_data_to_send = file_to_send;
    return OK;
}
status_t CTaskHttp::PrepareFileToReceiveData(CFileBase *file_to_receive)
{
    this->i_file_data_to_receive = file_to_receive;
    return OK;
}
status_t CTaskHttp::StartTask()
{
    if(task_type == TASK_TYPE_CLIENT)
        this->cur_step = STEP_CLIENT_PREPARE_1;
    else if(task_type == TASK_TYPE_SERVER)
        this->cur_step = STEP_SERVER_PREPARE_1;
    this->Resume();
    return OK;
}
status_t CTaskHttp::SetSocket(CSocket *i_socket)
{
    ASSERT(i_socket);
    this->i_socket = i_socket;
    return OK;
}

status_t CTaskHttp::Stop(status_t err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);

    this->SetCallbackParam(1,(void*)err);
    this->SetCallbackParam(2,this);
    this->RunCallback(EVENT_QUIT);

    return OK;
}
status_t CTaskHttp::SetAsServer()
{
    this->task_type = TASK_TYPE_SERVER;
    return OK;
}
status_t CTaskHttp::SetAsClient()
{
    this->task_type = TASK_TYPE_CLIENT;
    return OK;
}
status_t CTaskHttp::MakeDefHttpRetHeader(int32_t ret_val, fsize_t len)
{
    char str_len[1024];
    crt_fsize_to_str(len,str_len);
    mf_header->SetSize(0);
    mf_header->Printf("HTTP/1.1 %d\r\n",ret_val);
    mf_header->Printf("Content-Length:%s\r\n",str_len);
    mf_header->Puts("\r\n");
    return OK;
}
status_t CTaskHttp::MakeDefHttpRetHeader()
{
    ASSERT(this->i_file_data_to_send);
    return this->MakeDefHttpRetHeader(200,this->i_file_data_to_send->GetSize());
}

const char * CTaskHttp::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_CONNECTION_CLOSED)
        return "connection closed";
    if(err == ERROR_CONTENT_LENGTH)
        return "content length invalidate";
    return "unknown error";
}

status_t CTaskHttp::ReportError(int32_t err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskHttp stopped with error:");
    mem.Puts(this->ErrorToString(err));

    if(err != ERROR_NONE)
    {
        LOG("%s",mem.CStr());
    }
    return OK;
}
status_t CTaskHttp::SetTimeout(int32_t _timeout)
{
    this->timeout=_timeout;
    return OK;
}

status_t CTaskHttp::on_socket_reader_event(void **p)
{
    int event = (int_ptr_t)p[0];
    CTaskHttp *self = (CTaskHttp*)p[10];
    ASSERT(self);
    
    if(event == CTaskSocketReader::EVENT_FINISH)
    {
        if(self->task_type == TASK_TYPE_CLIENT)
        {
            if(self->cur_step == STEP_CLIENT_READ_NORM_HTTP_DATA)
            {
                self->Stop(ERROR_NONE);
            }
            else if(self->cur_step == STEP_CLIENT_READ_CHUNKED_DATA)
            {
                self->Resume();
                self->cur_step = STEP_CLIENT_READ_CHUNKED_SIZE;
            }
        }
        
        else if(self->task_type == TASK_TYPE_SERVER)
        {
            if(self->cur_step == STEP_SERVER_READ_NORM_HTTP_DATA)
            {
                self->Resume();
                self->cur_step = STEP_SERVER_HANLDE_REQUEST;
            }
        }
    }
    else if(event == CTaskSocketReader::EVENT_QUIT)
    {
        int err = (int_ptr_t)p[1];
        if(err != CTaskSocketReader::ERROR_NONE)
            self->Stop(ERROR_CONNECTION_CLOSED);
        else
            self->Stop(ERROR_NONE);
        return OK;
    }

    return ERROR;
}

status_t CTaskHttp::on_socket_writer_event(void **p)
{
    int event = (int_ptr_t)p[0];
    CTaskHttp *self = (CTaskHttp*)p[10];
    ASSERT(self);
    
    if(event == CTaskSocketWriter::EVENT_FINISH)
    {
        if(self->task_type == TASK_TYPE_CLIENT)
        {
            if(self->cur_step == STEP_CLIENT_SEND_HTTP_HEADER)
            {
                self->Resume();
                self->cur_step = STEP_CLIENT_PREPARE_2;
            }
            
            else if(self->cur_step == STEP_CLIENT_SEND_HTTP_DATA)
            {
                self->Resume();
                self->cur_step = STEP_CLIENT_PREPARE_3;
            }
        }
        
        else if(self->task_type == TASK_TYPE_SERVER)
        {
            if(self->cur_step == STEP_SERVER_SEND_HTTP_HEADER)
            {
                self->Resume();
                self->cur_step = STEP_SERVER_PREPARE_2;
            }
            
            else if(self->cur_step == STEP_SERVER_SEND_HTTP_DATA)
            {
                self->Resume();
                self->cur_step = STEP_SERVER_LAST;
            }
        }
    }
    
    else if(event == CTaskSocketWriter::EVENT_QUIT)
    {
        int32_t err = (int_ptr_t)p[1];
        if(err != CTaskSocketWriter::ERROR_NONE)
            self->Stop(ERROR_CONNECTION_CLOSED);
        else
            self->Stop(ERROR_NONE);
    }
    
    return ERROR;
}
