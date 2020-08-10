// TaskSocketWriter.cpp: implementation of the CTaskSocketWriter class.
//
//////////////////////////////////////////////////////////////////////
#include "tasksocketwriter.h"
#include "mem.h"

int CTaskSocketWriter::EVENT_FINISH = 1;
int CTaskSocketWriter::EVENT_QUIT = 2;

status_t CTaskSocketWriter::ERROR_NONE = 0;
status_t CTaskSocketWriter::ERROR_CONNECTION_CLOSED = 1;
status_t CTaskSocketWriter::ERROR_READ_ERROR = 2;
status_t CTaskSocketWriter::ERROR_WRITE_TIMEOUT = 3;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTaskSocketWriter::CTaskSocketWriter()
{
    this->InitBasic();
}
CTaskSocketWriter::~CTaskSocketWriter()
{
    this->Destroy();
}
status_t CTaskSocketWriter::InitBasic()
{
    CTask::InitBasic();
    this->iSrcFile = NULL;
    this->iSocket = NULL;
    this->mBuf = NULL;
    this->mBufOffset = 0;
    this->mTimeout = 60*1024;
    this->mWriteTime = 0;   
    return OK;
}
status_t CTaskSocketWriter::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    NEW(this->mBuf,CMem);
    this->mBuf->Init();
    this->mBuf->Malloc(256*1024);
    return OK;
}
status_t CTaskSocketWriter::Destroy()
{
    DEL(this->mBuf);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskSocketWriter::Run(uint32_t interval)
{
    ASSERT(this->iSrcFile && this->iSocket);
    fsize_t rs,ws;

    if(!this->iSocket->IsConnected())
    {
        if(this->IsComplete())
            this->Stop(ERROR_NONE);
        else
            this->Stop(ERROR_CONNECTION_CLOSED);
        return ERROR;
    }

    if(mBufOffset >= this->mBuf->GetSize())
    {
        mBuf->SetSize(0);
        rs = this->iSrcFile->Read(mBuf->GetRawBuf(),(int_ptr_t)mBuf->GetMaxSize());
        if(rs <= 0)
        {
            this->Stop(ERROR_READ_ERROR);
            return ERROR;
        }
        this->mBuf->SetSize(rs);
        this->mBufOffset = 0;
    }   
    else
    {
        ws = this->iSocket->Write(mBuf->GetRawBuf() + mBufOffset,(int_ptr_t)(mBuf->GetSize() - mBufOffset));
        if(ws > 0)
        {
            mBufOffset += ws;
            this->mWriteTime = 0;
        }
        else if(ws == 0)
        {
            this->mWriteTime += interval;
            if(this->mWriteTime >= this->mTimeout)
            {
                this->iSocket->CloseConnect();
                this->Stop(ERROR_WRITE_TIMEOUT);
            }
            this->Sleep(10);
        }
        else
        {
            ASSERT(0);
        }
    }

    if(this->IsComplete())
    {
        this->Stop(ERROR_NONE);
    }
    return OK;
}
status_t CTaskSocketWriter::Start()
{
    this->mWriteTime = 0;
    this->ClearBuf();
    return this->Resume();
}
status_t CTaskSocketWriter::Stop(status_t err)
{
    if(this->IsDead())return ERROR;
    this->ReportError(err);
    ASSERT(this->iSocket);
    if(!this->iSocket->IsConnected())
    {
        this->Quit();
        this->SetCallbackParam(1,(void*)err);
        this->SetCallbackParam(2,this);
        this->RunCallback(EVENT_QUIT);
        return OK;
    }
    this->SetCallbackParam(1,(void*)err);
    this->SetCallbackParam(2,this);
    this->RunCallback(EVENT_FINISH);
    return this->Suspend();
}
status_t CTaskSocketWriter::IsComplete()
{
    ASSERT(this->iSrcFile);
    return this->iSrcFile->IsEnd() && (mBufOffset >= this->mBuf->GetSize());
}
status_t CTaskSocketWriter::SetSocket(CSocket *i_socket)
{
    ASSERT(i_socket);
    this->iSocket = i_socket;
    return OK;
}

status_t CTaskSocketWriter::SetSrcFile(CFileBase *i_file)
{
    return this->SetSrcFile(0,i_file);
}

status_t CTaskSocketWriter::SetSrcFile(fsize_t start,CFileBase *i_file)
{
    ASSERT(i_file);
    this->ClearBuf();
    i_file->Seek(start);
    this->iSrcFile = i_file;
    return OK;
}

status_t CTaskSocketWriter::ClearBuf()
{
    this->mBufOffset = 0;
    this->mBuf->SetSize(0);
    return OK;
}
const char * CTaskSocketWriter::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_CONNECTION_CLOSED)
        return "connection closed";
    if(err == ERROR_READ_ERROR)
        return "read error";
    if(err == ERROR_WRITE_TIMEOUT)
        return "write timeout";
    return "unknown error";
}

status_t CTaskSocketWriter::ReportError(int32_t err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskSocketWriter ");
    mem.Printf("(%d) ",this->GetId());

    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));     
    LOG("%s",mem.CStr());

    return OK;
}
status_t CTaskSocketWriter::SetTimeout(int32_t timeout)
{
    this->mTimeout=timeout;
    return OK;
}
