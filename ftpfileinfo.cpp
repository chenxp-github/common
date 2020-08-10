#include "ftpfileinfo.h"

CFtpFileInfo::CFtpFileInfo()
{
    this->InitBasic();
}
CFtpFileInfo::~CFtpFileInfo()
{
    this->Destroy();
}
status_t CFtpFileInfo::InitBasic()
{
    this->mFileName = NULL;
    this->mMd5Check = NULL;
    this->mVersion = 0.0f;
    this->mFileSize = 0;
    return OK;
}
status_t CFtpFileInfo::Init()
{
    this->InitBasic();
    
    NEW(this->mFileName,CMem);
    this->mFileName->Init();
    this->mFileName->Malloc(1024);

    NEW(this->mMd5Check,CMem);
    this->mMd5Check->Init();
    this->mMd5Check->Malloc(64);

    return OK;
}
status_t CFtpFileInfo::Destroy()
{
    DEL(this->mMd5Check);
    DEL(this->mFileName);
    this->InitBasic();
    return OK;
}
status_t CFtpFileInfo::Copy(CFtpFileInfo *p)
{
    if(this == p)return OK;
    this->mFileName->StrCpy(p->mFileName);
    this->mMd5Check->StrCpy(p->mMd5Check);
    this->mVersion=p->mVersion;
    this->mFileSize=p->mFileSize;
    return OK;
}
status_t CFtpFileInfo::Comp(CFtpFileInfo *p)
{
    return this->mFileName->StrCmp(p->mFileName);
}
status_t CFtpFileInfo::Print()
{
    //add your code
    return TRUE;
}

status_t CFtpFileInfo::SetMd5Check(const char *md5)
{
    ASSERT(md5);
    this->mMd5Check->StrCpy(md5);
    return OK;
}

status_t CFtpFileInfo::SetFileName(const char *fileName)
{
    ASSERT(fileName);
    this->mFileName->StrCpy(fileName);
    return OK;
}

status_t CFtpFileInfo::SetVersion(double version)
{
    this->mVersion = version;
    return OK;
}

const char * CFtpFileInfo::GetFileName()
{
    return this->mFileName->CStr();
}   

const char * CFtpFileInfo::GetMd5Check()
{
    return this->mMd5Check->CStr();
}

double CFtpFileInfo::GetVersion()
{
    return this->mVersion;
}
fsize_t CFtpFileInfo::GetFileSize()
{
    return this->mFileSize;
}
status_t CFtpFileInfo::SetFileSize(fsize_t filesize)
{
    this->mFileSize=filesize;
    return OK;
}

status_t CFtpFileInfo::HasMd5Check()
{
    return this->mMd5Check->StrLen() > 0;
}

bool CFtpFileInfo::IsSizeAvailable()
{
    return this->mFileSize > 0;
}
