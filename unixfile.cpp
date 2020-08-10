#include "unixfile.h"
#include "syslog.h"

CUnixFile::CUnixFile()
{
    this->InitBasic();
}
CUnixFile::~CUnixFile()
{
    this->Destroy();
}
int CUnixFile::InitBasic()
{
    this->fd = 0;
    return OK;
}
int CUnixFile::Init()
{
    this->InitBasic();
    CFileBase::Init();
    //add your code
    return OK;
}
int CUnixFile::Destroy()
{
    if(this->fd)
    {
        close(this->fd);
        this->fd = 0;
    }
    
    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}

long CUnixFile::GetSize()
{
    long old_off,size = 0;
    
    ASSERT(this->fd);

    old_off = lseek(this->fd,0,SEEK_CUR);   
    size = lseek(this->fd,0,SEEK_END);      
    lseek(this->fd,old_off,SEEK_SET);
    
    return size;    
}

int CUnixFile::Seek(long nOffset)
{
    int ret;
    
    ASSERT(this->fd);

    if(nOffset < 0)
        ret = lseek(this->fd,0,SEEK_END);
    else
        ret = lseek(this->fd,nOffset,SEEK_SET);

    return ret;
}
long CUnixFile::Read(void *buf,long nByteToRead)
{
    int ret = 0;
    ASSERT(this->fd);
    ret = read(this->fd, buf,nByteToRead);    
    return ret;
}
long CUnixFile::Write(void *buf,long nByteToWrite)
{
    int ret = 0;
    ASSERT(this->fd);
    ret = write(this->fd,buf,nByteToWrite);
    return ret;
}

int CUnixFile::SetSize(long nSize)
{
    return this->AdjustOffset();
}

long CUnixFile::GetOffset()
{
    ASSERT(this->fd);
    return lseek(this->fd,0,SEEK_CUR);
}
int CUnixFile::AddBlock(long bsize)
{
    return OK;
}

long CUnixFile::GetMaxSize()
{
    return this->GetSize();
}

int CUnixFile::OpenFile(char *filename,int flag)
{
    ASSERT(this->fd == 0);
    
    this->fd = open((const char*)filename,flag);
    
    if(fd < 0)
    {
        LOG("open %s error\n",filename);
        return ERROR;
    }
    
    return OK;
}
