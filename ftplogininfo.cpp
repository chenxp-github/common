#include "ftplogininfo.h"

CFtpLoginInfo::CFtpLoginInfo()
{
    this->InitBasic();
}
CFtpLoginInfo::~CFtpLoginInfo()
{
    this->Destroy();
}
status_t CFtpLoginInfo::InitBasic()
{
    this->mServer = NULL;
    this->mUser = NULL;
    this->mPort = 21;
    this->mPass = NULL;

    return OK;
}
status_t CFtpLoginInfo::Init()
{
    this->InitBasic();
    NEW(this->mServer,CMem);
    this->mServer->Init();
    this->mServer->Malloc(256);

    NEW(this->mUser,CMem);
    this->mUser->Init();
    this->mUser->Malloc(256);

    NEW(this->mPass,CMem);
    this->mPass->Init();
    this->mPass->Malloc(256);

    return OK;
}
status_t CFtpLoginInfo::Destroy()
{
    DEL(this->mPass);
    DEL(this->mUser);
    DEL(this->mServer);
    this->InitBasic();
    return OK;
}
status_t CFtpLoginInfo::Copy(CFtpLoginInfo *p)
{
    if(this == p)
        return OK;
    this->mPass->StrCpy(p->mPass);
    this->mPort = p->mPort;
    this->mUser->StrCpy(p->mUser);
    this->mServer->StrCpy(p->mServer);
    return OK;
}
status_t CFtpLoginInfo::Comp(CFtpLoginInfo *p)
{
    ASSERT(p);
    int ret = this->mServer->StrICmp(p->mServer);
    if(ret != 0) return ret;    
    return this->mPort-p->mPort;
}
status_t CFtpLoginInfo::Print()
{
    //add your code
    return TRUE;
}

const char * CFtpLoginInfo::GetServer()
{
    return this->mServer->CStr();
}
status_t CFtpLoginInfo::SetServer(const char * server)
{
    this->mServer->StrCpy(server);
    return OK;
}
int32_t CFtpLoginInfo::GetPort()
{
    return this->mPort;
}
status_t CFtpLoginInfo::SetPort(int32_t port)
{
    this->mPort=port;
    return OK;
}
const char  * CFtpLoginInfo::GetUser()
{
    return this->mUser->CStr();
}
status_t CFtpLoginInfo::SetUser(const char  * user)
{
    ASSERT(user);
    this->mUser->StrCpy(user);
    return OK;
}
const char * CFtpLoginInfo::GetPass()
{
    return this->mPass->CStr();
}
status_t CFtpLoginInfo::SetPass(const char * pass)
{
    this->mPass->StrCpy(pass);
    return OK;
}
