// NesEmu.cpp: implementation of the CNesEmu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NesEmu.h"
#include "mem_tool.h"
#include "mem.h"
#include "nesmapper.h"
#include "nescpu.h"
#include "nesapu.h"
#include "nesrom.h"
#include "nespad.h"
#include "nesmmu.h"
#include "nesppu.h"
#include "winmisc.h"
#include "file.h"

static RGBQUAD _rgbQuard[64] =
{   
    {0x7F, 0x7F, 0x7F, 0}, {0xB0, 0x00, 0x20, 0}, {0xB8, 0x00, 0x28, 0}, {0xA0, 0x10, 0x60, 0},
    {0x78, 0x20, 0x98, 0}, {0x30, 0x10, 0xB0, 0}, {0x00, 0x30, 0xA0, 0}, {0x00, 0x40, 0x78, 0},
    {0x00, 0x58, 0x48, 0}, {0x00, 0x68, 0x38, 0}, {0x00, 0x6C, 0x38, 0}, {0x40, 0x60, 0x30, 0},
    {0x80, 0x50, 0x30, 0}, {0x00, 0x00, 0x00, 0}, {0x00, 0x00, 0x00, 0}, {0x00, 0x00, 0x00, 0},

    {0xBC, 0xBC, 0xBC, 0}, {0xF8, 0x60, 0x40, 0}, {0xFF, 0x40, 0x40, 0}, {0xF0, 0x40, 0x90, 0},
    {0xC0, 0x40, 0xD8, 0}, {0x60, 0x40, 0xD8, 0}, {0x00, 0x50, 0xE0, 0}, {0x00, 0x70, 0xC0, 0},
    {0x00, 0x88, 0x88, 0}, {0x00, 0xA0, 0x50, 0}, {0x10, 0xA8, 0x48, 0}, {0x68, 0xA0, 0x48, 0},
    {0xC0, 0x90, 0x40, 0}, {0x00, 0x00, 0x00, 0}, {0x00, 0x00, 0x00, 0}, {0x00, 0x00, 0x00, 0},

    {0xFF, 0xFF, 0xFF, 0}, {0xFF, 0xA0, 0x60, 0}, {0xFF, 0x80, 0x50, 0}, {0xFF, 0x70, 0xA0, 0},
    {0xFF, 0x60, 0xF0, 0}, {0xB0, 0x60, 0xFF, 0}, {0x30, 0x78, 0xFF, 0}, {0x00, 0xA0, 0xFF, 0},
    {0x20, 0xD0, 0xE8, 0}, {0x00, 0xE8, 0x98, 0}, {0x40, 0xF0, 0x70, 0}, {0x90, 0xE0, 0x70, 0},
    {0xE0, 0xD0, 0x60, 0}, {0x60, 0x60, 0x60, 0}, {0x00, 0x00, 0x00, 0}, {0x00, 0x00, 0x00, 0},

    {0xFF, 0xFF, 0xFF, 0}, {0xFF, 0xD0, 0x90, 0}, {0xFF, 0xB8, 0xA0, 0}, {0xFF, 0xB0, 0xC0, 0},
    {0xFF, 0xB0, 0xE0, 0}, {0xE8, 0xB8, 0xFF, 0}, {0xB8, 0xC8, 0xFF, 0}, {0xA0, 0xD8, 0xFF, 0},
    {0x90, 0xF0, 0xFF, 0}, {0x80, 0xF0, 0xC8, 0}, {0xA0, 0xF0, 0xA0, 0}, {0xC8, 0xFF, 0xA0, 0},
    {0xF0, 0xFF, 0xA0, 0}, {0xA0, 0xA0, 0xA0, 0}, {0x00, 0x00, 0x00, 0}, {0x00, 0x00, 0x00, 0}
};

static BITMAPINFOHEADER _BmpInfoHeader = { 40, NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, 1, 8, 0, NES_SCREEN_WIDTH * NES_SCREEN_HEIGHT, 0, 0, 0, 0 };
//////////////////////////////////////////////////////////////////////
// CNesConfig
//////////////////////////////////////////////////////////////////////
CNesConfig::CNesConfig()
{
    this->InitBasic();
}
int CNesConfig::InitBasic()
{
    //add your code
    return OK;
}

int CNesConfig::Init_NTSC()
{
    this->BaseClock = 21477270.0f;
    this->CpuClock = 1789772.5f;
    this->TotalScanlines = 262;
    this->ScanlineCycles = 1364;
    this->HDrawCycles = 1024;
    this->HBlankCycles = 340;
    this->ScanlineEndCycles = 4;
    this->FrameCycles = 1364 * 262;
    this->FrameIrqCycles = 29830;
    this->FrameRate = 60;
    this->FramePeriod = 1000.0f / 60.0f;

    return OK;
}

int CNesConfig::Init_PAL()
{
    this->BaseClock = 21281364.0f;
    this->CpuClock = 1773447.0f;
    this->TotalScanlines = 312;
    this->ScanlineCycles = 1362;
    this->HDrawCycles = 1024;
    this->HBlankCycles = 338;
    this->ScanlineEndCycles = 2;
    this->FrameCycles = 1362 * 312;
    this->FrameIrqCycles = 35469;
    this->FrameRate = 50;
    this->FramePeriod = 1000.0f / 50.0f;

    return OK;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNesEmu::CNesEmu()
{
    this->InitBasic();
}
CNesEmu::~CNesEmu()
{
    this->Destroy();
}
int CNesEmu::InitBasic()
{
    this->mapper = NULL;
    this->cpu = NULL;
    this->apu = NULL;
    this->rom = NULL;
    this->pad = NULL;
    this->mmu = NULL;
    this->ppu = NULL;

    this->nescfg = NULL;
    this->m_TapeOut = 0;
    this->m_TapeIn = 0;
    this->RenderMethod = POST_ALL_RENDER;
    this->base_cycles = 0;
    this->emul_cycles = 0;

    this->m_bNsfPlaying = 0;
    this->m_bNsfInit = 0;
    this->m_nNsfSongNo = 0;
    this->m_nNsfSongMode = 0;
    this->NES_scanline = 0;
    this->bZapper = 0;
    this->ZapperX = 0;
    this->ZapperY = 0;

    this->nIRQtype = 0;
    this->bFrameIRQ = 0;
    this->bVideoMode = 0;
    this->m_bDiskThrottle = FALSE;
    this->SAVERAM_SIZE = 0;

    this->p_bmp_info = NULL;
    this->screen_bit = NULL;
    this->color_mode = NULL;
    this->running = FALSE;
    
    return OK;
}
int CNesEmu::Init()
{
    this->InitBasic();
    
    NEW(this->nescfg,CNesConfig);
    this->nescfg->Init_PAL();

    NEW(this->ppu,CNesPpu);
    this->ppu->Init();

    NEW(this->mmu,CNesMmu);
    this->mmu->Init();

    NEW(this->pad,CNesPad);
    this->pad->Init();

    NEW(this->rom,CNesRom);
    this->rom->Init();

    NEW(this->apu,CNesApu);
    this->apu->Init();

    NEW(this->mapper,CNesMapper);
    this->mapper->Init();

    NEW(this->cpu,CNesCpu);
    this->cpu->Init();

    MALLOC(this->screen_bit,BYTE,NES_SCREEN_HEIGHT*NES_SCREEN_WIDTH);
    MALLOC(this->color_mode,BYTE,NES_SCREEN_HEIGHT);

    BYTE *_p;
    MALLOC(_p,BYTE,40+1024);
    this->p_bmp_info = (BITMAPINFO *)_p;

    memcpy(this->p_bmp_info, &_BmpInfoHeader, sizeof(BITMAPINFOHEADER));
    memcpy(((BYTE *)this->p_bmp_info) + sizeof(BITMAPINFOHEADER), _rgbQuard, 64 * 4);

    //link area
    this->ppu->nes = this;
    this->ppu->mmu = this->mmu;
    this->ppu->SetScreenPtr(this->screen_bit,this->color_mode);

    this->cpu->mmu = this->mmu;
    this->cpu->nes = this;
    this->cpu->mapper = this->mapper;
    this->cpu->apu = this->apu;
    
    this->mapper->mmu = this->mmu;
    this->mapper->nes = this;
    
    this->pad->nes = this;

    //check ptr area
    this->cpu->CheckLinkPtr();
    this->apu->CheckLinkPtr();
    this->pad->CheckLinkPtr();
    this->rom->CheckLinkPtr();
    this->mapper->CheckLinkPtr();
    
    return OK;
}
int CNesEmu::Destroy()
{
    FREE(this->color_mode);
    FREE(this->screen_bit);
    FREE(this->p_bmp_info);

    DEL(this->nescfg);
    DEL(this->ppu);
    DEL(this->mmu);
    DEL(this->pad);
    DEL(this->rom);
    DEL(this->apu);
    DEL(this->mapper);
    DEL(this->cpu);
    
    this->InitBasic();
    return OK;
}
int CNesEmu::Copy(CNesEmu *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
int CNesEmu::Comp(CNesEmu *p)
{
    return 0;
}
int CNesEmu::Print()
{
    //add your code
    return TRUE;
}
int CNesEmu::CheckLinkPtr()
{
    return TRUE;
}

BYTE CNesEmu::ReadReg(WORD addr)
{
    switch( addr & 0xFF ) 
    {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0A: case 0x0B:
        case 0x0C: case 0x0D: case 0x0E: case 0x0F:
        case 0x10: case 0x11: case 0x12: case 0x13:
            return  apu->Read( addr );
            break;
        case    0x15:
            return  apu->Read( addr );
            break;

        case    0x14:
            return  addr&0xFF;
            break;

        case    0x16:
            if( rom->IsVSUNISYSTEM() ) 
                return  pad->Read( addr );
            else
                return  pad->Read( addr ) | 0x40 | m_TapeOut;           
            break;
        case    0x17:
            if( rom->IsVSUNISYSTEM() ) 
                return  pad->Read( addr );
            else 
                return  pad->Read( addr ) | apu->Read( addr );          
            break;
        default:
            return  mapper->ExRead( addr );
            break;
    }

    return 0;
}

int CNesEmu::WriteReg(WORD addr, BYTE data)
{
    switch( addr & 0xFF ) {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0A: case 0x0B:
        case 0x0C: case 0x0D: case 0x0E: case 0x0F:
        case 0x10: case 0x11: case 0x12: case 0x13:
        case 0x15:
            apu->Write( addr, data );
            mmu->CPUREG[addr & 0xFF] = data;
            break;

        case    0x14:
            ppu->DMA( data );
            cpu->DMA( 514 ); // DMA Pending cycle
            mmu->CPUREG[addr & 0xFF] = data;
            break;

        case    0x16:
            mapper->ExWrite( addr, data );  // For VS-Unisystem
            pad->Write( addr, data );
            mmu->CPUREG[addr & 0xFF] = data;
            m_TapeIn = data;
            break;
        case    0x17:
            mmu->CPUREG[addr & 0xFF] = data;
            pad->Write( addr, data );
            apu->Write( addr, data );
            break;
        case    0x18:
            apu->Write( addr, data );
            break;

        default:
            mapper->ExWrite( addr, data );
            break;
    }

    return OK;
}

BYTE CNesEmu::Read(WORD addr)
{
    switch( addr>>13 ) {
        case    0x00:   // $0000-$1FFF
            return  mmu->RAM[addr&0x07FF];
        case    0x01:   // $2000-$3FFF
            return  ppu->Read( addr&0xE007 );
        case    0x02:   // $4000-$5FFF
            if( addr < 0x4100 ) {
                return  ReadReg( addr );
            } else {
                return  mapper->ReadLow( addr );
            }
            break;
        case    0x03:   // $6000-$7FFF
            return  mapper->ReadLow( addr );
        case    0x04:   // $8000-$9FFF
        case    0x05:   // $A000-$BFFF
        case    0x06:   // $C000-$DFFF
        case    0x07:   // $E000-$FFFF
            return  mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF];
    }

    return  0x00;   // Warning
}

int CNesEmu::Write(WORD addr, BYTE data)
{
    switch( addr>>13 ) {
        case    0x00:   // $0000-$1FFF
            mmu->RAM[addr&0x07FF] = data;
            break;
        case    0x01:   // $2000-$3FFF
            if( !rom->IsNSF() ) {
                ppu->Write( addr&0xE007, data );
            }
            break;
        case    0x02:   // $4000-$5FFF
            if( addr < 0x4100 ) {
                WriteReg( addr, data );
            } else {
                mapper->WriteLow( addr, data );
            }
            break;
        case    0x03:   // $6000-$7FFF
            mapper->WriteLow( addr, data );
            break;
        case    0x04:   // $8000-$9FFF
        case    0x05:   // $A000-$BFFF
        case    0x06:   // $C000-$DFFF
        case    0x07:   // $E000-$FFFF
            mapper->Write( addr, data );
            break;
    }

    return OK;
}

int CNesEmu::Clock(int cycles)
{
    //todo: implment it
    return OK;
}

int CNesEmu::SetRenderMethod(int type)
{
    this->RenderMethod = type;
    return OK;
}

int CNesEmu::GetRenderMethod()
{
    return this->RenderMethod;
}

int CNesEmu::EmulationCPU(int basecycles)
{
    int cycles;

    base_cycles += basecycles;
    cycles = (int)((base_cycles/12)-emul_cycles);

    if( cycles > 0 ) 
    {
        emul_cycles += cpu->EXEC( cycles );
    }

    return OK;
}

int CNesEmu::EmulateFrame(BOOL bDraw)
{
    int scanline = 0;

    if( rom->IsNSF() ) {
        EmulateNSF();
        return OK;
    }

    NES_scanline = scanline;

    if( RenderMethod != TILE_RENDER ) {
        bZapper = FALSE;
        while( TRUE ) {
            ppu->SetRenderScanline( scanline );

            if( scanline == 0 ) {           
                if( RenderMethod < POST_RENDER ) {
                    EmulationCPU( nescfg->ScanlineCycles );
                    ppu->FrameStart();
                    ppu->ScanlineNext();
                    mapper->HSync( scanline );
                    ppu->ScanlineStart();
                } else {
                    EmulationCPU( nescfg->HDrawCycles );
                    ppu->FrameStart();
                    ppu->ScanlineNext();
                    mapper->HSync( scanline );
                    EmulationCPU( FETCH_CYCLES*32 );
                    ppu->ScanlineStart();
                    EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
                }
            } else if( scanline < 240 ) {
                if( RenderMethod < POST_RENDER ) {
                    if( RenderMethod == POST_ALL_RENDER )
                        EmulationCPU( nescfg->ScanlineCycles );
                    if( bDraw ) {
                        ppu->Scanline( scanline, 1, 1 );
                    } else {
                        if( pad->IsZapperMode() && scanline == ZapperY ) {
                            ppu->Scanline( scanline, 1, 1 );
                        } else {
                            if( !ppu->IsSprite0( scanline ) ) {
                                ppu->DummyScanline( scanline );
                            } else {
                                ppu->Scanline( scanline, 1, 1 );
                            }
                        }
                    }
                    ppu->ScanlineNext();            
                    if( RenderMethod == PRE_ALL_RENDER )
                        EmulationCPU( nescfg->ScanlineCycles );
                    mapper->HSync( scanline );
                    ppu->ScanlineStart();
                } else {
                    if( RenderMethod == POST_RENDER )
                        EmulationCPU( nescfg->HDrawCycles );
                    if( bDraw ) {
                        ppu->Scanline( scanline,1, 1 );
                    } else {
                        if( pad->IsZapperMode() && scanline == ZapperY ) {
                            ppu->Scanline( scanline, 1, 1 );
                        } else {
                            if( !ppu->IsSprite0( scanline ) ) {
                                ppu->DummyScanline( scanline );
                            } else {
                                ppu->Scanline( scanline, 1, 1 );
                            }
                        }
                    }
                    if( RenderMethod == PRE_RENDER )
                        EmulationCPU( nescfg->HDrawCycles );
                    ppu->ScanlineNext();
                    mapper->HSync( scanline );
                    EmulationCPU( FETCH_CYCLES*32 );
                    ppu->ScanlineStart();
                    EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
                }
            } else  if( scanline == 240 ) {
                mapper->VSync();
                if( RenderMethod < POST_RENDER ) {
                    EmulationCPU( nescfg->ScanlineCycles );
                    mapper->HSync( scanline );
                } else {
                    EmulationCPU( nescfg->HDrawCycles );
                    mapper->HSync( scanline );
                    EmulationCPU( nescfg->HBlankCycles );
                }
            } else if( scanline <= nescfg->TotalScanlines-1 ) {
                pad->VSync();

                if( scanline == nescfg->TotalScanlines-1 ) {
                    ppu->VBlankEnd();
                }
                if( RenderMethod < POST_RENDER ) {
                    if( scanline == 241 ) {
                        ppu->VBlankStart();
                        if( mmu->PPUREG[0] & PPU_VBLANK_BIT ) {
                            cpu->NMI();
                        }
                    }
                    EmulationCPU( nescfg->ScanlineCycles );
                    mapper->HSync( scanline );
                } else {
                    if( scanline == 241 ) {
                        ppu->VBlankStart();
                        if( mmu->PPUREG[0] & PPU_VBLANK_BIT ) {
                            cpu->NMI();
                        }
                    }
                    EmulationCPU( nescfg->HDrawCycles );
                    mapper->HSync( scanline );
                    EmulationCPU( nescfg->HBlankCycles );
                }

                if( scanline == nescfg->TotalScanlines-1 ) {
                    break;
                }
            }
            if( pad->IsZapperMode() ) {
                if( scanline == ZapperY )
                    bZapper = TRUE;
                else
                    bZapper = FALSE;
            }

            scanline++;
            NES_scanline = scanline;
        }
    } else {
        bZapper = FALSE;
        while( TRUE ) {
            ppu->SetRenderScanline( scanline );

            if( scanline == 0 ) {
                // H-Draw (4fetches*32)
                EmulationCPU( FETCH_CYCLES*128 );
                ppu->FrameStart();
                ppu->ScanlineNext();
                EmulationCPU( FETCH_CYCLES*10 );
                mapper->HSync( scanline );
                EmulationCPU( FETCH_CYCLES*22 );
                ppu->ScanlineStart();
                EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
            } else if( scanline < 240 ) {
                if( bDraw ) {
                    ppu->Scanline( scanline, 1, 1 );
                    ppu->ScanlineNext();
                    EmulationCPU( FETCH_CYCLES*10 );
                    mapper->HSync( scanline );
                    EmulationCPU( FETCH_CYCLES*22 );
                    ppu->ScanlineStart();
                    EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
                } else {
                    if( pad->IsZapperMode() && scanline == ZapperY ) {
                        ppu->Scanline( scanline, 1, 1 );
                        ppu->ScanlineNext();
                        EmulationCPU( FETCH_CYCLES*10 );
                        mapper->HSync( scanline );
                        EmulationCPU( FETCH_CYCLES*22 );
                        ppu->ScanlineStart();
                        EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
                    } else {
                        if( !ppu->IsSprite0( scanline ) ) {
                            // H-Draw (4fetches*32)
                            EmulationCPU( FETCH_CYCLES*128 );
                            ppu->DummyScanline( scanline );
                            ppu->ScanlineNext();
                            EmulationCPU( FETCH_CYCLES*10 );
                            mapper->HSync( scanline );
                            EmulationCPU( FETCH_CYCLES*22 );
                            ppu->ScanlineStart();
                            EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
                        } else {
                            ppu->Scanline( scanline, 1, 1 );
                            ppu->ScanlineNext();
                            EmulationCPU( FETCH_CYCLES*10 );
                            mapper->HSync( scanline );
                            EmulationCPU( FETCH_CYCLES*22 );
                            ppu->ScanlineStart();
                            EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
                        }
                    }
                }
            } else if( scanline == 240 ) {          
                mapper->VSync();

                EmulationCPU( nescfg->HDrawCycles );
                // H-Sync
                mapper->HSync( scanline );

                EmulationCPU( nescfg->HBlankCycles );
            } else if( scanline <= nescfg->TotalScanlines-1 ) {
                pad->VSync();

                if( scanline == nescfg->TotalScanlines-1 ) {
                    ppu->VBlankEnd();
                }
                if( scanline == 241 ) {
                    ppu->VBlankStart();
                    if( mmu->PPUREG[0]&PPU_VBLANK_BIT ) {
                        cpu->NMI();
                    }
                }
                EmulationCPU( nescfg->HDrawCycles );

                // H-Sync
                mapper->HSync( scanline );

                EmulationCPU( nescfg->HBlankCycles );

                if( scanline == nescfg->TotalScanlines-1 ) {
                    break;
                }
            }
            if( pad->IsZapperMode() ) {
                if( scanline == ZapperY )
                    bZapper = TRUE;
                else
                    bZapper = FALSE;
            }

            scanline++;
            NES_scanline = scanline;
        }
    }

    return OK;
}

int CNesEmu::EmulateNSF()
{
    CNesR6502   reg;

    ppu->Reset();
    mapper->VSync();    

    if( m_bNsfPlaying ) {
        if( m_bNsfInit ) {
            memset( mmu->RAM, 0,NES_RAM_SIZE);
            if( !(rom->GetNsfHeader()->ExtraChipSelect&0x04) ) {
                memset( mmu->WRAM, 0,0x2000 );
            }

            apu->Reset();
            apu->Write( 0x4015, 0x0F );
            apu->Write( 0x4017, 0xC0 );
            apu->ExWrite( 0x4080, 0x80 );   // FDS Volume 0
            apu->ExWrite( 0x408A, 0xE8 );   // FDS Envelope Speed

            cpu->GetContext( &reg );
            reg.PC = 0x4710;    // Init Address
            reg.A  = (BYTE)m_nNsfSongNo;
            reg.X  = (BYTE)m_nNsfSongMode;
            reg.Y  = 0;
            reg.S  = 0xFF;
            reg.P  = Z_FLAG|R_FLAG|I_FLAG;
            cpu->SetContext( &reg );

            for( int i = 0; i < nescfg->TotalScanlines*60; i++ ) {
                EmulationCPU( nescfg->ScanlineCycles );
                cpu->GetContext( &reg );

                if( reg.PC == 0x4700 ) {
                    break;
                }
            }

            m_bNsfInit = FALSE;
        }

        cpu->GetContext( &reg );

        if( reg.PC == 0x4700 ) {
            reg.PC = 0x4720;    // Play Address
            reg.A  = 0;
            reg.S  = 0xFF;
            cpu->SetContext( &reg );
        }

        for( int i = 0; i < nescfg->TotalScanlines; i++ ) {
            EmulationCPU( nescfg->ScanlineCycles );
        }
    } else {
        cpu->GetContext( &reg );
        reg.PC = 0x4700;
        reg.S  = 0xFF;
        cpu->SetContext( &reg );

        EmulationCPU( nescfg->ScanlineCycles*nescfg->TotalScanlines );
    }

    return OK;
}


int CNesEmu::LoadFile(CFileBase *file)
{
    ASSERT(file);
    
    this->rom->LoadRom(file);
    
    this->mapper->CreateMapper(this->rom->GetMapperNo());

    this->SetIrqType(IRQ_HSYNC);
    this->SetFrameIRQmode(0);
    this->SetVideoMode(0);

    return OK;
}

int CNesEmu::SetIrqType(int nType)
{
    this->nIRQtype = nType;
    return OK;
}

int CNesEmu::GetIrqType()
{
    return this->nIRQtype;
    return OK;
}

int CNesEmu::SetFrameIRQmode(BOOL bMode)
{
    this->bFrameIRQ = bMode;
    return OK;
}


int CNesEmu::SetVideoMode(BOOL bMode)
{
    bVideoMode = bMode;
    if( !bVideoMode ) {
        nescfg->Init_NTSC();
    } else {
        nescfg->Init_PAL();
    }
    apu->SoundSetup();

    return OK;
}

int CNesEmu::Reset()
{
    // RAM Clear
    mmu->MemoryInitial();

    if( rom->GetPROM_CRC() == 0x29401686 ) {    // Minna no Taabou no Nakayoshi Dai Sakusen(J)
        ::memset( mmu->RAM, 0xFF, NES_RAM_SIZE);
    }

    // RAM set
    if( !rom->IsSAVERAM() && rom->GetMapperNo() != 20 ) {
        ::memset( mmu->WRAM, 0xFF, NES_WRAM_SIZE );
    }

    m_bDiskThrottle = FALSE;

    SetRenderMethod( PRE_RENDER );

    if( rom->IsPAL() ) {
        SetVideoMode( TRUE );
    }

    mmu->PROM = rom->GetPROM();
    mmu->VROM = rom->GetVROM();
    ASSERT(mmu->PROM);  

    mmu->PROM_8K_SIZE  = rom->GetPROM_SIZE()*2;
    mmu->PROM_16K_SIZE = rom->GetPROM_SIZE();
    mmu->PROM_32K_SIZE = rom->GetPROM_SIZE()/2;

    mmu->VROM_1K_SIZE = rom->GetVROM_SIZE()*8;
    mmu->VROM_2K_SIZE = rom->GetVROM_SIZE()*4;
    mmu->VROM_4K_SIZE = rom->GetVROM_SIZE()*2;
    mmu->VROM_8K_SIZE = rom->GetVROM_SIZE();

    if( mmu->VROM_8K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    } else {
        mmu->SetCRAM_8K_Bank( 0 );
    }

    if( rom->Is4SCREEN() ) {
        mmu->SetVRAM_Mirror( VRAM_MIRROR4 );
    } else if( rom->IsVMIRROR() ) {
        mmu->SetVRAM_Mirror( VRAM_VMIRROR );
    } else {
        mmu->SetVRAM_Mirror( VRAM_HMIRROR );
    }

    apu->SelectExSound( 0 );

    ppu->Reset();
    ASSERT( mapper->Reset() );

    // Trainer
    if( rom->IsTRAINER() ) {
        ::memcpy( mmu->WRAM+0x1000, rom->GetTRAINER(), 512 );
    }

    pad->Reset();
    cpu->Reset();
    apu->Reset();

    if( rom->IsNSF() ) {
        mapper->Reset();
    }

    base_cycles = emul_cycles = 0;

    return OK;
}

int CNesEmu::LoadFile(char *fn)
{
    CMem mem;

    mem.Init();
    mem.LoadFile(fn);

    return this->LoadFile(&mem);
}

int CNesEmu::GetScanline()
{
    return NES_scanline;
}

int CNesEmu::SetSAVERAM_SIZE(int size)
{
    SAVERAM_SIZE = size;
    return OK;
}

int CNesEmu::GetSAVERAM_SIZE()
{
    return this->SAVERAM_SIZE;
}

int CNesEmu::WriteBMP(CFileBase *file)
{
    ASSERT(file);
    DWORD t;

    file->SetSize(0);   
    file->Putc('B');
    file->Putc('M');
    
    t = 0xf436; file->Write(&t,4); //file size
    t = 0; file->Write(&t,4);   //reserved
    t = 1078;file->Write(&t,4); //off bits
    t = 40;file->Write(&t,4);   //biSize;
    t = 256; file->Write(&t,4); //width
    t = 240; file->Write(&t,4); //height
    t = 1; file->Write(&t,2);   //planes
    t = 8; file->Write(&t,2);   //Bit count
    t = 0; file->Write(&t,4);   //Compress
    t = 0; file->Write(&t,4);   //size of image
    t = 0; file->Write(&t,4);   
    t = 0; file->Write(&t,4);
    t = 256; file->Write(&t,4);
    t = 0; file->Write(&t,4);

    file->Write(&_rgbQuard,64*4);
    file->Write(&_rgbQuard,64*4);
    file->Write(&_rgbQuard,64*4);
    file->Write(&_rgbQuard,64*4);

    BYTE *p = this->screen_bit + 8;
    int i;

    for(i = 239; i >= 0; i-- ) 
    {
        file->Write(&p[NES_SCREEN_WIDTH*i], 256);
    }

    return OK;
}

int CNesEmu::WriteBMP(char *fn)
{
    ASSERT(fn);
    CFile file;

    file.Init();
    ASSERT( file.OpenFile(fn,"wb+") );
    return this->WriteBMP(&file);
}

int CNesEmu::SaveState(CFileBase *file)
{
    ASSERT(file);

    int i;
    
    file->SetSize(0);
    
    file->Write(&this->cpu->R.PC,sizeof(WORD));
    file->Write(&this->cpu->R.A,sizeof(BYTE));
    file->Write(&this->cpu->R.P,sizeof(BYTE));
    file->Write(&this->cpu->R.X,sizeof(BYTE));
    file->Write(&this->cpu->R.Y,sizeof(BYTE));
    file->Write(&this->cpu->R.S,sizeof(BYTE));
    file->Write(&this->cpu->R.INT_pending,sizeof(BYTE));

    file->Write(mmu->PPUREG,4);
    file->Write(&mmu->PPU7_Temp,1);
    file->Write(&mmu->loopy_t,2);
    file->Write(&mmu->loopy_v,2);
    file->Write(&mmu->loopy_x,2);
    file->Write(&mmu->PPU56Toggle,1);

    file->Write(mmu->RAM,2*1024);
    file->Write(mmu->BGPAL,16);
    file->Write(mmu->SPPAL,16);
    file->Write(mmu->SPRAM,256);

    if(rom->IsSAVERAM())
        file->Write(mmu->WRAM,8*1024);

    file->Write(mmu->CPU_MEM_TYPE,8);
    file->Write(mmu->CPU_MEM_PAGE,8*4);
    file->Write(mmu->PPU_MEM_TYPE,12);
    file->Write(mmu->PPU_MEM_PAGE,12*4);
    file->Write(mmu->CRAM_USED,8);

    for(i = 3; i < 8; i++)
    {
        if(mmu->CPU_MEM_TYPE[i] != BANKTYPE_ROM)
        {
            file->Write(mmu->CPU_MEM_BANK[i],8*1024);
        }
    }

    file->Write(mmu->VRAM,4*1024);

    for(i = 0; i < 8; i++)
    {
        if(mmu->CRAM_USED[i] != 0)
        {
            file->Write(&mmu->CRAM[0x1000 *i],4*1024);
        }
    }
    
    if(mapper->IsStateSave())
    {
        BYTE buf[256];
        mapper->SaveState(buf);
        file->Write(buf,256);
    }

    if(rom->GetMapperNo() == 20)
    {
        BYTE *lpDisk  = rom->GetPROM();
        BYTE *lpWrite = rom->GetDISK();
        int  DiskSize = 16+65500*rom->GetDiskNo();
        int DifferentSize = 0;
        DWORD data;
    
        for(i = 16; i < DiskSize; i++)
        {
            if(lpWrite[i])
                DifferentSize++;
        }

        file->Write(&DifferentSize,4);

        for(i = 16; i < DiskSize; i++)
        {
            if(lpWrite[i])
            {
                data = i & 0x00ffffff;
                data |= ((DWORD)lpDisk[i]&0xFF)<<24;
                file->Write(&data,4);
            }
        }
    }

    return OK;
}

int CNesEmu::LoadState(CFileBase *file)
{
    ASSERT(file);

    int i;

    file->Seek(0);
    
    file->Read(&this->cpu->R.PC,sizeof(WORD));
    file->Read(&this->cpu->R.A,sizeof(BYTE));
    file->Read(&this->cpu->R.P,sizeof(BYTE));
    file->Read(&this->cpu->R.X,sizeof(BYTE));
    file->Read(&this->cpu->R.Y,sizeof(BYTE));
    file->Read(&this->cpu->R.S,sizeof(BYTE));
    file->Read(&this->cpu->R.INT_pending,sizeof(BYTE));

    file->Read(mmu->PPUREG,4);
    file->Read(&mmu->PPU7_Temp,1);
    file->Read(&mmu->loopy_t,2);
    file->Read(&mmu->loopy_v,2);
    file->Read(&mmu->loopy_x,2);
    file->Read(&mmu->PPU56Toggle,1);

    file->Read(mmu->RAM,2*1024);
    file->Read(mmu->BGPAL,16);
    file->Read(mmu->SPPAL,16);
    file->Read(mmu->SPRAM,256);

    if(rom->IsSAVERAM())
        file->Read(mmu->WRAM,8*1024);

    file->Read(mmu->CPU_MEM_TYPE,8);
    file->Read(mmu->CPU_MEM_PAGE,8*4);
    file->Read(mmu->PPU_MEM_TYPE,12);
    file->Read(mmu->PPU_MEM_PAGE,12*4);
    file->Read(mmu->CRAM_USED,8);

    for(i = 3; i < 8; i++)
    {
        if(mmu->CPU_MEM_TYPE[i] != BANKTYPE_ROM)
        {
            file->Read(mmu->CPU_MEM_BANK[i],8*1024);
        }
        else
        {
            mmu->SetPROM_8K_Bank(i,mmu->CPU_MEM_PAGE[i]);
        }
    }

    file->Read(mmu->VRAM,4*1024);

    for(i = 0; i < 8; i++)
    {
        if(mmu->CRAM_USED[i] != 0)
        {
            file->Read(&mmu->CRAM[0x1000 *i],4*1024);
        }
    }
    
    for( i = 0; i < 12; i++ ) 
    {
        if( mmu->PPU_MEM_TYPE[i] == BANKTYPE_VROM ) 
        {
            mmu->SetVROM_1K_Bank( i, mmu->PPU_MEM_PAGE[i] );
        }
        else if( mmu->PPU_MEM_TYPE[i] == BANKTYPE_CRAM ) 
        {
            mmu->SetCRAM_1K_Bank( i, mmu->PPU_MEM_PAGE[i] );
        }
        else if( mmu->PPU_MEM_TYPE[i] == BANKTYPE_VRAM ) 
        {
            mmu->SetVRAM_1K_Bank( i, mmu->PPU_MEM_PAGE[i] );
        }
    }
    
                
    if(mapper->IsStateSave())
    {
        BYTE buf[256];
        
        file->Read(buf,256);
        mapper->LoadState(buf);
    }

    if(rom->GetMapperNo() == 20)
    {
        BYTE *lpDisk  = rom->GetPROM();
        BYTE *lpWrite = rom->GetDISK();
        DWORD  DiskSize = 16+65500*rom->GetDiskNo();
        int DifferentSize = 0;
        BYTE data;
        DWORD pos;
    
        file->Read(&DifferentSize,4);

        for(i = 0; i < DifferentSize; i++)
        {
            file->Read(&pos,4);
            data = (BYTE)(pos>>24);
            pos &= 0x00FFFFFF;
            if( pos >= 16 && pos < DiskSize ) 
            {
                lpDisk[pos] = data;
                lpWrite[pos] = 0xFF;
            }           
        }
    }

    return OK;
}
