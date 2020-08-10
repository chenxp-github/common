#ifndef __TASKFTPACCEPTER_H
#define __TASKFTPACCEPTER_H

#include "taskmgr.h"
#include "tcp.h"
#include "ftpconfiglist.h"

class CTaskFtpAccepter:public CTask{
public:
    CTcpServer *mServer;
    CFtpConfigList *iConfigs;   
    int32_t mTimeout;
    CMem *mAnonymousRoot;
    status_t mAllowAnonymous;
public:
    status_t AllowAnonymouse(status_t enable, const char *root);
    status_t SetTimeout(int to);
    status_t SetConfigs(CFtpConfigList *i_list);
    CTaskFtpAccepter();
    virtual ~CTaskFtpAccepter();
    status_t Init(CTaskMgr *mgr,int port);
    status_t Destroy();
    status_t InitBasic();
    status_t OnTimer(int32_t interval);
};

#endif
