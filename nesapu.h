// NesApu.h: interface for the CNesApu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESAPU_H__EE60EF32_A41D_4368_8F1A_3F37898DE058__INCLUDED_)
#define AFX_NESAPU_H__EE60EF32_A41D_4368_8F1A_3F37898DE058__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"

class CNesApu{
public:
public:
    BYTE ExRead( WORD addr );
    int SelectExSound( BYTE data );
    int SoundSetup();
    int ExWrite( WORD addr, BYTE data );
    int Reset();
    int Write( WORD addr, BYTE data );
    BYTE Read( WORD addr );
    CNesApu();
    virtual ~CNesApu();
    int Init();
    int Destroy();
    int Copy(CNesApu *p);
    int Comp(CNesApu *p);
    int Print();
    int InitBasic();
    int CheckLinkPtr();
};

#endif // !defined(AFX_NESAPU_H__EE60EF32_A41D_4368_8F1A_3F37898DE058__INCLUDED_)
