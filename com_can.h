#ifndef __COM_CAN_H
#define __COM_CAN_H

#include "serial.h"
#include "mem.h"
#include "memfile.h"
#include "queuefile.h"
#include "taskmgr.h"
#include "closure.h"

typedef struct {
	uint32_t id;
	uint8_t  data[8];
	uint8_t  len;
	uint8_t  ch;
	uint8_t  format;
	uint8_t  type;
} CanMessage_t;

typedef enum {
    STANDARD_FORMAT = 0, 
    EXTENDED_FORMAT
} CAN_FORMAT;

typedef enum {
    DATA_FRAME = 0, 
    REMOTE_FRAME
}CAN_FRAME;

class CComCan{
public:
    enum{
        EVENT_RECV_CAN_DATA = 1,
    };

    TASK_CONTAINER_DEFINE();
    WEAK_REF_DEFINE();

public:
    CSerial m_Serial;
    CQueueFile m_RecvBuf;
    CQueueFile m_SendBuf;
    CClosure m_Callback;
    
    int m_TaskReadingSerial;
    int m_TaskWritingSerial;
public:
    CComCan();
    virtual ~CComCan();

    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();

    CSerial* GetSerial();
    status_t OpenSerial(const char *name, 
        int baudrate = 115200, 
        int databits = 8,
        int stopbits = 1,
        int parity = 'n');

    CClosure *Callback();

    status_t StartReadingSerialTask();
    status_t StartWritingSerialTask();
    status_t OnReadSerial();

    status_t GetNextPackage(CMem *package);    
    status_t ParsePackage(CMem *package, CMem *raw);
    status_t SendRawData(CMem *raw);
    status_t SendCanMessage(CanMessage_t *msg);
};

#endif
