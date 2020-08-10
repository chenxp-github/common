#ifndef __FTPLOGININFO_H
#define __FTPLOGININFO_H

#include "mem.h"

#undef SetPort

class CFtpLoginInfo{
public:
    CMem *mServer;
    int32_t mPort;
    CMem *mUser,*mPass;
public:
    CFtpLoginInfo();
    virtual ~CFtpLoginInfo();
    status_t Init();
    status_t Destroy();
    status_t Copy(CFtpLoginInfo *p);
    status_t Comp(CFtpLoginInfo *p);
    status_t Print();
    status_t InitBasic();
    const char * GetServer();
    status_t SetServer(const char * server);
    int32_t GetPort();
    status_t SetPort(int32_t port);
    const char  * GetUser();
    status_t SetUser(const char  * user);
    const char * GetPass();
    status_t SetPass(const char * pass);
};

#endif
