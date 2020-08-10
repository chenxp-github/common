#ifndef __S2_NETLINKSOCKET_H
#define __S2_NETLINKSOCKET_H

#include "cruntime.h"
#include "mem.h"

class CNetLinkSocket{
public:
	int m_socket_fd;
public:
	CNetLinkSocket();
	virtual ~CNetLinkSocket();
	status_t InitBasic();
	status_t Init();
	status_t Destroy();

	status_t Create();
	status_t Bind();

	status_t RecvMsg(CMem *data);
	int SendMsg(const void *data, int len);
	int SendMsg(CMem *data);
};

#endif
