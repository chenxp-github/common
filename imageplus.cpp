// ImagePlus.cpp: implementation of the CImagePlus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "imageplus.h"
#include "mem_tool.h"
#include "syslog.h"
#include "dirmgr.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImagePlus::CImagePlus()
{
    this->InitBasic();
}
CImagePlus::~CImagePlus()
{
    this->Destroy();
}
status_t CImagePlus::InitBasic()
{
    //add your code
    return OK;
}
status_t CImagePlus::Init()
{
    this->InitBasic();
    this->mImg = NULL;
    return OK;
}
status_t CImagePlus::Destroy()
{
    //mImg is not alloced by new
    if(this->mImg)
    {
        delete this->mImg;
    }
    this->InitBasic();
    return OK;
}
status_t CImagePlus::Copy(CImagePlus *p)
{
    if(this == p)return OK;
    ASSERT(0);
    return OK;
}
status_t CImagePlus::Comp(CImagePlus *p)
{
    ASSERT(0);
    return 0;
}
status_t CImagePlus::Print()
{
    //add your code
    return TRUE;
}

status_t CImagePlus::LoadFromFileW(const wchar_t *filename)
{
    ASSERT(filename);
    
    this->Destroy();
    this->Init();
    ASSERT(CDirMgr::IsFileExistW(filename));
    this->mImg = Image::FromFile(filename);
    return this->mImg != NULL;
}

status_t CImagePlus::LoadFromStream(IStream *stream)
{
    ASSERT(stream);
    this->Destroy();
    this->Init();
    this->mImg = Image::FromStream(stream);
    return this->mImg != NULL;
}

status_t CImagePlus::SaveImage(CMemStream *stream,const wchar_t *type)
{
    ASSERT(stream && type);
    
    CMem mem;
    LOCAL_MEM(mem);

    mem.PutsW(L"image/");
    mem.PutsW(type);
    mem.PutcW(0);

    CLSID clImageClsid;
    GetImageCLSID(mem.GetPW(), &clImageClsid);
    
    this->mImg->Save(stream->GetStream(),&clImageClsid);
    stream->ReloadSize();

    return OK;

}

status_t CImagePlus::GetImageCLSID(const wchar_t *format, CLSID *pCLSID)
{
    UINT   num   =   0;   
    UINT   size   =   0;   
    ImageCodecInfo*   pImageCodecInfo   =   NULL;   
    GetImageEncodersSize(&num,   &size);   
    if(size   ==   0)   
        return   FALSE;
    
    pImageCodecInfo   =   (ImageCodecInfo*)(malloc(size));   
    if(pImageCodecInfo   ==   NULL)   
        return   FALSE;

    GetImageEncoders(num,   size,   pImageCodecInfo);   

    for(UINT   i   =   0;   i   <   num;   ++i)   
    {
        if(   wcscmp(pImageCodecInfo[i].MimeType,   format)   ==   0   )   
        {   
            *pCLSID   =   pImageCodecInfo[i].Clsid;   
            free(pImageCodecInfo);   
            return   TRUE;   
        }   
    }   
    free(pImageCodecInfo);   
    return   FALSE;   
}   

int32_t CImagePlus::GetWidth()
{
    return this->mImg->GetWidth();
}

int32_t CImagePlus::GetHeight()
{
    return this->mImg->GetHeight();
}

status_t CImagePlus::SaveImage(Image *img, const wchar_t *filename, const wchar_t *type)
{
    ASSERT(img && filename && type);
    
    CMemStream stream;
    SaveImage(img,&stream,type);
    stream.WriteToFileW(filename);  
    return OK;
}

status_t CImagePlus::SaveImage(Image *img, CMemStream *stream, const wchar_t *type)
{
    ASSERT(img && stream && type);
    
    CMem mem;
    LOCAL_MEM(mem);

    mem.PutsW(L"image/");
    mem.PutsW(type);
    mem.PutcW(0);

    CLSID clImageClsid;
    GetImageCLSID(mem.GetPW(), &clImageClsid);
    
    stream->Destroy();
    stream->Init();
    stream->Malloc(0);

    img->Save(stream->GetStream(),&clImageClsid);
    stream->ReloadSize();
    return OK;
}

Image * CImagePlus::GetImage()
{
    return this->mImg;
}
