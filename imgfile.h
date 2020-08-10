// ImgFile.h: interface for the CImgFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGFILE_H__C159D0E5_3403_4920_A835_6367EEB9C01D__INCLUDED_)
#define AFX_IMGFILE_H__C159D0E5_3403_4920_A835_6367EEB9C01D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "headerfile.h"
#include "vmem.h"

typedef struct 
{
    long header_size;
    long data_start,data_end,data_size,data_off;
    long const_start,const_end,const_size,const_off;
    long bss_start,bss_end,bss_size,bss_off;
    long text_start,text_end,text_size,text_off;
    long xcu_start,xcu_end,xcu_size,xcu_off;
}_ImgInfo;

class CImgFile{
public:
    CFileBase *i_file;
    _ImgInfo img_info;
public:
    int WriteToVMem(CVMem *vm);
    int LoadFile(CFileBase *pfile);
    CImgFile();
    ~CImgFile();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_IMGFILE_H__C159D0E5_3403_4920_A835_6367EEB9C01D__INCLUDED_)
