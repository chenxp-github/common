// ImagePlus.h: interface for the CImagePlus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEPLUS_H__FA913468_5858_4945_8627_21EB967918D3__INCLUDED_)
#define AFX_IMAGEPLUS_H__FA913468_5858_4945_8627_21EB967918D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "mem.h"
#include "memstream.h"

class CImagePlus{
public:
    Image *mImg;
public:
    Image * GetImage();
    static status_t SaveImage(Image *img, CMemStream *stream, const wchar_t *type);
    static status_t SaveImage(Image *img, const wchar_t *filename,  const wchar_t *type);
    int32_t GetHeight();
    int32_t GetWidth();
    static status_t CImagePlus::GetImageCLSID(const wchar_t *format, CLSID *pCLSID);
    status_t SaveImage(CMemStream *stream,const wchar_t *type);
    status_t LoadFromStream(IStream *stream);
    status_t LoadFromFileW(const wchar_t *filename);
    CImagePlus();
    virtual ~CImagePlus();
    status_t Init();
    status_t Destroy();
    status_t Copy(CImagePlus *p);
    status_t Comp(CImagePlus *p);
    status_t Print();
    status_t InitBasic();
};

typedef CImagePlus CAniImage;

#endif // !defined(AFX_IMAGEPLUS_H__FA913468_5858_4945_8627_21EB967918D3__INCLUDED_)
