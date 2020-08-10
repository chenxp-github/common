#ifndef __HTTPUTILS_H
#define __HTTPUTILS_H

#include "mem.h"

class CHttpUtils{
public:
    static status_t GetContentType(CFileBase *header, CMem *type);
    static status_t GetContentRange(CFileBase *header, fsize_t *start, fsize_t *end);
    static status_t ParseUrl(CMem *url,CFileBase *host, int32_t *port, CFileBase *path);
    static status_t GetLocation(CFileBase *header,CFileBase *file);
    static bool IsChunked(CFileBase *header);
    static fsize_t GetContentLength(CFileBase *header);
    static int32_t GetHttpRetValue(CFileBase *header);
};

#endif
