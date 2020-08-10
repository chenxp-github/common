// TaskSocketReader.cpp: implementation of the CTaskSocketReader class.
//
//////////////////////////////////////////////////////////////////////
#include "tasksocketreader.h"

int CTaskSocketReader::EVENT_FINISH = 1;
int CTaskSocketReader::EVENT_QUIT = 2;

status_t CTaskSocketReader::ERROR_NONE = 0;
status_t CTaskSocketReader::ERROR_CONNECTION_CLOSED = 1;
status_t CTaskSocketReader::ERROR_READ_TIMEOUT = 2;
status_t CTaskSocketReader::ERROR_WAIT_BUF_TIME_OUT = 3;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTaskSocketReader::CTaskSocketReader()
{
    this->InitBasic();
}
CTaskSocketReader::~CTaskSocketReader()
{
    this->Destroy();
}
status_t CTaskSocketReader::InitBasic()
{
    CTask::InitBasic();

    this->mQBuf = NULL;
    this->iSocket = NULL;
    this->iDstFile = NULL;
    this->mFileLen = 0;
    this->mTimeout = 60*1024;
    this->mReadTime = 0;
    this->mWaitBufTime = 0;
    this->mIsFinishMsgSend = false;

    return OK;
}
status_t CTaskSocketReader::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    
    NEW(this->mQBuf,CQueueFile);
    this->mQBuf->Init(64*1024);
    return OK;
}
status_t CTaskSocketReader::Destroy()
{
    DEL(this->mQBuf);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskSocketReader::Run(uint32_t interval)
{
    ASSERT(this->iSocket);
    fsize_t left_size,size,need_read;

    this->PutDataToDesFile();
    if(!this->iSocket->IsConnected())
    {
        if(this->IsComplete())
        {
            if(this->mQBuf->GetSize() <= 0)
            {
                this->Stop(ERROR_NONE);
            }
            else
            {
                this->mWaitBufTime += interval;
                if(this->mWaitBufTime >= this->mTimeout)
                    this->Stop(ERROR_WAIT_BUF_TIME_OUT);
                this->Sleep(10);
            }
        }
        else
        {
            this->Stop(ERROR_CONNECTION_CLOSED);
        }
        return ERROR;
    }

    left_size = this->mQBuf->GetFreeSize();
    if(left_size <= 0)
        return ERROR;

    CMem mem;
    LOCAL_MEM_WITH_SIZE(mem,64*1024);
    need_read = mem.GetMaxSize();
    if(need_read > left_size)
        need_read = left_size;
    size = this->iSocket->Read(mem.GetRawBuf(),(int_ptr_t)need_read);
    if(size > 0)
    {
        this->mReadTime = 0;
        this->mQBuf->Write(mem.GetRawBuf(),(int_ptr_t)size);
    }
    else if(size == 0)
    {
        this->mReadTime += interval;
        if(this->mReadTime >= this->mTimeout)
        {
            this->iSocket->CloseConnect();
            this->Stop(ERROR_READ_TIMEOUT);
        }
        this->Sleep(10);
    }
    else
    {
        ASSERT(0);
    }

    return OK;
}

status_t CTaskSocketReader::SetDstFile(CFileBase *i_file, fsize_t read_len)
{
    this->mIsFinishMsgSend = false;
    this->iDstFile = i_file;
    this->mFileLen = read_len;
    return OK;
}
status_t CTaskSocketReader::IsComplete()
{
    return this->mFileLen <= 0;
}
status_t CTaskSocketReader::PutDataToDesFile()
{
    if(this->iDstFile == NULL)
        return ERROR;
    
    fsize_t size = this->mQBuf->GetSize();
    if(size > 0)
    {
        if(size > this->mFileLen)
            size = this->mFileLen;
        this->mFileLen -= size;

        char buf[FILEBASE_LBUF_SIZE];
        while(size > 0)
        {
            int_ptr_t rs = this->mQBuf->Read(buf,FILEBASE_LBUF_SIZE);
            if(rs > 0)
            {
                this->iDstFile->Write(buf,rs);
                size -= rs;
            }
        }
    }   

    if(this->IsComplete() && (!mIsFinishMsgSend))
    {   
        this->mIsFinishMsgSend = true;
        this->SetCallbackParam(1,this);
        this->RunCallback(EVENT_FINISH);            
        return OK;
    }
    return OK;
}
status_t CTaskSocketReader::SetSocket(CSocket *i_socket)
{
    ASSERT(i_socket);
    this->iSocket = i_socket;
    return OK;
}
status_t CTaskSocketReader::IsBufEmpty()
{
    return this->mQBuf->GetSize() <= 0;
}

status_t CTaskSocketReader::Stop(status_t err)
{
    if(this->IsDead())return ERROR;
    this->Quit();
    this->ReportError(err);
    this->PutDataToDesFile();
    this->SetCallbackParam(1,(void*)err);
    this->SetCallbackParam(2,this);
    this->RunCallback(EVENT_QUIT);
    return OK;
}

status_t CTaskSocketReader::Start()
{
    this->mIsFinishMsgSend = false;
    this->mWaitBufTime = 0;
    this->mReadTime = 0;
    this->Resume();
    return OK;
}

const char * CTaskSocketReader::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_CONNECTION_CLOSED)
        return "connection closed";
    if(err ==  ERROR_READ_TIMEOUT)
        return "read timeout";      
    if(err == ERROR_WAIT_BUF_TIME_OUT)
        return "wait buf time out";
    return "unknown error";
}

status_t CTaskSocketReader::ReportError(int32_t err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskSocketReader ");
    mem.Printf("(%d) ",this->GetId());
    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));     
    LOG("%s",mem.CStr());
    return OK;
}
status_t CTaskSocketReader::SetTimeout(int32_t timeout)
{
    this->mTimeout=timeout;
    return OK;
}

bool CTaskSocketReader::BufHasEmptyLine()
{
    return this->mQBuf->HasEmptyLine();
}

bool CTaskSocketReader::BufHasWholeLine()
{
    return this->mQBuf->HasWholeLine();
}

CQueueFile * CTaskSocketReader::GetQueueBuf()
{
    return this->mQBuf;
}

CFileBase * CTaskSocketReader::GetDstFile()
{
    return this->iDstFile;
}
