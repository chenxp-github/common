#include "ftpthreadinfo.h"

CFtpThreadInfo::CFtpThreadInfo()
{
    this->InitBasic();
}
CFtpThreadInfo::~CFtpThreadInfo()
{
    this->Destroy();
}
status_t CFtpThreadInfo::InitBasic()
{
    TASK_CONTAINER_INIT();
    this->offset = 0;
    this->maxSize = 0;
    this->size = 0;
    this->task_ftp_thread = 0;
    return OK;
}
status_t CFtpThreadInfo::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    this->SetTaskMgr(mgr);
    return OK;
}
status_t CFtpThreadInfo::Destroy()
{
    QuitTask(&this->task_ftp_thread);
    this->InitBasic();
    return OK;
}
status_t CFtpThreadInfo::Copy(CFtpThreadInfo *p)
{
    if(this == p)
        return OK;
    this->offset = p->offset;
    this->maxSize = p->maxSize;
    this->size = p->size;
    return OK;
}
status_t CFtpThreadInfo::Comp(CFtpThreadInfo *p)
{
    return 0;
}
status_t CFtpThreadInfo::Print()
{
    LOG("offset=%d\r\n",this->offset);
    LOG("maxSize=%d\r\n",this->maxSize);
    LOG("size=%d\r\n",this->size);
    return TRUE;
}
/********************************************************************/
CTaskFtpThread * CFtpThreadInfo::CreateTaskFtpThread(CFileBase *dstFile,const char *fileName,fsize_t offset, fsize_t size,CFtpLoginInfo *info)
{
    ASSERT(dstFile && fileName && info);
    ASSERT(!IsTask(this->task_ftp_thread));

    this->offset = offset;
    this->maxSize = size;
    this->size = 0;

    CTaskFtpThread *pt;
    NEW(pt,CTaskFtpThread);
    pt->Init(this->GetTaskMgr());
    pt->SetInterval(1);
    pt->SetDstFile(dstFile,this->offset,this->maxSize);
    pt->SetFileName(fileName);
    pt->SetLoginInfo(info);

    this->task_ftp_thread = pt->GetID();
    return pt;
}

fsize_t CFtpThreadInfo::GetSize()
{
    this->UpdateSize();
    return this->size;
}

status_t CFtpThreadInfo::UpdateSize()
{
    CTaskFtpThread *pt = (CTaskFtpThread*)GetTask(this->task_ftp_thread);
    if(pt != NULL)
    {
        this->size = pt->GetDownloadedSize();
    }
    return OK;
}

status_t CFtpThreadInfo::GetUncompleteBlock(fsize_t *offset, fsize_t *size)
{
    ASSERT(offset && size);
    *offset = this->offset + this->GetSize();
    *size = this->maxSize - this->GetSize();
    return OK;
}

bool CFtpThreadInfo::IsTaskRunning()
{
    return IsTask(this->task_ftp_thread);
}

bool CFtpThreadInfo::IsComplete()
{
    return this->GetSize() == this->maxSize;
}
