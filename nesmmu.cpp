// NesMmu.cpp: implementation of the CNesMmu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NesMmu.h"
#include "mem_tool.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNesMmu::CNesMmu()
{
    this->InitBasic();
}
CNesMmu::~CNesMmu()
{
    this->Destroy();
}
int CNesMmu::InitBasic()
{
    this->CPU_MEM_BANK = 0;
    this->CPU_MEM_TYPE = 0;
    this->CPU_MEM_PAGE = 0;
    this->PPU_MEM_BANK = 0;
    this->PPU_MEM_TYPE = 0;
    this->PPU_MEM_PAGE = 0;
    this->CRAM_USED = 0;
    this->RAM = 0;
    this->WRAM = 0;
    this->DRAM = 0;
    this->XRAM = 0;
    this->ERAM = 0;
    this->CRAM = 0;
    this->VRAM = 0;
    this->SPRAM = 0;
    this->BGPAL = 0;
    this->SPPAL = 0;
    this->CPUREG = 0;
    this->PPUREG = 0;
    this->FrameIRQ = 0;
    this->PPU56Toggle = 0;
    this->PPU7_Temp = 0;
    this->loopy_t = 0;
    this->loopy_v = 0;
    this->loopy_x = 0;
    this->PROM = 0;
    this->VROM = 0;
    this->PROM_ACCESS = 0;
    this->PROM_8K_SIZE = 0;
    this->PROM_16K_SIZE = 0;
    this->PROM_32K_SIZE = 0;
    this->VROM_1K_SIZE = 0;
    this->VROM_2K_SIZE = 0;
    this->VROM_4K_SIZE = 0;
    this->VROM_8K_SIZE = 0;
    
    return OK;
}
int CNesMmu::Init()
{
    this->InitBasic();
    
    MALLOC(this->CPU_MEM_BANK,BYTE*,NES_CPU_MEM_BANK_SIZE);
    MALLOC(this->CPU_MEM_TYPE,BYTE,NES_CPU_MEM_TYPE_SIZE);
    MALLOC(this->CPU_MEM_PAGE,int,NES_CPU_MEM_PAGE_SIZE);
    MALLOC(this->PPU_MEM_BANK,BYTE*,NES_PPU_MEM_BANK_SIZE);
    MALLOC(this->PPU_MEM_TYPE,BYTE,NES_PPU_MEM_TYPE_SIZE);
    MALLOC(this->PPU_MEM_PAGE,int,NES_PPU_MEM_PAGE_SIZE);
    MALLOC(this->CRAM_USED,BYTE,NES_CRAM_USED_SIZE);
    MALLOC(this->RAM,BYTE,NES_RAM_SIZE);
    MALLOC(this->WRAM,BYTE,NES_WRAM_SIZE);
    MALLOC(this->DRAM,BYTE,NES_DRAM_SIZE);
    MALLOC(this->XRAM,BYTE,NES_XRAM_SIZE);
    MALLOC(this->ERAM,BYTE,NES_ERAM_SIZE);
    MALLOC(this->CRAM,BYTE,NES_CRAM_SIZE);
    MALLOC(this->VRAM,BYTE,NES_VRAM_SIZE);
    MALLOC(this->SPRAM,BYTE,NES_SPRAM_SIZE);
    MALLOC(this->BGPAL,BYTE,NES_BGPAL_SIZE);
    MALLOC(this->SPPAL,BYTE,NES_SPPAL_SIZE);
    MALLOC(this->CPUREG,BYTE,NES_CPUREG_SIZE);
    MALLOC(this->PPUREG,BYTE,NES_PPUREG_SIZE);

    this->MemoryInitial();

    return OK;
}
int CNesMmu::Destroy()
{
    FREE(this->CPU_MEM_BANK);
    FREE(this->CPU_MEM_TYPE);
    FREE(this->CPU_MEM_PAGE);
    FREE(this->PPU_MEM_BANK);
    FREE(this->PPU_MEM_TYPE);
    FREE(this->PPU_MEM_PAGE);
    FREE(this->CRAM_USED);
    FREE(this->RAM);
    FREE(this->WRAM);
    FREE(this->DRAM);
    FREE(this->XRAM);
    FREE(this->ERAM);
    FREE(this->CRAM);
    FREE(this->VRAM);
    FREE(this->SPRAM);
    FREE(this->BGPAL);
    FREE(this->SPPAL);
    FREE(this->CPUREG);
    FREE(this->PPUREG);

    this->InitBasic();

    return OK;
}
int CNesMmu::Copy(CNesMmu *p)
{
    if(this == p)
        return OK;
    return OK;
}
int CNesMmu::Comp(CNesMmu *p)
{
    return 0;
}
int CNesMmu::Print()
{
    return TRUE;
}

int CNesMmu::MemoryInitial()
{
    int i;

    for(i = 0; i < NES_PPU_MEM_BANK_SIZE; i++)
        this->PPU_MEM_BANK[i] = 0;
    for(i = 0; i < NES_PPU_MEM_TYPE_SIZE; i++)
        this->PPU_MEM_TYPE[i] = 0;
    for(i = 0; i < NES_PPU_MEM_PAGE_SIZE; i++)
        this->PPU_MEM_PAGE[i] = 0;
    for(i = 0; i < NES_CRAM_USED_SIZE; i++)
        this->CRAM_USED[i] = 0;
    for(i = 0; i < NES_RAM_SIZE; i++)
        this->RAM[i] = 0;
    for(i = 0; i < NES_WRAM_SIZE; i++)
        this->WRAM[i] = 0;
    for(i = 0; i < NES_DRAM_SIZE; i++)
        this->DRAM[i] = 0;
    for(i = 0; i < NES_XRAM_SIZE; i++)
        this->XRAM[i] = 0;
    for(i = 0; i < NES_ERAM_SIZE; i++)
        this->ERAM[i] = 0;
    for(i = 0; i < NES_CRAM_SIZE; i++)
        this->CRAM[i] = 0;
    for(i = 0; i < NES_VRAM_SIZE; i++)
        this->VRAM[i] = 0;
    for(i = 0; i < NES_SPRAM_SIZE; i++)
        this->SPRAM[i] = 0;
    for(i = 0; i < NES_BGPAL_SIZE; i++)
        this->BGPAL[i] = 0;
    for(i = 0; i < NES_SPPAL_SIZE; i++)
        this->SPPAL[i] = 0;
    for(i = 0; i < NES_CPUREG_SIZE; i++)
        this->CPUREG[i] = 0;
    for(i = 0; i < NES_PPUREG_SIZE; i++)
        this->PPUREG[i] = 0;

    this->FrameIRQ = 0xC0;
    
    PROM = VROM = NULL;

    PROM_8K_SIZE = PROM_16K_SIZE = PROM_32K_SIZE = 1;
    VROM_1K_SIZE = VROM_2K_SIZE = VROM_4K_SIZE = VROM_8K_SIZE = 1;

    for(i = 0; i < NES_CPU_MEM_BANK_SIZE; i++)
    {
        this->CPU_MEM_BANK[i] = 0;
        this->CPU_MEM_TYPE[i] = BANKTYPE_ROM;
        this->CPU_MEM_PAGE[i] = 0;
    }
            
    SetPROM_Bank( 0, RAM,  BANKTYPE_RAM );
    SetPROM_Bank( 3, WRAM, BANKTYPE_RAM );
    
    SetPROM_Bank( 1, XRAM, BANKTYPE_ROM );
    SetPROM_Bank( 2, XRAM, BANKTYPE_ROM );

    return OK;
}

void CNesMmu::SetPROM_Bank(BYTE page, BYTE *ptr, BYTE type)
{
    CPU_MEM_BANK[page] = ptr;
    CPU_MEM_TYPE[page] = type;
    CPU_MEM_PAGE[page] = 0; 
}
void CNesMmu::SetPROM_8K_Bank( BYTE page, int bank )
{
    bank %= PROM_8K_SIZE;
    CPU_MEM_BANK[page] = PROM+0x2000*bank;
    CPU_MEM_TYPE[page] = BANKTYPE_ROM;
    CPU_MEM_PAGE[page] = bank;
}

void CNesMmu::SetPROM_16K_Bank( BYTE page, int bank )
{
    SetPROM_8K_Bank( page+0, bank*2+0 );
    SetPROM_8K_Bank( page+1, bank*2+1 );
}

void CNesMmu::SetPROM_32K_Bank( int bank )
{
    SetPROM_8K_Bank( 4, bank*4+0 );
    SetPROM_8K_Bank( 5, bank*4+1 );
    SetPROM_8K_Bank( 6, bank*4+2 );
    SetPROM_8K_Bank( 7, bank*4+3 );
}

void CNesMmu::SetPROM_32K_Bank( int bank0, int bank1, int bank2, int bank3 )
{
    SetPROM_8K_Bank( 4, bank0 );
    SetPROM_8K_Bank( 5, bank1 );
    SetPROM_8K_Bank( 6, bank2 );
    SetPROM_8K_Bank( 7, bank3 );
}

void CNesMmu::SetVROM_Bank( BYTE page, BYTE * ptr, BYTE type )
{
    PPU_MEM_BANK[page] = ptr;
    PPU_MEM_TYPE[page] = type;
    PPU_MEM_PAGE[page] = 0;
}

void CNesMmu::SetVROM_1K_Bank( BYTE page, int bank )
{
    bank %= VROM_1K_SIZE;
    PPU_MEM_BANK[page] = VROM+0x0400*bank;
    PPU_MEM_TYPE[page] = BANKTYPE_VROM;
    PPU_MEM_PAGE[page] = bank;
}

void CNesMmu::SetVROM_2K_Bank( BYTE page, int bank )
{
    SetVROM_1K_Bank( page+0, bank*2+0 );
    SetVROM_1K_Bank( page+1, bank*2+1 );
}

void CNesMmu::SetVROM_4K_Bank( BYTE page, int bank )
{
    SetVROM_1K_Bank( page+0, bank*4+0 );
    SetVROM_1K_Bank( page+1, bank*4+1 );
    SetVROM_1K_Bank( page+2, bank*4+2 );
    SetVROM_1K_Bank( page+3, bank*4+3 );
}

void CNesMmu::SetVROM_8K_Bank( int bank )
{
    for( int i = 0; i < 8; i++ ) 
    {
        SetVROM_1K_Bank( i, bank*8+i );
    }
}

void CNesMmu::SetVROM_8K_Bank( int bank0, int bank1, int bank2, int bank3,
             int bank4, int bank5, int bank6, int bank7 )
{
    SetVROM_1K_Bank( 0, bank0 );
    SetVROM_1K_Bank( 1, bank1 );
    SetVROM_1K_Bank( 2, bank2 );
    SetVROM_1K_Bank( 3, bank3 );
    SetVROM_1K_Bank( 4, bank4 );
    SetVROM_1K_Bank( 5, bank5 );
    SetVROM_1K_Bank( 6, bank6 );
    SetVROM_1K_Bank( 7, bank7 );
}

void CNesMmu::SetCRAM_1K_Bank( BYTE page, int bank )
{
    bank &= 0x1F;
    PPU_MEM_BANK[page] = CRAM+0x0400*bank;
    PPU_MEM_TYPE[page] = BANKTYPE_CRAM;
    PPU_MEM_PAGE[page] = bank;

    CRAM_USED[bank>>2] = 0xFF;  
}

void CNesMmu::SetCRAM_2K_Bank( BYTE page, int bank )
{
    SetCRAM_1K_Bank( page+0, bank*2+0 );
    SetCRAM_1K_Bank( page+1, bank*2+1 );
}

void CNesMmu::SetCRAM_4K_Bank( BYTE page, int bank )
{
    SetCRAM_1K_Bank( page+0, bank*4+0 );
    SetCRAM_1K_Bank( page+1, bank*4+1 );
    SetCRAM_1K_Bank( page+2, bank*4+2 );
    SetCRAM_1K_Bank( page+3, bank*4+3 );
}

void CNesMmu::SetCRAM_8K_Bank( int bank )
{
    for( int i = 0; i < 8; i++ ) {
        SetCRAM_1K_Bank( i, bank*8+i ); // fix
    }
}

void CNesMmu::SetVRAM_1K_Bank( BYTE page, int bank )
{
    bank &= 3;
    PPU_MEM_BANK[page] = VRAM+0x0400*bank;
    PPU_MEM_TYPE[page] = BANKTYPE_VRAM;
    PPU_MEM_PAGE[page] = bank;
}

void CNesMmu::SetVRAM_Bank( int bank0, int bank1, int bank2, int bank3 )
{
    SetVRAM_1K_Bank(  8, bank0 );
    SetVRAM_1K_Bank(  9, bank1 );
    SetVRAM_1K_Bank( 10, bank2 );
    SetVRAM_1K_Bank( 11, bank3 );
}

void CNesMmu::SetVRAM_Mirror( int type )
{
    switch( type ) {
        case VRAM_HMIRROR:
            SetVRAM_Bank( 0, 0, 1, 1 );
            break;
        case VRAM_VMIRROR:
            SetVRAM_Bank( 0, 1, 0, 1 );
            break;
        case VRAM_MIRROR4L:
            SetVRAM_Bank( 0, 0, 0, 0 );
            break;
        case VRAM_MIRROR4H:
            SetVRAM_Bank( 1, 1, 1, 1 );
            break;
        case VRAM_MIRROR4:
            SetVRAM_Bank( 0, 1, 2, 3 );
            break;
    }
}

void CNesMmu::SetVRAM_Mirror( int bank0, int bank1, int bank2, int bank3 )
{
    SetVRAM_1K_Bank(  8, bank0 );
    SetVRAM_1K_Bank(  9, bank1 );
    SetVRAM_1K_Bank( 10, bank2 );
    SetVRAM_1K_Bank( 11, bank3 );
}

