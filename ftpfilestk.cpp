

#include "ftpfileinfostk.h"
#include "mem_tool.h"
#include "syslog.h"

CFtpFileInfoStk::CFtpFileInfoStk()
{
    this->InitBasic();  
}
CFtpFileInfoStk::~CFtpFileInfoStk()
{
    this->Destroy();
}
status_t CFtpFileInfoStk::InitBasic()
{
    this->mIndex = NULL;
    this->mTop = 0;;
    this->mSize = 0;    
    this->callback_comp = def_comp;
    this->callback_copy = def_copy; 
    crt_memset(param_comp,0,sizeof(param_comp));
    crt_memset(param_copy,0,sizeof(param_copy));
    return OK;
}
status_t CFtpFileInfoStk::Init(int32_t init_size)
{
    int32_t i;
    this->InitBasic();  
    this->mSize = init_size;
    MALLOC(this->mIndex,CFtpFileInfo * ,this->mSize);
    for(i = 0; i < this->mSize; i++)
        this->mIndex[i] = NULL;
    return OK;
}
status_t CFtpFileInfoStk::Destroy()
{
    int32_t i;
    if(this->mIndex == NULL)
        return ERROR;
    for(i = 0; i < this->mSize; i++)
    {
        DEL(this->mIndex[i]);
    }
    FREE(this->mIndex);
    this->InitBasic();  
    return OK;
}
status_t CFtpFileInfoStk::IsEmpty()
{
    return this->mTop <= 0;
}
status_t CFtpFileInfoStk::IsFull()
{
    return this->mTop >= this->mSize;
}

status_t CFtpFileInfoStk::AutoResize()
{
    if(this->IsFull())
    {
        REALLOC(this->mIndex,CFtpFileInfo *,this->mSize,this->mSize * 2);
        this->mSize *= 2;
        for(int32_t i = this->mTop; i < this->mSize; i++)
        {
            this->mIndex[i] = NULL;
        }
    }

    return OK;
}

status_t CFtpFileInfoStk::Push(CFtpFileInfo *ftpfileinfo)
{
    ASSERT(ftpfileinfo && this->callback_copy);
    return this->PushPtr(this->CloneNode(ftpfileinfo));
}

status_t CFtpFileInfoStk::PushPtr(CFtpFileInfo *ftpfileinfo)
{
    ASSERT(ftpfileinfo);    
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mIndex[this->mTop] = ftpfileinfo;
    this->mTop++;
    return OK;
}

CFtpFileInfo * CFtpFileInfoStk::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->mTop--;
    return this->mIndex[this->mTop];
}

status_t CFtpFileInfoStk::Pop(CFtpFileInfo *ftpfileinfo)
{
    ASSERT(!this->IsEmpty() && this->callback_copy);    
    this->mTop--;
    param_copy[0] = ftpfileinfo;
    param_copy[1] = this->mIndex[this->mTop];
    this->callback_copy(param_copy);
    DEL(this->mIndex[this->mTop]);
    return OK;
}
status_t CFtpFileInfoStk::Print()
{
    int32_t i;
    for(i = 0; i <this->mTop; i++)
    {
        this->mIndex[i]->Print();
        LOG(",");
    }
    LOG("\n");
    LOG("mSize=%ld\n",this->mSize);
    LOG("mTop=%ld\n",this->mTop);
    LOG("bottom=%ld\n",0);
    return OK;
}
int32_t CFtpFileInfoStk::GetLen()
{
    return this->mTop;
}

int32_t CFtpFileInfoStk::Search_Pos(CFtpFileInfo *ftpfileinfo)
{
    int32_t i;
    ASSERT(this->callback_comp);    
    for(i=0;i<this->mTop;i++)
    {
        param_comp[0] = this->mIndex[i];
        param_comp[1] = ftpfileinfo;
        if(this->callback_comp(param_comp) == 0)
            return i;
    }
    return -1;
}

CFtpFileInfo * CFtpFileInfoStk::Search(CFtpFileInfo *ftpfileinfo)
{
    int32_t pos = this->Search_Pos(ftpfileinfo);
    if(pos >= 0 && pos < this->mTop)
        return this->mIndex[pos];;
    return NULL;
}
CFtpFileInfo * CFtpFileInfoStk::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->mIndex[this->mTop - 1];
}
status_t CFtpFileInfoStk::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->mTop--;
    DEL(this->mIndex[this->mTop]);
    return OK;
}
status_t CFtpFileInfoStk::Clear()
{
    while(this->DelTop());
    return OK;
}

CFtpFileInfo * CFtpFileInfoStk::GetElem(int32_t mIndex)
{
    if(mIndex < 0 || mIndex >= this->mTop)
        return NULL;
    return this->mIndex[mIndex];
}

CFtpFileInfo * CFtpFileInfoStk::BSearch_Node(CFtpFileInfo *ftpfileinfo,int32_t order)
{
    int32_t i;
    i = this->BSearch(ftpfileinfo,order);
    return this->GetElem(i);
}
int32_t CFtpFileInfoStk::BSearch(CFtpFileInfo *ftpfileinfo,int32_t order)
{
    int32_t find,pos;
    pos = this->BSearch_Pos(ftpfileinfo,order,&find);
    if(find) return pos;
    return -1;
}

status_t CFtpFileInfoStk::InsertElemPtr(int32_t i, CFtpFileInfo *ftpfileinfo)
{
    ASSERT(ftpfileinfo);
    ASSERT(i >= 0 && i <= this->mTop);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(int32_t k = this->mTop; k > i; k--)
    {
        this->mIndex[k] = this->mIndex[k - 1];
    }
    this->mIndex[i] = ftpfileinfo;
    this->mTop++;
    return OK;
}

status_t CFtpFileInfoStk::InsertElem(int32_t i, CFtpFileInfo *ftpfileinfo)
{
    ASSERT(ftpfileinfo);
    return this->InsertElemPtr(i,this->CloneNode(ftpfileinfo));
}

CFtpFileInfo * CFtpFileInfoStk::RemoveElem(int32_t index)
{
    CFtpFileInfo *p;
    
    ASSERT(index >= 0 && index < this->mTop);
    p = this->GetElem(index);

    for(int32_t k = index; k < this->mTop-1; k++)
    {
        this->mIndex[k] = this->mIndex[k + 1];
    }
    this->mTop --;
    this->mIndex[mTop] = NULL;
    return p;
}

status_t CFtpFileInfoStk::DelElem(int32_t i)
{
    CFtpFileInfo *p = this->RemoveElem(i);
    if(p != NULL)
    {
        DEL(p);
        return OK;
    }
    return ERROR;
}

status_t CFtpFileInfoStk::InsOrderedPtr(CFtpFileInfo *ftpfileinfo, int32_t order,int32_t unique)
{
    int32_t pos,find;
    pos = this->BSearch_Pos(ftpfileinfo,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElemPtr(pos,ftpfileinfo);
}

status_t CFtpFileInfoStk::InsOrdered(CFtpFileInfo *ftpfileinfo, int32_t order,int32_t unique)
{
    ASSERT(ftpfileinfo);
    int32_t pos,find;
    pos = this->BSearch_Pos(ftpfileinfo,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElem(pos,ftpfileinfo);
}

int32_t CFtpFileInfoStk::BSearch_Pos(CFtpFileInfo *ftpfileinfo, int32_t order, int32_t *find_flag)
{
    int32_t low,high,mid,c;

    low = 0; high=this->GetLen() - 1;
    ASSERT(this->callback_comp);    

    while(low<=high)
    {
        mid = (low+high) >> 1;

        param_comp[0] = this->mIndex[mid];
        param_comp[1] = ftpfileinfo;

        if(this->callback_comp(param_comp) == 0)
        {
            *find_flag = TRUE;
            return mid;
        }

        param_comp[0] = this->mIndex[mid];
        param_comp[1] = ftpfileinfo;
        
        if(order == 0)
            c = this->callback_comp(param_comp) > 0;
        else
            c = this->callback_comp(param_comp) < 0;

        if(c)
            high = mid-1;
        else
            low = mid+1;
    }
    *find_flag = FALSE;
    return low;
}

status_t CFtpFileInfoStk::Sort(int32_t order)
{
    int32_t i,len;
    CFtpFileInfoStk tmp;

    len = this->GetLen();
    tmp.Init(len);

    for(i = 0; i < len; i++)
    {
        tmp.PushPtr(this->GetElem(i));
    }

    this->mTop = 0;
    for(i = 0; i < len; i++)
    {
        this->InsOrderedPtr(tmp.PopPtr(),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.mIndex[i] = NULL;
    }

    return OK;
}
CFtpFileInfo * CFtpFileInfoStk::CloneNode(CFtpFileInfo *ftpfileinfo)
{
    param_copy[0] = NULL;
    param_copy[1] = ftpfileinfo;
    this->callback_copy(param_copy);
    CFtpFileInfo *new_ftpfileinfo = (CFtpFileInfo*)param_copy[0];
    ASSERT(new_ftpfileinfo);
    return new_ftpfileinfo;
}
status_t CFtpFileInfoStk::Copy(CFtpFileInfoStk *stk)
{
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(int32_t i = 0; i < stk->GetLen(); i++)
    {
        CFtpFileInfo *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CFtpFileInfoStk::def_comp(void **p)
{
    ASSERT(p);
    ASSERT(p[0] && p[1]);

    CFtpFileInfo *ftpfileinfo0 = (CFtpFileInfo *)p[0];
    CFtpFileInfo *ftpfileinfo1 = (CFtpFileInfo *)p[1];
    return ftpfileinfo0->Comp(ftpfileinfo1);
}

status_t CFtpFileInfoStk::def_copy(void **p)
{
    ASSERT(p);
    ASSERT(p[1]);

    CFtpFileInfo *dst = (CFtpFileInfo *)p[0];
    CFtpFileInfo *src = (CFtpFileInfo *)p[1];

    if(dst == NULL)
    {
        NEW(dst,CFtpFileInfo);
        dst->Init();
        p[0] = dst;
    }
    
    return dst->Copy(src);
}

