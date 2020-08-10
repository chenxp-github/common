// NesPpu.h: interface for the CNesPpu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESPPU_H__60D25710_9818_459B_A393_B135903339B8__INCLUDED_)
#define AFX_NESPPU_H__60D25710_9818_459B_A393_B135903339B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"
#include "common.h" // Added by ClassView

#define NES_SCREEN_WIDTH    (256+16)
#define NES_SCREEN_HEIGHT   240

// PPU Control Register #1  PPU #0
#define PPU_VBLANK_BIT      0x80
#define PPU_SPHIT_BIT       0x40
#define PPU_SP16_BIT        0x20
#define PPU_BGTBL_BIT       0x10
#define PPU_SPTBL_BIT       0x08
#define PPU_INC32_BIT       0x04
#define PPU_NAMETBL_BIT     0x03

// PPU Control Register #2  PPU #1
#define PPU_BGCOLOR_BIT     0xE0
#define PPU_SPDISP_BIT      0x10
#define PPU_BGDISP_BIT      0x08
#define PPU_SPCLIP_BIT      0x04
#define PPU_BGCLIP_BIT      0x02
#define PPU_COLORMODE_BIT   0x01

// PPU Status Register      PPU #2
#define PPU_VBLANK_FLAG     0x80
#define PPU_SPHIT_FLAG      0x40
#define PPU_SPMAX_FLAG      0x20
#define PPU_WENABLE_FLAG    0x10

// SPRITE Attribute
#define SP_VMIRROR_BIT      0x80
#define SP_HMIRROR_BIT      0x40
#define SP_PRIORITY_BIT     0x20
#define SP_COLOR_BIT        0x03

class CNesMmu;
class CNesEmu;

typedef struct{
    BYTE    y;
    BYTE    tile;
    BYTE    attr;
    BYTE    x;
} CNesSprite;

class CNesPpu{
public:
    CNesMmu *mmu;
    CNesEmu *nes;

    BOOL    bExtLatch;  // For MMC5
    BOOL    bChrLatch;  // For MMC2/MMC4
    BOOL    bExtNameTable;  // For Super Monkey no Dai Bouken
    BOOL    bExtMono;   // For Final Fantasy

    WORD    loopy_y;
    WORD    loopy_shift;

    BYTE    *lpScreen;
    BYTE    *lpScanline;
    int     ScanlineNo;
    BYTE    *lpColormode;

    // Reversed bits
    BYTE    *Bit2Rev;

    // For VS-Unisystem
    BOOL    bVSMode;
    BYTE    VSSecurityData;
    int     nVSColorMap;
    BYTE    *VSColorMap[5];
public:
    int SetExtLatchMode( BOOL bMode );
    WORD GetTILEY();
    WORD GetPPUADDR();
    int SetExtMonoMode( BOOL bMode );
    BOOL IsDispON();
    int SetExtNameTableMode( BOOL bMode );
    int SetScreenPtr( BYTE *lpScn, BYTE *lpMode );
    int DummyScanline( int scanline );
    int IsSprite0( int scanline );
    int Scanline( int scanline, BOOL bMax, BOOL bLeftClip );
    int ScanlineNext();
    int ScanlineStart();
    int SetRenderScanline(int scanline );
    int FrameEnd();
    int FrameStart();
    int VBlankEnd();
    int VBlankStart();
    int DMA(BYTE data);
    int Write( WORD addr, BYTE data );
    BYTE Read( WORD addr );
    int Reset();
    CNesPpu();
    virtual ~CNesPpu();
    int Init();
    int Destroy();
    int Copy(CNesPpu *p);
    int Comp(CNesPpu *p);
    int Print();
    int InitBasic();
    int CheckLinkPtr();
};

#endif // !defined(AFX_NESPPU_H__60D25710_9818_459B_A393_B135903339B8__INCLUDED_)
