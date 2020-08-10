#ifndef __FTPFILEINFOSTK_H
#define __FTPFILEINFOSTK_H

#include "ftpfileinfo.h"

class CFtpFileInfoStk{
public:
    CFtpFileInfo **mIndex;
    int32_t mTop;
    int32_t mSize;
    void *param_comp[MAX_CALLBACK_PARAMS];
    void *param_copy[MAX_CALLBACK_PARAMS];
    status_t (*callback_comp)(void **p);
    status_t (*callback_copy)(void **p);
public:     
    status_t Copy(CFtpFileInfoStk *stk);
    CFtpFileInfo * CloneNode(CFtpFileInfo *ftpfileinfo);
    static status_t def_comp(void**p);
    static status_t def_copy(void**p);
    CFtpFileInfo * RemoveElem(int32_t index);
    status_t InsertElemPtr(int32_t i, CFtpFileInfo *ftpfileinfo);
    CFtpFileInfo * PopPtr();
    status_t AutoResize();
    status_t PushPtr(CFtpFileInfo *ftpfileinfo);
    status_t InitBasic();  
    int32_t BSearch_Pos(CFtpFileInfo *ftpfileinfo,int32_t order,int32_t *find_flag);
    status_t InsOrderedPtr(CFtpFileInfo *ftpfileinfo,int32_t order,int32_t unique);
    status_t InsOrdered(CFtpFileInfo *ftpfileinfo,int32_t order,int32_t unique);
    status_t DelElem(int32_t i);
    status_t InsertElem(int32_t i,CFtpFileInfo *ftpfileinfo);
    int32_t BSearch(CFtpFileInfo *ftpfileinfo,int32_t order);
    CFtpFileInfo * BSearch_Node(CFtpFileInfo *ftpfileinfo,int32_t order);
    status_t Sort(int32_t order);
    CFtpFileInfo * GetElem(int32_t index);
    CFtpFileInfo * GetTopPtr();
    CFtpFileInfo * Search(CFtpFileInfo *ftpfileinfo);
    int32_t Search_Pos(CFtpFileInfo *ftpfileinfo);
    CFtpFileInfoStk();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int32_t init_size);
    status_t IsEmpty();
    status_t IsFull();
    status_t Pop(CFtpFileInfo *ftpfileinfo);
    status_t Print();
    status_t Push(CFtpFileInfo *ftpfileinfo);
    int32_t GetLen();
    ~CFtpFileInfoStk();
};

#endif
