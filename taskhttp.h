// TaskHttpClient.h: interface for the CTaskHttp class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_TASKHTTP_H__F0B0B557_F41B_4E78_9038_F6C9C6C26406__INCLUDED_)
#define AFX_TASKHTTP_H__F0B0B557_F41B_4E78_9038_F6C9C6C26406__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "taskmgr.h"
#include "tasksocketreader.h"
#include "tasksocketwriter.h"
#include "memfile.h"
#include "tcp.h"

class CTaskHttp:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_CONNECTION_CLOSED;
    static status_t ERROR_CONTENT_LENGTH;

    static int EVENT_SERVER_HANDLE_REQUEST;
    static int EVENT_QUIT;
    static int EVENT_GET_HEADER;

    int32_t cur_step;
    CMemFile *mf_header;
    CFileBase *i_file_data_to_send;
    CFileBase *i_file_data_to_receive;
    int32_t tid_socket_reader;
    int32_t tid_socket_writer;
    CSocket *i_socket;
    int32_t task_type;
    int32_t timeout;
public:
    static status_t on_socket_reader_event(void **p);
    static status_t on_socket_writer_event(void **p);
    status_t SetTimeout(int32_t _timeout);
    const char * ErrorToString(int32_t err);
    status_t ReportError(int32_t err);
    status_t MakeDefHttpRetHeader();
    status_t MakeDefHttpRetHeader(int32_t ret_val,fsize_t len);
    status_t SetAsClient();
    status_t SetAsServer();
    status_t OnTimer_Server();
    status_t OnTimer_Client();
    status_t Stop(status_t err);
    status_t SetSocket(CSocket *i_socket);
    status_t StartTask();
    status_t PrepareFileToReceiveData(CFileBase *file_to_receive);
    status_t PrepareDataFileToSend(CFileBase *file_to_send);
    status_t PrepareHttpHeader(CFileBase *header);
    CTaskSocketWriter * GetSocketWriter(status_t renew);
    CTaskSocketReader * GetSocketReader(status_t renew);
    CTaskHttp();
    virtual ~CTaskHttp();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
};
#endif // !defined(AFX_TASKHTTP_H__F0B0B557_F41B_4E78_9038_F6C9C6C26406__INCLUDED_)
