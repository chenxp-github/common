// VMem.cpp: implementation of the CVMem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VMem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVMem::CVMem()
{
    this->InitBasic();
}
CVMem::~CVMem()
{
    this->Destroy();
}
int CVMem::InitBasic()
{
    this->err_flag_ptr = NULL;
    return OK;
}
int CVMem::Init()
{
    this->InitBasic();
    return OK;
}
int  CVMem::Destroy()
{
    this->InitBasic();
    return OK;
}

WORD CVMem::Read16Mem(long addr)
{
    BYTE b1,b2;

    b1 = this->Read8Mem(addr);
    b2 = this->Read8Mem(addr + 1);

    return (b2<<8)|b1;
}

int CVMem::Write16Mem(long addr, WORD mem16)
{
    this->Write8Mem(addr,(BYTE)mem16);
    this->Write8Mem(addr+1,mem16>>8);

    return OK;
}
DWORD CVMem::Read32Mem(long addr)
{
    BYTE b1,b2,b3,b4;

    b1 = this->Read8Mem(addr + 0);
    b2 = this->Read8Mem(addr + 1);
    b3 = this->Read8Mem(addr + 2);
    b4 = this->Read8Mem(addr + 3);
    
    return (b4<<24)|(b3<<16)|(b2<<8)|b1;
}

int CVMem::Write32Mem(long addr, DWORD mem32)
{
    this->Write8Mem(addr+0,(BYTE)(mem32));
    this->Write8Mem(addr+1,(BYTE)(mem32>>8));
    this->Write8Mem(addr+2,(BYTE)(mem32>>16));
    this->Write8Mem(addr+3,(BYTE)(mem32>>24));

    return OK;
}

QWORD CVMem::Read64Mem(long addr)
{
    QWORD dw1,dw2;
    
    dw1 = this->Read32Mem(addr);
    dw2 = this->Read32Mem(addr + 4);
    
    return (dw2<<32) | dw1;
}

int CVMem::Write64Mem(long addr, QWORD qw)
{
    this->Write32Mem(addr,(DWORD)qw);
    this->Write32Mem(addr + 4,(DWORD)(qw>>32));

    return OK;
}

//return value is the length of the string
int CVMem::ReadString(long vaddr, CFileBase *str_des)
{
    char ch;
    long len = 0;

    ASSERT(str_des);

    str_des->SetSize(0);

    ch = this->Read8Mem(vaddr + len);
    while(ch && !IsError())
    {
        len ++;
        str_des->Putc(ch);      
        ch = this->Read8Mem(vaddr + len);
    }

    str_des->Putc(0);

    return len;
}

int CVMem::WriteMem(long vaddr, void *buf, long size)
{
    char *p = (char*)buf;

    for(long i = 0; i < size && !IsError(); i++)
    {
        this->Write8Mem(vaddr+i,p[i]);
    }

    return OK;
}

int CVMem::ReadMem(long vaddr, void *buf, long size)
{
    char *p = (char*)buf;

    for(long i = 0; i < size && !IsError(); i++)
    {
        p[i] = this->Read8Mem(vaddr+i);
    }

    return OK;
}

int CVMem::SetError()
{
    if(this->err_flag_ptr)
    {
        *(this->err_flag_ptr) = true;
        return OK;
    }

    return ERROR;
}

int CVMem::IsError()
{
    ASSERT(this->err_flag_ptr);
    return *(this->err_flag_ptr);
}
