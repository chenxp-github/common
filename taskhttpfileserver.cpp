#include "taskhttpfileserver.h"
#include "dirmgr.h"
#include "encoder.h"
#include "fileinfolist.h"

CTaskHttpFileServer::CTaskHttpFileServer()
{
    this->InitBasic();
}
CTaskHttpFileServer::~CTaskHttpFileServer()
{
    this->Destroy();
}
status_t CTaskHttpFileServer::InitBasic()
{
    CTaskHttp::InitBasic();
    
    this->mf_data_r = NULL;
    this->mf_tmp = NULL;
    this->server = NULL;
    this->file_to_send = NULL;
    this->root_path = NULL;
    this->emu_img = 0;
    return OK;
}
status_t CTaskHttpFileServer::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTaskHttp::Init(mgr);

    NEW(this->mf_tmp,CMemFile);
    this->mf_tmp->Init();

    NEW(this->mf_data_r,CMemFile);
    this->mf_data_r->Init();

    NEW(this->server,CTcpServer);
    this->server->Init();

    NEW(this->file_to_send,CFile);
    this->file_to_send->Init();

    this->SetSocket(this->server);
    this->PrepareFileToReceiveData(this->mf_data_r);    
    this->PrepareDataFileToSend(NULL);
    
    this->callback_server_handle_request = handle_request;
    this->param_server_handle_request[0] = this;

    return OK;
}
status_t CTaskHttpFileServer::Destroy()
{
    LOG("Task %d exit\r\n",this->id);
    DEL(this->mf_data_r);
    DEL(this->server);
    DEL(this->file_to_send);
    DEL(this->mf_tmp);
    CTaskHttp::Destroy();
    this->InitBasic();
    return OK;
}

status_t CTaskHttpFileServer::handle_request(void **param)
{
    ASSERT(param);

    CTaskHttpFileServer *self = (CTaskHttpFileServer*)param[0];
    ASSERT(self);

    CMem verb,mem,buf;
    LOCAL_MEM(verb);
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);

    self->GetVerb(&verb,&mem);
    CEncoder::UrlDecode(&mem,&buf);

    if(verb.StrICmp("get") == 0)
    {
        self->GetListHtml(buf.GetPW());     
    }
    
    return OK;
}

status_t CTaskHttpFileServer::GetVerb(CFileBase *verb,CFileBase *val)
{
    ASSERT(verb && val);
    this->mf_header->Seek(0);
    this->mf_header->SetSplitChars(" \t\n");
    this->mf_header->ReadString(verb);
    this->mf_header->ReadString(val);
    return OK;
}

status_t CTaskHttpFileServer::GetListHtml(const wchar_t *path)
{
    ASSERT(path && this->root_path);
    
    CMem mem;   
    LOCAL_MEM(mem);

    mem.StrCpyW(path);
    CDirMgr::ToAbsPathW(this->root_path,&mem);

    if(CDirMgr::IsDirExistW(mem.GetPW()))
    {
        CFileInfoList list;
        list.Init(1024);
        list.GetFromDirW(mem.GetPW());
        list.SortFolderFirstW(0);
        this->PrepareDataFileToSend(this->mf_tmp);
        this->ListToHtml(&list,this->mf_tmp);
        this->MakeHttpRetHeader(L"_tmp.html");
    }
    else
    {
        this->file_to_send->Destroy();
        this->file_to_send->Init();
        if(!this->file_to_send->OpenFileW(mem.GetPW(),L"rb"))
        {
            LOGW(L"Open %s error\n",mem.GetPW());
            this->PrepareDataFileToSend(NULL);
            return ERROR;
        }       
        this->PrepareDataFileToSend(this->file_to_send);
        this->MakeHttpRetHeader(mem.GetPW());
    }
    return OK;
}

status_t CTaskHttpFileServer::ListToHtml(CFileInfoList *list, CFileBase *html)
{
    ASSERT(list && html);
    
    html->SetSize(0);
    CMem mem;
    LOCAL_MEM(mem);

    this->MakeHtmlHeader(html);

    CFileInfo *pi;
    for(int i = 0; i < list->GetLen(); i++)
    {
        pi = list->GetElem(i);
        ASSERT(pi);
        
        html->PutsW(L"<a href=\"");
        html->PutsW(pi->mFileName);
        if(pi->mIsDir)
            html->PutsW(L"/");
        html->PutsW(L"\"");

        if(pi->mIsDir)
            html->PutsW(L" style=\"color:#006600\"");
        else
            html->PutsW(L" style=\"color:#990000\"");

        html->PutsW(L">");

        if(pi->mIsDir)
            html->PutsW(L"&lt;");
        
        html->PutsW(pi->mFileName);

        if(pi->mIsDir)
            html->PutsW(L"&gt;");
        html->PutsW(L"</a>      ");

        if(!pi->mIsDir)
        {
            this->SizeToString(pi->mSize,&mem);
            html->PutsW(&mem);  
        }

        html->PutsW(L"\r\n");
    }
    
    this->MakeHtmlTail(html);

    CEncoder::UnicodeToUtf8(html);
    return OK;
}

int CTaskHttpFileServer::SetRootPath(const wchar_t *root)
{
    this->root_path = root;
    return OK;
}

int CTaskHttpFileServer::MakeHttpRetHeader(const wchar_t *filename)
{
    ASSERT(this->i_file_data_to_send && filename);

    CMem mem,buf;
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);

    CDirMgr::GetFileNameW(filename,&mem,FN_EXT);
    this->GetContentTypeByExt(mem.GetPW(),&buf);
    mf_header->SetSize(0);
    mf_header->Puts("HTTP/1.1 200 OK\r\n");

    mf_header->Puts("Content-Type: ");
    mf_header->Puts(buf.p);
    mf_header->Puts("\r\n");

    mf_header->Puts("Connection: Keep-Alive\r\n");
    mf_header->Printf("Content-Length:%d\r\n",this->i_file_data_to_send->GetSize());
    mf_header->Puts("\r\n");

    return OK;
}

status_t CTaskHttpFileServer::GetContentTypeByExt(const char *ext, CFileBase *file)
{
    ASSERT(ext && file);

const char *_str = 
"htm,text/html\n"
"html,text/html\n"
"txt,text/plain\n"
"jpg,image/jpeg\n"
"gif,image/gif\n"
"mp3,audio\n"
"3gp,video\n"
"apk,application/x-msdownload\n"
;
    status_t find = false;

    CMem mem,buf,tmp;
    LOCAL_MEM(buf);
    LOCAL_MEM(tmp);

    mem.Init();
    mem.SetP((char*)_str);
    mem.Seek(0);

    buf.SetSplitChars(",");
    while(mem.ReadLine(&buf))
    {
        buf.Seek(0);
        buf.ReadString(&tmp);
        if(tmp.StrICmp(ext) == 0)
        {
            find = true;
            buf.ReadString(&tmp);
            break;
        }
    }

    if(!find)
    {
        tmp.SetSize(0);
        if(this->emu_img)
            tmp.Puts("image/jpeg");
        else
            tmp.Puts("application/x-msdownload");
        tmp.Putc(0);
    }
    file->SetSize(0);
    file->WriteFile(&tmp);
    return OK;
}

status_t CTaskHttpFileServer::GetContentTypeByExt(const wchar_t *ext, CFileBase *file)
{
    ASSERT(ext && file);

    CMem mem;
    LOCAL_MEM(mem);
    mem.StrCpyW(ext);
    CEncoder::UnicodeToGb(&mem);
    return this->GetContentTypeByExt(mem.p,file);
}

status_t CTaskHttpFileServer::EmuImage(status_t enable)
{
    this->emu_img = enable;
    return OK;
}

status_t CTaskHttpFileServer::MakeHtmlHeader(CFileBase *html)
{
    ASSERT(html);

    CMem mem;
    LOCAL_MEM(mem);
    html->PutsW(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    html->PutsW(L"<!DOCTYPE html PUBLIC \"-//WAPFORUM//DTD XHTML Mobile 1.0//EN\" \"http://www.wapforum.org/DTD/xhtml-mobile10.dtd\">\r\n");
    html->PutsW(L"<html>\r\n");
    html->PutsW(L"<head>\r\n");
    html->PutsW(L"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\r\n");
    html->PutsW(L"<style type=\"text/css\">\r\n");
    html->PutsW(L"\tbody{line-height:1.6em;}\r\n");
    html->PutsW(L"</style>\r\n");
    html->PutsW(L"\r\n");
    html->PutsW(L"</head>\r\n");
    html->PutsW(L"<body>\r\n");
    html->PutsW(L"\r\n");
    html->PutsW(L"<h3 align=\"center\">\r\n");

    mem.StrCpy("Moon Http服务器");
    CEncoder::GbToUnicode(&mem);
    html->PutsW(&mem);

    html->PutsW(L"</h3>\r\n");
    html->PutsW(L"\r\n");
    html->PutsW(L"<pre>\r\n");

    mem.StrCpy("返回上一级目录");
    CEncoder::GbToUnicode(&mem);

    html->PutsW(L"<a href=\"..\" style=\"color:#FF0000\">[");
    html->PutsW(&mem);
    html->PutsW(L"]</a><br/>");


    return OK;
}

status_t CTaskHttpFileServer::MakeHtmlTail(CFileBase *html)
{
    ASSERT(html);
    html->PutsW(L"</pre>\r\n");
    html->PutsW(L"<p align=\"center\">\r\n");
    
    CMem mem;
    LOCAL_MEM(mem);

    mem.StrCpy("更多软件请访问:");
    CEncoder::GbToUnicode(&mem);
    html->PutsW(&mem);

    html->PutsW(L"<a href=\"http://www.yueyuesoft.com\" style=\"color:#FF0000\">");

    mem.StrCpy("月月软件工作室");
    CEncoder::GbToUnicode(&mem);
    html->PutsW(&mem);

    html->PutsW(L"</a>\r\n");
    html->PutsW(L"</p>\r\n");
    html->PutsW(L"</body>\r\n");
    html->PutsW(L"</html>\r\n");
    return OK;
}

status_t CTaskHttpFileServer::SizeToString(fsize_t size, CFileBase *str)
{
    ASSERT(str);
    
    str->SetSize(0);
    
    int h,l;


    if(size < 0) size = 0;

    if(size < 1024)
        str->PrintfW(L"%d",size);

    if(size >= 1024 && size < 1024*1024)
    {
        h = size / 1024;
        l = (size - h * 1024)*100 / 1024;
        str->PrintfW(L"%d.%02d KB",h,l);
    }

    if(size >= 1024*1024 && size < 1024*1024*1024)
    {
        h = size / 1024 / 1024;
        l = (size - h * 1024 * 1024)*100 /1024/1024;
        str->PrintfW(L"%d.%02d MB",h,l);
    }

    if(size >= 1024*1024*1024)
    {
        h = size / 1024/1024/1024;
        l = (size - h * 1024*1024*1024)*100 /1024/1024/1024;
        str->PrintfW(L"%d.%02d GB",h,l);
    }

    str->PutcW(0);

    return OK;
}
