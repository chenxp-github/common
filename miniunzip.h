#ifndef __MINIUNZIP_H
#define __MINIUNZIP_H

#include "minizip_unzip.h"
#include "minizip_iofile.h"
#include "filebase.h"
#include "mem.h"

class CMiniUnzip{
public:
    CFileBase *iZipFile;
    zlib_filefunc_def *mUnFunc;
    unzFile uFile;
public:
    bool IsCurFileDir();
    status_t CloseCurFile();
    status_t ExtractCurFile(CFileBase *out);
    status_t OpenCurFile();
    fsize_t ReadCurFile(void *buf, fsize_t len);
    status_t LocateFile(const char *fileName, bool caseSensitivy);
    fsize_t GetCurFileSize();
    status_t GetCurFileName(CMem *name);
    status_t GotoNextFile();
    status_t GotoFirstFile();
    int32_t GetFileCount();
    status_t SetZipFile(CFileBase *file);
    CMiniUnzip();
    virtual ~CMiniUnzip();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};

#endif
