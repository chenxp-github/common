#ifndef __TASKHTTPFILESERVER_H
#define __TASKHTTPFILESERVER_H

#include "taskhttp.h"
#include "file.h"
#include "fileinfolist.h"

class CTaskHttpFileServer:public CTaskHttp{
public:
    CMemFile *mf_data_r;
    CMemFile *mf_tmp;
    CTcpServer *server;
    CFile *file_to_send;
    const wchar_t *root_path;
    status_t emu_img;
public:
    static status_t SizeToString(fsize_t size, CFileBase *str);
    static status_t MakeHtmlTail(CFileBase *html);
    static status_t MakeHtmlHeader(CFileBase *html);
    status_t EmuImage(status_t enable);
    status_t GetContentTypeByExt(const wchar_t *ext, CFileBase *file);
    status_t GetContentTypeByExt(const char *ext, CFileBase *file);
    int MakeHttpRetHeader(const wchar_t *filename);
    int SetRootPath(const wchar_t *root);
    status_t ListToHtml(CFileInfoList *list,CFileBase *html);
    status_t GetListHtml(const wchar_t *path);
    status_t GetVerb(CFileBase *verb,CFileBase *val);
    static status_t handle_request(void **param);
    CTaskHttpFileServer();
    virtual ~CTaskHttpFileServer();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
};

#endif
