// DanciDict.cpp: implementation of the CDanciDict class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DanciDict.h"
#include "indexfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDanciDict::CDanciDict()
{
    this->InitBasic();
}
CDanciDict::~CDanciDict()
{
    this->Destroy();
}
int CDanciDict::InitBasic()
{
    this->i_index_file = NULL;
    return OK;
}
int CDanciDict::Init()
{
    this->InitBasic();
    NEW(this->i_index_file,CIndexFile);
    this->i_index_file->Init();

    return OK;
}
int CDanciDict::Destroy()
{
    DEL(this->i_index_file);
    this->InitBasic();
    return OK;
}
int CDanciDict::Print()
{
    //add your code
    return TRUE;
}

int CDanciDict::LoadIndexFile(CFileBase *file)
{
    ASSERT(file);
    return this->i_index_file->LoadIndexFile(file);;
}

int CDanciDict::GetMaxDanci()
{
    return this->i_index_file->GetTotalBlocks();
}

int CDanciDict::GetDanci(int index, CDanci *danci)
{
    ASSERT(danci && this->i_index_file);
    
    CMemFile mf;
    CMem mem;

    danci->Clear();
    
    if(index < 0  || index >= this->GetMaxDanci())
        return ERROR;

    mf.Init();
    LOCAL_MEM(mem);
    
    this->i_index_file->GetBlock(index,&mf);
    mf.Seek(0);

    while(mf.ReadLine(&mem))
    {
        if(mem.p[0] == '@')
            danci->SetWord(mem.p + 1);
        else if(mem.p[0] == '#')
            danci->SetYinBiao(mem.p + 1);
        else if(mem.p[0] == '$')
            danci->AddHanyu(mem.p + 1);
    }

    return OK;
}
int CDanciDict::BSearch_Pos(CDanci *node, int order, int *find_flag)
{
    ASSERT(node);

    CMem mem;
    LOCAL_MEM(mem);
    mem.WriteFile(node->word);
    
    return this->BSearch_Pos(mem.p,order,find_flag);
}
int CDanciDict::BSearch_Pos(char *str, int order, int *find_flag)
{
    long low,high,mid,c;
    CDanci dmid;

    ASSERT(order == 0 || order == 1);
        
    low = 0; high=this->GetMaxDanci() - 1;

    dmid.Init();

    while(low <= high)
    {
        mid = (low+high) >> 1;
        this->GetDanci(mid,&dmid);
            
        if(dmid.Comp(str) == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
        if(order == 0)
            c = dmid.Comp(str) > 0;
        else
            c = dmid.Comp(str) < 0;
        if(c)
            high = mid-1;
        else
            low = mid+1;
    }
    
    *find_flag = FALSE;

    return low;
}

int CDanciDict::GetDictName(CFileBase *file)
{
    ASSERT(file && this->i_index_file);

    CMem mem;
    char ch;

    LOCAL_MEM(mem);
    this->i_index_file->GetHeader(&mem);

    mem.Seek(32);
    file->SetSize(0);
    while(!mem.IsEnd())
    {
        ch = mem.Getc();
        if(ch == 0)
            break;
        else
            file->Putc(ch);
    }

    file->Putc(0);

    return OK;
}

int CDanciDict::GetDictType()
{
    ASSERT(this->i_index_file);

    CMem mem;
    int type = 0;

    LOCAL_MEM(mem);
    this->i_index_file->GetHeader(&mem);

    mem.Seek(8);
    mem.Read(&type,4);

    return type;
}
