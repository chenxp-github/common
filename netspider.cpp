// NetSpider.cpp: implementation of the CNetSpider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetSpider.h"
#include "htmreader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNetSpider::CNetSpider()
{
    this->InitBasic();
}
CNetSpider::~CNetSpider()
{
    this->Destroy();
}
int CNetSpider::InitBasic()
{
    this->stk_new = 0;
    this->stk_used = 0;
    this->http = NULL;  
    this->callback_get_file = NULL;
    this->param_get_file = NULL;
    this->cur_path = 0;
    this->cur_root = 0;
    this->running = 0;

    return OK;
}
int CNetSpider::Init()
{
    this->InitBasic();
    
    NEW(this->http,CHttpClient);
    NEW(this->stk_new,CMemStk);
    NEW(this->stk_used,CMemStk);
    NEW(this->cur_path,CMem);
    NEW(this->cur_root,CMem);

    this->http->Init(); 
    this->stk_new->Init(1024*10);
    this->stk_used->Init(1024*10);

    this->cur_path->Init();
    this->cur_path->Malloc(1024);

    this->cur_root->Init();
    this->cur_root->Malloc(1024);

    this->http->callback_read = NULL;
    this->http->SetTimeout(2*60*1000);

    return OK;
}
int CNetSpider::Destroy()
{
    DEL(this->stk_new);
    DEL(this->stk_used);
    DEL(this->http);
    DEL(this->cur_path);
    DEL(this->cur_root);

    this->InitBasic();
    return OK;
}
int CNetSpider::Copy(CNetSpider *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
int CNetSpider::Comp(CNetSpider *p)
{
    return 0;
}
int CNetSpider::Print()
{
    //add your code
    return TRUE;
}

int CNetSpider::SetProxy(char *server, int port, char *proxy_auth)
{
    return this->http->SetProxy(server,port,proxy_auth);
}

int CNetSpider::AddNew(CMem *url)
{
    ASSERT(url);

    if(this->stk_used->Search(url))
        return ERROR;
    if(this->stk_new->Search(url))
        return ERROR;

    this->stk_new->Push(url);
    return OK;
}

int CNetSpider::AddNew(char *url)
{
    ASSERT(url);
    
    CMem mem;
    mem.Init();
    mem.SetP(url);

    return this->AddNew(&mem);
}

int CNetSpider::AddHtml(CFileBase *file)
{
    ASSERT(file);
    
    CMemFile mf;
    CMem mem,buf;

    LOCAL_MEM(mem);
    LOCAL_MEM(buf);

    mf.Init();
    CHtmReader::GetAllLink(file,&mf);

    mf.Seek(0);
    while(mf.ReadLine(&mem))
    {
        if(mem.p[0] == 0)
            continue;

        if(mem.StartWith("http",0,1))
        {
            this->AddNew(&mem);
        }
        else if(mem.StartWith("/",0,1))
        {
            buf.SetSize(0);
            buf.Puts(this->cur_root);
            buf.Puts(&mem); 
            buf.Putc(0);
            this->AddNew(&buf);
        }
        else if(mem.StartWith("javascript",0,1))
        {
            //nothing
        }
        else if(!mem.StartWith(".",0,1))
        {
            buf.SetSize(0);
            buf.Puts(this->cur_path);
            buf.Putc('/');
            buf.Puts(&mem); 
            buf.Putc(0);
            this->AddNew(&buf);
        }
        
    }
    
    return OK;
}

int CNetSpider::GetNewUrl(CFileBase *file)
{
    ASSERT(file);
    CMem *pmem;

    file->SetSize(0);

    pmem = this->stk_new->GetElem(0);
    if(pmem == NULL)
        return ERROR;

    file->Puts(pmem->p);
    file->Putc(0);

    this->stk_new->DelElem(0);

    return OK;
}

int CNetSpider::Run()
{
    CMem mem;
    CMem mf;
    int is_htm;

    LOCAL_MEM(mem);
    mf.Init();
    mf.Malloc(1024*1024);

    this->running = 1;

    while(this->GetNewUrl(&mem) && running)
    {               
        mf.SetSize(0);
        this->GetUrlPath(&mem,this->cur_path);
        this->GetUrlRoot(&mem,this->cur_root);

        this->http->GetFile(&mem,&mf);      
        is_htm = this->IsHtml(&mem);
        
        if(is_htm)
            this->AddHtml(&mf);

        if(this->callback_get_file)
        {
            if( this->callback_get_file(this->param_get_file,&mem,&mf,is_htm) == ERROR)
                break;
        }

        this->AddUsed(&mem);
    }

    this->running = 0;

    return OK;
}

int CNetSpider::AddUsed(CMem *url)
{
    if(this->stk_used->Search(url))
        return ERROR;
    return this->stk_used->Push(url);
}

int CNetSpider::GetUrlPath(CMem *url, CFileBase *path)
{
    ASSERT(url && path);
    
    int i,pos = 0,len;

    path->SetSize(0);
    len = url->StrLen();

    for(i = len - 1; i >= 0; i--)
    {
        if(url->CharAt(i) == '/')
        {
            if(url->CharAt(i-1) == '/')
            {
                pos = len;
                break;
            }
            else
            {
                pos = i;
                break;
            }
        }
    }

    path->WriteFile(url,0,pos);
    path->Putc(0);
    return OK;
}

int CNetSpider::GetUrlRoot(CMem *url, CFileBase *root)
{
    ASSERT(url && root);
    
    int pos,i,len;
    char ch;

    root->SetSize(0);

    len = url->StrLen();

    url->Seek(0);   
    pos = url->SearchStr("//",0,0);
    
    if(pos >= 0)
    {
        root->WriteFile(url,0,pos+2);
        for(i = pos + 2; i<len; i++)
        {
            ch = url->CharAt(i);
            if(ch == '/')
                break;

            root->Putc(ch);
        }
    }

    root->Putc(0);
    return OK;
}

int CNetSpider::IsHtml(CMem *url)
{
    ASSERT(url);
    int i,len,pos;
    char ch;
    CMem ext;

    LOCAL_MEM(ext);

    len = url->StrLen();

    if(len <= this->cur_root->StrLen())
        return TRUE;

    pos = -1;
    for(i = len-1; i >= 0; i--)
    {
        ch = url->CharAt(i);
        if(ch == '.')
        {
            pos = i;
            break;
        }
        if(ch == '/')
        {
            pos = -1;
            break;
        }
    }

    if(pos >= 0)
    {
        ext.Puts(url->p + pos + 1);
        ext.Putc(0);

        if(CFileBase::InStrList(ext.p,"js,bmp,jpg,jpeg,css,gif,txt,exe,zip,rar,pdf",0))
            return FALSE;
    }

    return TRUE;
}

int CNetSpider::ClearAll()
{
    this->stk_new->Clear();
    this->stk_used->Clear();

    return OK;
}

int CNetSpider::Stop()
{
    this->running = 0;
    this->http->CancelIo();
    return OK;
}
