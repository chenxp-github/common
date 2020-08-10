// NesEmu.h: interface for the CNesEmu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESEMU_H__9B6B69B1_36BF_4869_B7BC_B48C017A6674__INCLUDED_)
#define AFX_NESEMU_H__9B6B69B1_36BF_4869_B7BC_B48C017A6674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"
#include "common.h" // Added by ClassView
#include "filebase.h"

#define POST_ALL_RENDER  0
#define PRE_ALL_RENDER   1
#define POST_RENDER      2
#define PRE_RENDER       3
#define TILE_RENDER      4

#define IRQ_HSYNC       0
#define IRQ_CLOCK       1

class CNesMmu;
class CNesMapper;
class CNesCpu;
class CNesApu;
class CNesRom;
class CNesPad;
class CNesPpu;


class CNesConfig {
public:
    float   BaseClock;      // NTSC:21477270.0  PAL:21281364.0
    float   CpuClock;       // NTSC: 1789772.5  PAL: 1773447.0

    int TotalScanlines;     // NTSC: 262  PAL: 312

    int ScanlineCycles;     // NTSC:1364  PAL:1362

    int HDrawCycles;        // NTSC:1024  PAL:1024
    int HBlankCycles;       // NTSC: 340  PAL: 338
    int ScanlineEndCycles;  // NTSC:   4  PAL:   2

    int FrameCycles;        // NTSC:29829.52  PAL:35468.94
    int FrameIrqCycles;     // NTSC:29829.52  PAL:35468.94

    int FrameRate;      // NTSC:60(59.94) PAL:50
    float   FramePeriod;        // NTSC:16.683    PAL:20.0
public:
    int Init_NTSC();
    int Init_PAL();
    CNesConfig();
    int InitBasic();
}; 

class CNesEmu{
public:
    CNesMapper *mapper;
    CNesCpu *cpu;
    CNesApu *apu;
    CNesRom *rom;
    CNesPad *pad;
    CNesMmu *mmu;
    CNesPpu *ppu;
    CNesConfig *nescfg;

    BYTE m_TapeOut;
    BYTE m_TapeIn;
    int RenderMethod;
    SQWORD  base_cycles;
    SQWORD  emul_cycles;

    BOOL    m_bNsfPlaying;
    BOOL    m_bNsfInit;
    int m_nNsfSongNo;
    int m_nNsfSongMode;
    int NES_scanline;
    BOOL bZapper;
    int ZapperX, ZapperY;

    BOOL bVideoMode;
    int nIRQtype;
    BOOL bFrameIRQ;
    BOOL m_bDiskThrottle;
    int SAVERAM_SIZE;

    BITMAPINFO *p_bmp_info;
    BYTE *screen_bit;
    BYTE *color_mode;  //don't know what is it for. 
    BOOL running;
public:
    int LoadState(CFileBase *file);
    int SaveState(CFileBase *file);
    int WriteBMP(char *fn);
    int WriteBMP(CFileBase *file);
    int GetSAVERAM_SIZE();
    int SetSAVERAM_SIZE( int size );
    int GetScanline();
    int LoadFile(char *fn);
    int Reset();
    int SetVideoMode( BOOL bMode );
    int SetFrameIRQmode(BOOL bMode);
    int GetIrqType();
    int SetIrqType(int  nType);
    int LoadFile(CFileBase *file);
    int EmulateNSF();
    int EmulateFrame( BOOL bDraw );
    int EmulationCPU( int basecycles );
    int GetRenderMethod();
    int SetRenderMethod( int type );
    int Clock( int cycles );
    int Write( WORD addr, BYTE data );
    BYTE Read( WORD addr );
    int WriteReg( WORD addr, BYTE data );
    BYTE ReadReg( WORD addr );
    CNesEmu();
    virtual ~CNesEmu();
    int Init();
    int Destroy();
    int Copy(CNesEmu *p);
    int Comp(CNesEmu *p);
    int Print();
    int InitBasic();
    int CheckLinkPtr();
};

#endif // !defined(AFX_NESEMU_H__9B6B69B1_36BF_4869_B7BC_B48C017A6674__INCLUDED_)
