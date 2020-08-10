// NesPad.cpp: implementation of the CNesPad class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NesPad.h"
#include "syslog.h"
#include "nesemu.h"
#include "nesrom.h"

CNesBtnState::CNesBtnState()
{
    this->InitBasic();
}
int CNesBtnState::InitBasic()
{
    this->AKeyDown = 0;
    this->ASKeyDown = 0;
    this->BKeyDown = 0;
    this->BSKeyDown = 0;

    return OK;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNesPad::CNesPad()
{
    this->InitBasic();
}
CNesPad::~CNesPad()
{
    this->Destroy();
}
int CNesPad::InitBasic()
{
    this->nes = NULL;
    this->bZapperMode = FALSE;      
    return OK;
}
int CNesPad::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
int CNesPad::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
int CNesPad::Copy(CNesPad *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
int CNesPad::Comp(CNesPad *p)
{
    return 0;
}
int CNesPad::Print()
{
    //add your code
    return TRUE;
}
int CNesPad::CheckLinkPtr()
{
    ASSERT(this->nes);
    return TRUE;
}

BYTE CNesPad::Read(WORD addr)
{
    if(addr == 0x4016)
        return this->GetValue(0);
    else if(addr == 0x4017)
        return this->GetValue(1);
    return  0;
}

int CNesPad::Write(WORD addr, BYTE data)
{
    if(addr == 0x4016)
        this->InputBursh(data);
    return OK;
}

BOOL CNesPad::IsZapperMode()
{
     return bZapperMode;
}

int CNesPad::VSync()
{
    return OK;
}

int CNesPad::Reset()
{
    int i;

    m_nCurFrame = 0;
    m_bStrobe   = FALSE;

    for(i = 0; i < NES_PAD_NUM; i++)
    {
        this->m_CurIndex[i] = 0;
        this->m_PadBit[i] = 0;
        this->m_BtnState[i].InitBasic();
    }

    return OK;
}

int CNesPad::SetValue(int pad, int index, BYTE val)
{
    DWORD * PadBit = &m_PadBit[pad];
    DWORD mask = index;
    if (pad & 0x2)
        mask <<= 8;
    if (val)
        *PadBit |= mask;
    else
        *PadBit &= ~mask;

    return OK;
}

int CNesPad::SetState(int pad, int index, BYTE val)
{
    if (index == NES_JOY_PAD_AS)
    {
        m_BtnState[pad].ASKeyDown = val;
        m_BtnState[pad].AKeyDown  = FALSE;
        SetValue(pad, NES_JOY_PAD_A, val);
    }
    else if (index == NES_JOY_PAD_BS)
    {
        m_BtnState[pad].BSKeyDown = val;
        m_BtnState[pad].BKeyDown  = FALSE;
        SetValue(pad, NES_JOY_PAD_B, val);
    }
    else if (index == NES_JOY_PAD_A)
    {
        if (!m_BtnState[pad].ASKeyDown)
        {
            m_BtnState[pad].AKeyDown = val;
            SetValue(pad, NES_JOY_PAD_A, val);
        }
    }
    else if (index == NES_JOY_PAD_B)
    {
        if (!m_BtnState[pad].BSKeyDown)
        {
            m_BtnState[pad].BKeyDown = val;
            SetValue(pad, NES_JOY_PAD_B, val);
        }
    }
    else
    {
        SetValue(pad, index, val);
    }

    return OK;
}

int CNesPad::InputBursh(BYTE burst)
{
    if (burst & 0x1 && m_bStrobe == FALSE)
        m_bStrobe = TRUE;
    else if (!(burst & 0x1) && m_bStrobe)
    {
        m_bStrobe = FALSE;
        m_CurIndex[0] = m_CurIndex[1] = 0;
    }

    return OK;
}

BYTE CNesPad::GetValue(int padbit)
{
    BYTE ret = (BYTE)((m_PadBit[padbit] >> m_CurIndex[padbit]) & 0x1);
    m_CurIndex[padbit]++;
    if (m_CurIndex[padbit] >= 24)
    {
        ::memset(m_CurIndex, 0, sizeof(m_CurIndex));
    }
    return ret;
}

int CNesPad::UpdateValue()
{
    if (m_nCurFrame++ % 4)
        return OK;

    for (int i = 0; i < NES_PAD_NUM; i++)
    {
        if (m_BtnState[i].ASKeyDown)
        {
            DWORD mask = NES_JOY_PAD_A;
            if (i >= 2)
                mask <<= 8;
            m_PadBit[i] ^= mask;
        }
        else if (m_BtnState[i].AKeyDown)
        {
            SetValue(i, NES_JOY_PAD_A, 1);
        }

        if (m_BtnState[i].BSKeyDown)
        {
            DWORD mask = NES_JOY_PAD_B;
            if (i >= 2)
                mask <<= 8;
            m_PadBit[i] ^= mask;
        }
        else if (m_BtnState[i].BKeyDown)
        {
            SetValue(i, NES_JOY_PAD_B, 1);
        }
    }

    return OK;
}
