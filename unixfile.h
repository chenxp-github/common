#ifndef __UNIXFILE_H
#define __UNIXFILE_H

#include "common.h"
#include "filebase.h"

#ifdef _LINUX_
#include <unistd.h>
#include <fcntl.h>
#endif

class CUnixFile:public CFileBase{
public:
    int fd;
public:
    CUnixFile();
    virtual ~CUnixFile();
    int Init();
    int Destroy();
    int InitBasic();
    long GetSize();
    int Seek(long nOffset);
    long Read(void *buf,long nByteToRead);
    long Write(void *buf,long nByteToWrite);
    int SetSize(long nSize);
    long GetOffset();
    int AddBlock(long bsize);
    long GetMaxSize();
    int OpenFile(char *filename,int flag);
};

#endif
