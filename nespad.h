// NesPad.h: interface for the CNesPad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESPAD_H__E2980161_4B27_4E76_8410_724EDA55105B__INCLUDED_)
#define AFX_NESPAD_H__E2980161_4B27_4E76_8410_724EDA55105B__INCLUDED_

#include "common.h" // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"

#define NES_PAD_NUM         2

#define NES_JOY_PAD_1       0
#define NES_JOY_PAD_2       1
#define NES_JOY_PAD_3       2
#define NES_JOY_PAD_4       3

#define NES_JOY_PAD_A       0x01
#define NES_JOY_PAD_B       0x02
#define NES_JOY_PAD_SELECT  0x04
#define NES_JOY_PAD_START   0x08
#define NES_JOY_PAD_UP      0x10
#define NES_JOY_PAD_DOWN    0x20
#define NES_JOY_PAD_LEFT    0x40
#define NES_JOY_PAD_RIGHT   0x80
#define NES_JOY_PAD_AS      0X101
#define NES_JOY_PAD_BS      0X102

class CNesEmu;

class CNesBtnState 
{
public:
    BOOL AKeyDown;
    BOOL BKeyDown;
    BOOL ASKeyDown;
    BOOL BSKeyDown;
public:
    int InitBasic();
    CNesBtnState();
};

class CNesPad{
public:
    CNesEmu *nes;
    BOOL bZapperMode;
    int      m_nCurFrame;
    BOOL     m_bStrobe;
    int      m_CurIndex[NES_PAD_NUM];
    DWORD    m_PadBit[NES_PAD_NUM];
    CNesBtnState m_BtnState[NES_PAD_NUM];
public:
    CNesPad();
    virtual ~CNesPad();
    int InitBasic();
    int Init();
    int Destroy();
    int Copy(CNesPad *p);
    int Comp(CNesPad *p);
    int Print();
    int CheckLinkPtr();
    BYTE Read(WORD addr);
    int Write(WORD addr, BYTE data);
    BOOL IsZapperMode();
    int VSync();
    int Reset();
    int SetValue(int pad, int index, BYTE val);
    int SetState(int pad, int index, BYTE val);
    int InputBursh(BYTE burst);
    BYTE GetValue(int padbit);
    int UpdateValue();
};

#endif // !defined(AFX_NESPAD_H__E2980161_4B27_4E76_8410_724EDA55105B__INCLUDED_)
