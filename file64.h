#ifndef __FILE64_H
#define __FILE64_H

#include "common.h"
#include "filebase.h"

#if _LINUX_
#include <fcntl.h>
#endif

class CFile64: public CFileBase{
public:
    int32_t mFd;
public:
    status_t IsEnd64();
    int64_t Seek64(int64_t off);
    int64_t GetOffset64();
    int64_t GetSize64();
    fsize_t GetOffset();
    fsize_t GetSize();
    fsize_t Read(void *buf,fsize_t n);
    fsize_t Write(const void *buf,fsize_t n);
    fsize_t SetSize(fsize_t ssize);
    fsize_t Seek(fsize_t off);
    fsize_t AddBlock(fsize_t bsize);
    fsize_t GetMaxSize();   
    uint32_t StrToMode(const char *str);
    status_t Open(const char *filename, const char *mode);
    CFile64();
    virtual ~CFile64();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};

#endif

