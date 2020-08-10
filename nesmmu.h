// NesMmu.h: interface for the CNesMmu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESMMU_H__79A73456_EB47_4BC8_B708_9B69065E39C3__INCLUDED_)
#define AFX_NESMMU_H__79A73456_EB47_4BC8_B708_9B69065E39C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"

#define NES_CPU_MEM_BANK_SIZE   8
#define NES_CPU_MEM_TYPE_SIZE   8
#define NES_CPU_MEM_PAGE_SIZE   8
#define NES_PPU_MEM_BANK_SIZE   12
#define NES_PPU_MEM_TYPE_SIZE   12
#define NES_PPU_MEM_PAGE_SIZE   12
#define NES_CRAM_USED_SIZE      16
#define NES_RAM_SIZE            (8*1024)
#define NES_WRAM_SIZE           (128*1024)
#define NES_DRAM_SIZE           (40*1024)
#define NES_XRAM_SIZE           (8*1024)
#define NES_ERAM_SIZE           (32*1024)
#define NES_CRAM_SIZE           (32*1024)
#define NES_VRAM_SIZE           (4*1024)
#define NES_SPRAM_SIZE          0x100
#define NES_BGPAL_SIZE          0x10
#define NES_SPPAL_SIZE          0x10
#define NES_CPUREG_SIZE         0x18
#define NES_PPUREG_SIZE         0x04


// For PROM (CPU)
#define BANKTYPE_ROM    0x00
#define BANKTYPE_RAM    0xFF
#define BANKTYPE_DRAM   0x01
#define BANKTYPE_MAPPER 0x80

// For VROM/VRAM/CRAM (PPU)
#define BANKTYPE_VROM   0x00
#define BANKTYPE_CRAM   0x01
#define BANKTYPE_VRAM   0x80

#define VRAM_HMIRROR    0x00    // Horizontal
#define VRAM_VMIRROR    0x01    // Virtical
#define VRAM_MIRROR4    0x02    // All screen
#define VRAM_MIRROR4L   0x03    // PA10 L $2000-$23FF
#define VRAM_MIRROR4H   0x04    // PA10 H $2400-$27FF

class CNesMmu{
public:
    BYTE **CPU_MEM_BANK;
    BYTE *CPU_MEM_TYPE;
    int  *CPU_MEM_PAGE;
    BYTE **PPU_MEM_BANK;
    BYTE *PPU_MEM_TYPE;
    int  *PPU_MEM_PAGE;
    BYTE *CRAM_USED;
    BYTE *RAM;
    BYTE *WRAM;
    BYTE *DRAM;
    BYTE *XRAM;
    BYTE *ERAM;
    BYTE *CRAM;
    BYTE *VRAM;
    BYTE *SPRAM;
    BYTE *BGPAL;
    BYTE *SPPAL;
    BYTE *CPUREG;
    BYTE *PPUREG;
    BYTE FrameIRQ;
    BYTE PPU56Toggle;
    BYTE PPU7_Temp;
    WORD loopy_t;
    WORD loopy_v;
    WORD loopy_x;
    BYTE *PROM;
    BYTE *VROM;
    BYTE *PROM_ACCESS;
    int PROM_8K_SIZE;
    int PROM_16K_SIZE; 
    int PROM_32K_SIZE;
    int VROM_1K_SIZE; 
    int VROM_2K_SIZE; 
    int VROM_4K_SIZE;  
    int VROM_8K_SIZE;
public:
    int SetFrameIRQmode(BOOL bMode);
    void SetPROM_8K_Bank( BYTE page, int bank );
    void SetPROM_16K_Bank( BYTE page, int bank );
    void SetPROM_32K_Bank( int bank );
    void SetPROM_32K_Bank( int bank0, int bank1, int bank2, int bank3 );
    void SetVROM_Bank( BYTE page, BYTE * ptr, BYTE type );
    void SetVROM_1K_Bank( BYTE page, int bank );
    void SetVROM_2K_Bank( BYTE page, int bank );
    void SetVROM_4K_Bank( BYTE page, int bank );
    void SetVROM_8K_Bank( int bank );   
    void SetCRAM_1K_Bank( BYTE page, int bank );
    void SetCRAM_2K_Bank( BYTE page, int bank );
    void SetCRAM_4K_Bank( BYTE page, int bank );
    void SetCRAM_8K_Bank( int bank );
    void SetVRAM_1K_Bank( BYTE page, int bank );
    void SetVRAM_Bank( int bank0, int bank1, int bank2, int bank3 );
    void SetVRAM_Mirror( int type );
    void SetVRAM_Mirror( int bank0, int bank1, int bank2, int bank3 );
    void SetVROM_8K_Bank( int bank0, int bank1, int bank2, int bank3, int bank4, int bank5, int bank6, int bank7 );
    void SetPROM_Bank( BYTE page, BYTE *ptr, BYTE type );

    int MemoryInitial();
    CNesMmu();
    virtual ~CNesMmu();
    int Init();
    int Destroy();
    int Copy(CNesMmu *p);
    int Comp(CNesMmu *p);
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_NESMMU_H__79A73456_EB47_4BC8_B708_9B69065E39C3__INCLUDED_)
