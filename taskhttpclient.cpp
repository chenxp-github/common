#include "taskhttpclient.h"
#include "mem.h"
#include "misc.h"
#include "httpheader.h"
#include "mem_tool.h"
#include "syslog.h"

enum{
    STEP_PREPARE_HEADER_TO_SEND = 1,
    STEP_SEND_HEADER,
    STEP_PREPARE_DATA_TO_SEND,
    STEP_SEND_DATA,
    STEP_PREPARE_READ_HEADER,
    STEP_READ_HEADER,
    STEP_PREPARE_READ_DATA,
    STEP_READ_DATA,
    STEP_READ_CHUNK_SIZE,
    STEP_READ_CHUNK_DATA,
    STEP_READ_CHUNK_TAIL,
    STEP_READ_CHUNK_TAIL_AND_DONE,
};

static status_t on_socket_rw_event(CClosure *closure)
{
    CLOSURE_PARAM_OBJ(CTaskHttpClient*,self,10);
    CLOSURE_PARAM_INT(event,0);

    if(event == CSocketReaderWriter::EVENT_ERROR)
    {
        CLOSURE_PARAM_INT(err,1);
        CLOSURE_PARAM_OBJ(const char*,err_str,2);

        if(err == CSocketReaderWriter::ERROR_CONNECTION_CLOSED)
        {
            if(self->mNoContentLength)
            {
                self->mCallback->Run(self->EVENT_DONE);
                self->Stop(self->ERROR_NONE);
                return OK;
            }
        }
        LOG("CSocketReaderWriter got error: %s",err_str);
        self->Stop(self->ERROR_SOCKET_RW_ERROR);
    }
    return OK;
}

////////////////////////////////////////////////////
CTaskHttpClient::CTaskHttpClient()
{
    this->InitBasic();
}
CTaskHttpClient::~CTaskHttpClient()
{
    this->Destroy();
}
status_t CTaskHttpClient::InitBasic()
{
    CTask::InitBasic();
    this->mStep = 0;
    this->mSocketRw = 0;
    this->iCurFile = NULL;
    this->mTmpLine = NULL;
    this->mNoContentLength = false;
    this->mCallback = NULL;
    return OK;
}
status_t CTaskHttpClient::Init(CTaskMgr *mgr)
{
    this->Destroy();
    CTask::Init(mgr);
    
    NEW(this->mCallback,CClosure);
    this->mCallback->Init();

    NEW(this->mSocketRw,CSocketReaderWriter);
    this->mSocketRw->Init();
    this->mSocketRw->Callback()->SetFunc(on_socket_rw_event);
    this->mSocketRw->Callback()->SetParamPointer(10,this);

    NEW(this->mTmpLine,CMem);
    this->mTmpLine->Init();
    this->mTmpLine->Malloc(64);

    return OK;
}
status_t CTaskHttpClient::Destroy()
{
    DEL(this->mTmpLine);
    DEL(this->mSocketRw);
    DEL(this->mCallback);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

status_t CTaskHttpClient::Run(uint32_t interval)
{
    if(this->mStep == STEP_PREPARE_HEADER_TO_SEND)
    {       
        if(this->mCallback->Run(EVENT_PREPARE_HEADER_TO_SEND))
        {
            this->mStep = STEP_SEND_HEADER;
            this->mSocketRw->PrepareFileForWrite(this->iCurFile,0,this->iCurFile->GetSize());
        }
        else
        {
            this->Sleep(10);
        }
    }
    else if(this->mStep == STEP_SEND_HEADER)
    {
        if(this->mSocketRw->DoWrite(interval))
        {
            this->mCallback->Run(EVENT_HEADER_SEND_OK);
            this->mStep = STEP_PREPARE_DATA_TO_SEND;
        }
        else
        {
            this->Sleep(1);
        }
    }
    else if(this->mStep == STEP_PREPARE_DATA_TO_SEND)
    {       
        if(this->mCallback->Run(EVENT_PREPARE_DATA_TO_SEND))
        {
            if(this->iCurFile)
                this->mSocketRw->PrepareFileForWrite(this->iCurFile,0,this->iCurFile->GetSize());
            else
                this->mSocketRw->PrepareFileForWrite(NULL,0,0);
            this->mStep = STEP_SEND_DATA;
        }
        else
        {
            this->Sleep(10);
        }
    }
    else if(this->mStep == STEP_SEND_DATA)
    {
        if(this->mSocketRw->DoWrite(interval))
        {
            this->mCallback->Run(EVENT_DATA_SEND_OK);
            this->mStep = STEP_PREPARE_READ_HEADER;
        }
        else
        {
            this->Sleep(1);
        }
    }
    else if(this->mStep == STEP_PREPARE_READ_HEADER)
    {               
        if(this->mCallback->Run(EVENT_PREPARE_FILE_TO_RECV_HEADER))
        {           
            this->mNoContentLength = false;
            this->mSocketRw->PrepareFileForRead(this->iCurFile,-1,true);
            this->mStep = STEP_READ_HEADER;
        }
        else
        {
            this->Sleep(10);
        }
    }
    else if(this->mStep == STEP_READ_HEADER)
    {
        if(this->mSocketRw->DoReadUntilEmptyLine(interval))
        {
            mCallback->SetParamPointer(1,this->iCurFile);
            this->mCallback->Run(EVENT_GOT_HEADER);
            this->mStep = STEP_PREPARE_READ_DATA;
        }
        else
        {
            this->Sleep(1);
        }
    }
    else if(this->mStep == STEP_PREPARE_READ_DATA)
    {
        fsize_t len = 0;
        CHttpHeader header;

        header.Init();
        header.Load(this->iCurFile);

        bool is_chunk = false;
        is_chunk = header.IsChunked();

        mCallback->SetParamInt(1,is_chunk);

        if(!is_chunk)
        {
            len = header.GetContentLength();    
            if(len < 0)this->mNoContentLength = true;
            mCallback->SetParamPointer(2,&len);
        }

        if(this->mCallback->Run(EVENT_PREPARE_FILE_TO_RECV_DATA))
        {
            if(!is_chunk)
            {
                this->mSocketRw->PrepareFileForRead(this->iCurFile,len,false);
                this->mStep = STEP_READ_DATA;
            }
            else
            {
                this->mSocketRw->PrepareFileForRead(this->mTmpLine,-1,true);
                this->mStep = STEP_READ_CHUNK_SIZE;
            }
        }
        else
        {
            this->Sleep(10);
        }
    }

    else if(this->mStep == STEP_READ_DATA)
    {
        if(this->mSocketRw->DoRead(interval))
        {
            this->mCallback->Run(EVENT_DONE);
            this->Stop(ERROR_NONE);
        }
        else
        {
            this->Sleep(1);
        }
    }

    else if(this->mStep == STEP_READ_CHUNK_SIZE)
    {
        if(this->mSocketRw->DoReadUntilEol(interval))
        {
            this->mTmpLine->Trim();
            fsize_t size = (fsize_t)hex2dec_64(this->mTmpLine->CStr());
            if(size <= 0)
            {
                this->mSocketRw->PrepareFileForRead(this->mTmpLine,2,true);
                this->mStep = STEP_READ_CHUNK_TAIL_AND_DONE;
            }
            else
            {
                this->mSocketRw->PrepareFileForRead(this->iCurFile,size,false);
                this->mStep = STEP_READ_CHUNK_DATA;
            }           
        }
        else
        {
            this->Sleep(1);
        }
    }

    else if(this->mStep == STEP_READ_CHUNK_DATA)
    {
        if(this->mSocketRw->DoRead(interval))
        {
            this->mSocketRw->PrepareFileForRead(this->mTmpLine,2,true);
            this->mStep = STEP_READ_CHUNK_TAIL;
        }
        else
        {
            this->Sleep(1);
        }
    }

    else if(this->mStep == STEP_READ_CHUNK_TAIL)
    {
        if(this->mSocketRw->DoRead(interval))
        {
            this->mSocketRw->PrepareFileForRead(this->mTmpLine,-1,true);
            this->mStep = STEP_READ_CHUNK_SIZE;
        }
        else
        {
            this->Sleep(1);
        }
    }

    else if(this->mStep == STEP_READ_CHUNK_TAIL_AND_DONE)
    {
        if(this->mSocketRw->DoRead(interval))
        {
            this->mCallback->Run(EVENT_DONE);
            this->Stop(ERROR_NONE);
        }
        else
        {
            this->Sleep(1);
        }
    }
    return OK;
}

const char * CTaskHttpClient::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_SOCKET_RW_ERROR)
        return "socket error";
    return "unknown error";
}
status_t CTaskHttpClient::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskHttpClient ");
    mem.Printf("(%d) ",this->GetId());

    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));     
    LOG("%s",mem.CStr());
    return OK;
}
status_t CTaskHttpClient::Start()
{
    this->mStep = STEP_PREPARE_HEADER_TO_SEND;
    this->Resume();
    return OK;
}
status_t CTaskHttpClient::Stop(status_t err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);
    this->mCallback->SetParamInt(1,err);
    this->mCallback->Run(EVENT_STOP);
    return OK;
}

status_t CTaskHttpClient::SetSocket(CSocket *isocket)
{
    this->mSocketRw->SetSocket(isocket);
    return OK;
}

status_t CTaskHttpClient::PrepareFile(CFileBase *ifile)
{
    this->iCurFile = ifile;
    return OK;
}

CClosure* CTaskHttpClient::Callback()
{
    return mCallback;
}
