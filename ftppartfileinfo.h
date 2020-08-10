#ifndef __FTPPARTFILEINFO_H
#define __FTPPARTFILEINFO_H

#include "mem.h"
#include "xml.h"

class CFtpPartFileInfo{
public:
    fsize_t size;
    char *md5;
    double version;
    char *fileName;
    fsize_t totalSize;
public:
    bool IsComplete();
    bool HasMd5();
    CFtpPartFileInfo();
    virtual ~CFtpPartFileInfo();
    status_t Init();
    status_t Destroy();
    status_t Copy(CFtpPartFileInfo *p);
    status_t Comp(CFtpPartFileInfo *p);
    status_t Print();
    status_t InitBasic();
    const char *GetMd5();
    status_t SetMd5(const char *md5);
    const char *GetFileName();
    status_t SetFileName(const char *fileName);
    status_t SaveToXml(CFileBase *xml);
    status_t LoadFromXml(CXmlNode *root);

    status_t Clear();
    int32_t CompVersion(CFtpPartFileInfo *info);
};

#endif
