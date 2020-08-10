#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include "netlinksocket.h"
#include "syslog.h"
#include "mem_tool.h"

#define NETLINK_TEST 29
#define MAX_PAYLOAD  4096

CNetLinkSocket::CNetLinkSocket()
{
	this->InitBasic();
}
CNetLinkSocket::~CNetLinkSocket()
{
	this->Destroy();
}
status_t CNetLinkSocket::InitBasic()
{
	m_socket_fd = 0;
	return OK;
}
status_t CNetLinkSocket::Init()
{
	this->InitBasic();
	return OK;
}
status_t CNetLinkSocket::Destroy()
{
	if(m_socket_fd)
	{
		close(m_socket_fd);
		m_socket_fd = 0;
	}
	this->InitBasic();
	return OK;
}

status_t CNetLinkSocket::Create()
{
	ASSERT(m_socket_fd == 0);
 	m_socket_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
	ASSERT(m_socket_fd > 0);
	return OK;
}

status_t CNetLinkSocket::Bind()
{
	ASSERT(m_socket_fd > 0);
    struct sockaddr_nl rxaddr;
	memset(&rxaddr,0,sizeof(rxaddr));
	rxaddr.nl_family = AF_NETLINK;
	rxaddr.nl_pid = getpid();
	rxaddr.nl_groups = 0;
    int ret = bind(m_socket_fd, (struct sockaddr*)&rxaddr, sizeof(rxaddr));
	ASSERT(ret == 0);
	return ret == 0;
}

status_t CNetLinkSocket::RecvMsg(CMem *data)
{
	ASSERT(data);
	ASSERT(m_socket_fd > 0);
	
	struct sockaddr_nl local;
	socklen_t skaddr_nl_len;

	unsigned char tempData[4096] = {0};
	struct nlmsghdr *nlhdr = (struct nlmsghdr*)tempData;

	data->SetSize(0);

	int recvlen = recvfrom(
		m_socket_fd, 
		(unsigned char *)nlhdr, sizeof(tempData), 0,
		(struct sockaddr *)&local, &skaddr_nl_len);

	if(recvlen <= 0)return ERROR;

	int payload_len = NLMSG_PAYLOAD(nlhdr, 0);
	data->Write(NLMSG_DATA(nlhdr), payload_len);
	return OK;
}

int CNetLinkSocket::SendMsg(const void *data, int data_len)
{	
	uint8_t _buffer[NLMSG_SPACE(MAX_PAYLOAD)] = {0};
    struct nlmsghdr* nlhdr = NULL;
    uint32_t size = NLMSG_LENGTH(0);

    struct sockaddr_nl txaddr;
	memset(&txaddr,0,sizeof(txaddr));

	txaddr.nl_family = AF_NETLINK;
    txaddr.nl_pid = 0;
    txaddr.nl_groups = 0;
	
    if(data_len > 0) 
	{
        size = NLMSG_LENGTH(data_len);
		ASSERT(size < NLMSG_SPACE(MAX_PAYLOAD));
    }

    nlhdr = (struct nlmsghdr *)_buffer;
    nlhdr->nlmsg_len = size;
    nlhdr->nlmsg_pid = getpid();
    nlhdr->nlmsg_flags = 0;
	
    if(data_len > 0) 
	{
       memcpy(NLMSG_DATA(nlhdr), data, data_len);
    }

    struct iovec iov = {
        .iov_base = (void*)nlhdr,
        .iov_len = nlhdr->nlmsg_len,
    };

    struct msghdr msg = {
        .msg_name = (void*)&txaddr,
        .msg_namelen = sizeof(txaddr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    
    return sendmsg(m_socket_fd, &msg, 0);
}

int CNetLinkSocket::SendMsg(CMem *data)
{
	ASSERT(data);
	return this->SendMsg(data->GetRawBuf(),data->GetSize());
}
