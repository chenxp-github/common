// HtmReader.h: interface for the CHtmReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMREADER_H__08C0A81C_A7C9_43E5_AD9D_DE091F4DD2D0__INCLUDED_)
#define AFX_HTMREADER_H__08C0A81C_A7C9_43E5_AD9D_DE091F4DD2D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "memfile.h"
#include "mem.h"
#include "memstk.h"
#include "common.h" // Added by ClassView

enum
{
    HTM_RET_ERR,
    HTM_RET_SINGLE,
    HTM_RET_ENTER,
    HTM_RET_LEAVE,
};

class CHtmReader{
public:
    enum
    {
        HTM_STATUS_ERR,
        HTM_STATUS_NORMAL,
        HTM_STATUS_COMMENT,
        HTM_STATUS_TAG,
        HTM_STATUS_VALUE,
    };
public:
    CFileBase *i_file_htm;
    CMemFile *mf_attrib;
    CMemFile *mf_value;
    CMemStk  *mem_stk;
    status_t status;
public:
    int32_t SearchWordList(CFileBase *file,const char *list, status_t case_sensitive);
    status_t ProcessStyle();
    static status_t Htm2Txt(CFileBase *file_htm,CFileBase *file_txt);
    static status_t GetAllLink(CFileBase *file_htm,CFileBase *file_links);
    CMem * GetStackTop();
    status_t ReadAttribStr(CFileBase *file);
    status_t ProcessComment();
    status_t ProcessScript();
    status_t DelStackTop();
    status_t Skip(const char *str);
    status_t SetHtmFile(CFileBase *file);
    status_t EnterNextTag();
    status_t IsFileEmpty(CFileBase *file);
    status_t GetCurPath(CFileBase *file);
    CHtmReader();
    virtual ~CHtmReader();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};

#endif // !defined(AFX_HTMREADER_H__08C0A81C_A7C9_43E5_AD9D_DE091F4DD2D0__INCLUDED_)
