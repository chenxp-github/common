#include "taskftpaccepter.h"
#include "taskftpserver.h"

CTaskFtpAccepter::CTaskFtpAccepter()
{
    this->InitBasic();
}
CTaskFtpAccepter::~CTaskFtpAccepter()
{
    this->Destroy();
}
status_t CTaskFtpAccepter::InitBasic()
{
    CTask::InitBasic();
    this->mServer = NULL;
    this->iConfigs = NULL;
    this->mTimeout = 60*1000;
    this->mAllowAnonymous = FALSE;
    this->mAnonymousRoot = NULL;
    return OK;
}

status_t CTaskFtpAccepter::Init(CTaskMgr *mgr,int port)
{
    this->InitBasic();
    CTask::Init(mgr);
    NEW(this->mServer,CTcpServer);
    this->mServer->Init();
    this->mServer->InitServer();
    this->mServer->SetPort(port);
    this->mServer->SetBlocking(0);

    NEW(this->mAnonymousRoot,CMem);
    this->mAnonymousRoot->Init();
    this->mAnonymousRoot->Malloc(1024);

    return OK;
}

status_t CTaskFtpAccepter::Destroy()
{
    DEL(this->mAnonymousRoot);
    DEL(this->mServer);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

status_t CTaskFtpAccepter::OnTimer(int32_t interval)
{
    int32_t snum = this->mServer->Accept();
    if(snum > 0)
    {
        LOCAL_MEM(mem);
        int32_t port;

        this->mServer->GetClientIpAndPort(&mem,&port);

        ASSERT(this->iConfigs);
        CTaskFtpServer *ftp;
        NEW(ftp,CTaskFtpServer);
        ftp->Init(this->GetTaskMgr());
        ftp->SetTimeout(this->mTimeout);
        ftp->SetServerSocket(snum,mem.CStr());
        ftp->Start();
        ftp->SetInterval(1);
        ftp->SetConfigs(this->iConfigs);
        ftp->AllowAnonymouse(this->mAllowAnonymous);
        ftp->SetAnonymouseRootPath(this->mAnonymousRoot->CStr());
        ftp->PrepareResponse("220 moon ftp server by chenxp");

        LOG("new ftp server %d",ftp->id);
    }

    return OK;
}

status_t CTaskFtpAccepter::SetConfigs(CFtpConfigList *i_list)
{
    this->iConfigs = i_list;
    return OK;
}

status_t CTaskFtpAccepter::SetTimeout(int to)
{
    this->mTimeout = to;
    return OK;
}

status_t CTaskFtpAccepter::AllowAnonymouse(status_t enable, const char *root)
{
    ASSERT(root);
    this->mAnonymousRoot->StrCpy(root);
    this->mAllowAnonymous = enable;
    return OK;
}
