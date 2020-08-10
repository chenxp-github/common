// NesMapper.h: interface for the CNesMapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESH__7042AC40_DBB5_4A81_825E_4B4AD0E7F37E__INCLUDED_)
#define AFX_NESH__7042AC40_DBB5_4A81_825E_4B4AD0E7F37E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"
#include "common.h" // Added by ClassView

class CNesMmu;
class CNesEmu;

class CNesMapper{
public:
    int current_mapper;
    CNesMmu *mmu;
    CNesEmu *nes;

    int     patch;
    int     irq_clock;
    BYTE    reg[32];
    BYTE    prg0, prg1;
    BYTE    chr[32];
    BYTE    exram[128];
    BYTE    we_sram;
    BYTE    protectflag;

    BYTE    irq_type;
    BYTE    irq_enable;
    WORD    irq_counter;
    BYTE    irq_latch;
    BYTE    irq_request;
    BYTE    irq_preset;
    BYTE    irq_preset_vbl;
    BYTE    irq_occur;
    BYTE    irq_mode;
    BYTE    vs_patch;
    BYTE    vs_index;

    BYTE    shift, regbuf;
    BYTE    wram_patch;
    BYTE    wram_bank;
    BYTE    wram_count;
    WORD    last_addr;
    WORD    addrmask;

    BYTE    reg5000;
    BYTE    reg5100;
    BYTE    a3, p_mode;
    BYTE    exsound_enable;
public:
    int Mapper019_LoadState( BYTE *p );
    int Mapper019_SaveState( BYTE *p );
    int Mapper019_Clock(int cycles );
    int Mapper019_Write( WORD addr, BYTE data );
    int Mapper019_WriteLow( WORD addr, BYTE data );
    BYTE Mapper019_ReadLow( WORD addr );
    int Mapper019_Reset();
    int Mapper062_Write( WORD addr, BYTE data );
    int Mapper062_Reset();
    int Mapper017_LoadState(BYTE *p);
    int Mapper017_SaveState(BYTE *p);
    int Mapper017_HSync( int scanline );
    int Mapper017_WriteLow( WORD addr, BYTE data );
    int Mapper017_Reset();
    int Mapper241_Reset();
    int Mapper241_Write( WORD addr, BYTE data );
    int Mapper114_Reset();
    int Mapper114_WriteLow( WORD addr, BYTE data );
    int Mapper114_Write( WORD addr, BYTE data );
    int Mapper114_Clock( int scanline );
    int Mapper114_HSync( int scanline );
    int Mapper114_SetBank_CPU();
    int Mapper114_SetBank_PPU();
    int Mapper114_SaveState( BYTE * p );
    int Mapper114_LoadState( BYTE * p );
    int Mapper091_Reset();
    int Mapper091_WriteLow( WORD addr, BYTE data );
    int Mapper091_HSync( int scanline );
    int Mapper091_SaveState( BYTE * p );
    int Mapper091_LoadState( BYTE * p );
    int Mapper073_Reset();
    int Mapper073_Write( WORD addr, BYTE data );
    int Mapper073_Clock( int cycles );
    int Mapper073_SaveState( BYTE * p );
    int Mapper073_LoadState( BYTE * p );
    int Mapper240_Reset();
    int Mapper240_WriteLow( WORD addr, BYTE data );
    int Mapper071_Reset();
    int Mapper071_WriteLow( WORD addr, BYTE data );
    int Mapper071_Write( WORD addr, BYTE data );
    int Mapper034_Reset();
    int Mapper034_WriteLow( WORD addr, BYTE data );
    int Mapper034_Write( WORD addr, BYTE data );
    int Mapper246_Reset();
    int Mapper246_WriteLow( WORD addr, BYTE data );
    int Mapper018_Reset();
    int Mapper018_Write( WORD addr, BYTE data );
    int Mapper018_Clock( int cycles );
    int Mapper018_SaveState( BYTE * p );
    int Mapper018_LoadState( BYTE * p );
    int Mapper078_Reset();
    int Mapper078_Write( WORD addr, BYTE data );
    int Mapper022_Reset();
    int Mapper022_Write( WORD addr, BYTE data );
    int Mapper069_Reset();
    int Mapper069_Write( WORD addr, BYTE data );
    int Mapper069_Clock( int cycles );
    int Mapper069_HSync( int scanline );
    int Mapper069_SaveState( BYTE * p );
    int Mapper069_LoadState( BYTE * p );
    int Mapper164_Reset();
    int Mapper164_WriteLow(WORD addr, BYTE data);
    int Mapper164_SetBank_CPU();
    int Mapper164_SetBank_PPU();
    int Mapper164_PPU_ExtLatchX( int x );
    int Mapper164_PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );
    int Mapper164_SaveState( BYTE * p );
    int Mapper164_LoadState( BYTE * p );
    int Mapper074_Reset();
    int Mapper074_Write( WORD addr, BYTE data );
    int Mapper074_HSync( int scanline );
    int Mapper074_SetBank_CPU();
    int Mapper074_SetBank_PPU();
    int Mapper074_SetBank_PPUSUB( int bank, int page );
    int Mapper074_SaveState( BYTE * p );
    int Mapper074_LoadState( BYTE * p );
    int Mapper015_Reset();
    int Mapper015_Write( WORD addr, BYTE data );
    int Mapper245_Reset();
    int Mapper245_Write( WORD addr, BYTE data );
    int Mapper245_Clock( int cycles );
    int Mapper245_HSync( int scanline );
    int Mapper245_SetBank_CPU();
    int Mapper245_SetBank_PPU();
    int Mapper245_SaveState( BYTE * p );
    int Mapper245_LoadState( BYTE * p );
    int Mapper254_Reset();
    int Mapper254_WriteLow( WORD addr, BYTE data );
    BYTE Mapper254_ReadLow( WORD addr );
    int Mapper254_Write( WORD addr, BYTE data );
    int Mapper254_Clock( int cycles );
    int Mapper254_HSync( int scanline );
    int Mapper254_SetBank_CPU();
    int Mapper254_SetBank_PPU();
    int Mapper254_SaveState( BYTE * p );
    int Mapper254_LoadState( BYTE * p );
    int Mapper025_Reset();
    int Mapper025_Write( WORD addr, BYTE data );
    int Mapper025_Clock( int cycles );
    int Mapper025_SaveState( BYTE * p );
    int Mapper025_LoadState( BYTE * p );
    int Mapper023_Reset();
    int Mapper023_Write( WORD addr, BYTE data );
    int Mapper023_Clock( int cycles );
    int Mapper023_SaveState( BYTE * p );
    int Mapper023_LoadState( BYTE * p );
    int Mapper001_LoadState( BYTE *p );
    int Mapper001_SaveState( BYTE *p );
    int Mapper001_Write( WORD addr, BYTE data );
    int Mapper001_Reset();
    int Mapper004_Write( WORD addr, BYTE data );
    int Mapper004_WriteLow( WORD addr, BYTE data );
    BYTE Mapper004_ReadLow( WORD addr );
    int Mapper004_LoadState( BYTE *p );
    int Mapper004_SaveState( BYTE *p );
    int Mapper004_HSync(int scanline );
    int Mapper004_SetBank_PPU();
    int Mapper004_SetBank_CPU();
    int Mapper004_Reset();
    int Mapper024_LoadState( BYTE *p );
    int Mapper024_SaveState( BYTE *p );
    int Mapper024_Clock( int cycles );
    int Mapper024_Write( WORD addr, BYTE data );
    int Mapper024_Reset();
    int Mapper002_WriteLow( WORD addr, BYTE data );
    int CheckLinkPtr();
    int Mapper003_Write(WORD addr, BYTE data);
    int Mapper003_Reset();
    int Mapper007_Write(WORD addr, BYTE data);
    int Mapper007_Reset();
    int Mapper002_Write(WORD addr,BYTE data);
    int Mapper002_Reset();
    int Mapper000_Reset();
    int CreateMapper(int num);
    int intite( int cmd, BYTE data );
    int Reset();    
    BYTE ReadLow_Def( WORD addr );
    BYTE ReadLow( WORD addr );
    int WriteLow_Def( WORD addr, BYTE data );
    int WriteLow( WORD addr, BYTE data );
    int Write( WORD addr, BYTE data ) ;
    int Read( WORD addr, BYTE data ) ;
    BYTE ExRead( WORD addr )    ;
    int ExWrite( WORD addr, BYTE data ) ;
    BYTE ExCmdRead ( int cmd )  ;
    int ExCmdWrite( int cmd, BYTE data ) ;
    int HSync( int scanline ) ;
    int VSync() ;
    int Clock( int cycles );
    int PPU_Latch( WORD addr );
    int PPU_ChrLatch( WORD addr );
    int PPU_ExtLatchX( int x );
    int PPU_ExtLatch( WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );
    BOOL IsStateSave() ;
    int SaveState( BYTE * p ) ;
    int LoadState( BYTE * p ) ;
    CNesMapper();
    virtual ~CNesMapper();
    int Init();
    int Destroy();
    int Copy(CNesMapper *p);
    int Comp(CNesMapper *p);
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_NESH__7042AC40_DBB5_4A81_825E_4B4AD0E7F37E__INCLUDED_)
