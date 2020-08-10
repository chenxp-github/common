// TaskSocketReader.h: interface for the CTaskSocketReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKSOCKETREADER_H__FC705B9C_313E_4002_AACD_79ED93AE6C7F__INCLUDED_)
#define AFX_TASKSOCKETREADER_H__FC705B9C_313E_4002_AACD_79ED93AE6C7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"
#include "tcp.h"
#include "queuefile.h"
#include "taskmgr.h"

class CTaskSocketReader:public CTask{
public:
    static status_t ERROR_NONE;
    static status_t ERROR_CONNECTION_CLOSED;
    static status_t ERROR_READ_TIMEOUT;
    static status_t ERROR_WAIT_BUF_TIME_OUT;

    static int EVENT_FINISH;
    static int EVENT_QUIT;

    CQueueFile *mQBuf;
    CSocket *iSocket;
    CFileBase *iDstFile;
    fsize_t mFileLen;
    int mTimeout;
    int mReadTime;
    int mWaitBufTime;
    bool mIsFinishMsgSend;
public:
    CFileBase * GetDstFile();
    CQueueFile * GetQueueBuf();
    bool BufHasWholeLine();
    bool BufHasEmptyLine();
    status_t SetTimeout(int32_t timeout);
    status_t ReportError(int32_t err);
    const char * ErrorToString(int32_t err);
    status_t Start();
    status_t Stop(status_t err);
    status_t IsBufEmpty();
    status_t SetSocket(CSocket *i_socket);
    status_t PutDataToDesFile();
    status_t IsComplete();
    status_t SetDstFile(CFileBase *i_file,fsize_t read_len);
    CTaskSocketReader();
    virtual ~CTaskSocketReader();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t Run(uint32_t interval);
};

#endif // !defined(AFX_TASKSOCKETREADER_H__FC705B9C_313E_4002_AACD_79ED93AE6C7F__INCLUDED_)
