// NesCpu.h: interface for the CNesCpu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESCPU_H__92B68589_280F_4168_BC42_D381D6544F46__INCLUDED_)
#define AFX_NESCPU_H__92B68589_280F_4168_BC42_D381D6544F46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"
#include "filebase.h"

// 6502 status flags
#define C_FLAG      0x01        // 1: Carry
#define Z_FLAG      0x02        // 1: Zero
#define I_FLAG      0x04        // 1: Irq disabled
#define D_FLAG      0x08        // 1: Decimal mode flag (NES unused)
#define B_FLAG      0x10        // 1: Break
#define R_FLAG      0x20        // 1: Reserved (Always 1)
#define V_FLAG      0x40        // 1: Overflow
#define N_FLAG      0x80        // 1: Negative

#define IRQ_FRAMEIRQ    0x04
#define IRQ_DPCM    0x08
#define IRQ_MAPPER  0x10
#define IRQ_MAPPER2 0x20
#define IRQ_TRIGGER 0x40
#define IRQ_TRIGGER2    0x80

#define IRQ_MASK    (~(NMI_FLAG|IRQ_FLAG))

// Vector
#define NMI_VECTOR  0xFFFA
#define RES_VECTOR  0xFFFC
#define IRQ_VECTOR  0xFFFE

#define FETCH_CYCLES    8

class CNesMapper;
class CNesMmu;
class CNesEmu;
class CNesApu;

class CNesR6502{
public:
    WORD    PC; /* Program counter   */
    BYTE    A;  /* CPU registers     */
    BYTE    P;
    BYTE    X;
    BYTE    Y;
    BYTE    S;
    BYTE    INT_pending;
public:
    CNesR6502();
    virtual ~CNesR6502();
    int InitBasic();
};

class CNesCpu{
public:
    CNesMapper* mapper;
    CNesMmu *mmu;
    CNesEmu *nes;
    CNesApu *apu;
    CNesR6502   R;

    int TOTAL_cycles;
    int DMA_cycles;

    // PTR
    BYTE *  STACK;
    // Zero & Negative table
    BYTE    *ZN_Table;
    // Clock process
    BOOL    m_bClockProcess;

    int nmicount;
public:
    int SetIRQ( BYTE mask );
    int ClrIRQ( BYTE mask );
    int Reset();
    int SetContext(CNesR6502 *r);
    int GetContext( CNesR6502 *r );
    inline int WR6502( WORD addr, BYTE data );
    inline WORD RD6502W( WORD addr );
    inline BYTE RD6502( WORD addr );
    inline WORD OP6502W( WORD addr );
    inline BYTE OP6502( WORD addr );
    int EXEC( INT request_cycles );
    int DMA( int cycles );
    int NMI();
    int CheckLinkPtr();
    CNesCpu();
    virtual ~CNesCpu();
    int Init();
    int Destroy();
    int Copy(CNesCpu *p);
    int Comp(CNesCpu *p);
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_NESCPU_H__92B68589_280F_4168_BC42_D381D6544F46__INCLUDED_)
