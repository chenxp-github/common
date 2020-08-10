#include "com_can.h"
#include "syslog.h"
#include "mem_tool.h"
#include "minitask.h"

static const uint8_t start_tag[2] = {0xaa,0xaa};
static const uint8_t end_tag[2] = {0x55,0x55};

#define NEED_ESCAPE(ch)\
(ch == 0xa5 || ch == 0xaa || ch == 0x55)\

CComCan::CComCan()
{
    this->InitBasic();
}

CComCan::~CComCan()
{
    this->Destroy();
}

status_t CComCan::InitBasic()
{
    TASK_CONTAINER_CLEAR();
    WEAK_REF_CLEAR();
    this->m_Serial.InitBasic();
    m_RecvBuf.InitBasic();
    m_SendBuf.InitBasic();
    m_Callback.InitBasic();
    m_TaskReadingSerial = 0;
    m_TaskWritingSerial = 0;
    return OK;
}

status_t CComCan::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    
    TASK_CONTAINER_INIT(mgr);
    this->m_Serial.Init();
    m_RecvBuf.Init(64*1024);
    m_SendBuf.Init(64*1024);
    m_Callback.Init();
    return OK;
}

status_t CComCan::Destroy()
{
    WEAK_REF_DESTROY();
    QuitTask(&m_TaskWritingSerial);
    QuitTask(&m_TaskReadingSerial);
    m_Callback.Destroy();
    m_RecvBuf.Destroy();
    this->m_Serial.Destroy();
    this->InitBasic();
    return OK;
}

CSerial* CComCan::GetSerial()
{
    return &m_Serial;
}

status_t CComCan::OpenSerial(const char *name, int baudrate, int databits, int stopbits, int parity)
{
    ASSERT(name);

    if(!m_Serial.Open(name))
        return ERROR;

    if(!m_Serial.Configure(baudrate,databits,stopbits,parity))
        return ERROR;
    
    return OK;    
}

CClosure *CComCan::Callback()
{
    return &m_Callback;
}

status_t CComCan::OnReadSerial()
{
    char buf[1024];

    size_t size = sizeof(buf);
    if((size_t)m_RecvBuf.GetFreeSize() < size)
    {
        size = (size_t)m_RecvBuf.GetFreeSize();
    }

    int n = m_Serial.Read(buf,size);
    if(n > 0)
    {
        m_RecvBuf.Write(buf,n);
    }

    LOCAL_MEM(package);
    LOCAL_MEM(raw);

    if(this->GetNextPackage(&package))
    {
        if(ParsePackage(&package,&raw))
        {
            ASSERT(raw.GetSize() == sizeof(CanMessage_t));
            CanMessage_t *msg = (CanMessage_t*)raw.GetRawBuf();
            m_Callback.SetParamPointer(1,msg);
            m_Callback.SetParamPointer(2,this);
            m_Callback.Run(EVENT_RECV_CAN_DATA);
        }
    }
    return OK;
}

status_t CComCan::StartReadingSerialTask()
{
    ASSERT(!IsTask(m_TaskReadingSerial));

    BEGIN_MINI_TASK(recv_task)
    {
        CComCan *self;
        status_t Run(uint32_t interval)
        {
            self->OnReadSerial();
            return OK;
        }
    }
    END_MINI_TASK(recv_task);

    recv_task->Init(GetTaskMgr());
    recv_task->self = this;
    recv_task->Start(1);    
    m_TaskReadingSerial = recv_task->GetId();
    return OK;
}

status_t CComCan::StartWritingSerialTask()
{
    if(IsTask(m_TaskWritingSerial))
    {
        ResumeTask(m_TaskWritingSerial);
        return OK;
    }   

    BEGIN_MINI_TASK(send_task)
    {
        CComCan *self;
        status_t Run(uint32_t interval)
        {    
            if(self->m_SendBuf.GetSize() > 0)
            {
                char buf[1024];
                int_ptr_t rs = self->m_SendBuf.PeekData(buf,sizeof(buf));
                if(rs > 0)
                {
                    int_ptr_t ws = self->m_Serial.Write(buf,rs);                    
                    if(ws > 0)
                    {
                        self->m_SendBuf.Read(NULL,ws);
                    }
                }
            }
            else
            {
                this->Suspend();
            }
            
            return OK;
        }
    }
    END_MINI_TASK(send_task);

    send_task->Init(GetTaskMgr());
    send_task->self = this;
    send_task->Start(1);    
    m_TaskWritingSerial = send_task->GetId();
    return OK;
}

status_t CComCan::GetNextPackage(CMem *package)
{
    ASSERT(package);

    package->SetSize(0);

    int_ptr_t s_pos = m_RecvBuf.FindTag(start_tag,2);

    if(s_pos >= 0)
    {
        int_ptr_t e_pos = m_RecvBuf.FindTag(end_tag,2);
        if(e_pos > s_pos)
        {
            if(s_pos > 0) 
            {
                m_RecvBuf.Read(NULL,s_pos);
            }    

            int size = e_pos - s_pos + 2;

            ASSERT(size < package->GetMaxSize());
            int_ptr_t rs = m_RecvBuf.Read(package->GetRawBuf(),size);
            ASSERT(rs == size);
            package->SetSize(rs);
            return OK;
        }
    }

    //drop receive buffer, if no tag found and buffer is full
    if(m_RecvBuf.GetFreeSize() == 0)
    {
        m_RecvBuf.Clear();
        return ERROR;
    }

    return ERROR;
}

status_t CComCan::ParsePackage(CMem *package, CMem *raw)
{
    ASSERT(package && raw);
    ASSERT(package->GetSize() > 5);
    int data_size = (int)(package->GetSize()-5);
    uint8_t *p = (uint8_t*)package->GetRawBuf()+2;

    uint8_t checksum_to_be_checked = package->CharAt(
        package->GetSize()-3);
    
    if(NEED_ESCAPE(checksum_to_be_checked))
        data_size--;

    raw->SetSize(0);
    uint8_t checksum = 0;

    bool meet_esc_char = false;
    for(int i = 0; i < data_size; i++)
    {        
        if(!meet_esc_char)
        {
            if(p[i] == 0xa5)
            {
                meet_esc_char = true;
            }
            else
            {
                checksum += p[i];
                raw->Putc(p[i]);
            }
        }
        else
        {
            checksum += p[i];
            raw->Putc(p[i]);
            meet_esc_char = false;
        }
    }
    
    if(checksum_to_be_checked == checksum)
    {
        return OK;
    }

    //checksum fail.
    raw->SetSize(0);
    return ERROR;
}

status_t CComCan::SendRawData(CMem *raw)
{
    ASSERT(raw);

    if(m_SendBuf.GetFreeSize() == 0)
        return ERROR;

    LOCAL_MEM_WITH_SIZE(tmp,8192);
    ASSERT(raw->GetSize() < tmp.GetMaxSize()/2);

    uint8_t checksum = 0;
    uint8_t *p = (uint8_t*)raw->GetRawBuf();
    int size = (int)raw->GetSize();

    tmp.Write(start_tag,2);

    for(int i = 0; i < size; i++)
    {
        if(NEED_ESCAPE(p[i]))
        {
            tmp.Putc((char)0xa5);
            tmp.Putc(p[i]);
            checksum += p[i];
        }
        else
        {
            tmp.Putc(p[i]);
            checksum += p[i];
        }        
    }
    
    if(NEED_ESCAPE(checksum))
    {
        tmp.Putc((char)0xa5);
    }

    tmp.Putc(checksum);
    tmp.Write(end_tag,2);

    if(m_SendBuf.GetFreeSize() > tmp.GetSize())
    {
        m_SendBuf.Write(tmp.GetRawBuf(),(int_ptr_t)tmp.GetSize());
        this->StartWritingSerialTask();
        return OK;
    }

    return ERROR;
}

status_t CComCan::SendCanMessage(CanMessage_t *msg)
{
    ASSERT(msg);
    CMem mem;
    mem.Init();
    mem.SetRawBuf(msg,sizeof(CanMessage_t),true);
    return this->SendRawData(&mem);
}

