// DanciStk.cpp: implementation of the DanciStk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DanciStk.h"
#include "indexfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDanciStk::CDanciStk()
{
    this->InitBasic();  
}
CDanciStk::~CDanciStk()
{
    this->Destroy();
}
int CDanciStk::InitBasic()
{
    this->index = NULL;
    this->top = 0;;
    this->size = 0;
    return OK;
}
int CDanciStk::Init(long init_size)
{
    int i;

    this->InitBasic();  
    this->size = init_size;
    MALLOC(this->index,CDanci * ,this->size);
    for(i = 0; i < this->size; i++)
        this->index[i] = NULL;
    return OK;
}
int CDanciStk::Destroy()
{
    int i;
    if(this->index == NULL)
        return ERROR;
    for(i = 0; i < this->size; i++)
    {
        if(this->index[i])
        {
            this->index[i]->Destroy();
            DEL(this->index[i]);
        }
    }
    FREE(this->index);
    this->InitBasic();    
    return OK;
}
int CDanciStk::IsEmpty()
{
    if(this->top <= 0)
        return TRUE;
    else
        return FALSE;
}
int CDanciStk::IsFull()
{
    if(this->top >= this->size)
        return TRUE;
    return FALSE;
}
int CDanciStk::Push(CDanci *node)
{
    CDanci *p;
    p = this->PushEmpty();
    if(p == NULL)
        return ERROR;
    p->Copy(node);
    return OK;
}
int CDanciStk::Pop(CDanci *node)
{
    if(this->IsEmpty())
        return ERROR;
    this->top--;
    node->Copy(this->index[this->top]);
    this->index[this->top]->Destroy();
    DEL(this->index[this->top]);
    return OK;
}
int CDanciStk::Print()
{
    int i;
    for(i = 0; i <this->top; i++)
    {
        this->index[i]->Print();
        LOG(",");
    }
    LOG("\n");
    LOG("size=%ld\n",this->size);
    LOG("top=%ld\n",this->top);
    LOG("bottom=%ld\n",0);
    return OK;
}
long CDanciStk::GetLen()
{
    return this->top;
}
long CDanciStk::Search_Pos(CDanci *node)
{
    int i;
    for(i=0;i<this->top;i++)
    {
        if(this->index[i]->Comp(node) == 0)
            return i;
    }
    return -1;
}

CDanci * CDanciStk::Search(CDanci *node)
{
    long pos = this->Search_Pos(node);
    if(pos >= 0 && pos < this->top)
        return this->index[pos];;
    return NULL;
}
CDanci * CDanciStk::GetTop()
{
    if(this->IsEmpty())
        return NULL;
    return this->index[this->top - 1];
}
int CDanciStk::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->top--;
    this->index[this->top]->Destroy();
    DEL(this->index[this->top]);
    return OK;
}
int CDanciStk::Clear()
{
    while(this->DelTop());
    return OK;
}
CDanci *CDanciStk::PushEmpty()
{
    if(this->IsFull())
    {
        REALLOC(this->index,CDanci *,this->size * 2);
        this->size *= 2;
        for(int i = this->top; i < this->size; i++)
        {
            this->index[i] = 0;
        }
    }
    NEW(this->index[this->top] , CDanci );
    this->index[this->top]->Init();
    this->top++;
    return this->index[this->top - 1];
}
CDanci * CDanciStk::GetElem(long index)
{
    if(index < 0 || index >= this->top)
        return NULL;
    return this->index[index];
}
int CDanciStk::Sort(int order)
{
    long i,j,c;    
    CDanci *pt;
    for(j=this->GetLen(); j > 0; j--)
    {
        for(i = 0; i < j - 1; i++)
        {
            if(order==0)
                c = index[i]->Comp(index[i+1]) > 0;
            else
                c = index[i]->Comp(index[i+1]) < 0;
            if(c)
            {
                pt = index[i];
                index[i] = index[i + 1];
                index[i + 1] = pt;                
            }
        }
    }
    return OK;
}
CDanci * CDanciStk::BSearch_CDanci(CDanci *node,int order)
{
    long i;
    i = this->BSearch(node,order);
    return this->GetElem(i);
}
long CDanciStk::BSearch(CDanci *node,int order)
{
    int find,pos;
    pos = this->BSearch_Pos(node,order,&find);
    if(find) return pos;
    return -1;
}
int CDanciStk::InsertElem(long i, CDanci *node)
{
    //insert a node at pos i
    CDanci *new_node;
    ASSERT(i >= 0 && i <= this->top);
    //Add a new node
    new_node = this->PushEmpty(); 
    ASSERT(new_node);
    //copy value
    new_node->Copy(node);
    for(int k = this->top - 1; k >= i; k--)
    {
        this->index[k] = this->index[k - 1];
    }
    this->index[i] = new_node;
    return OK;
}
int CDanciStk::DelElem(long i)
{
    CDanci *p;
    ASSERT(i >= 0 && i < this->top);
    p = this->GetElem(i);
    DEL(p);
    for(int k = i; k < this->top; k++)
    {
        this->index[k] = this->index[k + 1];
    }
    this->top --;
    this->index[top] = NULL;
    return OK;
}
int CDanciStk::InsOrdered(CDanci *node, int order,int unique)
{
    int pos,find;
    pos = this->BSearch_Pos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElem(pos,node);
}
long CDanciStk::BSearch_Pos(CDanci *node, int order, int *find_flag)
{
    long low,high,mid,c;
    ASSERT(order == 0 || order == 1);
    low = 0; high=this->GetLen() - 1;
    while(low<=high)
    {
        mid = (low + high) >> 1;
        if(node->Comp(this->index[mid]) == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
        if(order == 0)
            c = this->index[mid]->Comp(node) > 0;
        else
            c = this->index[mid]->Comp(node) < 0;
        if(c)
            high = mid-1;
        else
            low = mid+1;
    }
    *find_flag = FALSE;
    return low;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int CDanciStk::BSearch_Pos(char *word, int order, int *find_flag)
{
    CDanci d;

    d.Init();
    d.SetWord(word);

    return this->BSearch_Pos(&d,order,find_flag);
}

int CDanciStk::WriteToFile(CFileBase *file)
{
    ASSERT(file);

    CDanci *pd;
    int i;
    CMem mem;

    LOCAL_MEM(mem);

    for(i = 0; i < this->GetLen(); i++)
    {
        pd = this->GetElem(i);
        ASSERT(pd);     
        pd->DumpToFile(file);
    }

    return OK;
}

int CDanciStk::WriteToFile(char *fn)
{
    CFile file;

    file.Init();
    file.OpenFile(fn,"wb+");

    return this->WriteToFile(&file);
}

int CDanciStk::MkIndexFile(CFileBase *file,char *dict_name,int type)
{
    ASSERT(file);

    CIndexFile idx;
    int param[5];

    idx.Init();

    param[0] = 0; //index
    param[1] = (int)this;
    
    idx.header_size = 128;
    idx.call_back_param = param;
    idx.call_back_read_block = this->call_back_mk_index_file;
    
    idx.CreateIndexFile(file);

    file->Seek(8);
    file->Write(&type,4);   //write type

    file->Seek(32);
    file->Puts(dict_name);
    file->Putc(0);

    return OK;
}

int CDanciStk::call_back_mk_index_file(int *param, CFileBase *file)
{
    ASSERT(file);
    
    CDanciStk *pstk;
    CDanci *pd;
    int old;

    old = file->GetOffset();

    pstk = (CDanciStk*)param[1];
    pd = pstk->GetElem(param[0]++);
    
    if(pd == NULL)
        return 0;
    
    pd->DumpToFile(file);

    return file->GetOffset() - old;
}

int CDanciStk::LoadDa3File(CFileBase *file_da3)
{
    CKingDa3File kf;
    int param[3];

    this->Clear();

    kf.Init();
    kf.LoadDa3File(file_da3);

    param[0] = 0; //index
    param[1] = (int)this;
    kf.GetAllDanci(param,call_back_get_danci);
    
    return OK;
}

int CDanciStk::call_back_get_danci(int *param, CDanci *d)
{
    CDanciStk *stk = (CDanciStk*)param[1];  
    ASSERT(stk);

    param[0] ++;
    if((param[0] & ((2<<10) - 1)) == 0)
    {
        LOG("%d\n",param[0]);
    }
    return stk->InsOrdered(d,0,1);
}

int CDanciStk::LoadDa3File(char *fn)
{
    CFile file;

    file.Init();
    file.SetBufSize(1024*32);
    ASSERT( file.OpenFile(fn,"rb") );
    return this->LoadDa3File(&file);
}

