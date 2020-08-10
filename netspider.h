// NetSpider.h: interface for the CNetSpider class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETSPIDER_H__381B6142_B286_455E_82BA_33FD56DBC490__INCLUDED_)
#define AFX_NETSPIDER_H__381B6142_B286_455E_82BA_33FD56DBC490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "httpclient.h"
#include "memstk.h"

class CNetSpider{
public:
    CMem *cur_path,*cur_root;
    CMemStk *stk_new;
    CMemStk *stk_used;
    CHttpClient *http;
    int running;
    int *param_get_file;
    int (*callback_get_file)(int *param,CMem *url,CFileBase *htm_file,int is_htm);
public:
    int Stop();
    int ClearAll();
    int IsHtml(CMem *url);
    int GetUrlRoot(CMem *url,CFileBase *root);
    int GetUrlPath(CMem *url,CFileBase *path);
    int AddUsed(CMem *url);
    int Run();
    int GetNewUrl(CFileBase *file);
    int AddHtml(CFileBase *file);
    int AddNew(char *url);
    int AddNew(CMem *url);
    int SetProxy(char *server,int port,char *proxy_auth);
    CNetSpider();
    virtual ~CNetSpider();
    int Init();
    int Destroy();
    int Copy(CNetSpider *p);
    int Comp(CNetSpider *p);
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_NETSPIDER_H__381B6142_B286_455E_82BA_33FD56DBC490__INCLUDED_)
