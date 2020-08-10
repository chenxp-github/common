#include "uri.h"
#include "syslog.h"
#include "mem_tool.h"

CUri::CUri()
{
    this->InitBasic();
}
CUri::~CUri()
{
    this->Destroy();
}
status_t CUri::InitBasic()
{
    this->mScheme = NULL;
    this->mUserName = NULL;
    this->mPassword = NULL;
    this->mHostName = NULL;
    this->mPort = 80;
    this->mPath = NULL;
    this->mQuery = NULL;
    this->mFragment = NULL;
    return OK;
}
status_t CUri::Init()
{
    this->InitBasic();

    NEW(this->mScheme,CMem);
    this->mScheme->Init();

    NEW(this->mUserName,CMem);
    this->mUserName->Init();

    NEW(this->mPassword,CMem);
    this->mPassword->Init();

    NEW(this->mHostName,CMem);
    this->mHostName->Init();

    NEW(this->mPath,CMem);
    this->mPath->Init();

    NEW(this->mQuery,CMem);
    this->mQuery->Init();

    NEW(this->mFragment,CMem);
    this->mFragment->Init();

    return OK;
}
status_t CUri::Destroy()
{
    DEL(this->mScheme);
    DEL(this->mUserName);
    DEL(this->mPassword);
    DEL(this->mHostName);
    DEL(this->mPath);
    DEL(this->mQuery);
    DEL(this->mFragment);
    this->InitBasic();
    return OK;
}
status_t CUri::Copy(CUri *p)
{
    ASSERT(p && this != p);
    this->mScheme->Copy(p->mScheme);
    this->mUserName->Copy(p->mUserName);
    this->mPassword->Copy(p->mPassword);
    this->mHostName->Copy(p->mHostName);
    this->mPort = p->mPort;
    this->mPath->Copy(p->mPath);
    this->mQuery->Copy(p->mQuery);
    this->mFragment->Copy(p->mFragment);
    return OK;
}
int CUri::Comp(CUri *p)
{
    ASSERT(p);
    return 0;
}
status_t CUri::Print()
{
    syslog_printf("Scheme = %s",this->mScheme->CStr());
    syslog_printf("UserName = %s",this->mUserName->CStr());
    syslog_printf("Password = %s",this->mPassword->CStr());
    syslog_printf("HostName = %s",this->mHostName->CStr());
    syslog_printf("Port = %d",mPort);
    syslog_printf("Path = %s",this->mPath->CStr());
    syslog_printf("Query = %s",this->mQuery->CStr());
    syslog_printf("Fragment = %s",this->mFragment->CStr());
    return OK;
}
status_t CUri::Clear()
{
    this->Destroy();
    this->Init();
    return OK;
}
status_t CUri::SetScheme(CMem *scheme)
{
    ASSERT(scheme);
    this->mScheme->Copy(scheme);
    return OK;
}
status_t CUri::SetScheme(const char *scheme)
{
    ASSERT(scheme);
    CMem mem_scheme(scheme);
    return this->SetScheme(&mem_scheme);
}
status_t CUri::SetUserName(CMem *username)
{
    ASSERT(username);
    this->mUserName->Copy(username);
    return OK;
}
status_t CUri::SetUserName(const char *username)
{
    ASSERT(username);
    CMem mem_username(username);
    return this->SetUserName(&mem_username);
}
status_t CUri::SetPassword(CMem *password)
{
    ASSERT(password);
    this->mPassword->Copy(password);
    return OK;
}
status_t CUri::SetPassword(const char *password)
{
    ASSERT(password);
    CMem mem_password(password);
    return this->SetPassword(&mem_password);
}
status_t CUri::SetHostName(CMem *hostname)
{
    ASSERT(hostname);
    this->mHostName->Copy(hostname);
    return OK;
}
status_t CUri::SetHostName(const char *hostname)
{
    ASSERT(hostname);
    CMem mem_hostname(hostname);
    return this->SetHostName(&mem_hostname);
}
status_t CUri::SetPort(int port)
{
    this->mPort = port;
    return OK;
}
status_t CUri::SetPath(CMem *path)
{
    ASSERT(path);
    this->mPath->Copy(path);
    return OK;
}
status_t CUri::SetPath(const char *path)
{
    ASSERT(path);
    CMem mem_path(path);
    return this->SetPath(&mem_path);
}
status_t CUri::SetQuery(CMem *query)
{
    ASSERT(query);
    this->mQuery->Copy(query);
    return OK;
}
status_t CUri::SetQuery(const char *query)
{
    ASSERT(query);
    CMem mem_query(query);
    return this->SetQuery(&mem_query);
}
status_t CUri::SetFragment(CMem *fragment)
{
    ASSERT(fragment);
    this->mFragment->Copy(fragment);
    return OK;
}
status_t CUri::SetFragment(const char *fragment)
{
    ASSERT(fragment);
    CMem mem_fragment(fragment);
    return this->SetFragment(&mem_fragment);
}

CMem* CUri::GetScheme()
{
    return this->mScheme;
}
const char* CUri::GetSchemeStr()
{
    return this->mScheme->CStr();
}
CMem* CUri::GetUserName()
{
    return this->mUserName;
}
const char* CUri::GetUserNameStr()
{
    return this->mUserName->CStr();
}
CMem* CUri::GetPassword()
{
    return this->mPassword;
}
const char* CUri::GetPasswordStr()
{
    return this->mPassword->CStr();
}
CMem* CUri::GetHostName()
{
    return this->mHostName;
}
const char* CUri::GetHostNameStr()
{
    return this->mHostName->CStr();
}
CMem* CUri::GetPath()
{
    return this->mPath;
}
const char* CUri::GetPathStr()
{
    return this->mPath->CStr();
}
CMem* CUri::GetQuery()
{
    return this->mQuery;
}
const char* CUri::GetQueryStr()
{
    return this->mQuery->CStr();
}
CMem* CUri::GetFragment()
{
    return this->mFragment;
}
const char* CUri::GetFragmentStr()
{
    return this->mFragment->CStr();
}

static void skip_slash(CFileBase *file)
{
    char ch;
    while(!file->IsEnd())
    {
        ch = file->Getc();
        if(ch != '/')
        {
            file->SeekBack(1);
            break;
        }
    }
}

status_t CUri::ParseString(CFileBase *str)
{
    ASSERT(str);
    this->Clear();
    
    fsize_t old_offset;
    char ch;
    LOCAL_MEM(tmp);

    //read scheme
    str->Seek(0);
    bool hit_scheme = false;
    while(!str->IsEnd())
    {
        ch = str->Getc();
        if(ch == ':')
        {
            char ch1 = str->Getc();
            char ch2 = str->Getc();
            if(ch1 == '/' && ch2=='/')
            {
                hit_scheme = true;
                this->SetScheme(&tmp);
                skip_slash(str);
                break;
            }
        }
        tmp.Putc(ch);
    }

    if(!hit_scheme)str->Seek(0);
    old_offset = str->GetOffset();

    //read user name and password
    bool hit_colon = false;
    bool hit_at = false;
    tmp.SetSize(0);

    while(!str->IsEnd())
    {
        ch = str->Getc();
        
        if(ch == ':')
        {
            hit_colon = true;
            this->SetUserName(&tmp);
            tmp.SetSize(0);
            continue;
        }
        else if(ch == '@')
        {
            hit_at = true;
            if(hit_colon)
            {
                this->SetPassword(&tmp);
            }
            else
            {
                this->SetUserName(&tmp);
            }
            break;
        }
        else if(ch == '/')
        {
            break;
        }
        tmp.Putc(ch);
    }
    
    if(!hit_at)
    {
        str->Seek(old_offset);
        this->SetHostName("");
        this->SetPassword("");
    }

    //read host name and port
    hit_colon = false;
    bool hit_slash = false;
    tmp.SetSize(0);
    while(!str->IsEnd())
    {
        ch = str->Getc();
    
        if(ch == ':')
        {
            hit_colon = true;
            this->SetHostName(&tmp);
            tmp.SetSize(0);
            continue;
        }

        else if(ch == '/')
        {
            if(hit_colon)
            {
                this->SetPort(atoi(tmp.CStr()));
            }
            else
            {
                hit_slash = true;
                this->SetHostName(&tmp);
            }

            break;
        }

        tmp.Putc(ch);
    }

    if(!hit_slash)
    {
        if(hit_colon)
            this->SetPort(atoi(tmp.CStr()));
        else
            this->SetHostName(&tmp);
    }

    //read path
    bool hit_ask = false;
    bool hit_pound = false;

    tmp.SetSize(0);
    while(!str->IsEnd())
    {
        ch = str->Getc();

        if(ch == '?')
        {
            this->SetPath(&tmp);
            hit_ask = true;
            break;
        }
        else if(ch == '#')
        {
            this->SetPath(&tmp);
            hit_pound = true;
            break;
        }

        tmp.Putc(ch);
    }
    
    if(!hit_ask && !hit_pound)
    {
        this->SetPath(&tmp);
    }

    if(!hit_pound)
    {
        //read query
        tmp.SetSize(0);
        while(!str->IsEnd())
        {
            ch = str->Getc();
            if(ch == '#')
            {
                this->SetQuery(&tmp);
                hit_pound = true;
                break;
            }
            tmp.Putc(ch);
        }
        
        if(!hit_pound)
        {
            this->SetQuery(&tmp);
        }
    }

    //read fragment
    tmp.SetSize(0);
    while(!str->IsEnd())
    {
        ch = str->Getc();
        tmp.Putc(ch);
    }
    this->SetFragment(&tmp);
    return OK;
}

status_t CUri::ParseString(const char *str)
{
    CMem mem(str);
    return this->ParseString(&mem);
}

int CUri::GetPort()
{
    return this->mPort;
}

bool CUri::HasHostName()
{
    return this->mHostName->StrLen() > 0;
}
