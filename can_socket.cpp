#include "can_socket.h"
#include "syslog.h"
#include "mem_tool.h"

#define CHECK_HANDLE() ASSERT(m_handle > 0)

CCanSocket::CCanSocket()
{
    this->InitBasic();
}

CCanSocket::~CCanSocket()
{
    this->Destroy();
}

status_t CCanSocket::InitBasic()
{
    this->m_handle = 0;
    this->m_can_id = 0;
    return OK;
}

status_t CCanSocket::Init()
{
    this->InitBasic();

    return OK;
}

status_t CCanSocket::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CCanSocket::Create()
{
    ASSERT(m_handle == 0);
    m_handle = socket(PF_CAN, SOCK_RAW, CAN_RAW);  
    return m_handle > 0;
}

status_t CCanSocket::Bind(const char *can_name)
{
    ASSERT(can_name);
    CHECK_HANDLE();
    
    struct ifreq ifr;  
    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name, can_name);

	/* determine the interface index */  
	int res = ioctl(m_handle, SIOCGIFINDEX, &ifr);
    ASSERT(res == 0);

  	struct sockaddr_can addr;  

    addr.can_family = AF_CAN;  
	addr.can_ifindex = ifr.ifr_ifindex;
	res = bind(m_handle, (struct sockaddr *)&addr, sizeof(addr));
    ASSERT(res == 0);

	res = setsockopt(m_handle, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);  
    ASSERT(res == 0);

    return OK;
}

status_t CCanSocket::SetCanFilter(uint32_t can_id, uint32_t can_mask)
{
    CHECK_HANDLE();    
    struct can_filter rfilter;
    rfilter.can_id   = can_id;  
    rfilter.can_mask = can_mask;  
    int res = setsockopt(m_handle, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));  
    return res == 0;
}

status_t CCanSocket::SetCanFilter(uint32_t can_id)
{
    return this->SetCanFilter(can_id,CAN_SFF_MASK);
}

int CCanSocket::Write(const struct can_frame *frame)
{
    ASSERT(frame);
    CHECK_HANDLE();
    return write(m_handle,frame,sizeof(struct can_frame));
}

int CCanSocket::Write(CMem *data)
{
    ASSERT(data);
    ASSERT(m_can_id > 0);
    ASSERT(data->GetSize() <= CAN_MAX_DLEN);

    struct can_frame frame;
    frame.can_id = m_can_id;
    frame.can_dlc = data->GetSize();
    memcpy(frame.data,data->GetRawBuf(),frame.can_dlc);

    return this->Write(&frame);
}

int CCanSocket::CanNameToIndex(const char *can_name)
{
    struct ifreq ifr;  
    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name, can_name);
	int res = ioctl(m_handle, SIOCGIFINDEX, &ifr);    
    ASSERT(res == 0);
    return ifr.ifr_ifindex;
}

int CCanSocket::Read(struct can_frame *frame)
{
    ASSERT(frame);
    CHECK_HANDLE();
    return read(m_handle, frame, sizeof(struct can_frame));
}

int CCanSocket::Read(CMem *buf)
{
    ASSERT(buf);
    CHECK_HANDLE();

    buf->SetSize(0);

    struct can_frame frame;

    int n = read(m_handle, &frame, sizeof(frame));

    if(n > 0)
    {
        buf->Write(frame.data,frame.can_dlc);
    }
    return n;
}

status_t CCanSocket::SetCanId(int id)
{
    this->m_can_id = id;
    return OK;
}

int CCanSocket::GetCanId()
{
    return m_can_id;
}

