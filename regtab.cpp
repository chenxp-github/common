// RegTab.cpp: implementation of the CRegTab class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegTab.h"
#include "enums.h"

typedef struct{
    char *name;
    int bytes;
    int index;
}_Reg;

static const _Reg _regs[] = {
{"eax",4,REG_EAX},
{"ebx",4,REG_EBX},
{"ecx",4,REG_ECX},
{"edx",4,REG_EDX},
{"eex",4,REG_EEX},
{"esi",4,REG_ESI},
{"esp",4,REG_ESP},
{"edi",4,REG_EDI},
{"ebp",4,REG_EBP},
{"eip",4,REG_EIP},
{"al",1,REG_AL},
{"bl",1,REG_BL},
{"cl",1,REG_CL},
{"dl",1,REG_DL},
{"el",1,REG_EL},
{"ah",1,REG_AH},
{"bh",1,REG_BH},
{"ch",1,REG_CH},
{"dh",1,REG_DH},
{"eh",1,REG_EH},
{"ax",2,REG_AX},
{"bx",2,REG_BX},
{"cx",2,REG_CX},
{"dx",2,REG_DX},
{"ex",2,REG_EX},
{"di",2,REG_DI},
{"si",2,REG_SI},
{"st(0)",8,REG_ST0},
{"st(1)",8,REG_ST1},
{"st(2)",8,REG_ST2},
{"st(3)",8,REG_ST3},
{"st(4)",8,REG_ST4},
{"st(5)",8,REG_ST5},
{"st(6)",8,REG_ST6},
{"st(7)",8,REG_ST7},
{"st(8)",8,REG_ST8},
{"st(9)",8,REG_ST9},
{"_R0",4,REG_R0},
{"_R1",4,REG_R1},
{"_R2",4,REG_R2},
{"_R3",4,REG_R3},
{"_R4",4,REG_R4},
{"_R5",4,REG_R5},
{"_R6",4,REG_R6},
{"_R7",4,REG_R7},
{"_R8",4,REG_R8},
{"_R9",4,REG_R9},
{"_R10",4,REG_R10},
{"flag",4,REG_FLAG},
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegTab::CRegTab()
{
    this->index = NULL;
    this->top = 0;
}

CRegTab::~CRegTab()
{
    this->Destroy();
}

int CRegTab::Init(long init_size)
{
    int i;

    this->top = 0;
    this->size = init_size;

    MALLOC(this->index,CReg * ,this->size);
    
    for(i = 0; i < this->size; i++)
        this->index[i] = NULL;
    
    return OK;
}

int CRegTab::Destroy()
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

    this->index=NULL;
    this->top=0;
    this->size=0;
    
    return OK;
}

int CRegTab::IsEmpty()
{
    if(this->top <= 0)
        return TRUE;
    else
        return FALSE;
}

int CRegTab::IsFull()
{
    if(this->top >= this->size)
        return TRUE;

    return FALSE;
}

int CRegTab::Push(CReg *node)
{
    CReg *p;

    p = this->PushEmpty();

    if(p == NULL)
        return ERROR;

    p->Copy(node);

    return OK;
}

int CRegTab::Pop(CReg *node)
{
    if(this->IsEmpty())
        return ERROR;

    this->top--;

    node->Copy(this->index[this->top]);
    
    this->index[this->top]->Destroy();

    DEL(this->index[this->top]);
    
    return OK;
}

int CRegTab::Print()
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

long CRegTab::GetLen()
{
    return this->top;
}

CReg * CRegTab::Search(CReg *node)
{
    int i;
    
    for(i=0;i<this->top;i++)
    {
        if(this->index[i]->Comp(node) == 0)
            return this->index[i];
    }

    return NULL;
}

CReg * CRegTab::GetTop()
{
    if(this->IsEmpty())
        return NULL;

    return this->index[this->top - 1];
}

int CRegTab::DelTop()
{
    if(this->IsEmpty())
        return ERROR;

    this->top--;

    this->index[this->top]->Destroy();
    
    DEL(this->index[this->top]);

    return OK;
}

int CRegTab::Clear()
{
    while(this->DelTop());
    return OK;
}
CReg *CRegTab::PushEmpty()
{
    if(this->IsFull())
    {
        REALLOC(this->index,CReg *,this->size * 2);

        this->size *= 2;

        for(int i = this->top; i < this->size; i++)
        {
            this->index[i] = 0;
        }
    }
    
    NEW(this->index[this->top] , CReg );
    this->index[this->top]->Init();
  
    this->top++;
    
    return this->index[this->top - 1];
}

CReg * CRegTab::GetElem(long index)
{
    if(index < 0 || index >= this->top)
        return NULL;

    return this->index[index];
}

int CRegTab::Sort(int order)
{

    long i,j,c;    

    CReg *pt;

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

CReg * CRegTab::BSearch_Node(CReg *node,int order)
{
    long i;

    i = this->BSearch(node,order);

    return this->GetElem(i);
}

long CRegTab::BSearch(CReg *node,int order)
{
    int find,pos;

    pos = this->BSearch_Pos(node,order,&find);

    if(find) return pos;

    return -1;
}
int CRegTab::InsertElem(long i, CReg *node)
{
    //insert a node at pos i
    CReg *new_node;

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
int CRegTab::DelElem(long i)
{
    CReg *p;
    
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

int CRegTab::InsOrdered(CReg *node, int order,int unique)
{
    int pos,find;

    pos = this->BSearch_Pos(node,order,&find);

    if(find && unique)
        return ERROR;

    return this->InsertElem(pos,node);
}

long CRegTab::BSearch_Pos(CReg *node, int order, int *find_flag)
{
    long low,high,mid,c;
    long flag = 0,pos = -1;

    ASSERT(order == 0 || order == 1);

    low = 0; high=this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high)/2;
        
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
/*=======================================================*/
/*=======================================================*/
int CRegTab::InitDefault()
{
    int i,max;
    CReg *p,*p1,*p2,*p3;
    
    max = sizeof(_regs) / sizeof(_Reg);
    this->Init(max + 1);

    for(i = 0; i <max; i++)
    {
        p = this->PushEmpty();  
        ASSERT(p);
        //copy value
        p->name = _regs[i].name;
        p->bytes = _regs[i].bytes;
        p->index = _regs[i].index;
        p->val.val_64 = 0;
    }
    
    this->Sort(0);
    
    char *ex[]  = {"eax","ebx","ecx","edx","eex","edi","esi"};
    char *x[]   = {"ax", "bx", "cx", "dx", "ex","di","si"};
    char *l[]   = {"al", "bl", "cl", "dl", "el","",""};
    char *h[]   = {"ah", "bh", "ch", "dh", "eh","",""};
    
    for(i = 0;i < sizeof(ex)/sizeof(*ex); i++)
    {
        p = this->BSearch(ex[i]);
        p1 = this->BSearch(x[i]);
        p2 = this->BSearch(l[i]);
        p3 = this->BSearch(h[i]);

        ASSERT(p);

        if(p1)
            p1->val.ptr_16 = (WORD*)(&p->val.val_32);
        if(p2)
            p2->val.ptr_8 = (BYTE*)(&p->val.val_32);
        if(p3 && p2)
            p3->val.ptr_8 = p2->val.ptr_8 + 1;
    }

    return OK;
}

int CRegTab::IsReg(char *str)
{
    CReg reg,*p;    

    reg.Init();
    reg.name = str;

    p = this->BSearch_Node(&reg,0);
    
    reg.Destroy();

    if(p)
        return TRUE;

    return FALSE;
}
//得到寄存器的编号，而不是在表中的顺序
int CRegTab::GetRegNum(char *reg_str)
{
    int ret = -1;
    CReg *preg;

    preg = this->BSearch(reg_str);
    
    if(preg)
    {
        ret = preg->index;
    }

    return ret;
}

CReg * CRegTab::BSearch(char *reg_name)
{
    CReg reg;

    reg.Init();
    reg.name = reg_name;

    return this->BSearch_Node(&reg,0);
}

int CRegTab::SetRegVal(char *str_reg, _RegVal val)
{
    CReg *pr;

    pr = this->BSearch(str_reg);

    ASSERT(pr);

    pr->SetVal(&val);

    return OK;
}

int CRegTab::SetRegVal(long index, _RegVal val)
{
    CReg *pr;

    pr = this->GetElem(index);
    ASSERT(pr);

    pr->SetVal(&val);

    return OK;
}

CReg * CRegTab::GetRegFromNum(int reg_num)
{
    CReg *p;

    for(int i = 0; i< this->GetLen(); i++)
    {
        p = this->GetElem(i);
        ASSERT(p);

        if(p->index == reg_num)
            return p;
    }

    return NULL;
}
