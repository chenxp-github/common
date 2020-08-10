// RawDotBmp.h: interface for the CRawDotBmp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAWDOTBMP_H__E775DDB7_4AC3_4486_97D6_27DA32F21C15__INCLUDED_)
#define AFX_RAWDOTBMP_H__E775DDB7_4AC3_4486_97D6_27DA32F21C15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "canvas.h"
#include "filebase.h"

class CRawDotBmp{
public:
    COLORREF *dot;
    int width,height;
public:
    int GreyValue(COLORREF color);
    int SetPixel(int i,int j,COLORREF color);
    int Save(CCanvas *can);
    int Load(CCanvas *can);
    CRawDotBmp();
    virtual ~CRawDotBmp();
    int Init();
    int SetSize(int width,int height);
    int Destroy();
    int Copy(CRawDotBmp *p);
    int Comp(CRawDotBmp *p);
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_RAWDOTBMP_H__E775DDB7_4AC3_4486_97D6_27DA32F21C15__INCLUDED_)
