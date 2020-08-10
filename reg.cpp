// Reg.cpp: implementation of the CReg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReg::CReg()
{
    this->name = NULL;
    this->bytes = 0;
}
CReg::~CReg()
{
    Destroy();
}
int CReg::Init()
{
    this->name = NULL;
    this->bytes = 0;
    this->val.val_64 = 0;

    return OK;
}
int  CReg::Destroy()
{
    this->name = NULL;
    this->bytes = NULL;

    return OK;
}
int  CReg::Copy(CReg *data)
{
    this->name = data->name;
    this->bytes = data->bytes;
    this->val.val_64 = data->val.val_64;
    this->index = data->index;

    return OK;
}
int CReg::Comp(CReg *data)
{
    ASSERT(data);
    ASSERT(this->name&&data->name);

    return stricmp(this->name , data->name);
}

int  CReg::Print()
{
    int *p;
    
    p = (int*)(&this->val.val_64);
    LOG("name = %s,bytes=%d,index = %d",name,bytes,index);
    switch(bytes)
    {
        case 1: 
            if(this->val.ptr_8 == NULL)
                LOG("error:uninitiated val.ptr_8\n");
            else
                LOG("val = %x\n",*(this->val.ptr_8));
            break;
        case 2: 
            if(this->val.ptr_16 == NULL)
                LOG("error:uninitiated val.ptr_16\n");
            else
                LOG("val = %x\n",*(this->val.ptr_16));
            break;
        case 4: LOG("val = %x\n",this->val.val_32);break;
        case 8: LOG("val = %x%x\n",*(p+1),*p);break;
        default: LOG("error:err bytes\n");
    }
    return TRUE;
}

int CReg::SetVal(_RegVal *v)
{
    ASSERT(v);

    if(this->bytes >= 4)
    {
        this->val.val_64 = v->val_64;
    }
    else if(this->bytes == 1)
    {
        ASSERT(this->val.ptr_8);
        *(this->val.ptr_8) = (BYTE)(v->val_32);
    }
    else if(this->bytes == 2)
    {
        ASSERT(this->val.ptr_16);
        *(this->val.ptr_16) = (WORD)(v->val_32);
    }
    return OK;
}

int CReg::GetVal(_RegVal *val)
{
    ASSERT(val);

    if(this->bytes >= 4)
    {
        val->val_64 = this->val.val_64;
    }
    else if(this->bytes == 1)
    {
        ASSERT(this->val.ptr_8);
        val->val_64 = *(this->val.ptr_8);
    }
    else if(this->bytes == 2)
    {
        ASSERT(this->val.ptr_16);
        val->val_64 = *(this->val.ptr_16);
    }
    
    return OK;
}
