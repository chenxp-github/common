// GdsFile.h: interface for the CGdsFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DANCI_H__9697FF59_AE37_4793_9D7D_75B0312BFB52__INCLUDED_)
#define AFX_DANCI_H__9697FF59_AE37_4793_9D7D_75B0312BFB52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"
#include "memfile.h"
#include "mem.h"
#include "file.h"

class CDanci{
public:
    CMemFile *word;
    CMemFile *yinbiao;
    CMemFile *hanyu;
public:
    int SetYinBiao(char *str);
    int AddHanyu(char *str);
    int DumpToFile(CFileBase *file);
    int AddHanyu(CFileBase *file);
    int SetWord(char *str);
    int Comp(char *str);
    int IsEmpty();
    int SetWord(CFileBase *file);
    int SetYinBiao(CFileBase *file);
    int SetHanYu(CFileBase *file);
    int Clear();
    CDanci();
    virtual ~CDanci();
    int Init();
    int Destroy();
    int Copy(CDanci *p);
    int Comp(CDanci *p);
    int Print();
    int InitBasic();
};

class CGdsFile{
public:
    int max_danci;
    CMemFile *mf_gds;
public:
    int GetDanci(int index,CDanci *danci);
    int LoadGdsFile(char *fn);
    int GetName(CFileBase *file);
    CGdsFile();
    virtual ~CGdsFile();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

//��ksdrip.exe�ѽ�ɽ�ʰԵ�dic�ļ�ת���õ���da3�ļ��Ķ�ȡ
class CKingDa3File{
public:
    CFileBase *file_da3;
public:
    int GetAllDanci(int *param,int (*get_danci)(int *param,CDanci *danci));
    int KingYinBiaoToBDC(CFileBase *yinbiao);
    int LoadDa3File(CFileBase *file);
    CKingDa3File();
    virtual ~CKingDa3File();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_GDSFILE_H__9697FF59_AE37_4793_9D7D_75B0312BFB52__INCLUDED_)
