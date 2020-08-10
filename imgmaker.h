// ImgMaker.h: interface for the CImgMaker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGMAKER_H__6F591933_EDA1_4EED_8ACD_39A957B566C1__INCLUDED_)
#define AFX_IMGMAKER_H__6F591933_EDA1_4EED_8ACD_39A957B566C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xml.h"
#include "objbox.h"
#include "diskmaker.h"
#include "memstk.h"
#include "project.h"
#include "wndlog.h"

class CImgMaker{
public: 
    CXml *i_xml;
    CObjBox *i_objbox;
    CDiskMaker *disk_maker;
    CMemStk *asm_cache;
    CWndLog *i_wnd_log;
    CFileBase *i_file_img;
    int ret_error;
public:
    int MakeUserFile(CFileBase *file);
    int LoadFiles(CXmlNode *px);
    int LoadAllFiles(CFileBase *file_disk);
    CImgMaker();
    ~CImgMaker();
    int Init();
    int Destroy();
    int InitBasic();
    int MainLoop();
};

#endif // !defined(AFX_IMGMAKER_H__6F591933_EDA1_4EED_8ACD_39A957B566C1__INCLUDED_)
