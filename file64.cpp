#include "file64.h"
#include "syslog.h"

CFile64::CFile64()
{
    this->InitBasic();
}
CFile64::~CFile64()
{
    this->Destroy();
}
status_t CFile64::InitBasic()
{
    this->mFd = 0;
    return OK;
}
status_t CFile64::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CFile64::Destroy()
{
#if _LINUX_
    if(this->mFd)
    {
        close(this->mFd);
        this->mFd = 0;
    }
#else
    ASSERT(0);
#endif
    this->InitBasic();
    return OK;
}

status_t CFile64::Open(const char *filename, const char *mode)
{
    ASSERT(filename && mode);
    ASSERT(this->mFd == 0);
    uint32_t m = this->StrToMode(mode);
    this->mFd = open(filename,m);
    if(this->mFd == 0)
        return ERROR;
    return OK;
}

uint32_t CFile64::StrToMode(const char *str)
{
    ASSERT(str);
    
    uint32_t mode = O_RDONLY;
    
    for(int32_t i = 0; i < (int32_t)strlen(str); i++)
    {
        if(str[i] == 'w' || str[i] == 'W')
            mode |= O_CREAT;
        if(str[i] == 'a' || str[i] == 'A')
            mode |= O_APPEND;
        if(str[i] == '+')
            mode |= O_RDWR;
        if(str[i] == 'r' || str[i] == 'R')
            mode |= O_RDONLY;
    }
    return mode;
}

fsize_t CFile64::GetOffset()
{
    return (fsize_t)this->GetOffset64();
}

fsize_t CFile64::GetSize()
{
    return (fsize_t)this->GetSize64();
}

fsize_t CFile64::Read(void *buf,fsize_t n)
{
    ASSERT(this->mFd);
    return read(this->mFd,buf,n);
}

fsize_t CFile64::Write(const void *buf,fsize_t n)
{
    ASSERT(this->mFd);
    return write(this->mFd,buf,n);
}

fsize_t CFile64::SetSize(fsize_t ssize)
{
    return 0;
}

fsize_t CFile64::Seek(fsize_t off)
{
    return this->Seek64((int64_t)off);
}

fsize_t CFile64::AddBlock(fsize_t bsize)
{
    return 0;
}

fsize_t CFile64::GetMaxSize()
{
    return this->GetSize();
}

int64_t CFile64::GetSize64()
{
    ASSERT(this->mFd);
    int64_t old_off = lseek64(this->mFd,0,SEEK_CUR);
    lseek64(this->mFd,0,SEEK_END);
    int64_t size = lseek64(this->mFd,0,SEEK_CUR);
    lseek64(this->mFd,old_off,SEEK_SET);
    return size;
}

int64_t CFile64::GetOffset64()
{
    ASSERT(this->mFd);
    return lseek64(this->mFd,0,SEEK_CUR);
}

int64_t CFile64::Seek64(int64_t off)
{
    ASSERT(this->mFd);
    return lseek64(this->mFd,off,SEEK_SET);
}

status_t CFile64::IsEnd64()
{
    return this->GetOffset64() >= this->GetSize64();
}

