#ifndef __URI_H
#define __URI_H

#include "mem.h"

class CUri{
public:
    CMem *mScheme;
    CMem *mUserName;
    CMem *mPassword;
    CMem *mHostName;
    int mPort;
    CMem *mPath;
    CMem *mQuery;
    CMem *mFragment;
public:
    bool HasHostName();
    int GetPort();
    status_t ParseString(const char *str);
    status_t ParseString(CFileBase *str);
    status_t SetScheme(CMem *scheme);
    status_t SetScheme(const char *scheme);
    status_t SetUserName(CMem *username);
    status_t SetUserName(const char *username);
    status_t SetPassword(CMem *password);
    status_t SetPassword(const char *password);
    status_t SetHostName(CMem *hostname);
    status_t SetHostName(const char *hostname);
    status_t SetPort(int port);
    status_t SetPath(CMem *path);
    status_t SetPath(const char *path);
    status_t SetQuery(CMem *query);
    status_t SetQuery(const char *query);
    status_t SetFragment(CMem *fragment);
    status_t SetFragment(const char *fragment);
    CMem* GetScheme();
    const char* GetSchemeStr();
    CMem* GetUserName();
    const char* GetUserNameStr();
    CMem* GetPassword();
    const char* GetPasswordStr();
    CMem* GetHostName();
    const char* GetHostNameStr();
    CMem* GetPath();
    const char* GetPathStr();
    CMem* GetQuery();
    const char* GetQueryStr();
    CMem* GetFragment();
    const char* GetFragmentStr();
    CUri();
    virtual ~CUri();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CUri *p);
    int Comp(CUri *p);
    status_t Print();
    status_t Clear();
};

#endif


