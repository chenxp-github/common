// NesApu.cpp: implementation of the CNesApu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NesApu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNesApu::CNesApu()
{
    this->InitBasic();
}
CNesApu::~CNesApu()
{
    this->Destroy();
}
int CNesApu::InitBasic()
{
    //add your code
    return OK;
}
int CNesApu::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
int CNesApu::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
int CNesApu::Copy(CNesApu *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
int CNesApu::Comp(CNesApu *p)
{
    return 0;
}
int CNesApu::Print()
{
    //add your code
    return TRUE;
}
int CNesApu::CheckLinkPtr()
{
    //add your code
    return TRUE;
}

BYTE CNesApu::Read(WORD addr)
{
    return 0;
}

int CNesApu::Write(WORD addr, BYTE data)
{
    return OK;
}

int CNesApu::Reset()
{
    return OK;
}

int CNesApu::ExWrite(WORD addr, BYTE data)
{
    return OK;
}

int CNesApu::SoundSetup()
{
    return OK;
}

int CNesApu::SelectExSound(BYTE data)
{
    return OK;
}

BYTE CNesApu::ExRead(WORD addr)
{
    return 0;
}
