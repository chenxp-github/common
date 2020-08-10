// NesMapper.cpp: implementation of the CNesMapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NesMapper.h"
#include "syslog.h"
#include "nesemu.h"
#include "nesmmu.h"
#include "nesrom.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nescpu.h"

#define MMC3_IRQ_KLAX       1
#define MMC3_IRQ_SHOUGIMEIKAN   2
#define MMC3_IRQ_DAI2JISUPER    3
#define MMC3_IRQ_DBZ2       4
#define MMC3_IRQ_ROCKMAN3   5

static int _save_state_mapper_table[] = {
1,4,5,6,9,10,12,16,17,18,19,21,23,24,25,26,27,32,33,40,41,42,43,44,45,46,47,48,50,51,57,64,65,67,68,69,
73,74,75,76,82,83,85,88,90,91,95,96,100,105,109,110,112,114,115,116,117,118,119,134,135,142,160,164,165,
167,182,183,187,189,190,191,198,226,232,234,236,243,245,248,249,251,252,254,255};

#define SAVE_STATE_MAPPER_TABLE_SIZE (sizeof(_save_state_mapper_table) / sizeof(int))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNesMapper::CNesMapper()
{
    this->InitBasic();
}
CNesMapper::~CNesMapper()
{
    this->Destroy();
}
int CNesMapper::InitBasic()
{
    this->current_mapper = -1;
    this->mmu = NULL;
    this->nes = NULL;
    this->patch = 0;
    this->irq_clock = 0;
    this->irq_counter = 0;
    this->irq_enable = 0;
    this->irq_latch = 0;

    return OK;
}
int CNesMapper::Init()
{
    this->InitBasic();    
    return OK;
}
int CNesMapper::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
int CNesMapper::Copy(CNesMapper *p)
{
    if(this == p)
        return OK;
    return OK;
}
int CNesMapper::Comp(CNesMapper *p)
{
    return 0;
}
int CNesMapper::Print()
{
    return TRUE;
}

int CNesMapper::Reset()
{
    switch(this->current_mapper)
    {
        case 0: return this->Mapper000_Reset(); break;
        case 1: return this->Mapper001_Reset(); break;
        case 2: return this->Mapper002_Reset(); break;
        case 3: return this->Mapper003_Reset(); break;
        case 4: return this->Mapper004_Reset(); break;
        case 7: return this->Mapper007_Reset(); break;
        case 15: return this->Mapper015_Reset(); break;
        case 17: return this->Mapper017_Reset(); break;
        case 18: return this->Mapper018_Reset(); break;
        case 19: return this->Mapper019_Reset(); break;
        case 22: return this->Mapper022_Reset(); break;
        case 23: return this->Mapper023_Reset(); break;
        case 24: return this->Mapper024_Reset(); break;
        case 25: return this->Mapper025_Reset(); break;
        case 34: return this->Mapper034_Reset(); break;
        case 62: return this->Mapper062_Reset(); break;
        case 69: return this->Mapper069_Reset(); break;
        case 71: return this->Mapper071_Reset(); break;
        case 73: return this->Mapper073_Reset(); break;
        case 74: return this->Mapper074_Reset(); break;
        case 78: return this->Mapper078_Reset(); break;
        case 91: return this->Mapper091_Reset(); break;
        case 114: return this->Mapper114_Reset(); break;
        case 164: return this->Mapper164_Reset(); break;
        case 240: return this->Mapper240_Reset(); break;
        case 241: return this->Mapper241_Reset(); break;
        case 245: return this->Mapper245_Reset(); break;
        case 246: return this->Mapper246_Reset(); break;
        case 254: return this->Mapper254_Reset(); break;
        default: 
            LOG("unsupported mapper number %d\n",this->current_mapper);
            return ERROR;

    }

    return OK;
}

BYTE CNesMapper::ReadLow_Def( WORD addr )
{
    if( addr >= 0x6000 && addr <= 0x7FFF ) {
        return  mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF];
    }

    return  (BYTE)(addr>>8);
}

// $4100-$7FFF Lower Memory read
BYTE CNesMapper::ReadLow( WORD addr )
{
    switch(this->current_mapper)
    {
        case 4: return this->Mapper004_ReadLow(addr); break;
        case 254:   return this->Mapper254_ReadLow(addr); break;
        case 19: return this->Mapper019_ReadLow(addr);break;
        default: return this->ReadLow_Def(addr);
    }
    return  0;
}
int CNesMapper::WriteLow_Def( WORD addr, BYTE data )
{
    // $6000-$7FFF WRAM
    if( addr >= 0x6000 && addr <= 0x7FFF ) {
        mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
    }
    return OK;
}
// $4100-$7FFF Lower Memory write
int CNesMapper::WriteLow( WORD addr, BYTE data )
{
    switch(this->current_mapper)
    {
        case 2:return this->Mapper002_WriteLow(addr,data); break;
        case 4:return this->Mapper004_WriteLow(addr,data); break;
        case 17: return this->Mapper017_WriteLow(addr,data); break;
        case 19: return this->Mapper019_WriteLow(addr,data); break;
        case 34:return this->Mapper034_WriteLow(addr,data); break;
        case 71:return this->Mapper071_WriteLow(addr,data); break;
        case 91:return this->Mapper091_WriteLow(addr,data); break;
        case 114:return this->Mapper114_WriteLow(addr,data); break;
        case 164:return this->Mapper164_WriteLow(addr,data); break;
        case 240:return this->Mapper240_WriteLow(addr,data); break;
        case 246:return this->Mapper246_WriteLow(addr,data); break;
        case 254:return this->Mapper254_WriteLow(addr,data); break;
        default: this->WriteLow_Def(addr,data);
    }

    return OK;
}

// $8000-$FFFF Memory write
int CNesMapper::Write( WORD addr, BYTE data ) 
{
    switch(this->current_mapper)
    {
        case 1: return this->Mapper001_Write(addr,data); break;
        case 2: return this->Mapper002_Write(addr,data); break;
        case 3: return this->Mapper003_Write(addr,data); break;
        case 4: return this->Mapper004_Write(addr,data); break;
        case 7: return this->Mapper007_Write(addr,data); break;
        case 15: return this->Mapper015_Write(addr,data); break;
        case 18: return this->Mapper018_Write(addr,data); break;
        case 19: return this->Mapper019_Write(addr,data); break;
        case 22: return this->Mapper022_Write(addr,data); break;
        case 23: return this->Mapper023_Write(addr,data); break;
        case 24: return this->Mapper024_Write(addr,data); break;
        case 25: return this->Mapper025_Write(addr,data); break;
        case 34: return this->Mapper034_Write(addr,data); break;
        case 62: return this->Mapper062_Write(addr,data); break;
        case 69: return this->Mapper069_Write(addr,data); break;
        case 71: return this->Mapper071_Write(addr,data); break;
        case 73: return this->Mapper073_Write(addr,data); break;
        case 74: return this->Mapper074_Write(addr,data); break;
        case 78: return this->Mapper078_Write(addr,data); break;
        case 114: return this->Mapper114_Write(addr,data); break;
        case 241: return this->Mapper241_Write(addr,data); break;
        case 245: return this->Mapper245_Write(addr,data); break;
        case 254: return this->Mapper254_Write(addr,data); break;
    }
    return OK;
}

// $8000-$FFFF Memory read(Dummy)
int CNesMapper::Read( WORD addr, BYTE data ) 
{
    return OK;
}

// $4018-$40FF Extention register read/write
BYTE CNesMapper::ExRead( WORD addr )    
{ 
    return 0x00; 
}

int CNesMapper::ExWrite( WORD addr, BYTE data ) 
{
    return OK;
}

BYTE CNesMapper::ExCmdRead ( int cmd )  
{ 
    return 0x00; 
}

int CNesMapper::intite( int cmd, BYTE data ) 
{
    return OK;
}

// H sync/V sync/Clock sync
int CNesMapper::HSync( int scanline ) 
{
    switch(this->current_mapper)
    {
        case 4: return this->Mapper004_HSync(scanline); break;
        case 17:return this->Mapper017_HSync(scanline); break;
        case 69: return this->Mapper069_HSync(scanline); break;
        case 74: return this->Mapper074_HSync(scanline); break;
        case 91: return this->Mapper091_HSync(scanline); break;
        case 114: return this->Mapper114_HSync(scanline); break;
        case 245: return this->Mapper245_HSync(scanline); break;
        case 254: return this->Mapper254_HSync(scanline); break;
    }
    return OK;
}
int CNesMapper::VSync() 
{
    return OK;
}
int CNesMapper::Clock( int cycles )
{
    switch(this->current_mapper)
    {
        case 18: return this->Mapper018_Clock(cycles); break;
        case 19: return this->Mapper019_Clock(cycles); break;
        case 23: return this->Mapper023_Clock(cycles); break;
        case 24: return this->Mapper024_Clock(cycles); break;       
        case 25: return this->Mapper025_Clock(cycles); break;       
        case 69: return this->Mapper069_Clock(cycles); break;       
        case 73: return this->Mapper073_Clock(cycles); break;       
        case 114: return this->Mapper114_Clock(cycles); break;      
        case 245: return this->Mapper245_Clock(cycles); break;          
        case 254: return this->Mapper254_Clock(cycles); break;      
    }

    return OK;
}

// PPU address bus latch
int CNesMapper::PPU_Latch( WORD addr )
{
    return OK;
}

// PPU Character latch
int CNesMapper::PPU_ChrLatch( WORD addr )
{
    return OK;
}

// PPU Extention character/palette
int CNesMapper::PPU_ExtLatchX( int x )
{
    switch(this->current_mapper)
    {
        case 164: return this->Mapper164_PPU_ExtLatchX(x); break;
    }
    return OK;
}
int CNesMapper::PPU_ExtLatch( WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
    switch(this->current_mapper)
    {
        case 164: return this->Mapper164_PPU_ExtLatch(addr,chr_l,chr_h,attr); break;
    }

    return OK;
}

// For State save
BOOL CNesMapper::IsStateSave() 
{ 
    int i;

    for(i = 0; i < SAVE_STATE_MAPPER_TABLE_SIZE; i++)
    {
        if(this->current_mapper == _save_state_mapper_table[i])
            return TRUE;
    }

    return FALSE; 
}
int CNesMapper::SaveState( BYTE * p ) 
{
    switch(this->current_mapper)
    {
        case 1: return this->Mapper001_SaveState(p); break;
        case 4: return this->Mapper004_SaveState(p); break;
        case 17: return this->Mapper017_SaveState(p); break;
        case 18: return this->Mapper018_SaveState(p); break;
        case 19: return this->Mapper019_SaveState(p); break;
        case 23: return this->Mapper023_SaveState(p); break;
        case 24: return this->Mapper024_SaveState(p); break;
        case 25: return this->Mapper025_SaveState(p); break;
        case 69: return this->Mapper069_SaveState(p); break;
        case 73: return this->Mapper073_SaveState(p); break;
        case 74: return this->Mapper074_SaveState(p); break;
        case 91: return this->Mapper091_SaveState(p); break;
        case 114: return this->Mapper114_SaveState(p); break;
        case 164: return this->Mapper164_SaveState(p); break;
        case 245: return this->Mapper245_SaveState(p); break;
        case 254: return this->Mapper254_SaveState(p); break;
        default: LOG("unsupported mapper no %d WriteState\n",this->current_mapper);
    }
    return OK;
}
int CNesMapper::LoadState( BYTE * p ) 
{
    switch(this->current_mapper)
    {
        case 1: return this->Mapper001_LoadState(p); break;
        case 4: return this->Mapper004_LoadState(p); break;
        case 17: return this->Mapper017_LoadState(p); break;
        case 18: return this->Mapper018_LoadState(p); break;
        case 19: return this->Mapper019_LoadState(p); break;
        case 23: return this->Mapper023_LoadState(p); break;
        case 24: return this->Mapper024_LoadState(p); break;
        case 25: return this->Mapper025_LoadState(p); break;
        case 69: return this->Mapper069_LoadState(p); break;
        case 73: return this->Mapper073_LoadState(p); break;
        case 74: return this->Mapper074_LoadState(p); break;
        case 91: return this->Mapper091_LoadState(p); break;
        case 114: return this->Mapper114_LoadState(p); break;
        case 164: return this->Mapper164_LoadState(p); break;
        case 245: return this->Mapper245_LoadState(p); break;       
        case 254: return this->Mapper254_LoadState(p); break;
        default: LOG("unsupported mapper no %d LoadState\n",this->current_mapper);
    }
    return OK;
}

int CNesMapper::CreateMapper(int num)
{
    this->current_mapper = num;
    return OK;
}

int CNesMapper::Mapper000_Reset()
{
    switch( mmu->PROM_16K_SIZE ) {
        default:
        case    1:  // 16K only
            mmu->SetPROM_16K_Bank( 4, 0 );
            mmu->SetPROM_16K_Bank( 6, 0 );
            break;
        case    2:  // 32K
            mmu->SetPROM_32K_Bank( 0 );
            break;
    }

    DWORD   crc = nes->rom->GetPROM_CRC();
    if( crc == 0x4e7db5af ) {   // Circus Charlie(J)
        nes->SetRenderMethod( POST_RENDER );
    }
    if( crc == 0x57970078 ) {   // F-1 Race(J)
        nes->SetRenderMethod( POST_RENDER );
    }
    if( crc == 0xaf2bbcbc       // Mach Rider(JU)
     || crc == 0x3acd4bf1 ) {   // Mach Rider(Alt)(JU)
        nes->SetRenderMethod( POST_RENDER );
    }

    return OK;
}

int CNesMapper::Mapper002_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    patch = 0;

    DWORD   crc = nes->rom->GetPROM_CRC();

    if( crc == 0x8c3d54e8       // Ikari(J)
     || crc == 0x655efeed       // Ikari Warriors(U)
     || crc == 0x538218b2 ) {   // Ikari Warriors(E)
        patch = 1;
    }

    if( crc == 0xb20c1030 ) {   // Shanghai(J)(original)
        patch = 2;
    }

    return OK;
}

int CNesMapper::Mapper002_Write(WORD addr, BYTE data)
{
    if( patch != 2 )
        mmu->SetPROM_16K_Bank( 4, data );
    else
        mmu->SetPROM_16K_Bank( 4, data>>4 );
    return OK;
}

int CNesMapper::Mapper002_WriteLow(WORD addr, BYTE data)
{
    if( !nes->rom->IsSAVERAM() ) {
        if( addr >= 0x5000 && patch == 1 )
            mmu->SetPROM_16K_Bank( 4, data );
    } else {
        if( addr >= 0x6000 && addr <= 0x7FFF ) {
            mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
        }
    }

    return OK;
}

int CNesMapper::Mapper007_Reset()
{
    patch = 0;

    mmu->SetPROM_32K_Bank( 0 );
    mmu->SetVRAM_Mirror( VRAM_MIRROR4L );

    DWORD   crc = nes->rom->GetPROM_CRC();
    if( crc == 0x3c9fe649 ) {   // WWF Wrestlemania Challenge(U)
        mmu->SetVRAM_Mirror( VRAM_VMIRROR );
        patch = 1;
    }
    if( crc == 0x09874777 ) {   // Marble Madness(U)
        nes->SetRenderMethod( TILE_RENDER );
    }

    if( crc == 0x279710DC       // Battletoads (U)
     || crc == 0xCEB65B06 ) {   // Battletoads Double Dragon (U)
        nes->SetRenderMethod( PRE_ALL_RENDER );
        ::memset( mmu->WRAM, 0, NES_WRAM_SIZE );
    }

    return OK;
}

int CNesMapper::Mapper007_Write(WORD addr, BYTE data)
{
    mmu->SetPROM_32K_Bank( data & 0x07 );

    if( !patch ) {
        if( data & 0x10 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
        else          mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
    }
    return OK;
}

int CNesMapper::Mapper003_Reset()
{
    switch( mmu->PROM_16K_SIZE ) {
        case    1:  // 16K only
            mmu->SetPROM_16K_Bank( 4, 0 );
            mmu->SetPROM_16K_Bank( 6, 0 );
            break;
        case    2:  // 32K
            mmu->SetPROM_32K_Bank( 0 );
            break;
    }
    DWORD   crc = nes->rom->GetPROM_CRC();

    if( crc == 0x2b72fe7e ) {   // Ganso Saiyuuki - Super Monkey Dai Bouken(J)
        nes->SetRenderMethod( TILE_RENDER );
        nes->ppu->SetExtNameTableMode( TRUE );
    }
    return OK;
}

int CNesMapper::Mapper003_Write(WORD addr, BYTE data)
{
    mmu->SetVROM_8K_Bank( data );
    return OK;
}

int CNesMapper::CheckLinkPtr()
{   
    ASSERT(this->mmu);
    ASSERT(this->nes);

    return OK;
}

int CNesMapper::Mapper024_Reset()
{
    irq_enable = 0;
    irq_counter = 0;
    irq_latch = 0;
    irq_clock = 0;

    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }
    nes->SetRenderMethod( POST_RENDER );
    nes->apu->SelectExSound( 1 );
    return OK;
}

int CNesMapper::Mapper024_Write(WORD addr, BYTE data)
{
    switch( addr & 0xF003 ) {
        case 0x8000:
            mmu->SetPROM_16K_Bank( 4, data );
            break;

        case 0x9000: case 0x9001: case 0x9002:
        case 0xA000: case 0xA001: case 0xA002:
        case 0xB000: case 0xB001: case 0xB002:
            nes->apu->ExWrite( addr, data );
            break;

        case 0xB003:
            data = data & 0x0C;
            if( data == 0x00 )  mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            else if( data == 0x04 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
            else if( data == 0x08 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
            else if( data == 0x0C ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
            break;

        case 0xC000:
            mmu->SetPROM_8K_Bank( 6, data );
            break;

        case 0xD000:
            mmu->SetVROM_1K_Bank( 0, data );
            break;

        case 0xD001:
            mmu->SetVROM_1K_Bank( 1, data );
            break;

        case 0xD002:
            mmu->SetVROM_1K_Bank( 2, data );
            break;

        case 0xD003:
            mmu->SetVROM_1K_Bank( 3, data );
            break;

        case 0xE000:
            mmu->SetVROM_1K_Bank( 4, data );
            break;

        case 0xE001:
            mmu->SetVROM_1K_Bank( 5, data );
            break;

        case 0xE002:
            mmu->SetVROM_1K_Bank( 6, data );
            break;

        case 0xE003:
            mmu->SetVROM_1K_Bank( 7, data );
            break;

        case 0xF000:
            irq_latch = data;
            break;
        case 0xF001:
            irq_enable = data & 0x03;
            if( irq_enable & 0x02 ) {
                irq_counter = irq_latch;
                irq_clock = 0;
            }
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case 0xF002:
            irq_enable = (irq_enable & 0x01) * 3;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
    }

    return OK;
}

int CNesMapper::Mapper024_Clock(int cycles)
{
    if( irq_enable & 0x02 ) {
        if( (irq_clock+=cycles) >= 0x72 ) {
            irq_clock -= 0x72;
            if( irq_counter == 0xFF ) {
                irq_counter = irq_latch;
                nes->cpu->SetIRQ( IRQ_MAPPER );
            } else {
                irq_counter++;
            }
        }
    }

    return OK;
}

int CNesMapper::Mapper024_SaveState(BYTE *p)
{
    p[0] = irq_enable;
    p[1] = (BYTE)irq_counter;
    p[2] = irq_latch;

    ::memcpy(p+3,&irq_clock,4);

    return OK;
}

int CNesMapper::Mapper024_LoadState(BYTE *p)
{
    irq_enable  = p[0];
    irq_counter = p[1];
    irq_latch   = p[2];

    ::memcpy(&irq_clock,p+3,4);

    return OK;
}

int CNesMapper::Mapper004_Reset()
{
    int i;

    for( i = 0; i < 8; i++ ) {
        reg[i] = 0x00;
    }

    prg0 = 0;
    prg1 = 1;

    this->Mapper004_SetBank_CPU();

    chr[1] = 0;
    chr[23] = 2;
    chr[4]  = 4;
    chr[5]  = 5;
    chr[6]  = 6;
    chr[7]  = 7;
    this->Mapper004_SetBank_PPU();

    we_sram  = 0;   // Disable
    irq_enable = 0; // Disable
    irq_counter = 0;
    irq_latch = 0xFF;
    irq_request = 0;
    irq_preset = 0;
    irq_preset_vbl = 0;

    nes->SetIrqType( IRQ_CLOCK );
    irq_type = 0;

    DWORD   crc = nes->rom->GetPROM_CRC();

    if( crc == 0x5c707ac4 ) {   // Mother(J)
        nes->SetIrqType( IRQ_HSYNC );
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0xcb106f49 ) {   // F-1 Sensation(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x1170392a ) {   // Karakuri Kengou Den - Musashi Road - Karakuri Nin Hashiru!(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0x14a01c70 ) {   // Gun-Dec(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0xeffeea40 ) {   // For Klax(J)
        irq_type = MMC3_IRQ_KLAX;
        nes->SetIrqType( IRQ_HSYNC );
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0xc17ae2dc ) {   // God Slayer - Haruka Tenkuu no Sonata(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x126ea4a0 ) {   // Summer Carnival '92 - Recca(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0x1f2f4861 ) {   // J League Fighting Soccer - The King of Ace Strikers(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0x5a6860f1 ) {   // Shougi Meikan '92(J)
        irq_type = MMC3_IRQ_SHOUGIMEIKAN;
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0xae280e20 ) {   // Shougi Meikan '93(J)
        irq_type = MMC3_IRQ_SHOUGIMEIKAN;
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0xe19a2473 ) {   // Sugoro Quest - Dice no Senshi Tachi(J)
        nes->SetIrqType( IRQ_HSYNC );
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x702d9b33 ) {   // Star Wars - The Empire Strikes Back(Victor)(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0xa9a0d729 ) {   // Dai Kaijuu - Deburas(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0xc5fea9f2 ) {   // Dai 2 Ji - Super Robot Taisen(J)
        irq_type = MMC3_IRQ_DAI2JISUPER;
    }
    if( crc == 0xd852c2f7 ) {   // Time Zone(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0xecfd3c69 ) {   // Taito Chase H.Q.(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x7a748058 ) {   // Tom & Jerry (and Tuffy)(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0xaafe699c ) {   // Ninja Ryukenden 3 - Yomi no Hakobune(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x6cc62c06 ) {   // Hoshi no Kirby - Yume no Izumi no Monogatari(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x877dba77 ) {   // My Life My Love - Boku no Yume - Watashi no Negai(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0x6f96ed15 ) {   // Max Warrior - Wakusei Kaigenrei(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0x8685f366 ) {   // Matendouji(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x8635fed1 ) {   // Mickey Mouse 3 - Yume Fuusen(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x26ff3ea2 ) {   // Yume Penguin Monogatari(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0x7671bc51 ) {   // Red Ariimaa 2(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0xade11141 ) {   // Wanpaku Kokkun no Gourmet World(J)
        nes->SetIrqType( IRQ_HSYNC );
    }
    if( crc == 0x7c7ab58e ) {   // Walkuere no Bouken - Toki no Kagi Densetsu(J)
        nes->SetRenderMethod( POST_RENDER );
    }
    if( crc == 0x26ff3ea2 ) {   // Yume Penguin Monogatari(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x126ea4a0 ) {   // Summer Carnival '92 Recca(J)
        nes->SetRenderMethod( TILE_RENDER );
    }

    if( crc == 0x1d2e5018       // Rockman 3(J)
     || crc == 0x6b999aaf ) {   // Megaman 3(U)
        irq_type = MMC3_IRQ_ROCKMAN3;
    }

    if( crc == 0xd88d48d7 ) {   // Kick Master(U)
        irq_type = MMC3_IRQ_ROCKMAN3;
    }

    if( crc == 0xA67EA466 ) {   // Alien 3(U)
        nes->SetRenderMethod( TILE_RENDER );
    }

    if( crc == 0xe763891b ) {   // DBZ2
        irq_type = MMC3_IRQ_DBZ2;
    }

    // VS-Unisystem
    vs_patch = 0;
    vs_index = 0;

    if( crc == 0xeb2dba63       // VS TKO Boxing
     || crc == 0x98cfe016 ) {   // VS TKO Boxing (Alt)
        vs_patch = 1;
    }
    if( crc == 0x135adf7c ) {   // VS Atari RBI Baseball
        vs_patch = 2;
    }
    if( crc == 0xf9d3b0a3       // VS Super Xevious
     || crc == 0x9924980a       // VS Super Xevious (b1)
     || crc == 0x66bb838f ) {   // VS Super Xevious (b2)
        vs_patch = 3;
    }

    return OK;
}

int CNesMapper::Mapper004_SetBank_CPU()
{
    if( reg[0] & 0x40 ) {
        mmu->SetPROM_32K_Bank( mmu->PROM_8K_SIZE-2, prg1, prg0, mmu->PROM_8K_SIZE-1 );
    } else {
        mmu->SetPROM_32K_Bank( prg0, prg1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    }
    return OK;
}

int CNesMapper::Mapper004_SetBank_PPU()
{
    if( mmu->VROM_1K_SIZE ) {
        if( reg[0] & 0x80 ) {
            mmu->SetVROM_8K_Bank( chr[4], chr[5], chr[6], chr[7],
                     chr[1], chr[1]+1, chr[23], chr[23]+1 );
        } else {
            mmu->SetVROM_8K_Bank( chr[1], chr[1]+1, chr[23], chr[23]+1,
                     chr[4], chr[5], chr[6], chr[7] );
        }
    } else {
        if( reg[0] & 0x80 ) {
            mmu->SetCRAM_1K_Bank( 4, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 5, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 6, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 7, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 0, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 1, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 2, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 3, chr[7]&0x07 );
        } else {
            mmu->SetCRAM_1K_Bank( 0, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 1, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 2, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 3, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 4, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 5, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 6, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 7, chr[7]&0x07 );
        }
    }

    return OK;
}

int CNesMapper::Mapper004_HSync(int scanline)
{
    if( irq_type == MMC3_IRQ_KLAX ) {
        if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
            if( irq_enable ) {
                if( irq_counter == 0 ) {
                    irq_counter = irq_latch;
                    irq_request = 0xFF;
                }
                if( irq_counter > 0 ) {
                    irq_counter--;
                }
            }
        }
        if( irq_request ) {
            nes->cpu->SetIRQ( IRQ_MAPPER );
        }
    } else if( irq_type == MMC3_IRQ_ROCKMAN3 ) {
        if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
            if( irq_enable ) {
                if( !(--irq_counter) ) {
                    irq_request = 0xFF;
                    irq_counter = irq_latch;
                }
            }
        }
        if( irq_request ) {
            nes->cpu->SetIRQ( IRQ_MAPPER );
        }
    } else {
        if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
            if( irq_preset_vbl ) {
                irq_counter = irq_latch;
                irq_preset_vbl = 0;
            }
            if( irq_preset ) {
                irq_counter = irq_latch;
                irq_preset = 0;
                if( irq_type == MMC3_IRQ_DAI2JISUPER && scanline == 0 ) {
                    irq_counter--;
                }
            } else if( irq_counter > 0 ) {
                irq_counter--;
            }

            if( irq_counter == 0 ) {
                if( irq_enable ) {
                    irq_request = 0xFF;

                    nes->cpu->SetIRQ( IRQ_MAPPER );
                }
                irq_preset = 0xFF;
            }
        }
    }

    return OK;
}

int CNesMapper::Mapper004_SaveState(BYTE *p)
{
    int i;

    for( i = 0; i < 8; i++ ) {
        p[i] = reg[i];
    }
    p[ 8] = prg0;
    p[ 9] = prg1;
    p[10] = chr[1];
    p[11] = chr[23];
    p[12] = chr[4];
    p[13] = chr[5];
    p[14] = chr[6];
    p[15] = chr[7];
    p[16] = irq_enable;
    p[17] = (BYTE)irq_counter;
    p[18] = irq_latch;
    p[19] = irq_request;
    p[20] = irq_preset;
    p[21] = irq_preset_vbl;

    return OK;
}

int CNesMapper::Mapper004_LoadState(BYTE *p)
{
    int i;

    for( i = 0; i < 8; i++ ) {
        reg[i] = p[i];
    }
    prg0  = p[ 8];
    prg1  = p[ 9];
    chr[1] = p[10];
    chr[23] = p[11];
    chr[4]  = p[12];
    chr[5]  = p[13];
    chr[6]  = p[14];
    chr[7]  = p[15];
    irq_enable  = p[16];
    irq_counter = (int)p[17];
    irq_latch   = p[18];
    irq_request = p[19];
    irq_preset  = p[20];
    irq_preset_vbl = p[21];

    return OK;
}

BYTE CNesMapper::Mapper004_ReadLow(WORD addr)
{
    if( !vs_patch ) {
        if( addr >= 0x5000 && addr <= 0x5FFF ) {
            return  mmu->XRAM[addr-0x4000];
        }
    } else if( vs_patch == 1 ) {
        // VS TKO Boxing Security
        if( addr == 0x5E00 ) {
            vs_index = 0;
            return  0x00;
        } else if( addr == 0x5E01 ) {
            BYTE    VS_TKO_Security[32] = {
                0xff, 0xbf, 0xb7, 0x97, 0x97, 0x17, 0x57, 0x4f,
                0x6f, 0x6b, 0xeb, 0xa9, 0xb1, 0x90, 0x94, 0x14,
                0x56, 0x4e, 0x6f, 0x6b, 0xeb, 0xa9, 0xb1, 0x90,
                0xd4, 0x5c, 0x3e, 0x26, 0x87, 0x83, 0x13, 0x00 };
            return  VS_TKO_Security[(vs_index++) & 0x1F];
        }
    } else if( vs_patch == 2 ) {
        // VS Atari RBI Baseball Security
        if( addr == 0x5E00 ) {
            vs_index = 0;
            return  0x00;
        } else if( addr == 0x5E01 ) {
            if( vs_index++ == 9 )
                return  0x6F;
            else
                return  0xB4;
        }
    } else if( vs_patch == 3 ) {
        // VS Super Xevious
        switch( addr ) {
            case    0x54FF:
                return  0x05;
            case    0x5678:
                if( vs_index )
                    return  0x00;
                else
                    return  0x01;
                break;
            case    0x578f:
                if( vs_index )
                    return  0xD1;
                else
                    return  0x89;
                break;
            case    0x5567:
                if( vs_index ) {
                    vs_index = 0;
                    return  0x3E;
                } else {
                    vs_index = 1;
                    return  0x37;
                }
                break;
            default:
                break;
        }
    }

    return  ReadLow_Def( addr );
}

int CNesMapper::Mapper004_WriteLow(WORD addr, BYTE data)
{
    if( addr >= 0x5000 && addr <= 0x5FFF ) {
        mmu->XRAM[addr-0x4000] = data;
    } else {
        WriteLow_Def( addr, data );
    }

    return OK;
}

int CNesMapper::Mapper004_Write(WORD addr, BYTE data)
{
    switch( addr & 0xE001 ) {
        case    0x8000:
            reg[0] = data;
            this->Mapper004_SetBank_CPU();
            this->Mapper004_SetBank_PPU();
            break;
        case    0x8001:
            reg[1] = data;

            switch( reg[0] & 0x07 ) {
                case    0x00:
                    chr[1] = data & 0xFE;
                    this->Mapper004_SetBank_PPU();
                    break;
                case    0x01:
                    chr[23] = data & 0xFE;
                    this->Mapper004_SetBank_PPU();
                    break;
                case    0x02:
                    chr[4] = data;
                    this->Mapper004_SetBank_PPU();
                    break;
                case    0x03:
                    chr[5] = data;
                    this->Mapper004_SetBank_PPU();
                    break;
                case    0x04:
                    chr[6] = data;
                    this->Mapper004_SetBank_PPU();
                    break;
                case    0x05:
                    chr[7] = data;
                    this->Mapper004_SetBank_PPU();
                    break;
                case    0x06:
                    prg0 = data;
                    this->Mapper004_SetBank_CPU();
                    break;
                case    0x07:
                    prg1 = data;
                    this->Mapper004_SetBank_CPU();
                    break;
            }
            break;
        case    0xA000:
            reg[2] = data;
            if( !nes->rom->Is4SCREEN() ) {
                if( data & 0x01 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                else          mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            }
            break;
        case    0xA001:
            reg[3] = data;
            break;
        case    0xC000:
            reg[4] = data;
            if( irq_type == MMC3_IRQ_KLAX || irq_type == MMC3_IRQ_ROCKMAN3 ) {
                irq_counter = data;
            } else {
                irq_latch = data;
            }
            if( irq_type == MMC3_IRQ_DBZ2 ) {
                irq_latch = 0x07;
            }
            break;
        case    0xC001:
            reg[5] = data;
            if( irq_type == MMC3_IRQ_KLAX || irq_type == MMC3_IRQ_ROCKMAN3 ) {
                irq_latch = data;
            } else {
                if( (nes->GetScanline() < 240) || (irq_type == MMC3_IRQ_SHOUGIMEIKAN) ) {
                    irq_counter |= 0x80;
                    irq_preset = 0xFF;
                } else {
                    irq_counter |= 0x80;
                    irq_preset_vbl = 0xFF;
                    irq_preset = 0;
                }
            }
            break;
        case    0xE000:
            reg[6] = data;
            irq_enable = 0;
            irq_request = 0;

            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xE001:
            reg[7] = data;
            irq_enable = 1;
            irq_request = 0;
            break;
    }

    return OK;
}

int CNesMapper::Mapper001_Reset()
{
    reg[0] = 0x0C;      // D3=1,D2=1
    reg[1] = reg[2] = reg[3] = 0;
    shift = regbuf = 0;

    patch = 0;
    wram_patch = 0;

    if( mmu->PROM_16K_SIZE < 32 ) {
        mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    } else {
        // For 512K/1M byte Cartridge
        mmu->SetPROM_16K_Bank( 4, 0 );
        mmu->SetPROM_16K_Bank( 6, 16-1 );

        patch = 1;
    }

    DWORD   crc = nes->rom->GetPROM_CRC();

    if( crc == 0xb8e16bd0 ) {   // Snow Bros.(J)
        patch = 2;
    }
    if( crc == 0xc96c6f04 ) {   // Venus Senki(J)
        nes->SetRenderMethod( POST_ALL_RENDER );
    }

    if( crc == 0x4d2edf70 ) {   // Night Rider(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0xcd2a73f0 ) {   // Pirates!(U)
        nes->SetRenderMethod( TILE_RENDER );
        patch = 2;
    }

    if( crc == 0x11469ce3 ) {   // Viva! Las Vegas(J)
    }
    if( crc == 0xd878ebf5 ) {   // Ninja Ryukenden(J)
        nes->SetRenderMethod( POST_ALL_RENDER );
    }

    if( crc == 0x466efdc2 ) {   // Final Fantasy(J)
        nes->SetRenderMethod( TILE_RENDER );
        nes->ppu->SetExtMonoMode( TRUE );
    }
    if( crc == 0xc9556b36 ) {   // Final Fantasy I&II(J)
        nes->SetRenderMethod( TILE_RENDER );
        nes->ppu->SetExtMonoMode( TRUE );
        nes->SetSAVERAM_SIZE( 16*1024 );
        wram_patch = 2;
    }

    if( crc == 0x717e1169 ) {   // Cosmic Wars(J)
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }

    if( crc == 0xC05D2034 ) {   // Snake's Revenge(U)
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }

    if( crc == 0xb8747abf       // Best Play - Pro Yakyuu Special(J)
     || crc == 0x29449ba9       // Nobunaga no Yabou - Zenkoku Ban(J)
     || crc == 0x2b11e0b0       // Nobunaga no Yabou - Zenkoku Ban(J)(alt)
     || crc == 0x4642dda6       // Nobunaga's Ambition(U)
     || crc == 0xfb69743a       // Aoki Ookami to Shiroki Mejika - Genghis Khan(J)
     || crc == 0x2225c20f       // Genghis Khan(U)
     || crc == 0xabbf7217       // Sangokushi(J)
    ) {

        nes->SetSAVERAM_SIZE( 16*1024 );
        wram_patch = 1;
        wram_bank  = 0;
        wram_count = 0;
    }

    return OK;
}

int CNesMapper::Mapper001_Write(WORD addr, BYTE data)
{
    if( wram_patch == 1 && addr == 0xBFFF ) {
        wram_count++;
        wram_bank += data&0x01;
        if( wram_count == 5 ) {
            if( wram_bank ) {
                mmu->SetPROM_Bank( 3, &mmu->WRAM[0x2000], BANKTYPE_RAM );
            } else {
                mmu->SetPROM_Bank( 3, &mmu->WRAM[0x0000], BANKTYPE_RAM );
            }
            wram_bank = wram_count = 0;
        }
    }

    if( patch != 1 ) {
        if((addr & 0x6000) != (last_addr & 0x6000)) {
            shift = regbuf = 0;
        }
        last_addr = addr;
    }

    if( data & 0x80 ) {
        shift = regbuf = 0;
        reg[0] |= 0x0C;     // D3=1,D2=1
        return OK;
    }

    if( data&0x01 ) regbuf |= 1<<shift;
    if( ++shift < 5 )
        return OK;
    addr = (addr&0x7FFF)>>13;
    reg[addr] = regbuf;

    regbuf = 0;
    shift = 0;

    if( patch != 1 ) {
    // For Normal Cartridge
        switch( addr ) {
            case    0:
                if( reg[0] & 0x02 ) {
                    if( reg[0] & 0x01 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                    else            mmu->SetVRAM_Mirror( VRAM_VMIRROR );
                } else {
                    if( reg[0] & 0x01 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
                    else            mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
                }
                break;
            case    1:
                // Register #1
                if( mmu->VROM_1K_SIZE ) {
                    if( reg[0] & 0x10 ) {
                        // CHR 4K bank lower($0000-$0FFF)
                        mmu->SetVROM_4K_Bank( 0, reg[1] );
                        // CHR 4K bank higher($1000-$1FFF)
                        mmu->SetVROM_4K_Bank( 4, reg[2] );
                    } else {
                        // CHR 8K bank($0000-$1FFF)
                        mmu->SetVROM_8K_Bank( reg[1]>>1 );
                    }
                } else {
                    // For Romancia
                    if( reg[0] & 0x10 ) {
                        mmu->SetCRAM_4K_Bank( 0, reg[1] );
                    }
                }
                break;
            case    2:
                // Register #2
                if( mmu->VROM_1K_SIZE ) {
                    if( reg[0] & 0x10 ) {
                        // CHR 4K bank lower($0000-$0FFF)
                        mmu->SetVROM_4K_Bank( 0, reg[1] );
                        // CHR 4K bank higher($1000-$1FFF)
                        mmu->SetVROM_4K_Bank( 4, reg[2] );
                    } else {
                        // CHR 8K bank($0000-$1FFF)
                        mmu->SetVROM_8K_Bank( reg[1]>>1 );
                    }
                } else {
                    // For Romancia
                    if( reg[0] & 0x10 ) {
                        mmu->SetCRAM_4K_Bank( 4, reg[2] );
                    }
                }
                break;
            case    3:
                if( !(reg[0] & 0x08) ) {
                // PRG 32K bank ($8000-$FFFF)
                    mmu->SetPROM_32K_Bank( reg[3]>>1 );
                } else {
                    if( reg[0] & 0x04 ) {
                    // PRG 16K bank ($8000-$BFFF)
                        mmu->SetPROM_16K_Bank( 4, reg[3] );
                        mmu->SetPROM_16K_Bank( 6, mmu->PROM_16K_SIZE-1 );
                    } else {
                    // PRG 16K bank ($C000-$FFFF)
                        mmu->SetPROM_16K_Bank( 6, reg[3] );
                        mmu->SetPROM_16K_Bank( 4, 0 );
                    }
                }
                break;
        }
    } else {
        // For 512K/1M byte Cartridge
        int PROM_BASE = 0;
        if( mmu->PROM_16K_SIZE >= 32 ) {
            PROM_BASE = reg[1] & 0x10;
        }

        // For FinalFantasy I&II
        if( wram_patch == 2 ) {
            if( !(reg[1] & 0x18) ) {
                mmu->SetPROM_Bank( 3, &mmu->WRAM[0x0000], BANKTYPE_RAM );
            } else {
                mmu->SetPROM_Bank( 3, &mmu->WRAM[0x2000], BANKTYPE_RAM );
            }
        }

        // Register #0
        if( addr == 0 ) {
            if( reg[0] & 0x02 ) {
                if( reg[0] & 0x01 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                else            mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            } else {
                if( reg[0] & 0x01 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
                else            mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
            }
        }
        // Register #1
        if( mmu->VROM_1K_SIZE ) {
            if( reg[0] & 0x10 ) {
            // CHR 4K bank lower($0000-$0FFF)
                mmu->SetVROM_4K_Bank( 0, reg[1] );
            } else {
            // CHR 8K bank($0000-$1FFF)
                mmu->SetVROM_8K_Bank( reg[1]>>1 );
            }
        } else {
            // For Romancia
            if( reg[0] & 0x10 ) {
                mmu->SetCRAM_4K_Bank( 0, reg[1] );
            }
        }
        // Register #2
        if( mmu->VROM_1K_SIZE ) {
            if( reg[0] & 0x10 ) {
            // CHR 4K bank higher($1000-$1FFF)
                mmu->SetVROM_4K_Bank( 4, reg[2] );
            }
        } else {
            // For Romancia
            if( reg[0] & 0x10 ) {
                mmu->SetCRAM_4K_Bank( 4, reg[2] );
            }
        }
        // Register #3
        if( !(reg[0] & 0x08) ) {
        // PRG 32K bank ($8000-$FFFF)
            mmu->SetPROM_32K_Bank( (reg[3]&(0xF+PROM_BASE))>>1 );
        } else {
            if( reg[0] & 0x04 ) {
            // PRG 16K bank ($8000-$BFFF)
                mmu->SetPROM_16K_Bank( 4, PROM_BASE+(reg[3]&0x0F) );
                if( mmu->PROM_16K_SIZE >= 32 ) mmu->SetPROM_16K_Bank( 6, PROM_BASE+16-1 );
            } else {
            // PRG 16K bank ($C000-$FFFF)
                mmu->SetPROM_16K_Bank( 6, PROM_BASE+(reg[3]&0x0F) );
                if( mmu->PROM_16K_SIZE >= 32 ) mmu->SetPROM_16K_Bank( 4, PROM_BASE );
            }
        }
    }

    return OK;
}

int CNesMapper::Mapper001_SaveState(BYTE *p)
{
    p[0] = reg[0];
    p[1] = reg[1];
    p[2] = reg[2];
    p[3] = reg[3];
    p[4] = shift;
    p[5] = regbuf;

    p[6] = wram_bank;
    p[7] = wram_count;

    return OK;
}

int CNesMapper::Mapper001_LoadState(BYTE *p)
{
    reg[0] = p[0];
    reg[1] = p[1];
    reg[2] = p[2];
    reg[3] = p[3];
    shift  = p[4];
    regbuf = p[5];

    wram_bank  = p[6];
    wram_count = p[7];

    return OK;
}

int CNesMapper::Mapper023_Reset()
{
    addrmask = 0xFFFF;

    for( int i = 0; i < 8; i++ ) {
        reg[i] = i;
    }
    reg[8] = 0;

    irq_enable = 0;
    irq_counter = 0;
    irq_latch = 0;
    irq_clock = 0;

    reg[9] = 1;

    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    mmu->SetVROM_8K_Bank( 0 );
    
    DWORD   crc = nes->rom->GetPROM_CRC();

    if( crc == 0x93794634       // Akumajou Special Boku Dracula Kun(J)
     || crc == 0xc7829dae       // Akumajou Special Boku Dracula Kun(T-Eng)
     || crc == 0xf82dc02f ) {   // Akumajou Special Boku Dracula Kun(T-Eng v1.02)
        addrmask = 0xF00C;
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }
    if( crc == 0xdd53c4ae ) {   // Tiny Toon Adventures(J)
        nes->SetRenderMethod( POST_ALL_RENDER );
    }

    return OK;
}

int CNesMapper::Mapper023_Write( WORD addr, BYTE data )
{
    switch( addr & addrmask ) {
        case 0x8000:
        case 0x8004:
        case 0x8008:
        case 0x800C:
            if(reg[8]) {
                mmu->SetPROM_8K_Bank( 6, data );
            } else {
                mmu->SetPROM_8K_Bank( 4, data );
            }
            break;

        case 0x9000:
            if( data != 0xFF ) {
                data &= 0x03;
                if( data == 0 )      mmu->SetVRAM_Mirror( VRAM_VMIRROR );
                else if( data == 1 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                else if( data == 2 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
                else             mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
            }
            break;

        case 0x9008:
            reg[8] = data & 0x02;
            break;

        case 0xA000:
        case 0xA004:
        case 0xA008:
        case 0xA00C:
            mmu->SetPROM_8K_Bank( 5, data );
            break;

        case 0xB000:
            reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 0, reg[0] );
            break;
        case 0xB001:
        case 0xB004:
            reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 0, reg[0] );
            break;

        case 0xB002:
        case 0xB008:
            reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 1, reg[1] );
            break;

        case 0xB003:
        case 0xB00C:
            reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 1, reg[1] );
            break;

        case 0xC000:
            reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 2, reg[2] );
            break;

        case 0xC001:
        case 0xC004:
            reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 2, reg[2] );
            break;

        case 0xC002:
        case 0xC008:
            reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 3, reg[3] );
            break;

        case 0xC003:
        case 0xC00C:
            reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 3, reg[3] );
            break;

        case 0xD000:
            reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 4, reg[4] );
            break;

        case 0xD001:
        case 0xD004:
            reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 4, reg[4] );
            break;

        case 0xD002:
        case 0xD008:
            reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 5, reg[5] );
            break;

        case 0xD003:
        case 0xD00C:
            reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 5, reg[5] );
            break;

        case 0xE000:
            reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 6, reg[6] );
            break;

        case 0xE001:
        case 0xE004:
            reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 6, reg[6] );
            break;

        case 0xE002:
        case 0xE008:
            reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 7, reg[7] );
            break;

        case 0xE003:
        case 0xE00C:
            reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 7, reg[7] );
            break;

        case 0xF000:
            irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case 0xF004:
            irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;

        case 0xF008:
            irq_enable = data & 0x03;
            irq_counter = irq_latch;
            irq_clock = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;

        case 0xF00C:
            irq_enable = (irq_enable & 0x01) * 3;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
    }
    return OK;
}

int CNesMapper::Mapper023_Clock( int cycles )
{
    if( irq_enable & 0x02 ) {
        irq_clock += cycles*3;
        while( irq_clock >= 341 ) {
            irq_clock -= 341;
            irq_counter++;
            if( irq_counter == 0 ) {
                irq_counter = irq_latch;
                nes->cpu->SetIRQ( IRQ_MAPPER );
            }
        }
    }
    return OK;
}

int CNesMapper::Mapper023_SaveState( BYTE * p )
{
    for( int i = 0; i < 9; i++ ) {
        p[i] = reg[i];
    }
    p[ 9] = irq_enable;
    p[10] = (BYTE)irq_counter;
    p[11] = irq_latch;
    
    ::memcpy(p+12,&irq_clock,4);
    
    return OK;
}

int CNesMapper::Mapper023_LoadState( BYTE * p )
{
    for( int i = 0; i < 9; i++ ) {
        reg[i] = p[i];
    }
    irq_enable  = p[ 9];
    irq_counter = p[10];
    irq_latch   = p[11];
    
    ::memcpy(&irq_clock,p+12,4);

    return OK;
}

int CNesMapper::Mapper025_Reset()
{
    for( int i = 0; i < 11; i++ ) {
        reg[i] = 0;
    }
    reg[9] = mmu->PROM_8K_SIZE-2;

    irq_enable = 0;
    irq_counter = 0;
    irq_latch = 0;
    irq_occur = 0;
    irq_clock = 0;

    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    DWORD   crc = nes->rom->GetPROM_CRC();
    if( crc == 0xc71d4ce7 ) {   // Gradius II(J)
//      nes->SetRenderMethod( POST_RENDER );
    }
    if( crc == 0xa2e68da8 ) {   // For Racer Mini Yonku - Japan Cup(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0xea74c587 ) {   // For Teenage Mutant Ninja Turtles(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    if( crc == 0x5f82cb7d ) {   // For Teenage Mutant Ninja Turtles 2(J)
    }
    if( crc == 0x0bbd85ff ) {   // For Bio Miracle Bokutte Upa(J)
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }

    return OK;
}

int CNesMapper::Mapper025_Write( WORD addr, BYTE data )
{
    switch( addr & 0xF000 ) {
        case    0x8000:
            if(reg[10] & 0x02) {
                reg[9] = data;
                mmu->SetPROM_8K_Bank( 6, data );
            } else {
                reg[8] = data;
                mmu->SetPROM_8K_Bank( 4, data );
            }
            break;
        case    0xA000:
            mmu->SetPROM_8K_Bank( 5, data );
            break;
    }

    switch( addr & 0xF00F ) {
        case    0x9000:
            data &= 0x03;
            if( data == 0 )      mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            else if( data == 1 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
            else if( data == 2 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
            else             mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
            break;

        case 0x9001:
        case 0x9004:
            if((reg[10] & 0x02) != (data & 0x02)) {
                BYTE    swap = reg[8];
                reg[8] = reg[9];
                reg[9] = swap;

                mmu->SetPROM_8K_Bank( 4, reg[8] );
                mmu->SetPROM_8K_Bank( 6, reg[9] );
            }
            reg[10] = data;
            break;

        case 0xB000:
            reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 0, reg[0] );
            break;
        case 0xB002:
        case 0xB008:
            reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 0, reg[0] );
            break;

        case 0xB001:
        case 0xB004:
            reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 1, reg[1] );
            break;
        case 0xB003:
        case 0xB00C:
            reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 1, reg[1] );
            break;

        case 0xC000:
            reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 2, reg[2] );
            break;
        case 0xC002:
        case 0xC008:
            reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 2, reg[2] );
            break;

        case 0xC001:
        case 0xC004:
            reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 3, reg[3] );
            break;
        case 0xC003:
        case 0xC00C:
            reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 3, reg[3] );
            break;

        case 0xD000:
            reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 4, reg[4] );
            break;
        case 0xD002:
        case 0xD008:
            reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 4, reg[4] );
            break;

        case 0xD001:
        case 0xD004:
            reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 5, reg[5] );
            break;
        case 0xD003:
        case 0xD00C:
            reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 5, reg[5] );
            break;

        case 0xE000:
            reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 6, reg[6] );
            break;
        case 0xE002:
        case 0xE008:
            reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 6, reg[6] );
            break;

        case 0xE001:
        case 0xE004:
            reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 7, reg[7] );
            break;
        case 0xE003:
        case 0xE00C:
            reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 7, reg[7] );
            break;

        case 0xF000:
            irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;

        case 0xF002:
        case 0xF008:
            irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;

        case 0xF001:
        case 0xF004:
            irq_enable = data & 0x03;
            irq_counter = irq_latch;
            irq_clock = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;

        case 0xF003:
        case 0xF00C:
            irq_enable = (irq_enable & 0x01)*3;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
    }

    return OK;
}

int CNesMapper::Mapper025_Clock( int cycles )
{
    if( irq_enable & 0x02 ) {
        irq_clock += cycles*3;
        while( irq_clock >= 341 ) {
            irq_clock -= 341;
            irq_counter++;
            if( irq_counter == 0 ) {
                irq_counter = irq_latch;
                nes->cpu->SetIRQ( IRQ_MAPPER );
            }
        }
    }

    return OK;
}

int CNesMapper::Mapper025_SaveState( BYTE * p )
{
    for( int i = 0; i < 11; i++ ) {
        p[i] = reg[i];
    }
    p[11] = irq_enable;
    p[12] = irq_occur;
    p[13] = irq_latch;
    p[14] = (BYTE)irq_counter;
    
    ::memcpy(p+15,&irq_clock,4);

    return OK;
}

int CNesMapper::Mapper025_LoadState( BYTE * p )
{
    for( int i = 0; i < 11; i++ ) {
        reg[i] = p[i];
    }
    irq_enable  = p[11];
    irq_occur   = p[12];
    irq_latch   = p[13];
    irq_counter = p[14];

    ::memcpy(&irq_clock,p+15,4);

    return OK;
}
//////////////////////////////////////////////////////////////////////////
// CNesMapper::Mapper254  Pokemon Pirate Cart                                       //
//////////////////////////////////////////////////////////////////////////
int CNesMapper::Mapper254_Reset()
{
    for( int i = 0; i < 8; i++ ) {
        reg[i] = 0x00;
    }

    protectflag = 0;

    prg0 = 0;
    prg1 = 1;
    this->Mapper254_SetBank_CPU();

    chr[1] = 0;
    chr[23] = 2;
    chr[4]  = 4;
    chr[5]  = 5;
    chr[6]  = 6;
    chr[7]  = 7;
    this->Mapper254_SetBank_PPU();

    irq_enable = 0; // Disable
    irq_counter = 0;
    irq_latch = 0;
    irq_request = 0;

    return OK;
}

int CNesMapper::Mapper254_WriteLow( WORD addr, BYTE data )
{
    switch( addr & 0xF000 ) {
        case    0x6000:
        case    0x7000:
            mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
            break;
    }
    return OK;
}
BYTE    CNesMapper::Mapper254_ReadLow( WORD addr )
{
    if( addr>=0x6000 ) {
        if( protectflag ) {
            return  (mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF]);
        } else {
            return  ((mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF])^0x1);
        }
    }
    return  ReadLow_Def( addr );
}

int CNesMapper::Mapper254_Write( WORD addr, BYTE data )
{
    switch( addr & 0xE001 ) {
        case    0x8000:
            protectflag=0xFF;
            reg[0] = data;
            this->Mapper254_SetBank_CPU();
            this->Mapper254_SetBank_PPU();
            break;
        case    0x8001:
            reg[1] = data;

            switch( reg[0] & 0x07 ) {
                case    0x00:
                    chr[1] = data & 0xFE;
                    this->Mapper254_SetBank_PPU();
                    break;
                case    0x01:
                    chr[23] = data & 0xFE;
                    this->Mapper254_SetBank_PPU();
                    break;
                case    0x02:
                    chr[4] = data;
                    this->Mapper254_SetBank_PPU();
                    break;
                case    0x03:
                    chr[5] = data;
                    this->Mapper254_SetBank_PPU();
                    break;
                case    0x04:
                    chr[6] = data;
                    this->Mapper254_SetBank_PPU();
                    break;
                case    0x05:
                    chr[7] = data;
                    this->Mapper254_SetBank_PPU();
                    break;
                case    0x06:
                    prg0 = data;
                    this->Mapper254_SetBank_CPU();
                    break;
                case    0x07:
                    prg1 = data;
                    this->Mapper254_SetBank_CPU();
                    break;
            }
            break;
        case    0xA000:
            reg[2] = data;
            if( !nes->rom->Is4SCREEN() ) {
                if( data & 0x01 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                else          mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            }
            break;
        case    0xA001:
            reg[3] = data;
            break;
        case    0xC000:
            reg[4] = data;
            irq_counter = data;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xC001:
            reg[5] = data;
            irq_latch = data;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xE000:
            reg[6] = data;
            irq_enable = 0;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xE001:
            reg[7] = data;
            irq_enable = 1;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
    }

    return OK;
}

int CNesMapper::Mapper254_Clock( int cycles )
{
    return OK;
}

int CNesMapper::Mapper254_HSync( int scanline )
{
    if( (scanline >= 0 && scanline <= 239) ) {
        if( nes->ppu->IsDispON() ) {
            if( irq_enable && !irq_request ) {
                if( scanline == 0 ) {
                    if( irq_counter ) {
                        irq_counter--;
                    }
                }
                if( !(irq_counter--) ) {
                    irq_request = 0xFF;
                    irq_counter = irq_latch;
                    nes->cpu->SetIRQ( IRQ_MAPPER );
                }
            }
        }
    }
    return OK;
}

int CNesMapper::Mapper254_SetBank_CPU()
{
    if( reg[0] & 0x40 ) {
        mmu->SetPROM_32K_Bank( mmu->PROM_8K_SIZE-2, prg1, prg0, mmu->PROM_8K_SIZE-1 );
    } else {
        mmu->SetPROM_32K_Bank( prg0, prg1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    }
    return OK;
}

int CNesMapper::Mapper254_SetBank_PPU()
{

    if( mmu->VROM_1K_SIZE ) {
        if( reg[0] & 0x80 ) {
            mmu->SetVROM_8K_Bank( chr[4], chr[5], chr[6], chr[7],
                     chr[1], chr[1]+1, chr[23], chr[23]+1 );
        } else {
            mmu->SetVROM_8K_Bank( chr[1], chr[1]+1, chr[23], chr[23]+1,
                     chr[4], chr[5], chr[6], chr[7] );
        }
    } else {
        if( reg[0] & 0x80 ) {
            mmu->SetCRAM_1K_Bank( 4, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 5, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 6, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 7, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 0, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 1, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 2, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 3, chr[7]&0x07 );
        } else {
            mmu->SetCRAM_1K_Bank( 0, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 1, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 2, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 3, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 4, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 5, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 6, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 7, chr[7]&0x07 );
        }
    }
    return OK;
}

int CNesMapper::Mapper254_SaveState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        p[i] = reg[i];
    }
    p[ 8] = prg0;
    p[ 9] = prg1;
    p[10] = chr[1];
    p[11] = chr[23];
    p[12] = chr[4];
    p[13] = chr[5];
    p[14] = chr[6];
    p[15] = chr[7];
    p[16] = irq_enable;
    p[17] = (BYTE)irq_counter;
    p[18] = irq_latch;
    p[19] = irq_request;
    p[20] = protectflag;

    return OK;
}

int CNesMapper::Mapper254_LoadState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        reg[i] = p[i];
    }
    prg0  = p[ 8];
    prg1  = p[ 9];
    chr[1] = p[10];
    chr[23] = p[11];
    chr[4]  = p[12];
    chr[5]  = p[13];
    chr[6]  = p[14];
    chr[7]  = p[15];
    irq_enable  = p[16];
    irq_counter = p[17];
    irq_latch   = p[18];
    irq_request = p[19];
    protectflag = p[20];

    return OK;
}
//////////////////////////////////////////////////////////////////////////
// CNesMapper::Mapper245  Yong Zhe Dou E Long                                       //
//////////////////////////////////////////////////////////////////////////

int CNesMapper::Mapper245_Reset()
{
    for( int i = 0; i < 8; i++ ) {
        reg[i] = 0x00;
    }

    prg0 = 0;
    prg1 = 1;

    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    we_sram  = 0;   // Disable
    irq_enable = 0; // Disable
    irq_counter = 0;
    irq_latch = 0;
    irq_request = 0;

    nes->SetIrqType( IRQ_CLOCK );

    return OK;
}

int CNesMapper::Mapper245_Write( WORD addr, BYTE data )
{
    switch( addr&0xF7FF ) {
        case    0x8000:
            reg[0] = data;
            break;
        case    0x8001:
            reg[1] = data;
            switch( reg[0] ) {
                case    0x00:
                    reg[3]=(data & 2 )<<5;
                    mmu->SetPROM_8K_Bank(6,0x3E | reg[3]);
                    mmu->SetPROM_8K_Bank(7,0x3F | reg[3]);
                    break;
                case    0x06:
                    prg0=data;
                    break;
                case    0x07:
                    prg1=data;
                    break;
            }
            mmu->SetPROM_8K_Bank( 4, prg0|reg[3] );
            mmu->SetPROM_8K_Bank( 5, prg1|reg[3] );
            break;
        case    0xA000:
            reg[2] = data;
            if( !nes->rom->Is4SCREEN() ) {
                if( data & 0x01 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                else          mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            }
            break;
        case    0xA001:
            
            break;
        case    0xC000:
            reg[4] = data;
            irq_counter = data;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xC001:
            reg[5] = data;
            irq_latch = data;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xE000:
            reg[6] = data;
            irq_enable = 0;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xE001:
            reg[7] = data;
            irq_enable = 1;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
    }

    return OK;
}

int CNesMapper::Mapper245_Clock( int cycles )
{
    return OK;
}

int CNesMapper::Mapper245_HSync( int scanline )
{
    if( (scanline >= 0 && scanline <= 239) ) {
        if( nes->ppu->IsDispON() ) {
            if( irq_enable && !irq_request ) {
                if( scanline == 0 ) {
                    if( irq_counter ) {
                        irq_counter--;
                    }
                }
                if( !(irq_counter--) ) {
                    irq_request = 0xFF;
                    irq_counter = irq_latch;
                    nes->cpu->SetIRQ( IRQ_MAPPER );
                }
            }
        }
    }
    return OK;
}

int CNesMapper::Mapper245_SetBank_CPU()
{
    mmu->SetPROM_32K_Bank( prg0, prg1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    return OK;
}

int CNesMapper::Mapper245_SetBank_PPU()
{
    if( mmu->VROM_1K_SIZE ) {
        if( reg[0] & 0x80 ) {
            mmu->SetVROM_8K_Bank( chr[4], chr[5], chr[6], chr[7],
                     chr[23]+1, chr[23], chr[1]+1, chr[1] );
        } else {
            mmu->SetVROM_8K_Bank( chr[1], chr[1]+1, chr[23], chr[23]+1,
                     chr[4], chr[5], chr[6], chr[7] );
        }
    } else {
        if( reg[0] & 0x80 ) {
            mmu->SetCRAM_1K_Bank( 4, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 5, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 6, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 7, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 0, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 1, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 2, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 3, chr[7]&0x07 );
        } else {
            mmu->SetCRAM_1K_Bank( 0, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 1, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 2, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 3, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 4, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 5, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 6, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 7, chr[7]&0x07 );
        }
    }
    return OK;
}

int CNesMapper::Mapper245_SaveState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        p[i] = reg[i];
    }
    p[ 8] = prg0;
    p[ 9] = prg1;
    p[10] = chr[1];
    p[11] = chr[23];
    p[12] = chr[4];
    p[13] = chr[5];
    p[14] = chr[6];
    p[15] = chr[7];
    p[16] = irq_enable;
    p[17] = (BYTE)irq_counter;
    p[18] = irq_latch;
    p[19] = irq_request;
    return OK;
}

int CNesMapper::Mapper245_LoadState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        reg[i] = p[i];
    }
    prg0  = p[ 8];
    prg1  = p[ 9];
    chr[1] = p[10];
    chr[23] = p[11];
    chr[4]  = p[12];
    chr[5]  = p[13];
    chr[6]  = p[14];
    chr[7]  = p[15];
    irq_enable  = p[16];
    irq_counter = (int)p[17];
    irq_latch   = p[18];
    irq_request = p[19];
    return OK;
}

int CNesMapper::Mapper015_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, 2, 3 );
    return OK;
}

int CNesMapper::Mapper015_Write( WORD addr, BYTE data )
{
    switch( addr ) {
        case    0x8000:
            if( data&0x80 ) {
                mmu->SetPROM_8K_Bank( 4, (data&0x3F)*2+1 );
                mmu->SetPROM_8K_Bank( 5, (data&0x3F)*2+0 );
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+3 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+2 );
            } else {
                mmu->SetPROM_8K_Bank( 4, (data&0x3F)*2+0 );
                mmu->SetPROM_8K_Bank( 5, (data&0x3F)*2+1 );
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+2 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+3 );
            }
            if( data&0x40 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
            else        mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            break;
        case    0x8001:
            if( data&0x80 ) {
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+1 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+0 );
            } else {
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+0 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+1 );
            }
            break;
        case    0x8002:
            if( data&0x80 ) {
                mmu->SetPROM_8K_Bank( 4, (data&0x3F)*2+1 );
                mmu->SetPROM_8K_Bank( 5, (data&0x3F)*2+1 );
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+1 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+1 );
            } else {
                mmu->SetPROM_8K_Bank( 4, (data&0x3F)*2+0 );
                mmu->SetPROM_8K_Bank( 5, (data&0x3F)*2+0 );
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+0 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+0 );
            }
            break;
        case    0x8003:
            if( data&0x80 ) {
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+1 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+0 );
            } else {
                mmu->SetPROM_8K_Bank( 6, (data&0x3F)*2+0 );
                mmu->SetPROM_8K_Bank( 7, (data&0x3F)*2+1 );
            }
            if( data&0x40 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
            else        mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            break;
    }

    return OK;
}

int CNesMapper::Mapper074_Reset()
{
    for( int i = 0; i < 8; i++ ) {
        reg[i] = 0x00;
    }
    prg0 = 0;
    prg1 = 1;
    this->Mapper074_SetBank_CPU();

    chr[1] = 0;
    chr[23] = 2;
    chr[4]  = 4;
    chr[5]  = 5;
    chr[6]  = 6;
    chr[7]  = 7;
    this->Mapper074_SetBank_PPU();

    we_sram  = 0;   // Disable
    irq_enable = 0; // Disable
    irq_counter = 0;
    irq_latch = 0;
    irq_request = 0;

    DWORD   crc = nes->rom->GetPROM_CRC();

    patch = 0;
    if( crc == 0x37ae04a8 ) {
        patch = 1;
        nes->SetRenderMethod( TILE_RENDER );
    }

    return OK;
}

int CNesMapper::Mapper074_Write( WORD addr, BYTE data )
{
    switch( addr & 0xE001 ) {
        case    0x8000:
            reg[0] = data;
            this->Mapper074_SetBank_CPU();
            this->Mapper074_SetBank_PPU();
            break;
        case    0x8001:
            reg[1] = data;

            switch( reg[0] & 0x07 ) {
                case    0x00:
                    chr[1] = data & 0xFE;
                    this->Mapper074_SetBank_PPU();
                    break;
                case    0x01:
                    chr[23] = data & 0xFE;
                    this->Mapper074_SetBank_PPU();
                    break;
                case    0x02:
                    chr[4] = data;
                    this->Mapper074_SetBank_PPU();
                    break;
                case    0x03:
                    chr[5] = data;
                    this->Mapper074_SetBank_PPU();
                    break;
                case    0x04:
                    chr[6] = data;
                    this->Mapper074_SetBank_PPU();
                    break;
                case    0x05:
                    chr[7] = data;
                    this->Mapper074_SetBank_PPU();
                    break;
                case    0x06:
                    prg0 = data;
                    this->Mapper074_SetBank_CPU();
                    break;
                case    0x07:
                    prg1 = data;
                    this->Mapper074_SetBank_CPU();
                    break;
            }
            break;
        case    0xA000:
            reg[2] = data;
            if( !nes->rom->Is4SCREEN() ) {
                if( data & 0x01 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                else          mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            }
            break;
        case    0xA001:
            reg[3] = data;
            break;
        case    0xC000:
            reg[4] = data;
            irq_counter = data;
            irq_request = 0;
            break;
        case    0xC001:
            reg[5] = data;
            irq_latch = data;
            irq_request = 0;
            break;
        case    0xE000:
            reg[6] = data;
            irq_enable = 0;
            irq_request = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xE001:
            reg[7] = data;
            irq_enable = 1;
            irq_request = 0;
            break;
    }   
    
    return OK;
}

int CNesMapper::Mapper074_HSync( int scanline )
{
    if( (scanline >= 0 && scanline <= 239) ) {
        if( nes->ppu->IsDispON() ) {
            if( irq_enable && !irq_request ) {
                if( scanline == 0 ) {
                    if( irq_counter ) {
                        irq_counter--;
                    }
                }
                if( !(irq_counter--) ) {
                    irq_request = 0xFF;
                    irq_counter = irq_latch;
                    nes->cpu->SetIRQ( IRQ_MAPPER );
                }
            }
        }
    }

    return OK;
}

int CNesMapper::Mapper074_SetBank_CPU()
{
    if( reg[0] & 0x40 ) {
        mmu->SetPROM_32K_Bank( mmu->PROM_8K_SIZE-2, prg1, prg0, mmu->PROM_8K_SIZE-1 );
    } else {
        mmu->SetPROM_32K_Bank( prg0, prg1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    }

    return OK;
}

int CNesMapper::Mapper074_SetBank_PPU()
{
    if( mmu->VROM_1K_SIZE ) {
        if( reg[0] & 0x80 ) {
            this->Mapper074_SetBank_PPUSUB( 4, chr[1]+0 );
            this->Mapper074_SetBank_PPUSUB( 5, chr[1]+1 );
            this->Mapper074_SetBank_PPUSUB( 6, chr[23]+0 );
            this->Mapper074_SetBank_PPUSUB( 7, chr[23]+1 );
            this->Mapper074_SetBank_PPUSUB( 0, chr[4] );
            this->Mapper074_SetBank_PPUSUB( 1, chr[5] );
            this->Mapper074_SetBank_PPUSUB( 2, chr[6] );
            this->Mapper074_SetBank_PPUSUB( 3, chr[7] );
        } else {
            this->Mapper074_SetBank_PPUSUB( 0, chr[1]+0 );
            this->Mapper074_SetBank_PPUSUB( 1, chr[1]+1 );
            this->Mapper074_SetBank_PPUSUB( 2, chr[23]+0 );
            this->Mapper074_SetBank_PPUSUB( 3, chr[23]+1 );
            this->Mapper074_SetBank_PPUSUB( 4, chr[4] );
            this->Mapper074_SetBank_PPUSUB( 5, chr[5] );
            this->Mapper074_SetBank_PPUSUB( 6, chr[6] );
            this->Mapper074_SetBank_PPUSUB( 7, chr[7] );
        }
    } else {
        if( reg[0] & 0x80 ) {
            mmu->SetCRAM_1K_Bank( 4, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 5, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 6, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 7, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 0, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 1, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 2, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 3, chr[7]&0x07 );
        } else {
            mmu->SetCRAM_1K_Bank( 0, (chr[1]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 1, (chr[1]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 2, (chr[23]+0)&0x07 );
            mmu->SetCRAM_1K_Bank( 3, (chr[23]+1)&0x07 );
            mmu->SetCRAM_1K_Bank( 4, chr[4]&0x07 );
            mmu->SetCRAM_1K_Bank( 5, chr[5]&0x07 );
            mmu->SetCRAM_1K_Bank( 6, chr[6]&0x07 );
            mmu->SetCRAM_1K_Bank( 7, chr[7]&0x07 );
        }
    }

    return OK;
}

int CNesMapper::Mapper074_SetBank_PPUSUB( int bank, int page )
{
    if( !patch && (page == 8 || page == 9) ) {
        mmu->SetCRAM_1K_Bank( bank, page & 7 );
    } else if( patch == 1 && page >= 128 ) {
        mmu->SetCRAM_1K_Bank( bank, page & 7 );
    } else {
        mmu->SetVROM_1K_Bank( bank, page );
    }

    return OK;
}

int CNesMapper::Mapper074_SaveState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        p[i] = reg[i];
    }
    p[ 8] = prg0;
    p[ 9] = prg1;
    p[10] = chr[1];
    p[11] = chr[23];
    p[12] = chr[4];
    p[13] = chr[5];
    p[14] = chr[6];
    p[15] = chr[7];
    p[16] = irq_enable;
    p[17] = (BYTE)irq_counter;
    p[18] = irq_latch;
    p[19] = irq_request;

    return OK;
}

int CNesMapper::Mapper074_LoadState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        reg[i] = p[i];
    }
    prg0  = p[ 8];
    prg1  = p[ 9];
    chr[1] = p[10];
    chr[23] = p[11];
    chr[4]  = p[12];
    chr[5]  = p[13];
    chr[6]  = p[14];
    chr[7]  = p[15];
    irq_enable  = p[16];
    irq_counter = p[17];
    irq_latch   = p[18];
    irq_request = p[19];

    return OK;
}

int CNesMapper::Mapper164_Reset()
{
    reg5000 = 0;
    reg5100 = 0;
    Mapper164_SetBank_CPU();
    Mapper164_SetBank_PPU();
    nes->ppu->SetExtLatchMode( TRUE );

    return OK;
}

int CNesMapper::Mapper164_WriteLow(WORD addr, BYTE data)
{
    if(addr==0x5000){
        p_mode = data>>7;
        reg5000 = data;
        Mapper164_SetBank_CPU();
        Mapper164_SetBank_PPU();
    }else if(addr==0x5100){
        reg5100 = data;
        Mapper164_SetBank_CPU();
        Mapper164_SetBank_PPU();
    }else if(addr>=0x6000){
        mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
    }

    return OK;
}

int CNesMapper::Mapper164_SetBank_CPU()
{
    int mode, base, bank;

    base = (reg5100&1)<<5;
    mode = (reg5000>>4)&0x07;

    switch(mode){
        case 0:
        case 2:
        case 4:
        case 6:             /* NORMAL MODE */
            bank = (reg5000&0x0f);
            bank += (reg5000&0x20)>>1;
            mmu->SetPROM_16K_Bank(4, bank+base);
            mmu->SetPROM_16K_Bank(6, base+0x1f);
            break;
        case 1:
        case 3:             /* REG MODE */
            break;
        case 5:             /* 32K MODE */
            bank = (reg5000&0x0f);
            mmu->SetPROM_32K_Bank(bank+(base>>1));
            break;
        case 7:             /* HALF MODE */
            bank = (reg5000&0x0f);
            bank += (bank&0x08)<<1;
            mmu->SetPROM_16K_Bank(4, bank+base);
            bank = (bank&0x10)+0x0f;
            mmu->SetPROM_16K_Bank(6, base+0x1f);
            break;
        default:
            break;
    };
    return OK;  
}

int CNesMapper::Mapper164_SetBank_PPU()
{
    mmu->SetCRAM_8K_Bank(0);
    return OK;
}

int CNesMapper::Mapper164_PPU_ExtLatchX( int x )
{
    a3 = (x&1)<<3;
    return OK;
}

int CNesMapper::Mapper164_PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
    int loopy_v = nes->ppu->GetPPUADDR();
    int loopy_y = nes->ppu->GetTILEY();
    int tileofs = (mmu->PPUREG[0]&PPU_BGTBL_BIT)<<8;
    int attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
    int attrsft = (ntbladr&0x0040)>>4;
    BYTE *  pNTBL = mmu->PPU_MEM_BANK[ntbladr>>10];
    int ntbl_x  = ntbladr&0x001F;
    int tileadr;

    attradr &= 0x3FF;
    attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
    tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

    if(p_mode){
        tileadr = (tileadr&0xfff7)|a3;
        chr_l = chr_h = mmu->PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
    }else{
        chr_l = mmu->PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
        chr_h = mmu->PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
    }
    return OK;
}

int CNesMapper::Mapper164_SaveState( BYTE * p )
{
    p[0] = reg5000;
    p[1] = reg5100;
    p[2] = a3;
    p[3] = p_mode;
    return OK;
}

int CNesMapper::Mapper164_LoadState( BYTE * p )
{
    reg5000 = p[0];
    reg5100 = p[1];
    a3 = p[2];
    p_mode = p[3];
    return OK;
}

int CNesMapper::Mapper069_Reset()
{
    reg[0] = 0;
    irq_enable = 0;
    irq_counter = 0;

    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    nes->apu->SelectExSound(32);
    nes->SetIrqType( IRQ_CLOCK );

    patch = 0;

    DWORD   crc = nes->rom->GetPROM_CRC();

    if( crc == 0xfeac6916 ) {   // Honoo no Toukyuuji - Dodge Danpei 2(J)
        nes->SetRenderMethod( TILE_RENDER );
    }

    if( crc == 0xad28aef6 ) {   // Dynamite Batman(J) / Dynamite Batman - Return of the Joker(U)
        patch = 1;
    }

    return OK;
}

int CNesMapper::Mapper069_Write( WORD addr, BYTE data )
{
    switch( addr & 0xE000 ) {
        case    0x8000:
            reg[0] = data;
            break;

        case    0xA000:
            switch( reg[0] & 0x0F ) {
                case    0x00:   case    0x01:
                case    0x02:   case    0x03:
                case    0x04:   case    0x05:
                case    0x06:   case    0x07:
                    mmu->SetVROM_1K_Bank( reg[0]&0x07, data );
                    break;
                case    0x08:
                    if( !patch && !(data & 0x40) ) {
                        mmu->SetPROM_8K_Bank( 3, data );
                    }
                    break;
                case    0x09:
                    mmu->SetPROM_8K_Bank( 4, data );
                    break;
                case    0x0A:
                    mmu->SetPROM_8K_Bank( 5, data );
                    break;
                case    0x0B:
                    mmu->SetPROM_8K_Bank( 6, data );
                    break;

                case    0x0C:
                    data &= 0x03;
                    if( data == 0 )      mmu->SetVRAM_Mirror( VRAM_VMIRROR );
                    else if( data == 1 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                    else if( data == 2 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
                    else             mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
                    break;

                case    0x0D:
                    irq_enable = data;
                    nes->cpu->ClrIRQ( IRQ_MAPPER );
                    break;

                case    0x0E:
                    irq_counter = (irq_counter & 0xFF00) | data;
                    nes->cpu->ClrIRQ( IRQ_MAPPER );
                    break;

                case    0x0F:
                    irq_counter = (irq_counter & 0x00FF) | (data << 8);
                    nes->cpu->ClrIRQ( IRQ_MAPPER );
                    break;
            }
            break;

        case    0xC000:
        case    0xE000:
            nes->apu->ExWrite( addr, data );
            break;
    }

    return OK;
}

int CNesMapper::Mapper069_Clock( int cycles )
{
    if( irq_enable && (nes->GetIrqType() == IRQ_CLOCK) ) {
        irq_counter -= cycles;
        if( irq_counter <= 0 ) {
            nes->cpu->SetIRQ( IRQ_MAPPER );
            irq_enable = 0;
            irq_counter = 0xFF;
        }
    }

    return OK;
}

int CNesMapper::Mapper069_HSync( int scanline )
{
    if( irq_enable && (nes->GetIrqType() == IRQ_HSYNC) ) {
        irq_counter -= 114;
        if( irq_counter <= 0 ) {
            nes->cpu->SetIRQ( IRQ_MAPPER );
            irq_enable = 0;
            irq_counter = 0xFF;
        }
    }

    return OK;
}

int CNesMapper::Mapper069_SaveState( BYTE * p )
{
    int t;

    p[0] = reg[0];
    p[1] = irq_enable;
    
    t = irq_counter; ::memcpy(p+2,&t,4);

    return OK;
}

int CNesMapper::Mapper069_LoadState( BYTE * p )
{
    int t;

    reg[0] = p[0];
    irq_enable  = p[1];
    
    ::memcpy(&t,p+2,4); irq_counter = t;

    return OK;
}

int CNesMapper::Mapper022_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    return OK;
}

int CNesMapper::Mapper022_Write( WORD addr, BYTE data )
{
    switch( addr ) {
        case 0x8000:
            mmu->SetPROM_8K_Bank( 4, data );
            break;

        case 0x9000:
            data &= 0x03;
            if( data == 0 )      mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            else if( data == 1 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
            else if( data == 2 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
            else             mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
            break;

        case 0xA000:
            mmu->SetPROM_8K_Bank( 5, data );
            break;

        case 0xB000:
            mmu->SetVROM_1K_Bank( 0, data>>1 );
            break;

        case 0xB001:
            mmu->SetVROM_1K_Bank( 1, data>>1 );
            break;

        case 0xC000:
            mmu->SetVROM_1K_Bank( 2, data>>1 );
            break;

        case 0xC001:
            mmu->SetVROM_1K_Bank( 3, data>>1 );
            break;

        case 0xD000:
            mmu->SetVROM_1K_Bank( 4, data>>1 );
            break;

        case 0xD001:
            mmu->SetVROM_1K_Bank( 5, data>>1 );
            break;

        case 0xE000:
            mmu->SetVROM_1K_Bank( 6, data>>1 );
            break;

        case 0xE001:
            mmu->SetVROM_1K_Bank( 7, data>>1 );
            break;
    }

    return OK;
}

int CNesMapper::Mapper078_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_8K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    return OK;
}

int CNesMapper::Mapper078_Write( WORD addr, BYTE data )
{
    mmu->SetPROM_16K_Bank( 4, data&0x0F );
    mmu->SetVROM_8K_Bank( (data&0xF0)>>4 );

    if( (addr & 0xFE00) != 0xFE00 ) {
        if( data & 0x08 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
        else          mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
    }

    return OK;
}

int CNesMapper::Mapper018_Reset()
{
    for( int i = 0; i < 11; i++ ) {
        reg[i] = 0;
    }
    reg[2] = mmu->PROM_8K_SIZE-2;
    reg[3] = mmu->PROM_8K_SIZE-1;

    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    irq_enable  = 0;
    irq_mode    = 0;
    irq_counter = 0xFF;
    irq_latch   = 0xFF;

    DWORD   crc = nes->rom->GetPROM_CRC();

    if( crc == 0xefb1df9e ) {   // The Lord of King(J)
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }
    if( crc == 0x3746f951 ) {   // Pizza Pop!(J)
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }

    return OK;
}

int CNesMapper::Mapper018_Write( WORD addr, BYTE data )
{
    switch( addr ) {
        case    0x8000:
            reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
            mmu->SetPROM_8K_Bank( 4, reg[0] );
            break;
        case    0x8001:
            reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetPROM_8K_Bank( 4, reg[0] );
            break;
        case    0x8002:
            reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
            mmu->SetPROM_8K_Bank( 5, reg[1] );
            break;
        case    0x8003:
            reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetPROM_8K_Bank( 5, reg[1] );
            break;
        case    0x9000:
            reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
            mmu->SetPROM_8K_Bank( 6, reg[2] );
            break;
        case    0x9001:
            reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetPROM_8K_Bank( 6, reg[2] );
            break;

        case    0xA000:
            reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 0, reg[3] );
            break;
        case    0xA001:
            reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 0, reg[3] );
            break;
        case    0xA002:
            reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 1, reg[4] );
            break;
        case    0xA003:
            reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 1, reg[4] );
            break;

        case    0xB000:
            reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 2, reg[5] );
            break;
        case    0xB001:
            reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 2, reg[5] );
            break;
        case    0xB002:
            reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 3, reg[6] );
            break;
        case    0xB003:
            reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 3, reg[6] );
            break;

        case    0xC000:
            reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 4, reg[7] );
            break;
        case    0xC001:
            reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 4, reg[7] );
            break;
        case    0xC002:
            reg[8] = (reg[8] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 5, reg[8] );
            break;
        case    0xC003:
            reg[8] = (reg[8] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 5, reg[8] );
            break;

        case    0xD000:
            reg[9] = (reg[9] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 6, reg[9] );
            break;
        case    0xD001:
            reg[9] = (reg[9] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 6, reg[9] );
            break;
        case    0xD002:
            reg[10] = (reg[10] & 0xF0) | (data & 0x0F);
            mmu->SetVROM_1K_Bank( 7, reg[10] );
            break;
        case    0xD003:
            reg[10] = (reg[10] & 0x0F) | ((data & 0x0F) << 4);
            mmu->SetVROM_1K_Bank( 7, reg[10] );
            break;

        case    0xE000:
            irq_latch = (irq_latch & 0xFFF0) | (data & 0x0F);
            break;
        case    0xE001:
            irq_latch = (irq_latch & 0xFF0F) | ((data & 0x0F) << 4);
            break;
        case    0xE002:
            irq_latch = (irq_latch & 0xF0FF) | ((data & 0x0F) << 8);
            break;
        case    0xE003:
            irq_latch = (irq_latch & 0x0FFF) | ((data & 0x0F) << 12);
            break;

        case    0xF000:
                irq_counter = irq_latch;
            break;
        case    0xF001:
            irq_mode = (data>>1) & 0x07;
            irq_enable = (data & 0x01);
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;

        case    0xF002:
            data &= 0x03;
            if( data == 0 )      mmu->SetVRAM_Mirror( VRAM_HMIRROR );
            else if( data == 1 ) mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            else             mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
            break;
    }

    return OK;
}

int CNesMapper::Mapper018_Clock( int cycles )
{
    BOOL    bIRQ = FALSE;
    int irq_counter_old = irq_counter;

    if( irq_enable && irq_counter ) {
        irq_counter -= cycles;

        switch( irq_mode ) {
            case    0:
                if( irq_counter <= 0 ) {
                    bIRQ = TRUE;
                }
                break;
            case    1:
                if( (irq_counter & 0xF000) != (irq_counter_old & 0xF000) ) {
                    bIRQ = TRUE;
                }
                break;
            case    2:
            case    3:
                if( (irq_counter & 0xFF00) != (irq_counter_old & 0xFF00) ) {
                    bIRQ = TRUE;
                }
                break;
            case    4:
            case    5:
            case    6:
            case    7:
                if( (irq_counter & 0xFFF0) != (irq_counter_old & 0xFFF0) ) {
                    bIRQ = TRUE;
                }
                break;
        }

        if( bIRQ ) {
            irq_counter = 0;
            irq_enable = 0;
            nes->cpu->SetIRQ( IRQ_MAPPER );
        }
    }

    return OK;
}

int CNesMapper::Mapper018_SaveState( BYTE * p )
{
    int t;

    for( int i = 0; i < 11; i++ ) {
        p[i] = reg[i];
    }
    p[11] = irq_enable;
    p[12] = irq_mode;

    t = irq_counter; ::memcpy(p+13,&t,4);
    t = irq_latch; ::memcpy(p+17,&t,4);

    return OK;
}

int CNesMapper::Mapper018_LoadState( BYTE * p )
{
    int t;

    for( int i = 0; i < 11; i++ ) {
        p[i] = reg[i];
    }
    irq_enable  = p[11];
    irq_mode    = p[12];
    
    ::memcpy(&t,p+13,4); irq_counter = t;
    ::memcpy(&t,p+17,4); irq_latch = t;

    return OK;
}
int    CNesMapper::Mapper246_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    return OK;
}

int CNesMapper::Mapper246_WriteLow( WORD addr, BYTE data )
{
    if( addr>=0x6000 && addr<0x8000 ) {
        switch( addr ) {
            case 0x6000:
                mmu->SetPROM_8K_Bank( 4, data );
                break;
            case 0x6001:
                mmu->SetPROM_8K_Bank( 5, data );
                break;
            case 0x6002:
                mmu->SetPROM_8K_Bank( 6, data );
                break;
            case 0x6003: 
                mmu->SetPROM_8K_Bank( 7, data );
                break;
            case 0x6004:
                mmu->SetVROM_2K_Bank(0,data);
                break;
            case 0x6005:
                mmu->SetVROM_2K_Bank(2,data);
                break;
            case 0x6006:
                mmu->SetVROM_2K_Bank(4,data);
                break;
            case 0x6007:
                mmu->SetVROM_2K_Bank(6,data);
                break;
            default:
                mmu->CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
                break;
        }
    }

    return OK;
}

int CNesMapper::Mapper034_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    return OK;
}

int CNesMapper::Mapper034_WriteLow( WORD addr, BYTE data )
{
    switch( addr ) {
        case    0x7FFD:
            mmu->SetPROM_32K_Bank( data );
            break;
        case    0x7FFE:
            mmu->SetVROM_4K_Bank( 0, data );
            break;
        case    0x7FFF:
            mmu->SetVROM_4K_Bank( 4, data );
            break;
    }

    return OK;
}

int CNesMapper::Mapper034_Write( WORD addr, BYTE data )
{
    mmu->SetPROM_32K_Bank( data );
    return OK;
}

int CNesMapper::Mapper071_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    return OK;
}

int CNesMapper::Mapper071_WriteLow( WORD addr, BYTE data )
{
    if( (addr&0xE000)==0x6000 ) {
        mmu->SetPROM_16K_Bank( 4, data );
    }
    return OK;
}

int CNesMapper::Mapper071_Write( WORD addr, BYTE data )
{
    switch( addr&0xF000 ) {
        case    0x9000:
            if( data&0x10 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
            else        mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
            break;

        case    0xC000:
        case    0xD000:
        case    0xE000:
        case    0xF000:
            mmu->SetPROM_16K_Bank( 4, data );
            break;
    }
    return OK;
}

int CNesMapper::Mapper240_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    return OK;
}

int CNesMapper::Mapper240_WriteLow( WORD addr, BYTE data )
{
    if( addr>=0x4020 && addr<0x6000 ) {
        mmu->SetPROM_32K_Bank( (data&0xF0)>>4 );
        mmu->SetVROM_8K_Bank(data&0xF);
    }

    return OK;
}
int CNesMapper::Mapper073_Reset()
{
    irq_enable = 0;
    irq_counter = 0;

    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    return OK;
}

int CNesMapper::Mapper073_Write( WORD addr, BYTE data )
{
    switch( addr ) {
        case    0xF000:
            mmu->SetPROM_16K_Bank( 4, data );
            break;

        case    0x8000:
            irq_counter = (irq_counter & 0xFFF0) | (data & 0x0F);
            break;
        case    0x9000:
            irq_counter = (irq_counter & 0xFF0F) | ((data & 0x0F) << 4);
            break;
        case    0xA000:
            irq_counter = (irq_counter & 0xF0FF) | ((data & 0x0F) << 8);
            break;
        case    0xB000:
            irq_counter = (irq_counter & 0x0FFF) | ((data & 0x0F) << 12);
            break;
        case    0xC000:
            irq_enable = data & 0x02;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0xD000:
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
    }
    return OK;
}

int CNesMapper::Mapper073_Clock( int cycles )
{
    if( irq_enable ) {
        if( (irq_counter+=cycles) >= 0xFFFF ) {
            irq_enable = 0;
            irq_counter &= 0xFFFF;
            nes->cpu->SetIRQ( IRQ_MAPPER );
        }
    }
    return OK;
}

int CNesMapper::Mapper073_SaveState( BYTE * p )
{
    int t;
    p[0] = irq_enable;
    t = irq_counter; ::memcpy(p+1,&t,4);
    return OK;
}

int CNesMapper::Mapper073_LoadState( BYTE * p )
{
    int t;

    irq_enable  = p[0];     
    ::memcpy(&t,p+1,4); irq_counter = t;
    return OK;
}

int CNesMapper::Mapper091_Reset()
{
    mmu->SetPROM_32K_Bank( mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_8K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0, 0, 0, 0, 0, 0, 0, 0 );
    }

    irq_enable = 0;
    irq_counter = 0;

    nes->SetRenderMethod( POST_ALL_RENDER );
    return OK;
}

int CNesMapper::Mapper091_WriteLow( WORD addr, BYTE data )
{
    switch( addr & 0xF003 ) {
        case    0x6000:
        case    0x6001:
        case    0x6002:
        case    0x6003:
            mmu->SetVROM_2K_Bank( (addr&0x03)*2, data );
            break;

        case    0x7000:
            mmu->SetPROM_8K_Bank( 4, data );
            break;
        case    0x7001:
            mmu->SetPROM_8K_Bank( 5, data );
            break;

        case    0x7002:
            irq_enable = 0;
            irq_counter = 0;
            nes->cpu->ClrIRQ( IRQ_MAPPER );
            break;
        case    0x7003:
            irq_enable = 1;
            break;
    }
    return OK;
}

int CNesMapper::Mapper091_HSync( int scanline )
{
    if( (scanline >= 0 && scanline < 240) && nes->ppu->IsDispON() ) {
        if( irq_enable ) {
            irq_counter++;
        }
        if( irq_counter >= 8 ) {
            nes->cpu->SetIRQ( IRQ_MAPPER );
        }
    }
    return OK;
}

int CNesMapper::Mapper091_SaveState( BYTE * p )
{
    p[0] = irq_enable;
    p[1] = (BYTE)irq_counter;
    return OK;
}

int CNesMapper::Mapper091_LoadState( BYTE * p )
{
    irq_enable  = p[0];
    irq_counter = p[1];
    return OK;
}

int CNesMapper::Mapper114_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    reg[8] = reg[9] = reg[10] = 0;
    for( int i = 0; i < 8; i++ ) {
        reg[i] = 0;
    }
    irq_counter = 0;
    irq_occur = 0;

    nes->SetRenderMethod( POST_RENDER );
    return OK;
}

int CNesMapper::Mapper114_WriteLow( WORD addr, BYTE data )
{
    reg[10] = data;
    Mapper114_SetBank_CPU();
    return OK;
}

int CNesMapper::Mapper114_Write( WORD addr, BYTE data )
{
    if( addr == 0xE003 ) {
        irq_counter = data;
    } else
    if( addr == 0xE002 ) {
        irq_occur = 0;
        nes->cpu->ClrIRQ( IRQ_MAPPER );
    } else {
        switch( addr & 0xE000 ) {
            case    0x8000:
                if( data & 0x01 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
                else          mmu->SetVRAM_Mirror( VRAM_VMIRROR );
                break;
            case    0xA000:
                reg[9] = 1;
                reg[8] = data;
                break;
            case    0xC000:
                if( !reg[9] ) {
                    break;
                }
                reg[reg[8]&0x07] = data;
                switch( reg[8] & 0x07 ) {
                    case    0:
                    case    1:
                    case    2:
                    case    3:
                    case    6:
                    case    7:
                        Mapper114_SetBank_PPU();
                        break;
                    case    4:
                    case    5:
                        Mapper114_SetBank_CPU();
                        break;
                }
                reg[9] = 0;
                break;
        }
    }
    return OK;
}

int CNesMapper::Mapper114_Clock( int scanline )
{
    return OK;
}

int CNesMapper::Mapper114_HSync( int scanline )
{
    if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
        if( irq_counter ) {
            irq_counter--;
            if( !irq_counter ) {
                irq_occur = 0xFF;
                nes->cpu->SetIRQ( IRQ_MAPPER );
            }
        }
    }
    return OK;
}

int CNesMapper::Mapper114_SetBank_CPU()
{
    if( reg[10] & 0x80 ) {
        mmu->SetPROM_16K_Bank( 4, reg[10] & 0x1F );
    } else {
        mmu->SetPROM_8K_Bank( 4, reg[4] );
        mmu->SetPROM_8K_Bank( 5, reg[5] );
    }
    return OK;
}

int CNesMapper::Mapper114_SetBank_PPU()
{
    mmu->SetVROM_2K_Bank( 0, reg[0]>>1 );
    mmu->SetVROM_2K_Bank( 2, reg[2]>>1 );
    mmu->SetVROM_1K_Bank( 4, reg[6] );
    mmu->SetVROM_1K_Bank( 5, reg[1] );
    mmu->SetVROM_1K_Bank( 6, reg[7] );
    mmu->SetVROM_1K_Bank( 7, reg[3] );
    return OK;
}

int CNesMapper::Mapper114_SaveState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        p[i] = reg[i];
    }

    p[ 8] = reg[10];
    p[ 9] = reg[8];
    p[10] = reg[9];
    p[11] = (BYTE)irq_counter;
    p[12] = irq_occur;
    return OK;
}

int CNesMapper::Mapper114_LoadState( BYTE * p )
{
    for( int i = 0; i < 8; i++ ) {
        reg[i] = p[i];
    }
    reg[10] = p[ 8];
    reg[8] = p[ 9];
    reg[9] = p[10];
    irq_counter = p[11];
    irq_occur   = p[12];
    return OK;
}

int CNesMapper::Mapper241_Reset()
{
    mmu->SetPROM_32K_Bank( 0 );

    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }
    return OK;
}

int CNesMapper::Mapper241_Write( WORD addr, BYTE data )
{
    if( addr == 0x8000 ) {
        mmu->SetPROM_32K_Bank( data );
    }
    return OK;
}

int CNesMapper::Mapper017_Reset()
{
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );

    if( mmu->VROM_1K_SIZE ) {
        mmu->SetVROM_8K_Bank( 0 );
    }

    irq_enable = 0;
    irq_counter = 0;
    irq_latch = 0;
    
    return OK;
}

int CNesMapper::Mapper017_WriteLow(WORD addr, BYTE data)
{
    switch( addr ) {
    case    0x42FE:
        if( data&0x10 ) mmu->SetVRAM_Mirror( VRAM_MIRROR4H );
        else        mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
        break;
    case    0x42FF:
        if( data&0x10 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
        else        mmu->SetVRAM_Mirror( VRAM_VMIRROR );
        break;
        
    case    0x4501:
        irq_enable = 0;
        nes->cpu->ClrIRQ( IRQ_MAPPER );
        break;
    case    0x4502:
        irq_latch = (irq_latch&0xFF00)|data;
        break;
    case    0x4503:
        irq_latch = (irq_latch&0x00FF)|((int)data<<8);
        irq_counter = irq_latch;
        irq_enable = 0xFF;
        break;
        
    case    0x4504:
    case    0x4505:
    case    0x4506:
    case    0x4507:
        mmu->SetPROM_8K_Bank( addr&0x07, data );
        break;
        
    case    0x4510:
    case    0x4511:
    case    0x4512:
    case    0x4513:
    case    0x4514:
    case    0x4515:
    case    0x4516:
    case    0x4517:
        mmu->SetVROM_1K_Bank( addr&0x07, data );
        break;
        
    default:
        this->WriteLow_Def( addr, data );
        break;
    }
    
    return OK;
}

int CNesMapper::Mapper017_HSync(int scanline)
{
    if( irq_enable ) {
        if( irq_counter >= 0xFFFF-113 ) {
            nes->cpu->SetIRQ( IRQ_MAPPER );
            irq_counter &= 0xFFFF;
        } else {
            irq_counter += 113;
        }
    }

    return OK;
}

int CNesMapper::Mapper017_SaveState(BYTE *p)
{
    int t;

    p[0] = irq_enable;
    t = irq_counter; memcpy(&p[1],&t,4);
    t = irq_latch; memcpy(&p[5],&t,4);

    return OK;
}

int CNesMapper::Mapper017_LoadState(BYTE *p)
{
    int t;

    irq_enable = p[0];

    memcpy(&t,&p[1],4); irq_counter = t;
    memcpy(&t,&p[5],4); irq_latch = t;
    
    return OK;
}

int CNesMapper::Mapper062_Reset()
{
    mmu->SetPROM_32K_Bank( 0 );
    mmu->SetVROM_8K_Bank( 0 );
    return OK;
}

int CNesMapper::Mapper062_Write(WORD addr, BYTE data)
{
    switch( addr & 0xFF00 ) {
    case    0x8100:
        mmu->SetPROM_8K_Bank( 4, data );
        mmu->SetPROM_8K_Bank( 5, data+1 );
        break;
    case    0x8500:
        mmu->SetPROM_8K_Bank( 4, data );
        break;
    case    0x8700:
        mmu->SetPROM_8K_Bank( 5, data );
        break;
        mmu->SetVROM_1K_Bank( 0, data+0 );
        mmu->SetVROM_1K_Bank( 1, data+1 );
        mmu->SetVROM_1K_Bank( 2, data+2 );
        mmu->SetVROM_1K_Bank( 3, data+3 );
        mmu->SetVROM_1K_Bank( 4, data+4 );
        mmu->SetVROM_1K_Bank( 5, data+5 );
        mmu->SetVROM_1K_Bank( 6, data+6 );
        mmu->SetVROM_1K_Bank( 7, data+7 );
    }

    return OK;
}

int CNesMapper::Mapper019_Reset()
{
    patch = 0;
    
    reg[0] = reg[1] = reg[2] = 0;
    
    memset( exram, 0, sizeof(exram) );
    
    irq_enable = 0;
    irq_counter = 0;
    
    mmu->SetPROM_32K_Bank( 0, 1, mmu->PROM_8K_SIZE-2, mmu->PROM_8K_SIZE-1 );
    
    if( mmu->VROM_1K_SIZE >= 8 ) {
        mmu->SetVROM_8K_Bank( mmu->VROM_8K_SIZE-1 );
    }
    
    exsound_enable = 0xFF;
    
    DWORD   crc = nes->rom->GetPROM_CRC();
    
    if( crc == 0xb62a7b71 ) {   // Family Circuit '91(J)
        patch = 1;
    }
    
    if( crc == 0x02738c68 ) {   // Wagan Land 2(J)
        patch = 3;
    }
    if( crc == 0x14942c06 ) {   // Wagan Land 3(J)
        patch = 2;
    }
    
    if( crc == 0x968dcf09 ) {   // Final Lap(J)
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }
    if( crc == 0x3deac303 ) {   // Rolling Thunder(J)
        nes->SetRenderMethod( POST_ALL_RENDER );
    }
    
    if( crc == 0xb1b9e187 ) {   // For Kaijuu Monogatari(J)
        nes->SetRenderMethod( PRE_ALL_RENDER );
    }
    
    if( crc == 0x6901346e ) {   // For Sangokushi 2 - Haou no Tairiku(J)
        nes->SetRenderMethod( TILE_RENDER );
    }
    
    if( crc == 0xaf15338f       // For Mindseeker(J)
        || crc == 0xb1b9e187        // For Kaijuu Monogatari(J)
        || crc == 0x96533999        // Dokuganryuu Masamune(J)
        || crc == 0x3296ff7a        // Battle Fleet(J)
        || crc == 0xdd454208 ) {    // Hydlide 3(J)
        exsound_enable = 0;
    }
    
    if( crc == 0x429fd177 ) {   // Famista '90(J)
        exsound_enable = 0;
    }
    
    if( exsound_enable ) {
        nes->apu->SelectExSound( 0x10 );
    }
    
    return OK;
}

BYTE CNesMapper::Mapper019_ReadLow(WORD addr)
{
    BYTE    data = 0;
    
    switch( addr & 0xF800 ) {
    case    0x4800:
        if( addr == 0x4800 ) {
            if( exsound_enable ) {
                nes->apu->ExRead(addr);
                data = exram[reg[2]&0x7F];
            } else {
                data = mmu->WRAM[reg[2]&0x7F];
            }
            if( reg[2]&0x80 )
                reg[2] = (reg[2]+1)|0x80;
            return  data;
        }
        break;
    case    0x5000:
        return  (BYTE)irq_counter & 0x00FF;
    case    0x5800:
        return  (BYTE)((irq_counter>>8) & 0x7F);
    case    0x6000:
    case    0x6800:
    case    0x7000:
    case    0x7800:
        return  this->ReadLow_Def( addr );
    }
    
    return  (BYTE)(addr>>8);
}

int CNesMapper::Mapper019_WriteLow(WORD addr, BYTE data)
{
    switch( addr & 0xF800 ) {
    case    0x4800:
        if( addr == 0x4800 ) {
            if( exsound_enable ) {
                nes->apu->ExWrite( addr, data );
                exram[reg[2]&0x7F] = data;
            } else {
                mmu->WRAM[reg[2]&0x7F] = data;
            }
            if( reg[2]&0x80 )
                reg[2] = (reg[2]+1)|0x80;
        }
        break;
    case    0x5000:
        irq_counter = (irq_counter & 0xFF00) | (WORD)data;
        nes->cpu->ClrIRQ( IRQ_MAPPER );
        break;
    case    0x5800:
        irq_counter = (irq_counter & 0x00FF) | ((WORD)(data & 0x7F) << 8);
        irq_enable  = data & 0x80;
        nes->cpu->ClrIRQ( IRQ_MAPPER );
        break;
    case    0x6000:
    case    0x6800:
    case    0x7000:
    case    0x7800:
        this->WriteLow_Def( addr, data );
        break;
    }
    
    return OK;
}

int CNesMapper::Mapper019_Write(WORD addr, BYTE data)
{
    switch( addr & 0xF800 ) {
    case    0x8000:
        if( (data < 0xE0) || (reg[0] != 0) ) {
            mmu->SetVROM_1K_Bank( 0, data );
        } else {
            mmu->SetCRAM_1K_Bank( 0, data&0x1F );
        }
        break;
    case    0x8800:
        if( (data < 0xE0) || (reg[0] != 0) ) {
            mmu->SetVROM_1K_Bank( 1, data );
        } else {
            mmu->SetCRAM_1K_Bank( 1, data&0x1F );
        }
        break;
    case    0x9000:
        if( (data < 0xE0) || (reg[0] != 0) ) {
            mmu->SetVROM_1K_Bank( 2, data );
        } else {
            mmu->SetCRAM_1K_Bank( 2, data&0x1F );
        }
        break;
    case    0x9800:
        if( (data < 0xE0) || (reg[0] != 0) ) {
            mmu->SetVROM_1K_Bank( 3, data );
        } else {
            mmu->SetCRAM_1K_Bank( 3, data&0x1F );
        }
        break;
    case    0xA000:
        if( (data < 0xE0) || (reg[1] != 0) ) {
            mmu->SetVROM_1K_Bank( 4, data );
        } else {
            mmu->SetCRAM_1K_Bank( 4, data&0x1F );
        }
        break;
    case    0xA800:
        if( (data < 0xE0) || (reg[1] != 0) ) {
            mmu->SetVROM_1K_Bank( 5, data );
        } else {
            mmu->SetCRAM_1K_Bank( 5, data&0x1F );
        }
        break;
    case    0xB000:
        if( (data < 0xE0) || (reg[1] != 0) ) {
            mmu->SetVROM_1K_Bank( 6, data );
        } else {
            mmu->SetCRAM_1K_Bank( 6, data&0x1F );
        }
        break;
    case    0xB800:
        if( (data < 0xE0) || (reg[1] != 0) ) {
            mmu->SetVROM_1K_Bank( 7, data );
        } else {
            mmu->SetCRAM_1K_Bank( 7, data&0x1F );
        }
        break;
    case    0xC000:
        if( !patch ) {
            if( data <= 0xDF ) {
                mmu->SetVROM_1K_Bank( 8, data );
            } else {
                mmu->SetVRAM_1K_Bank( 8, data & 0x01 );
            }
        }
        break;
    case    0xC800:
        if( !patch ) {
            if( data <= 0xDF ) {
                mmu->SetVROM_1K_Bank( 9, data );
            } else {
                mmu->SetVRAM_1K_Bank( 9, data & 0x01 );
            }
        }
        break;
    case    0xD000:
        if( !patch ) {
            if( data <= 0xDF ) {
                mmu->SetVROM_1K_Bank( 10, data );
            } else {
                mmu->SetVRAM_1K_Bank( 10, data & 0x01 );
            }
        }
        break;
    case    0xD800:
        if( !patch ) {
            if( data <= 0xDF ) {
                mmu->SetVROM_1K_Bank( 11, data );
            } else {
                mmu->SetVRAM_1K_Bank( 11, data & 0x01 );
            }
        }
        break;
    case    0xE000:
        mmu->SetPROM_8K_Bank( 4, data & 0x3F );
        if( patch == 2 ) {
            if( data & 0x40 ) mmu->SetVRAM_Mirror( VRAM_VMIRROR );
            else          mmu->SetVRAM_Mirror( VRAM_MIRROR4L );
        }
        if( patch == 3 ) {
            if( data & 0x80 ) mmu->SetVRAM_Mirror( VRAM_HMIRROR );
            else          mmu->SetVRAM_Mirror( VRAM_VMIRROR );
        }
        break;
    case    0xE800:
        reg[0] = data & 0x40;
        reg[1] = data & 0x80;
        mmu->SetPROM_8K_Bank( 5, data & 0x3F );
        break;
    case    0xF000:
        mmu->SetPROM_8K_Bank( 6, data & 0x3F );
        break;
    case    0xF800:
        if( addr == 0xF800 ) {
            if( exsound_enable ) {
                nes->apu->ExWrite( addr, data );
            }
            reg[2] = data;
        }
        break;
    }
    
    return OK;
}

int CNesMapper::Mapper019_Clock(int cycles)
{
    if( irq_enable ) {
        if( (irq_counter+=cycles) >= 0x7FFF ) {
            irq_enable  = 0;
            irq_counter = 0x7FFF;
            nes->cpu->SetIRQ( IRQ_MAPPER );
        }
    }

    return OK;
}

int CNesMapper::Mapper019_SaveState(BYTE *p)
{
    p[0] = reg[0];
    p[1] = reg[1];
    p[2] = reg[2];
    p[3] = irq_enable;
    memcpy(&p[4],&irq_counter,sizeof(irq_counter));
    memcpy( &p[8], exram, sizeof(exram) );

    return OK;
}

int CNesMapper::Mapper019_LoadState(BYTE *p)
{
    reg[0] = p[0];
    reg[1] = p[1];
    reg[2] = p[2];
    irq_enable  = p[3];
    memcpy(&irq_counter,&p[4],sizeof(irq_counter));
    memcpy( exram, &p[8], sizeof(exram) );

    return OK;
}
