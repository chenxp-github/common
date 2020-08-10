#ifndef __FTPFILEINFO_H
#define __FTPFILEINFO_H

#include "mem.h"

class CFtpFileInfo{
public:
    CMem *mFileName;
    double mVersion;
    CMem *mMd5Check;
    fsize_t mFileSize;
public:
    status_t HasMd5Check();
    fsize_t GetFileSize();
    status_t SetFileSize(fsize_t filesize);
    double GetVersion();
    const char * GetMd5Check();
    const char * GetFileName();
    status_t SetVersion(double version);
    status_t SetFileName(const char *fileName);
    status_t SetMd5Check(const char *md5);
    CFtpFileInfo();
    virtual ~CFtpFileInfo();
    status_t Init();
    status_t Destroy();
    status_t Copy(CFtpFileInfo *p);
    status_t Comp(CFtpFileInfo *p);
    status_t Print();
    status_t InitBasic();
    bool IsSizeAvailable();
};

#endif
