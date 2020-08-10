#include "httputils.h"


int32_t CHttpUtils::GetHttpRetValue(CFileBase *header)
{
    ASSERT(header);
    int32_t ret = 0;
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);
    header->Seek(0);
    header->ReadLine(&mem);
    if(!mem.StartWith("HTTP",0,1))
        return ret;
    mem.SetSplitChars(" \t");
    mem.Seek(0);
    mem.ReadString(&buf);
    mem.ReadString(&buf);
    
    ret = atoi(buf.CStr());
    return ret;
}

fsize_t CHttpUtils::GetContentLength(CFileBase *header)
{
    ASSERT(header);
    status_t ret;
    LOCAL_MEM(mem);
    ret = header->ReadKeyValue("Content-Length",&mem);
    if(!ret)return -1;
    return crt_str_to_fsize(mem.CStr());
}

bool CHttpUtils::IsChunked(CFileBase *header)
{
    ASSERT(header);
    LOCAL_MEM(mem);
    header->ReadKeyValue("Transfer-encoding",&mem);
    if(mem.StrICmp("chunked") == 0)
        return true;
    return false;
}

status_t CHttpUtils::GetLocation(CFileBase *header, CFileBase *file)
{
    ASSERT(header && file);
    file->SetSize(0);
    return header->ReadKeyValue("Location",file);
}

status_t CHttpUtils::ParseUrl(CMem *url, CFileBase *host, int32_t *port, CFileBase *path)
{
    ASSERT(url && host && port && path);
    host->SetSize(0);
    path->SetSize(0);
    *port = 80; //default
    LOCAL_MEM(mem);
    
    if(!url->StartWith("http",0,1))
    {
        path->StrCpy(url);
        return OK;
    }
    url->SetSplitChars("/:");
    url->Seek(0);
    url->ReadString(&mem); //http
    url->ReadString(&mem); //host
    host->StrCpy(&mem);
    fsize_t old_off = url->GetOffset();
    url->ReadWord(&mem);
    if(mem.C(0) == ':')
    {
        url->ReadString(&mem);
        *port = atoi(mem.CStr());
    }
    else
    {
        url->Seek(old_off);
    }
    url->ReadLeftStr(path,1);
    if(path->StrLen() <= 0)
        path->StrCpy("/");
    return OK;
}

status_t CHttpUtils::GetContentRange(CFileBase *header, fsize_t *start, fsize_t *end)
{
    ASSERT(header && start && end);

    LOCAL_MEM(mem);

    if(!header->ReadKeyValue("Content-Range",&mem))
        return ERROR;

    mem.SetSplitChars(" \t-/");
    
    LOCAL_MEM(buf);

    mem.Seek(0);
    mem.ReadString(&buf); //bytes

    mem.ReadString(&buf);
    *start = atoi(buf.CStr());
    
    mem.ReadString(&buf);
    *end = atoi(buf.CStr());

    return OK;
}

status_t CHttpUtils::GetContentType(CFileBase *header, CMem *type)
{
    ASSERT(header && type);
    type->StrCpy("");

    LOCAL_MEM(mem);

    if(!header->ReadKeyValue("Content-Type",&mem))
        return ERROR;
    
    mem.SetSplitChars("/");
    mem.Seek(0);
    mem.ReadString(type);
    return OK;
}
