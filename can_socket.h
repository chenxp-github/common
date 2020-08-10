#ifndef __CAN_SOCKET_H
#define __CAN_SOCKET_H

#include <net/if.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/can.h> 
#include <linux/can/raw.h>

#include "mem.h"
#include "memfile.h"

class CCanSocket{
public:
    int m_handle;
    int m_can_id;
public:
    CCanSocket();
    ~CCanSocket();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Create();
    status_t Bind(const char *can_name);
    int Write(const struct can_frame *frame);
    int Write(CMem *data);
    status_t SetCanFilter(uint32_t can_id, uint32_t can_mask);
    status_t SetCanFilter(uint32_t can_id);
    status_t CanNameToIndex(const char *can_name);
    int Read(struct can_frame *frame);
    int Read(CMem *buf);  
    status_t SetCanId(int id);
    int GetCanId();
};

#endif
