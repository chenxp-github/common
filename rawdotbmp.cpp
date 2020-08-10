// RawDotBmp.cpp: implementation of the CRawDotBmp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RawDotBmp.h"
#include "mem_tool.h"
#include "canvas.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRawDotBmp::CRawDotBmp()
{
    this->InitBasic();
}
CRawDotBmp::~CRawDotBmp()
{
    this->Destroy();
}
int CRawDotBmp::InitBasic()
{
    this->width = 0;
    this->height = 0;
    this->dot = NULL;

    return OK;
}
int CRawDotBmp::Init()
{
    this->InitBasic();
    return OK;
}
int CRawDotBmp::SetSize(int width,int height)
{
    ASSERT(dot == NULL);
    
    this->width = width;
    this->height = height;

    MALLOC(this->dot,COLORREF,width * height);

    return OK;
}
int CRawDotBmp::Destroy()
{
    FREE(this->dot);
    this->InitBasic();
    return OK;
}
int CRawDotBmp::Copy(CRawDotBmp *p)
{
    if(this == p)
        return OK;
    return OK;
}
int CRawDotBmp::Comp(CRawDotBmp *p)
{
    return 0;
}
int CRawDotBmp::Print()
{
    return TRUE;
}

int CRawDotBmp::Load(CCanvas *can)
{
    ASSERT(can);
    
    int i,j,k = 0;

    this->SetSize(can->width,can->height);
    
    for(i = 0; i < can->height; i++)
    {
        for(j = 0; j < can->width; j++)
        {
            dot[k++] = can->GetPixel(i,j);
        }
    }

    return OK;
}

int CRawDotBmp::Save(CCanvas *can)
{
    ASSERT(can);

    int i,j,k = 0;

    for(i = 0; i < this->height; i++)
    {
        for(j = 0; j < this->width; j++)
        {
            can->SetPixel(i,j,dot[k++]);
        }
    }

    return OK;
}

int CRawDotBmp::SetPixel(int i, int j, COLORREF color)
{
    if(i < 0 || i >= this->height)
        return ERROR;
    if(j < 0 || j >= this->width)
        return ERROR;

    this->dot[i * this->width + j] = color;
    
    return OK;
}

int CRawDotBmp::GreyValue(COLORREF color)
{
    int r,g,b,grey;
    
    r = GetRValue(color);
    g = GetGValue(color);
    b = GetBValue(color);
    
    grey = (r*3+g*6+b)/10;
    
    return grey;
}

