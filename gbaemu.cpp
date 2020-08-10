// GbaEmu.cpp: implementation of the CGbaEmu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GbaEmu.h"
#include "mem_tool.h"
#include "misc.h"
#include "file.h"

#define R13_IRQ  18
#define R14_IRQ  19
#define SPSR_IRQ 20
#define R13_USR  26
#define R14_USR  27
#define R13_SVC  28
#define R14_SVC  29
#define SPSR_SVC 30
#define R13_ABT  31
#define R14_ABT  32
#define SPSR_ABT 33
#define R13_UND  34
#define R14_UND  35
#define SPSR_UND 36
#define R8_FIQ   37
#define R9_FIQ   38
#define R10_FIQ  39
#define R11_FIQ  40
#define R12_FIQ  41
#define R13_FIQ  42
#define R14_FIQ  43
#define SPSR_FIQ 44

#define VERBOSE_SWI                  1
#define VERBOSE_UNALIGNED_MEMORY     2
#define VERBOSE_ILLEGAL_WRITE        4
#define VERBOSE_ILLEGAL_READ         8
#define VERBOSE_DMA0                16
#define VERBOSE_DMA1                32
#define VERBOSE_DMA2                64
#define VERBOSE_DMA3               128
#define VERBOSE_UNDEFINED          256
#define VERBOSE_AGBPRINT           512

#define SYSTEM_SAVE_UPDATED 30
#define SYSTEM_SAVE_NOT_UPDATED 0

#define FLASH_READ_ARRAY         0
#define FLASH_CMD_1              1
#define FLASH_CMD_2              2
#define FLASH_AUTOSELECT         3
#define FLASH_CMD_3              4
#define FLASH_CMD_4              5
#define FLASH_CMD_5              6
#define FLASH_ERASE_COMPLETE     7
#define FLASH_PROGRAM            8
#define FLASH_SETBANK            9

#define EEPROM_IDLE           0
#define EEPROM_READADDRESS    1
#define EEPROM_READDATA       2
#define EEPROM_READDATA2      3
#define EEPROM_WRITEDATA      4

#define CPUReadByteQuick(addr) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]

#define CPUReadHalfWordQuick(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define CPUReadMemoryQuick(addr) \
  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define UPDATE_REG(address, value) WRITE16LE(((u16 *)&ioMem[address]),value)

#define CPU_BREAK_LOOP \
  cpuSavedTicks = cpuSavedTicks - *extCpuLoopTicks;\
  *extCpuLoopTicks = *extClockTicks;

#define CPU_BREAK_LOOP_2 \
  cpuSavedTicks = cpuSavedTicks - *extCpuLoopTicks;\
  *extCpuLoopTicks = *extClockTicks;\
  *extTicks = *extClockTicks;

#define debuggerWriteHalfWord(addr, value) \
  WRITE16LE((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask], (value))

#define debuggerReadHalfWord(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

const int TIMER_TICKS[4] = {
  1,
  64,
  256,
  1024
};

const int thumbCycles[] = {
//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 1
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 3
    1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 4
    2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 5
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,  // 6
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,  // 7
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,  // 8
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,  // 9
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // a
    1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1,  // b
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // c
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,  // d
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // e
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2   // f
};

const int gamepakRamWaitState[4] = { 4, 3, 2, 8 };
const int gamepakWaitState[8] =  { 4, 3, 2, 8, 4, 3, 2, 8 };
const int gamepakWaitState0[8] = { 2, 2, 2, 2, 1, 1, 1, 1 };
const int gamepakWaitState1[8] = { 4, 4, 4, 4, 1, 1, 1, 1 };
const int gamepakWaitState2[8] = { 8, 8, 8, 8, 1, 1, 1, 1 };

int memoryWait[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 0 };
int memoryWait32[16] =
  { 0, 0, 9, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 0 };
int memoryWaitSeq[16] =
  { 0, 0, 2, 0, 0, 0, 0, 0, 2, 2, 4, 4, 8, 8, 4, 0 };
int memoryWaitSeq32[16] =
  { 2, 0, 3, 0, 0, 2, 2, 0, 4, 4, 8, 8, 16, 16, 8, 0 };
int memoryWaitFetch[16] =
  { 3, 0, 3, 0, 0, 1, 1, 0, 4, 4, 4, 4, 4, 4, 4, 0 };
int memoryWaitFetch32[16] =
  { 6, 0, 6, 0, 0, 2, 2, 0, 8, 8, 8, 8, 8, 8, 8, 0 };

const int cpuMemoryWait[16] = {
  0, 0, 2, 0, 0, 0, 0, 0,
  2, 2, 2, 2, 2, 2, 0, 0
};
const int cpuMemoryWait32[16] = {
  0, 0, 3, 0, 0, 0, 0, 0,
  3, 3, 3, 3, 3, 3, 0, 0
};
  
const bool memory32[16] =
  { true, false, false, true, true, false, false, true, false, false, false, false, false, false, true, false};

int coeff[32] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
};

u32 myROM[] = {
0xEA000006,0xEA000093,0xEA000006,0x00000000,0x00000000,0x00000000,0xEA000088,0x00000000,
0xE3A00302,0xE1A0F000,0xE92D5800,0xE55EC002,0xE28FB03C,0xE79BC10C,0xE14FB000,0xE92D0800,
0xE20BB080,0xE38BB01F,0xE129F00B,0xE92D4004,0xE1A0E00F,0xE12FFF1C,0xE8BD4004,0xE3A0C0D3,
0xE129F00C,0xE8BD0800,0xE169F00B,0xE8BD5800,0xE1B0F00E,0x0000009C,0x0000009C,0x0000009C,
0x0000009C,0x000001F8,0x000001F0,0x000000AC,0x000000A0,0x000000FC,0x00000168,0xE12FFF1E,
0xE1A03000,0xE1A00001,0xE1A01003,0xE2113102,0x42611000,0xE033C040,0x22600000,0xE1B02001,
0xE15200A0,0x91A02082,0x3AFFFFFC,0xE1500002,0xE0A33003,0x20400002,0xE1320001,0x11A020A2,
0x1AFFFFF9,0xE1A01000,0xE1A00003,0xE1B0C08C,0x22600000,0x42611000,0xE12FFF1E,0xE92D0010,
0xE1A0C000,0xE3A01001,0xE1500001,0x81A000A0,0x81A01081,0x8AFFFFFB,0xE1A0000C,0xE1A04001,
0xE3A03000,0xE1A02001,0xE15200A0,0x91A02082,0x3AFFFFFC,0xE1500002,0xE0A33003,0x20400002,
0xE1320001,0x11A020A2,0x1AFFFFF9,0xE0811003,0xE1B010A1,0xE1510004,0x3AFFFFEE,0xE1A00004,
0xE8BD0010,0xE12FFF1E,0xE0010090,0xE1A01741,0xE2611000,0xE3A030A9,0xE0030391,0xE1A03743,
0xE2833E39,0xE0030391,0xE1A03743,0xE2833C09,0xE283301C,0xE0030391,0xE1A03743,0xE2833C0F,
0xE28330B6,0xE0030391,0xE1A03743,0xE2833C16,0xE28330AA,0xE0030391,0xE1A03743,0xE2833A02,
0xE2833081,0xE0030391,0xE1A03743,0xE2833C36,0xE2833051,0xE0030391,0xE1A03743,0xE2833CA2,
0xE28330F9,0xE0000093,0xE1A00840,0xE12FFF1E,0xE3A00001,0xE3A01001,0xE92D4010,0xE3A0C301,
0xE3A03000,0xE3A04001,0xE3500000,0x1B000004,0xE5CC3301,0xEB000002,0x0AFFFFFC,0xE8BD4010,
0xE12FFF1E,0xE5CC3208,0xE15C20B8,0xE0110002,0x10200002,0x114C00B8,0xE5CC4208,0xE12FFF1E,
0xE92D500F,0xE3A00301,0xE1A0E00F,0xE510F004,0xE8BD500F,0xE25EF004,0xE59FD044,0xE92D5000,
0xE14FC000,0xE10FE000,0xE92D5000,0xE3A0C302,0xE5DCE09C,0xE35E00A5,0x1A000004,0x05DCE0B4,
0x021EE080,0xE28FE004,0x159FF018,0x059FF018,0xE59FD018,0xE8BD5000,0xE169F00C,0xE8BD5000,
0xE25EF004,0x03007FF0,0x09FE2000,0x09FFC000,0x03007FE0
};

s16 sineTable[256] = {
  (s16)0x0000, (s16)0x0192, (s16)0x0323, (s16)0x04B5, (s16)0x0645, (s16)0x07D5, (s16)0x0964, (s16)0x0AF1,
  (s16)0x0C7C, (s16)0x0E05, (s16)0x0F8C, (s16)0x1111, (s16)0x1294, (s16)0x1413, (s16)0x158F, (s16)0x1708,
  (s16)0x187D, (s16)0x19EF, (s16)0x1B5D, (s16)0x1CC6, (s16)0x1E2B, (s16)0x1F8B, (s16)0x20E7, (s16)0x223D,
  (s16)0x238E, (s16)0x24DA, (s16)0x261F, (s16)0x275F, (s16)0x2899, (s16)0x29CD, (s16)0x2AFA, (s16)0x2C21,
  (s16)0x2D41, (s16)0x2E5A, (s16)0x2F6B, (s16)0x3076, (s16)0x3179, (s16)0x3274, (s16)0x3367, (s16)0x3453,
  (s16)0x3536, (s16)0x3612, (s16)0x36E5, (s16)0x37AF, (s16)0x3871, (s16)0x392A, (s16)0x39DA, (s16)0x3A82,
  (s16)0x3B20, (s16)0x3BB6, (s16)0x3C42, (s16)0x3CC5, (s16)0x3D3E, (s16)0x3DAE, (s16)0x3E14, (s16)0x3E71,
  (s16)0x3EC5, (s16)0x3F0E, (s16)0x3F4E, (s16)0x3F84, (s16)0x3FB1, (s16)0x3FD3, (s16)0x3FEC, (s16)0x3FFB,
  (s16)0x4000, (s16)0x3FFB, (s16)0x3FEC, (s16)0x3FD3, (s16)0x3FB1, (s16)0x3F84, (s16)0x3F4E, (s16)0x3F0E,
  (s16)0x3EC5, (s16)0x3E71, (s16)0x3E14, (s16)0x3DAE, (s16)0x3D3E, (s16)0x3CC5, (s16)0x3C42, (s16)0x3BB6,
  (s16)0x3B20, (s16)0x3A82, (s16)0x39DA, (s16)0x392A, (s16)0x3871, (s16)0x37AF, (s16)0x36E5, (s16)0x3612,
  (s16)0x3536, (s16)0x3453, (s16)0x3367, (s16)0x3274, (s16)0x3179, (s16)0x3076, (s16)0x2F6B, (s16)0x2E5A,
  (s16)0x2D41, (s16)0x2C21, (s16)0x2AFA, (s16)0x29CD, (s16)0x2899, (s16)0x275F, (s16)0x261F, (s16)0x24DA,
  (s16)0x238E, (s16)0x223D, (s16)0x20E7, (s16)0x1F8B, (s16)0x1E2B, (s16)0x1CC6, (s16)0x1B5D, (s16)0x19EF,
  (s16)0x187D, (s16)0x1708, (s16)0x158F, (s16)0x1413, (s16)0x1294, (s16)0x1111, (s16)0x0F8C, (s16)0x0E05,
  (s16)0x0C7C, (s16)0x0AF1, (s16)0x0964, (s16)0x07D5, (s16)0x0645, (s16)0x04B5, (s16)0x0323, (s16)0x0192,
  (s16)0x0000, (s16)0xFE6E, (s16)0xFCDD, (s16)0xFB4B, (s16)0xF9BB, (s16)0xF82B, (s16)0xF69C, (s16)0xF50F,
  (s16)0xF384, (s16)0xF1FB, (s16)0xF074, (s16)0xEEEF, (s16)0xED6C, (s16)0xEBED, (s16)0xEA71, (s16)0xE8F8,
  (s16)0xE783, (s16)0xE611, (s16)0xE4A3, (s16)0xE33A, (s16)0xE1D5, (s16)0xE075, (s16)0xDF19, (s16)0xDDC3,
  (s16)0xDC72, (s16)0xDB26, (s16)0xD9E1, (s16)0xD8A1, (s16)0xD767, (s16)0xD633, (s16)0xD506, (s16)0xD3DF,
  (s16)0xD2BF, (s16)0xD1A6, (s16)0xD095, (s16)0xCF8A, (s16)0xCE87, (s16)0xCD8C, (s16)0xCC99, (s16)0xCBAD,
  (s16)0xCACA, (s16)0xC9EE, (s16)0xC91B, (s16)0xC851, (s16)0xC78F, (s16)0xC6D6, (s16)0xC626, (s16)0xC57E,
  (s16)0xC4E0, (s16)0xC44A, (s16)0xC3BE, (s16)0xC33B, (s16)0xC2C2, (s16)0xC252, (s16)0xC1EC, (s16)0xC18F,
  (s16)0xC13B, (s16)0xC0F2, (s16)0xC0B2, (s16)0xC07C, (s16)0xC04F, (s16)0xC02D, (s16)0xC014, (s16)0xC005,
  (s16)0xC000, (s16)0xC005, (s16)0xC014, (s16)0xC02D, (s16)0xC04F, (s16)0xC07C, (s16)0xC0B2, (s16)0xC0F2,
  (s16)0xC13B, (s16)0xC18F, (s16)0xC1EC, (s16)0xC252, (s16)0xC2C2, (s16)0xC33B, (s16)0xC3BE, (s16)0xC44A,
  (s16)0xC4E0, (s16)0xC57E, (s16)0xC626, (s16)0xC6D6, (s16)0xC78F, (s16)0xC851, (s16)0xC91B, (s16)0xC9EE,
  (s16)0xCACA, (s16)0xCBAD, (s16)0xCC99, (s16)0xCD8C, (s16)0xCE87, (s16)0xCF8A, (s16)0xD095, (s16)0xD1A6,
  (s16)0xD2BF, (s16)0xD3DF, (s16)0xD506, (s16)0xD633, (s16)0xD767, (s16)0xD8A1, (s16)0xD9E1, (s16)0xDB26,
  (s16)0xDC72, (s16)0xDDC3, (s16)0xDF19, (s16)0xE075, (s16)0xE1D5, (s16)0xE33A, (s16)0xE4A3, (s16)0xE611,
  (s16)0xE783, (s16)0xE8F8, (s16)0xEA71, (s16)0xEBED, (s16)0xED6C, (s16)0xEEEF, (s16)0xF074, (s16)0xF1FB,
  (s16)0xF384, (s16)0xF50F, (s16)0xF69C, (s16)0xF82B, (s16)0xF9BB, (s16)0xFB4B, (s16)0xFCDD, (s16)0xFE6E
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGbaEmu::CGbaEmu()
{
    this->InitBasic();
}
CGbaEmu::~CGbaEmu()
{
    this->Destroy();
}
int CGbaEmu::InitBasic()
{
    //gfx.cpp
    this->line0 = 0;
    this->line1 = 0;
    this->line2 = 0;
    this->line3 = 0;
    this->lineOBJ = 0;
    this->lineOBJWin = 0;
    this->lineMix = 0;
    this->gfxInWin0 = 0;
    this->gfxInWin1 = 0;
    this->gfxBG2Changed = 0;
    this->gfxBG3Changed = 0;
    this->gfxBG2X = 0;
    this->gfxBG2Y = 0;
    this->gfxBG2LastX = 0;
    this->gfxBG2LastY = 0;
    this->gfxBG3X = 0;
    this->gfxBG3Y = 0;
    this->gfxBG3LastX = 0;
    this->gfxBG3LastY = 0;
    this->gfxLastVCOUNT = 0;
    //global.cpp
    this->reg = 0;
    this->map = 0;
    this->ioReadable = 0;
    this->N_FLAG = 0;
    this->C_FLAG = 0;
    this->Z_FLAG = 0;
    this->V_FLAG = 0;
    this->armState = true;
    this->armIrqEnable = true;
    this->armNextPC = 0x00000000;
    this->armMode = 0x1f;
    this->stop = 0x08000568;
    this->saveType = 0;
    this->useBios = false;
    this->skipBios = false;
    this->frameSkip = 1;
    this->speedup = false;
    this->synchronize = true;
    this->cpuDisableSfx = false;
    this->cpuIsMultiBoot = false;
    this->parseDebug = true;
    this->layerSettings = 0xff00;
    this->layerEnable = 0xff00;
    this->speedHack = false;
    this->cpuSaveType = 0;
    this->cpuEnhancedDetection = true;
    this->cheatsEnabled = true;
    this->bios = NULL;
    this->rom = NULL;
    this->internalRAM = NULL;
    this->workRAM = NULL;
    this->paletteRAM = NULL;
    this->vram = NULL;
    this->pix = NULL;
    this->oam = NULL;
    this->ioMem = NULL;
    this->DISPCNT  = 0x0080;
    this->DISPSTAT = 0x0000;
    this->VCOUNT   = 0x0000;
    this->BG0CNT   = 0x0000;
    this->BG1CNT   = 0x0000;
    this->BG2CNT   = 0x0000;
    this->BG3CNT   = 0x0000;
    this->BG0HOFS  = 0x0000;
    this->BG0VOFS  = 0x0000;
    this->BG1HOFS  = 0x0000;
    this->BG1VOFS  = 0x0000;
    this->BG2HOFS  = 0x0000;
    this->BG2VOFS  = 0x0000;
    this->BG3HOFS  = 0x0000;
    this->BG3VOFS  = 0x0000;
    this->BG2PA    = 0x0100;
    this->BG2PB    = 0x0000;
    this->BG2PC    = 0x0000;
    this->BG2PD    = 0x0100;
    this->BG2X_L   = 0x0000;
    this->BG2X_H   = 0x0000;
    this->BG2Y_L   = 0x0000;
    this->BG2Y_H   = 0x0000;
    this->BG3PA    = 0x0100;
    this->BG3PB    = 0x0000;
    this->BG3PC    = 0x0000;
    this->BG3PD    = 0x0100;
    this->BG3X_L   = 0x0000;
    this->BG3X_H   = 0x0000;
    this->BG3Y_L   = 0x0000;
    this->BG3Y_H   = 0x0000;
    this->WIN0H    = 0x0000;
    this->WIN1H    = 0x0000;
    this->WIN0V    = 0x0000;
    this->WIN1V    = 0x0000;
    this->WININ    = 0x0000;
    this->WINOUT   = 0x0000;
    this->MOSAIC   = 0x0000;
    this->BLDMOD   = 0x0000;
    this->COLEV    = 0x0000;
    this->COLY     = 0x0000;
    this->DM0SAD_L = 0x0000;
    this->DM0SAD_H = 0x0000;
    this->DM0DAD_L = 0x0000;
    this->DM0DAD_H = 0x0000;
    this->DM0CNT_L = 0x0000;
    this->DM0CNT_H = 0x0000;
    this->DM1SAD_L = 0x0000;
    this->DM1SAD_H = 0x0000;
    this->DM1DAD_L = 0x0000;
    this->DM1DAD_H = 0x0000;
    this->DM1CNT_L = 0x0000;
    this->DM1CNT_H = 0x0000;
    this->DM2SAD_L = 0x0000;
    this->DM2SAD_H = 0x0000;
    this->DM2DAD_L = 0x0000;
    this->DM2DAD_H = 0x0000;
    this->DM2CNT_L = 0x0000;
    this->DM2CNT_H = 0x0000;
    this->DM3SAD_L = 0x0000;
    this->DM3SAD_H = 0x0000;
    this->DM3DAD_L = 0x0000;
    this->DM3DAD_H = 0x0000;
    this->DM3CNT_L = 0x0000;
    this->DM3CNT_H = 0x0000;
    this->TM0D     = 0x0000;
    this->TM0CNT   = 0x0000;
    this->TM1D     = 0x0000;
    this->TM1CNT   = 0x0000;
    this->TM2D     = 0x0000;
    this->TM2CNT   = 0x0000;
    this->TM3D     = 0x0000;
    this->TM3CNT   = 0x0000;
    this->P1       = 0xFFFF;
    this->IE       = 0x0000;
    this->IF       = 0x0000;
    this->IME      = 0x0000;
    //gba.cpp
    this->cpuDmaTicksToUpdate = 0;
    this->cpuDmaCount = 0;
    this->cpuDmaHack = 0;
    this->cpuDmaLast = 0;
    this->dummyAddress = 0;
    this->extCpuLoopTicks = 0;
    this->extClockTicks = 0;
    this->extTicks = 0;
    this->gbaSaveType = 0;
    this->intState = false;
    this->stopState = false;
    this->holdState = false;
    this->holdType = 0;
    this->cpuSramEnabled = true;
    this->cpuFlashEnabled = true;
    this->cpuEEPROMEnabled = true;
    this->cpuEEPROMSensorEnabled = false;
    this->freezeWorkRAM = 0;
    this->freezeInternalRAM = 0;
    this->lcdTicks = 960;
    this->timer0On = false;
    this->timer0Ticks = 0;
    this->timer0Reload = 0;
    this->timer0ClockReload = 0;
    this->timer1On = false;
    this->timer1Ticks = 0;
    this->timer1Reload = 0;
    this->timer1ClockReload = 0;
    this->timer2On = false;
    this->timer2Ticks = 0;
    this->timer2Reload = 0;
    this->timer2ClockReload = 0;
    this->timer3On = false;
    this->timer3Ticks = 0;
    this->timer3Reload = 0;
    this->timer3ClockReload = 0;
    this->dma0Source = 0;
    this->dma0Dest = 0;
    this->dma1Source = 0;
    this->dma1Dest = 0;
    this->dma2Source = 0;
    this->dma2Dest = 0;
    this->dma3Source = 0;
    this->dma3Dest = 0;
    this->fxOn = false;
    this->windowOn = false;
    this->frameCount = 0;
    this->buffer = 0;
    this->lastTime = 0;
    this->count = 0;
    this->capture = 0;
    this->capturePrevious = 0;
    this->captureNumber = 0;
    this->cpuSavedTicks = 0;
    this->cpuSaveGameFunc = &CGbaEmu::flashSaveDecide;
    this->renderLine = &CGbaEmu::mode0RenderLine;
    this->biosProtected = 0;
    this->cpuBitsSet = 0;
    this->cpuLowestBitSet = 0;
    //sound.cpp
    this->soundQuality = 2;
    this->soundTicks = soundQuality * USE_TICKS_AS;
    this->SOUND_CLOCK_TICKS = soundQuality * USE_TICKS_AS;
    //vba.cpp
    this->systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    this->emulating = 0;
    //flash.cpp
    this->flashSaveMemory = 0;
    this->flashState = FLASH_READ_ARRAY;
    this->flashReadState = FLASH_READ_ARRAY;
    this->flashSize = 0x10000;
    this->flashDeviceID = 0x1b;
    this->flashManufacturerID = 0x32;
    this->flashBank = 0;
    //eeprom.cpp
    this->eepromMode = EEPROM_IDLE;
    this->eepromByte = 0;
    this->eepromBits = 0;
    this->eepromAddress = 0;
    this->eepromData = NULL;
    this->eepromBuffer = NULL;
    this->eepromInUse = false;
    this->eepromSize = 512;
    //rtc.cpp
    this->rtcEnabled = false;
    this->rtcClockData = NULL;
    //agbprint.cpp
    this->agbPrintEnabled = false;
    this->agbPrintProtect = false;
    //system.cpp
    this->systemRedShift = 3;
    this->systemGreenShift = 11;
    this->systemBlueShift = 19;
    this->systemColorDepth = 16;
    this->systemDebug = 0;
    this->systemVerbose = 0;
    this->systemSaveUpdateCounter = 0;
    this->systemFrameSkip = 0;
    this->systemColorMap32 = 0;
    this->systemColorMap16 = 0;
    this->systemGbPalette = 0;
    this->systemSoundOn = 0;
    this->systemRenderedFrames = 0;
    //added by cxp
    this->joypad = 0;

#if _GBA_VM
    this->gba_vm_suspend = 0;
    this->param_vm_bios_int = NULL;
    this->callback_vm_bios_int = NULL;
    this->systemFrameSkip = 0x7fffffff;
#endif

    return OK;
}
int CGbaEmu::Init()
{
    this->InitBasic();

    int i;

    MALLOC(this->freezeWorkRAM,bool,0x40000);
    MALLOC(this->freezeInternalRAM,bool,0x8000);
    MALLOC(this->buffer,char,1024);
    MALLOC(this->reg,reg_pair,45);
    MALLOC(this->map,memoryMap,256);
    MALLOC(this->ioReadable,bool,0x400);
    MALLOC(this->line0,u32,240);
    MALLOC(this->line1,u32,240);
    MALLOC(this->line2,u32,240);
    MALLOC(this->line3,u32,240);
    MALLOC(this->lineOBJ,u32,240);
    MALLOC(this->lineOBJWin,u32,240);
    MALLOC(this->lineMix,u32,240);
    MALLOC(this->gfxInWin0,bool,240);
    MALLOC(this->gfxInWin1,bool,240);
    MALLOC(this->flashSaveMemory,u8,0x20000);
    MALLOC(this->biosProtected,u8,4);
    MALLOC(this->eepromData,u8,0x2000);
    MALLOC(this->eepromBuffer,u8,16);
    MALLOC(this->rtcClockData,RTCCLOCKDATA,1);
    MALLOC(this->systemColorMap32,u32,0x10000);
    MALLOC(this->systemColorMap16,u16,0x10000);
    MALLOC(this->systemGbPalette,u16,24);
    MALLOC(this->cpuBitsSet,u8,256);
    MALLOC(this->cpuLowestBitSet,u8,256);

    for(i = 0; i < 0x40000; i++)
        this->freezeWorkRAM[i] = 0;
    for(i = 0; i < 0x8000; i++)
        this->freezeInternalRAM[i] = 0;
    for(i = 0; i < 1024; i++)
        this->buffer[i] = 0;
    for(i = 0; i < 45; i++)
        this->reg[i].I = 0;
    
    for(i = 0; i < 256; i++)
    {
        this->map[i].address = 0;
        this->map[i].mask = 0;
    }

    for(i = 0; i < 0x400; i++)
        this->ioReadable[i] = 0;

    for(i = 0; i < 240; i++)
        this->line0[i] = 0;
    for(i = 0; i < 240; i++)
        this->line1[i] = 0;
    for(i = 0; i < 240; i++)
        this->line2[i] = 0;
    for(i = 0; i < 240; i++)
        this->line3[i] = 0;
    for(i = 0; i < 240; i++)
        this->lineOBJ[i] = 0;
    for(i = 0; i < 240; i++)
        this->lineOBJWin[i] = 0;
    for(i = 0; i < 240; i++)
        this->lineMix[i] = 0;
    for(i = 0; i < 240; i++)
        this->gfxInWin0[i] = 0;
    for(i = 0; i < 240; i++)
        this->gfxInWin1[i] = 0;
    for(i = 0; i < 0x20000; i++)
        this->flashSaveMemory[i] = 0;
    for(i = 0; i < 4; i++)
        this->biosProtected[i] = 0;
    for(i = 0; i < 0x10000; i++)
        this->systemColorMap32[i] = 0;
    for(i = 0; i < 0x10000; i++)
        this->systemColorMap16[i] = 0;
    for(i = 0; i < 24; i++)
        this->systemGbPalette[i] = 0;
    for(i = 0; i < 256; i++)
        this->cpuBitsSet[i] = 0;
    for(i = 0; i < 256; i++)
        this->cpuLowestBitSet[i] = 0;
    return OK;
}
int CGbaEmu::Destroy()
{
    FREE(this->freezeWorkRAM);
    FREE(this->freezeInternalRAM);
    FREE(this->buffer);
    FREE(this->reg);
    FREE(this->map);
    FREE(this->ioReadable);
    FREE(this->line0);
    FREE(this->line1);
    FREE(this->line2);
    FREE(this->line3);
    FREE(this->lineOBJ);
    FREE(this->lineOBJWin);
    FREE(this->lineMix);
    FREE(this->gfxInWin0);
    FREE(this->gfxInWin1);
    FREE(this->flashSaveMemory);
    FREE(this->biosProtected);
    FREE(this->eepromData);
    FREE(this->eepromBuffer);
    FREE(this->rtcClockData);
    FREE(this->systemColorMap32);
    FREE(this->systemColorMap16);
    FREE(this->systemGbPalette);
    FREE(this->cpuBitsSet);
    FREE(this->cpuLowestBitSet);

    this->CPUCleanUp();

    this->InitBasic();
    return OK;
}
int CGbaEmu::InitColorMap()
{
    for(int i = 0; i < 24;) 
    {
        systemGbPalette[i++] = (0x1f) | (0x1f << 5) | (0x1f << 10);
        systemGbPalette[i++] = (0x15) | (0x15 << 5) | (0x15 << 10);
        systemGbPalette[i++] = (0x0c) | (0x0c << 5) | (0x0c << 10);
        systemGbPalette[i++] = 0;
    }
    
    systemRedShift = 0;
    systemGreenShift = 5;
    systemBlueShift = 11;

    switch(systemColorDepth) 
    {
    case 16:
        {
            for(int i = 0; i < 0x10000; i++) 
            {
                systemColorMap16[i] = ((i & 0x1f) << systemRedShift) |
                    (((i & 0x3e0) >> 5) << systemGreenShift) |
                    (((i & 0x7c00) >> 10) << systemBlueShift);
            }
        }
        break;
    case 24:
    case 32:
        {
            for(int i = 0; i < 0x10000; i++) 
            {
                systemColorMap32[i] = ((i & 0x1f) << systemRedShift) |
                    (((i & 0x3e0) >> 5) << systemGreenShift) |
                    (((i & 0x7c00) >> 10) << systemBlueShift);
            }
        }
        break;
    }
    return OK;
}

inline int CGbaEmu::CPUUpdateTicksAccess32(u32 address)
{
    return memoryWait32[(address>>24)&15];
}
inline int CGbaEmu::CPUUpdateTicksAccess16(u32 address)
{
    return memoryWait[(address>>24)&15];
}
inline int CGbaEmu::CPUUpdateTicksAccessSeq32(u32 address)
{
    return memoryWaitSeq32[(address>>24)&15];
}
inline int CGbaEmu::CPUUpdateTicksAccessSeq16(u32 address)
{
    return memoryWaitSeq[(address>>24)&15];
}
inline int CGbaEmu::CPUUpdateTicks()
{
    int cpuLoopTicks = lcdTicks;
    if(soundTicks < cpuLoopTicks)
        cpuLoopTicks = soundTicks;
    if(timer0On && !(TM0CNT & 4) && (timer0Ticks < cpuLoopTicks)) 
    {
        cpuLoopTicks = timer0Ticks;
    }
    if(timer1On && !(TM1CNT & 4) && (timer1Ticks < cpuLoopTicks)) 
    {
        cpuLoopTicks = timer1Ticks;
    }
    if(timer2On && !(TM2CNT & 4) && (timer2Ticks < cpuLoopTicks)) 
    {
        cpuLoopTicks = timer2Ticks;
    }
    if(timer3On && !(TM3CNT & 4) && (timer3Ticks < cpuLoopTicks)) 
    {
        cpuLoopTicks = timer3Ticks;
    }
    cpuSavedTicks = cpuLoopTicks;
    return cpuLoopTicks;
}

void CGbaEmu::CPUUpdateWindow0()
{
    int x00 = WIN0H>>8;
    int x01 = WIN0H & 255;
    if(x00 <= x01) 
    {
        for(int i = 0; i < 240; i++) 
        {
            gfxInWin0[i] = (i >= x00 && i < x01);
        }
    }
    else 
    {
        for(int i = 0; i < 240; i++) 
        {
            gfxInWin0[i] = (i >= x00 || i < x01);
        }
    }
}

void CGbaEmu::CPUUpdateWindow1()
{
    int x00 = WIN1H>>8;
    int x01 = WIN1H & 255;
    if(x00 <= x01) 
    {
        for(int i = 0; i < 240; i++) 
        {
            gfxInWin1[i] = (i >= x00 && i < x01);
        }
    }
    else 
    {
        for(int i = 0; i < 240; i++) 
        {
            gfxInWin1[i] = (i >= x00 || i < x01);
        }
    }
}

#define CLEAR_ARRAY(a) \
  {\
    u32 *array = (a);\
    for(int i = 0; i < 240; i++) {\
      *array++ = 0x80000000;\
    }\
  }\
  
void CGbaEmu::CPUUpdateRenderBuffers(bool force)
{
    if(!(layerEnable & 0x0100) || force) 
    {
        CLEAR_ARRAY(line0);
    }
    
    if(!(layerEnable & 0x0200) || force) 
    {
        CLEAR_ARRAY(line1);
    }
    
    if(!(layerEnable & 0x0400) || force) 
    {
        CLEAR_ARRAY(line2);
    }
    
    if(!(layerEnable & 0x0800) || force) 
    {
        CLEAR_ARRAY(line3);
    }
}

void CGbaEmu::CPUCleanUp()
{
    FREE(rom);
    FREE(vram);
    FREE(paletteRAM);
    FREE(internalRAM);
    FREE(workRAM);
    FREE(bios);
    FREE(pix);
    FREE(oam);  
    FREE(ioMem);
    
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    emulating = 0;
}

int CGbaEmu::CPULoadRom(CFileBase *file)
{
    ASSERT(file);
    
    int is_first_run = (rom == NULL);
    int size = 0x2000000;
    //int size = file->GetSize();
    
    file->Seek(0);
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

    if(is_first_run)
    {
        MALLOC(rom,u8,size);  //最大申请32M的空间
        MALLOC(workRAM,u8,0x40000);   //256K的内存空间
    }

    u8 *whereToLoad = rom;
    if(cpuIsMultiBoot)
        whereToLoad = workRAM;

    file->Read(whereToLoad,size);

    int i;  
    u16 *temp = (u16 *)(rom+((size+1)&~1));
    for(i = (size+1)&~1; i < 0x2000000; i+=2) 
    {
        WRITE16LE(temp, (i >> 1) & 0xFFFF);
        temp++;
    }

    if(is_first_run)
    {
        MALLOC(bios,u8,0x4000);
        MALLOC(internalRAM,u8,0x8000);
        MALLOC(paletteRAM,u8,0x400);
        MALLOC(vram,u8,0x20000);
        MALLOC(oam,u8,0x400);
        MALLOC(pix,u8, 4 * 241 * 162);
        MALLOC(ioMem,u8,0x400);
    }

    CPUUpdateRenderBuffers(true);

    return OK;
}
void CGbaEmu::WRITE32LE(u32 *addr, u32 value)
{
    u8 *p = (u8*)addr;
    
    p[0] = (u8)value;
    p[1] = (u8)(value>>8);
    p[2] = (u8)(value>>16);
    p[3] = (u8)(value>>24);
}

u32 CGbaEmu::READ32LE(u32 *addr)
{
    u8 *p = (u8*)addr;
    return p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}
void CGbaEmu::WRITE16LE(u16 *addr, u16 value)
{
    u8 *p = (u8*)addr;
    
    p[0] = (u8)value;
    p[1] = (u8)(value>>8);
}

u16 CGbaEmu::READ16LE(u16 *addr)
{
    u8 *p = (u8*)addr;
    return p[0] | (p[1]<<8);
}

inline void CGbaEmu::gfxClearArray(u32 *array)
{
    for(int i = 0; i < 240; i++) 
    {
        *array++ = 0x80000000;
    }
}
inline void CGbaEmu::gfxDrawTextScreen(u16 control, u16 hofs, u16 vofs, u32 *line)
{   
    u16 *palette = (u16 *)paletteRAM;
    u8 *charBase = &vram[((control >> 2) & 0x03) * 0x4000];
    u16 *screenBase = (u16 *)&vram[((control >> 8) & 0x1f) * 0x800];
    u32 prio = ((control & 3)<<25) + 0x1000000;
    int sizeX = 256;
    int sizeY = 256;
    switch((control >> 14) & 3) 
    {
        case 0:
            break;
        case 1:
            sizeX = 512;
            break;
        case 2:
            sizeY = 512;
            break;
        case 3:
            sizeX = 512;
            sizeY = 512;
            break;
    }
    int maskX = sizeX-1;
    int maskY = sizeY-1;
    bool mosaicOn = (control & 0x40) ? true : false;
    int xxx = hofs & maskX;
    int yyy = (vofs + VCOUNT) & maskY;
    int mosaicX = (MOSAIC & 0x000F)+1;
    int mosaicY = ((MOSAIC & 0x00F0)>>4)+1;
    if(mosaicOn) 
    {
        if((VCOUNT % mosaicY) != 0) 
        {
            mosaicY = (VCOUNT / mosaicY) * mosaicY;
            yyy = (vofs + mosaicY) & maskY;
        }
    }
    if(yyy > 255 && sizeY > 256) 
    {
        yyy &= 255;
        screenBase += 0x400;
        if(sizeX > 256)
            screenBase += 0x400;
    }
    int yshift = ((yyy>>3)<<5);
    if((control) & 0x80) 
    {
        u16 *screenSource = screenBase + 0x400 * (xxx>>8) + ((xxx & 255)>>3) + yshift;
        for(int x = 0; x < 240; x++) 
        {
            u16 data = READ16LE(screenSource);
            int tile = data & 0x3FF;
            int tileX = (xxx & 7);
            int tileY = yyy & 7;
            if(data & 0x0400)
                tileX = 7 - tileX;
            if(data & 0x0800)
                tileY = 7 - tileY;
            u8 color = charBase[tile * 64 + tileY * 8 + tileX];
            line[x] = color ? (READ16LE(&palette[color]) | prio): 0x80000000;
            if(data & 0x0400) 
            {
                if(tileX == 0)
                    screenSource++;
            }
            else if(tileX == 7)
                screenSource++;
            xxx++;
            if(xxx == 256) 
            {
                if(sizeX > 256)
                    screenSource = screenBase + 0x400 + yshift;
                else 
                {
                    screenSource = screenBase + yshift;
                    xxx = 0;
                }
            }
            else if(xxx >= sizeX) 
            {
                xxx = 0;
                screenSource = screenBase + yshift;
            }
        }
    }
    else 
    {
        u16 *screenSource = screenBase + 0x400*(xxx>>8)+((xxx&255)>>3) +
        yshift;
        for(int x = 0; x < 240; x++) 
        {
            u16 data = READ16LE(screenSource);
            int tile = data & 0x3FF;
            int tileX = (xxx & 7);
            int tileY = yyy & 7;
            if(data & 0x0400)
                tileX = 7 - tileX;
            if(data & 0x0800)
                tileY = 7 - tileY;
            u8 color = charBase[(tile<<5) + (tileY<<2) + (tileX>>1)];
            if(tileX & 1) 
            {
                color = (color >> 4);
            }
            else 
            {
                color &= 0x0F;
            }
            int pal = (READ16LE(screenSource)>>8) & 0xF0;
            line[x] = color ? (READ16LE(&palette[pal + color])|prio): 0x80000000;
            if(data & 0x0400) 
            {
                if(tileX == 0)
                    screenSource++;
            }
            else if(tileX == 7)
                screenSource++;
            xxx++;
            if(xxx == 256) 
            {
                if(sizeX > 256)
                    screenSource = screenBase + 0x400 + yshift;
                else 
                {
                    screenSource = screenBase + yshift;
                    xxx = 0;
                }
            }
            else if(xxx >= sizeX) 
            {
                xxx = 0;
                screenSource = screenBase + yshift;
            }
        }
    }
    if(mosaicOn) 
    {
        if(mosaicX > 1) 
        {
            int m = 1;
            for(int i = 0; i < 239; i++) 
            {
                line[i+1] = line[i];
                m++;
                if(m == mosaicX) 
                {
                    m = 1;
                    i++;
                }
            }
        }
    }
}
inline void CGbaEmu::gfxDrawRotScreen(u16 control, u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int& currentX, int& currentY,int changed,u32 *line)
{
    u16 *palette = (u16 *)paletteRAM;
    u8 *charBase = &vram[((control >> 2) & 0x03) * 0x4000];
    u8 *screenBase = (u8 *)&vram[((control >> 8) & 0x1f) * 0x800];
    int prio = ((control & 3) << 25) + 0x1000000;
    int sizeX = 128;
    int sizeY = 128;
    switch((control >> 14) & 3) 
    {
        case 0:
            break;
        case 1:
            sizeX = sizeY = 256;
            break;
        case 2:
            sizeX = sizeY = 512;
            break;
        case 3:
            sizeX = sizeY = 1024;
            break;
    }
    int dx = pa & 0x7FFF;
    if(pa & 0x8000)
        dx |= 0xFFFF8000;
    int dmx = pb & 0x7FFF;
    if(pb & 0x8000)
        dmx |= 0xFFFF8000;
    int dy = pc & 0x7FFF;
    if(pc & 0x8000)
        dy |= 0xFFFF8000;
    int dmy = pd & 0x7FFFF;
    if(pd & 0x8000)
        dmy |= 0xFFFF8000;
    if(VCOUNT == 0)
        changed = 3;
    if(changed & 1) 
    {
        currentX = (x_l) | ((x_h & 0x07FF)<<16);
        if(x_h & 0x0800)
            currentX |= 0xF8000000;
    }
    else 
    {
        currentX += dmx;
    }
    if(changed & 2) 
    {
        currentY = (y_l) | ((y_h & 0x07FF)<<16);
        if(y_h & 0x0800)
            currentY |= 0xF8000000;
    }
    else 
    {
        currentY += dmy;
    }
    int realX = currentX;
    int realY = currentY;
    if(control & 0x40) 
    {
        int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
        int y = (VCOUNT % mosaicY);
        realX -= y*dmx;
        realY -= y*dmy;
    }
    int xxx = (realX >> 8);
    int yyy = (realY >> 8);
    if(control & 0x2000) 
    {
        xxx %= sizeX;
        yyy %= sizeY;
        if(xxx < 0)
            xxx += sizeX;
        if(yyy < 0)
            yyy += sizeY;
    }
    if(control & 0x80) 
    {
        for(int x = 0; x < 240; x++) 
        {
            if(xxx < 0 ||
                yyy < 0 ||
                xxx >= sizeX ||
                yyy >= sizeY) 
            {
                line[x] = 0x80000000;
            }
            else 
            {
                int tile = screenBase[(xxx>>3) + (yyy>>3)*(sizeX>>3)];
                int tileX = (xxx & 7);
                int tileY = yyy & 7;
                u8 color = charBase[(tile<<6) + (tileY<<3) + tileX];
                line[x] = color ? (READ16LE(&palette[color])|prio): 0x80000000;
            }
            realX += dx;
            realY += dy;
            xxx = (realX >> 8);
            yyy = (realY >> 8);
            if(control & 0x2000) 
            {
                xxx %= sizeX;
                yyy %= sizeY;
                if(xxx < 0)
                    xxx += sizeX;
                if(yyy < 0)
                    yyy += sizeY;
            }
        }
    }
    else 
    {
        for(int x = 0; x < 240; x++) 
        {
            if(xxx < 0 ||
                yyy < 0 ||
                xxx >= sizeX ||
                yyy >= sizeY) 
            {
                line[x] = 0x80000000;
            }
            else 
            {
                int tile = screenBase[(xxx>>3) + (yyy>>3)*(sizeX>>3)];
                int tileX = (xxx & 7);
                int tileY = yyy & 7;
                u8 color = charBase[(tile<<6) + (tileY<<3) + tileX];
                line[x] = color ? (READ16LE(&palette[color])|prio): 0x80000000;
            }
            realX += dx;
            realY += dy;
            xxx = (realX >> 8);
            yyy = (realY >> 8);
            if(control & 0x2000) 
            {
                xxx %= sizeX;
                yyy %= sizeY;
                if(xxx < 0)
                    xxx += sizeX;
                if(yyy < 0)
                    yyy += sizeY;
            }
        }
    }
    if(control & 0x40) 
    {
        int mosaicX = (MOSAIC & 0xF) + 1;
        if(mosaicX > 1) 
        {
            int m = 1;
            for(int i = 0; i < 239; i++) 
            {
                line[i+1] = line[i];
                m++;
                if(m == mosaicX) 
                {
                    m = 1;
                    i++;
                }
            }
        }
    }
}
inline void CGbaEmu::gfxDrawRotScreen16Bit(u16 control,u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int& currentX, int& currentY,int changed,u32 *line)
{
    u16 *screenBase = (u16 *)&vram[0];
    int prio = ((control & 3) << 25) + 0x1000000;
    int sizeX = 240;
    int sizeY = 160;
    int startX = (x_l) | ((x_h & 0x07FF)<<16);
    if(x_h & 0x0800)
        startX |= 0xF8000000;
    int startY = (y_l) | ((y_h & 0x07FF)<<16);
    if(y_h & 0x0800)
        startY |= 0xF8000000;
    int dx = pa & 0x7FFF;
    if(pa & 0x8000)
        dx |= 0xFFFF8000;
    int dmx = pb & 0x7FFF;
    if(pb & 0x8000)
        dmx |= 0xFFFF8000;
    int dy = pc & 0x7FFF;
    if(pc & 0x8000)
        dy |= 0xFFFF8000;
    int dmy = pd & 0x7FFFF;
    if(pd & 0x8000)
        dmy |= 0xFFFF8000;
    if(VCOUNT == 0)
        changed = 3;
    if(changed & 1) 
    {
        currentX = (x_l) | ((x_h & 0x07FF)<<16);
        if(x_h & 0x0800)
            currentX |= 0xF8000000;
    }
    else
        currentX += dmx;
    if(changed & 2) 
    {
        currentY = (y_l) | ((y_h & 0x07FF)<<16);
        if(y_h & 0x0800)
            currentY |= 0xF8000000;
    }
    else 
    {
        currentY += dmy;
    }
    int realX = currentX;
    int realY = currentY;
    if(control & 0x40) 
    {
        int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
        int y = (VCOUNT % mosaicY);
        realX -= y*dmx;
        realY -= y*dmy;
    }
    int xxx = (realX >> 8);
    int yyy = (realY >> 8);
    for(int x = 0; x < 240; x++) 
    {
        if(xxx < 0 ||
            yyy < 0 ||
            xxx >= sizeX ||
            yyy >= sizeY) 
        {
            line[x] = 0x80000000;
        }
        else 
        {
            line[x] = (READ16LE(&screenBase[yyy * sizeX + xxx]) | prio);
        }
        realX += dx;
        realY += dy;
        xxx = (realX >> 8);
        yyy = (realY >> 8);
    }
    if(control & 0x40) 
    {
        int mosaicX = (MOSAIC & 0xF) + 1;
        if(mosaicX > 1) 
        {
            int m = 1;
            for(int i = 0; i < 239; i++) 
            {
                line[i+1] = line[i];
                m++;
                if(m == mosaicX) 
                {
                    m = 1;
                    i++;
                }
            }
        }
    }
}
inline void CGbaEmu::gfxDrawRotScreen256(u16 control, u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int &currentX, int& currentY,int changed,u32 *line)
{
    u16 *palette = (u16 *)paletteRAM;
    u8 *screenBase = (DISPCNT & 0x0010) ? &vram[0xA000] : &vram[0x0000];
    int prio = ((control & 3) << 25) + 0x1000000;
    int sizeX = 240;
    int sizeY = 160;
    int startX = (x_l) | ((x_h & 0x07FF)<<16);
    if(x_h & 0x0800)
        startX |= 0xF8000000;
    int startY = (y_l) | ((y_h & 0x07FF)<<16);
    if(y_h & 0x0800)
        startY |= 0xF8000000;
    int dx = pa & 0x7FFF;
    if(pa & 0x8000)
        dx |= 0xFFFF8000;
    int dmx = pb & 0x7FFF;
    if(pb & 0x8000)
        dmx |= 0xFFFF8000;
    int dy = pc & 0x7FFF;
    if(pc & 0x8000)
        dy |= 0xFFFF8000;
    int dmy = pd & 0x7FFFF;
    if(pd & 0x8000)
        dmy |= 0xFFFF8000;
    if(VCOUNT == 0)
        changed = 3;
    if(changed & 1) 
    {
        currentX = (x_l) | ((x_h & 0x07FF)<<16);
        if(x_h & 0x0800)
            currentX |= 0xF8000000;
    }
    else 
    {
        currentX += dmx;
    }
    if(changed & 2) 
    {
        currentY = (y_l) | ((y_h & 0x07FF)<<16);
        if(y_h & 0x0800)
            currentY |= 0xF8000000;
    }
    else 
    {
        currentY += dmy;
    }
    int realX = currentX;
    int realY = currentY;
    if(control & 0x40) 
    {
        int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
        int y = (VCOUNT / mosaicY) * mosaicY;
        realX = startX + y*dmx;
        realY = startY + y*dmy;
    }
    int xxx = (realX >> 8);
    int yyy = (realY >> 8);
    for(int x = 0; x < 240; x++) 
    {
        if(xxx < 0 ||
            yyy < 0 ||
            xxx >= sizeX ||
            yyy >= sizeY) 
        {
            line[x] = 0x80000000;
        }
        else 
        {
            u8 color = screenBase[yyy * 240 + xxx];
            line[x] = color ? (READ16LE(&palette[color])|prio): 0x80000000;
        }
        realX += dx;
        realY += dy;
        xxx = (realX >> 8);
        yyy = (realY >> 8);
    }
    if(control & 0x40) 
    {
        int mosaicX = (MOSAIC & 0xF) + 1;
        if(mosaicX > 1) 
        {
            int m = 1;
            for(int i = 0; i < 239; i++) 
            {
                line[i+1] = line[i];
                m++;
                if(m == mosaicX) 
                {
                    m = 1;
                    i++;
                }
            }
        }
    }
}
inline void CGbaEmu::gfxDrawRotScreen16Bit160(u16 control,u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int& currentX, int& currentY, int changed,u32 *line)
{
    u16 *screenBase = (DISPCNT & 0x0010) ? (u16 *)&vram[0xa000] :
    (u16 *)&vram[0];
    int prio = ((control & 3) << 25) + 0x1000000;
    int sizeX = 160;
    int sizeY = 128;
    int startX = (x_l) | ((x_h & 0x07FF)<<16);
    if(x_h & 0x0800)
        startX |= 0xF8000000;
    int startY = (y_l) | ((y_h & 0x07FF)<<16);
    if(y_h & 0x0800)
        startY |= 0xF8000000;
    int dx = pa & 0x7FFF;
    if(pa & 0x8000)
        dx |= 0xFFFF8000;
    int dmx = pb & 0x7FFF;
    if(pb & 0x8000)
        dmx |= 0xFFFF8000;
    int dy = pc & 0x7FFF;
    if(pc & 0x8000)
        dy |= 0xFFFF8000;
    int dmy = pd & 0x7FFFF;
    if(pd & 0x8000)
        dmy |= 0xFFFF8000;
    if(VCOUNT == 0)
        changed = 3;
    if(changed & 1) 
    {
        currentX = (x_l) | ((x_h & 0x07FF)<<16);
        if(x_h & 0x0800)
            currentX |= 0xF8000000;
    }
    else 
    {
        currentX += dmx;
    }
    if(changed & 2) 
    {
        currentY = (y_l) | ((y_h & 0x07FF)<<16);
        if(y_h & 0x0800)
            currentY |= 0xF8000000;
    }
    else 
    {
        currentY += dmy;
    }
    int realX = currentX;
    int realY = currentY;
    if(control & 0x40) 
    {
        int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
        int y = (VCOUNT / mosaicY) * mosaicY;
        realX = startX + y*dmx;
        realY = startY + y*dmy;
    }
    int xxx = (realX >> 8);
    int yyy = (realY >> 8);
    for(int x = 0; x < 240; x++) 
    {
        if(xxx < 0 ||
            yyy < 0 ||
            xxx >= sizeX ||
            yyy >= sizeY) 
        {
            line[x] = 0x80000000;
        }
        else 
        {
            line[x] = (READ16LE(&screenBase[yyy * sizeX + xxx]) | prio);
        }
        realX += dx;
        realY += dy;
        xxx = (realX >> 8);
        yyy = (realY >> 8);
    }
    if(control & 0x40) 
    {
        int mosaicX = (MOSAIC & 0xF) + 1;
        if(mosaicX > 1) 
        {
            int m = 1;
            for(int i = 0; i < 239; i++) 
            {
                line[i+1] = line[i];
                m++;
                if(m == mosaicX) 
                {
                    m = 1;
                    i++;
                }
            }
        }
    }
}
inline void CGbaEmu::gfxDrawSprites(u32 *lineOBJ)
{
    int m=0;
    gfxClearArray(lineOBJ);
    if(layerEnable & 0x1000) 
    {
        u16 *sprites = (u16 *)oam;
        u16 *spritePalette = &((u16 *)paletteRAM)[256];
        int mosaicY = ((MOSAIC & 0xF000)>>12) + 1;
        int mosaicX = ((MOSAIC & 0xF00)>>8) + 1;    
        for(int x = 0; x < 128 ; x++) 
        {
            u16 a0 = READ16LE(sprites++);
            u16 a1 = READ16LE(sprites++);
            u16 a2 = READ16LE(sprites++);
            sprites++;
            // ignore OBJ-WIN
            if((a0 & 0x0c00) == 0x0800)
                continue;
            int sizeY = 8;
            int sizeX = 8;
            switch(((a0 >>12) & 0x0c)|(a1>>14)) 
            {
                case 0:
                    break;
                case 1:
                    sizeX = sizeY = 16;
                    break;
                case 2:
                    sizeX = sizeY = 32;
                    break;
                case 3:
                    sizeX = sizeY = 64;
                    break;
                case 4:
                    sizeX = 16;
                    break;
                case 5:
                    sizeX = 32;
                    break;
                case 6:
                    sizeX = 32;
                    sizeY = 16;
                    break;
                case 7:
                    sizeX = 64;
                    sizeY = 32;
                    break;
                case 8:
                    sizeY = 16;
                    break;
                case 9:
                    sizeY = 32;
                    break;
                case 10:
                    sizeX = 16;
                    sizeY = 32;
                    break;
                case 11:
                    sizeX = 32;
                    sizeY = 64;
                    break;
                default:
                continue;
            }
            int sy = (a0 & 255);
            if(sy > 160)
                sy -= 256;
            if(a0 & 0x0100) 
            {
                int fieldX = sizeX;
                int fieldY = sizeY;
                if(a0 & 0x0200) 
                {
                    fieldX <<= 1;
                    fieldY <<= 1;
                }
                int t = VCOUNT - sy;
                if((t >= 0) && (t < fieldY)) 
                {
                    int sx = (a1 & 0x1FF);
                    if((sx < 240) || (((sx + fieldX) & 511) < 240)) 
                    {
                        // int t2 = t - (fieldY >> 1);
                        int rot = (a1 >> 9) & 0x1F;
                        u16 *OAM = (u16 *)oam;
                        int dx = READ16LE(&OAM[3 + (rot << 4)]);
                        if(dx & 0x8000)
                            dx |= 0xFFFF8000;
                        int dmx = READ16LE(&OAM[7 + (rot << 4)]);
                        if(dmx & 0x8000)
                            dmx |= 0xFFFF8000;
                        int dy = READ16LE(&OAM[11 + (rot << 4)]);
                        if(dy & 0x8000)
                            dy |= 0xFFFF8000;
                        int dmy = READ16LE(&OAM[15 + (rot << 4)]);
                        if(dmy & 0x8000)
                            dmy |= 0xFFFF8000;
                        if(a0 & 0x1000) 
                        {
                            t -= (t % mosaicY);
                        }
                        int realX = ((sizeX) << 7) - (fieldX >> 1)*dx - (fieldY>>1)*dmx
                        + t * dmx;
                        int realY = ((sizeY) << 7) - (fieldX >> 1)*dy - (fieldY>>1)*dmy
                        + t * dmy;
                        u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
                        if(a0 & 0x2000) 
                        {
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40)
                                inc = sizeX >> 2;
                            else
                                c &= 0x3FE;
                            for(int x = 0; x < fieldX; x++) 
                            {
                                int xxx = realX >> 8;
                                int yyy = realY >> 8;
                                if(xxx < 0 || xxx >= sizeX ||
                                    yyy < 0 || yyy >= sizeY ||
                                    sx >= 240);
                                else 
                                {
                                    u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
                                    + ((yyy & 7)<<3) + ((xxx >> 3)<<6) +
                                    (xxx & 7))&0x7FFF)];
                                    if ((color==0) && (((prio >> 25)&3) < 
                                    ((lineOBJ[sx]>>25)&3))) 
                                    {
                                        lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
                                        if((a0 & 0x1000) && m)
                                            lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                    }
                                    else if((color) && (prio < (lineOBJ[sx]&0xFF000000))) 
                                    {
                                        lineOBJ[sx] = READ16LE(&spritePalette[color]) | prio;
                                        if((a0 & 0x1000) && m)
                                            lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                    }
                                    if (a0 & 0x1000) 
                                    {
                                        m++;
                                        if (m==mosaicX)
                                            m=0;
                                    }
                                }
                                sx = (sx+1)&511;;
                                realX += dx;
                                realY += dy;
                            }
                        }
                        else 
                        {
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40)
                                inc = sizeX >> 3;
                            int palette = (a2 >> 8) & 0xF0;                 
                            for(int x = 0; x < fieldX; x++) 
                            {
                                int xxx = realX >> 8;
                                int yyy = realY >> 8;
                                if(xxx < 0 || xxx >= sizeX ||
                                    yyy < 0 || yyy >= sizeY ||
                                    sx >= 240);
                                else 
                                {
                                    u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
                                    + ((yyy & 7)<<2) + ((xxx >> 3)<<5) +
                                    ((xxx & 7)>>1))&0x7FFF)];
                                    if(xxx & 1)
                                        color >>= 4;
                                    else
                                        color &= 0x0F;
                                    if ((color==0) && (((prio >> 25)&3) < 
                                    ((lineOBJ[sx]>>25)&3))) 
                                    {
                                        lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
                                        if((a0 & 0x1000) && m)
                                            lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                    }
                                    else if((color) && (prio < (lineOBJ[sx]&0xFF000000))) 
                                    {
                                        lineOBJ[sx] = READ16LE(&spritePalette[palette+color]) | prio;
                                        if((a0 & 0x1000) && m)
                                            lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                    }
                                }
                                if((a0 & 0x1000) && m) 
                                {
                                    m++;
                                    if (m==mosaicX)
                                        m=0;
                                }
                                sx = (sx+1)&511;;
                                realX += dx;
                                realY += dy;
                            }
                        }
                    }
                }
            }
            else 
            {
                int t = VCOUNT - sy;
                if((t >= 0) && (t < sizeY)) 
                {
                    int sx = (a1 & 0x1FF);
                    if(((sx < 240)||(((sx+sizeX)&511)<240)) && !(a0 & 0x0200)) 
                    {
                        if(a0 & 0x2000) 
                        {
                            if(a1 & 0x2000)
                                t = sizeY - t - 1;
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40) 
                            {
                                inc = sizeX >> 2;
                            }
                            else 
                            {
                                c &= 0x3FE;
                            }
                            int xxx = 0;
                            if(a1 & 0x1000)
                                xxx = sizeX-1;
                            if(a0 & 0x1000) 
                            {
                                t -= (t % mosaicY);
                            }
                            int address = 0x10000 + ((((c+ (t>>3) * inc) << 5)
                            + ((t & 7) << 3) + ((xxx>>3)<<6) + (xxx & 7)) & 0x7FFF);
                            if(a1 & 0x1000)
                                xxx = 7;
                            u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
                            for(int xx = 0; xx < sizeX; xx++) 
                            {
                                if(sx < 240) 
                                {
                                    u8 color = vram[address];
                                    if ((color==0) && (((prio >> 25)&3) < 
                                    ((lineOBJ[sx]>>25)&3))) 
                                    {
                                        lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
                                        if((a0 & 0x1000) && m)
                                            lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                    }
                                    else if((color) && (prio < (lineOBJ[sx]&0xFF000000))) 
                                    {
                                        lineOBJ[sx] = READ16LE(&spritePalette[color]) | prio;
                                        if((a0 & 0x1000) && m)
                                            lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                    }
                                    if (a0 & 0x1000) 
                                    {
                                        m++;
                                        if (m==mosaicX)
                                            m=0;
                                    }
                                }
                                sx = (sx+1) & 511;
                                if(a1 & 0x1000) 
                                {
                                    xxx--;
                                    address--;
                                    if(xxx == -1) 
                                    {
                                        address -= 56;
                                        xxx = 7;
                                    }
                                    if(address < 0x10000)
                                        address += 0x8000;
                                }
                                else 
                                {
                                    xxx++;
                                    address++;
                                    if(xxx == 8) 
                                    {
                                        address += 56;
                                        xxx = 0;
                                    }
                                    if(address > 0x17fff)
                                        address -= 0x8000;
                                }
                            }
                        }
                        else 
                        {
                            if(a1 & 0x2000)
                                t = sizeY - t - 1;
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40) 
                            {
                                inc = sizeX >> 3;
                            }
                            int xxx = 0;
                            if(a1 & 0x1000)
                                xxx = sizeX - 1;
                            if(a0 & 0x1000) 
                            {
                                t -= (t % mosaicY);
                            }
                            int address = 0x10000 + ((((c + (t>>3) * inc)<<5)
                            + ((t & 7)<<2) + ((xxx>>3)<<5) + ((xxx & 7) >> 1))&0x7FFF);
                            u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
                            int palette = (a2 >> 8) & 0xF0;         
                            if(a1 & 0x1000) 
                            {
                                xxx = 7;
                                for(int xx = sizeX - 1; xx >= 0; xx--) 
                                {
                                    if(sx < 240) 
                                    {
                                        u8 color = vram[address];
                                        if(xx & 1) 
                                        {
                                            color = (color >> 4);
                                        }
                                        else
                                            color &= 0x0F;
                                        if ((color==0) && (((prio >> 25)&3) < 
                                        ((lineOBJ[sx]>>25)&3))) 
                                        {
                                            lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
                                            if((a0 & 0x1000) && m)
                                                lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                        }
                                        else if((color) && (prio < (lineOBJ[sx]&0xFF000000))) 
                                        {
                                            lineOBJ[sx] = READ16LE(&spritePalette[palette + color]) | prio;
                                            if((a0 & 0x1000) && m)
                                                lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                        }
                                    }
                                    if (a0 & 0x1000) 
                                    {
                                        m++;
                                        if (m==mosaicX)
                                            m=0;
                                    }
                                    sx = (sx+1) & 511;
                                    xxx--;
                                    if(!(xx & 1))
                                        address--;
                                    if(xxx == -1) 
                                    {
                                        xxx = 7;
                                        address -= 28;
                                    }
                                    if(address < 0x10000)
                                        address += 0x8000;
                                }
                            }
                            else 
                            {
                                for(int xx = 0; xx < sizeX; xx++) 
                                {
                                    if(sx < 240) 
                                    {
                                        u8 color = vram[address];
                                        if(xx & 1) 
                                        {
                                            color = (color >> 4);
                                        }
                                        else
                                            color &= 0x0F;
                                        if ((color==0) && (((prio >> 25)&3) < 
                                        ((lineOBJ[sx]>>25)&3))) 
                                        {
                                            lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
                                            if((a0 & 0x1000) && m)
                                                lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                        }
                                        else if((color) && (prio < (lineOBJ[sx]&0xFF000000))) 
                                        {
                                            lineOBJ[sx] = READ16LE(&spritePalette[palette + color]) | prio;
                                            if((a0 & 0x1000) && m)
                                                lineOBJ[sx]=(lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
                                        }
                                    }
                                    if (a0 & 0x1000) 
                                    {
                                        m++;
                                        if (m==mosaicX)
                                            m=0;
                                    }
                                    sx = (sx+1) & 511;
                                    xxx++;
                                    if(xx & 1)
                                        address++;
                                    if(xxx == 8) 
                                    {
                                        address += 28;
                                        xxx = 0;
                                    }
                                    if(address > 0x17fff)
                                        address -= 0x8000;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
inline void CGbaEmu::gfxDrawOBJWin(u32 *lineOBJWin)
{
    gfxClearArray(lineOBJWin);
    if(layerEnable & 0x8000) 
    {
        u16 *sprites = (u16 *)oam;
        // u16 *spritePalette = &((u16 *)paletteRAM)[256];
        for(int x = 0; x < 128 ; x++) 
        {
            u16 a0 = READ16LE(sprites++);
            u16 a1 = READ16LE(sprites++);
            u16 a2 = READ16LE(sprites++);
            sprites++;
            // ignore non OBJ-WIN
            if((a0 & 0x0c00) != 0x0800)
                continue;
            int sizeY = 8;
            int sizeX = 8;
            switch(((a0 >>12) & 0x0c)|(a1>>14)) 
            {
                case 0:
                    break;
                case 1:
                    sizeX = sizeY = 16;
                    break;
                case 2:
                    sizeX = sizeY = 32;
                    break;
                case 3:
                    sizeX = sizeY = 64;
                    break;
                case 4:
                    sizeX = 16;
                    break;
                case 5:
                    sizeX = 32;
                    break;
                case 6:
                    sizeX = 32;
                    sizeY = 16;
                    break;
                case 7:
                    sizeX = 64;
                    sizeY = 32;
                    break;
                case 8:
                    sizeY = 16;
                    break;
                case 9:
                    sizeY = 32;
                    break;
                case 10:
                    sizeX = 16;
                    sizeY = 32;
                    break;
                case 11:
                    sizeX = 32;
                    sizeY = 64;
                    break;
                default:
                continue;
            }
            int sy = (a0 & 255);
            if(sy > 160)
                sy -= 256;
            if(a0 & 0x0100) 
            {
                int fieldX = sizeX;
                int fieldY = sizeY;
                if(a0 & 0x0200) 
                {
                    fieldX <<= 1;
                    fieldY <<= 1;
                }
                int t = VCOUNT - sy;
                if((t >= 0) && (t < fieldY)) 
                {
                    int sx = (a1 & 0x1FF);
                    if((sx < 240) || (((sx + fieldX) & 511) < 240)) 
                    {
                        // int t2 = t - (fieldY >> 1);
                        int rot = (a1 >> 9) & 0x1F;
                        u16 *OAM = (u16 *)oam;
                        int dx = READ16LE(&OAM[3 + (rot << 4)]);
                        if(dx & 0x8000)
                            dx |= 0xFFFF8000;
                        int dmx = READ16LE(&OAM[7 + (rot << 4)]);
                        if(dmx & 0x8000)
                            dmx |= 0xFFFF8000;
                        int dy = READ16LE(&OAM[11 + (rot << 4)]);
                        if(dy & 0x8000)
                            dy |= 0xFFFF8000;
                        int dmy = READ16LE(&OAM[15 + (rot << 4)]);
                        if(dmy & 0x8000)
                            dmy |= 0xFFFF8000;
                        int realX = ((sizeX) << 7) - (fieldX >> 1)*dx - (fieldY>>1)*dmx
                        + t * dmx;
                        int realY = ((sizeY) << 7) - (fieldX >> 1)*dy - (fieldY>>1)*dmy
                        + t * dmy;
                        // u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
                        if(a0 & 0x2000) 
                        {
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40)
                                inc = sizeX >> 2;
                            else
                                c &= 0x3FE;
                            for(int x = 0; x < fieldX; x++) 
                            {
                                int xxx = realX >> 8;
                                int yyy = realY >> 8;
                                if(xxx < 0 || xxx >= sizeX ||
                                    yyy < 0 || yyy >= sizeY) 
                                {
                                }
                                else 
                                {
                                    u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
                                    + ((yyy & 7)<<3) + ((xxx >> 3)<<6) +
                                    (xxx & 7))&0x7fff)];
                                    if(color) 
                                    {
                                        lineOBJWin[sx] = 1;
                                    }
                                }
                                sx = (sx+1)&511;;
                                realX += dx;
                                realY += dy;
                            }
                        }
                        else 
                        {
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40)
                                inc = sizeX >> 3;
                            // int palette = (a2 >> 8) & 0xF0;                      
                            for(int x = 0; x < fieldX; x++) 
                            {
                                int xxx = realX >> 8;
                                int yyy = realY >> 8;
                                //              if(x == 0 || x == (sizeX-1) ||
                                //                 t == 0 || t == (sizeY-1)) {
                                //                lineOBJ[sx] = 0x001F | prio;
                                //              } else {
                                if(xxx < 0 || xxx >= sizeX ||
                                    yyy < 0 || yyy >= sizeY)
                                {
                                }
                                else 
                                {
                                    u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
                                    + ((yyy & 7)<<2) + ((xxx >> 3)<<5) +
                                    ((xxx & 7)>>1))&0x7fff)];
                                    if(xxx & 1)
                                        color >>= 4;
                                    else
                                        color &= 0x0F;
                                    if(color) 
                                    {
                                        lineOBJWin[sx] = 1;
                                    }
                                }
                                //            }
                                sx = (sx+1)&511;;
                                realX += dx;
                                realY += dy;
                            }
                        }
                    }
                }
            }
            else 
            {
                int t = VCOUNT - sy;
                if((t >= 0) && (t < sizeY)) 
                {
                    int sx = (a1 & 0x1FF);
                    if(((sx < 240)||(((sx+sizeX)&511)<240)) && !(a0 & 0x0200)) 
                    {
                        if(a0 & 0x2000) 
                        {
                            if(a1 & 0x2000)
                                t = sizeY - t - 1;
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40) 
                            {
                                inc = sizeX >> 2;
                            }
                            else 
                            {
                                c &= 0x3FE;
                            }
                            int xxx = 0;
                            if(a1 & 0x1000)
                                xxx = sizeX-1;
                            int address = 0x10000 + ((((c+ (t>>3) * inc) << 5)
                            + ((t & 7) << 3) + ((xxx>>3)<<6) + (xxx & 7))&0x7fff);
                            if(a1 & 0x1000)
                                xxx = 7;
                            // u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
                            for(int xx = 0; xx < sizeX; xx++) 
                            {
                                if(sx < 240) 
                                {
                                    u8 color = vram[address];
                                    if(color) 
                                    {
                                        lineOBJWin[sx] = 1;
                                    }
                                }
                                sx = (sx+1) & 511;
                                if(a1 & 0x1000) 
                                {
                                    xxx--;
                                    address--;
                                    if(xxx == -1) 
                                    {
                                        address -= 56;
                                        xxx = 7;
                                    }
                                    if(address < 0x10000)
                                        address += 0x8000;
                                }
                                else 
                                {
                                    xxx++;
                                    address++;
                                    if(xxx == 8) 
                                    {
                                        address += 56;
                                        xxx = 0;
                                    }
                                    if(address > 0x17fff)
                                        address -= 0x8000;
                                }
                            }
                        }
                        else 
                        {
                            if(a1 & 0x2000)
                                t = sizeY - t - 1;
                            int c = (a2 & 0x3FF);
                            if((DISPCNT & 7) > 2 && (c < 512))
                                continue;
                            int inc = 32;
                            if(DISPCNT & 0x40) 
                            {
                                inc = sizeX >> 3;
                            }
                            int xxx = 0;
                            if(a1 & 0x1000)
                                xxx = sizeX - 1;
                            int address = 0x10000 + ((((c + (t>>3) * inc)<<5)
                            + ((t & 7)<<2) + ((xxx>>3)<<5) + ((xxx & 7) >> 1))&0x7fff);
                            // u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
                            // int palette = (a2 >> 8) & 0xF0;              
                            if(a1 & 0x1000) 
                            {
                                xxx = 7;
                                for(int xx = sizeX - 1; xx >= 0; xx--) 
                                {
                                    if(sx < 240) 
                                    {
                                        u8 color = vram[address];
                                        if(xx & 1) 
                                        {
                                            color = (color >> 4);
                                        }
                                        else
                                            color &= 0x0F;
                                        if(color) 
                                        {
                                            lineOBJWin[sx] = 1;
                                        }
                                    }
                                    sx = (sx+1) & 511;
                                    xxx--;
                                    if(!(xx & 1))
                                        address--;
                                    if(xxx == -1) 
                                    {
                                        xxx = 7;
                                        address -= 28;
                                    }
                                    if(address < 0x10000)
                                        address += 0x8000;
                                }
                            }
                            else 
                            {
                                for(int xx = 0; xx < sizeX; xx++) 
                                {
                                    if(sx < 240) 
                                    {
                                        u8 color = vram[address];
                                        if(xx & 1) 
                                        {
                                            color = (color >> 4);
                                        }
                                        else
                                            color &= 0x0F;
                                        if(color) 
                                        {
                                            lineOBJWin[sx] = 1;
                                        }
                                    }
                                    sx = (sx+1) & 511;
                                    xxx++;
                                    if(xx & 1)
                                        address++;
                                    if(xxx == 8) 
                                    {
                                        address += 28;
                                        xxx = 0;
                                    }
                                    if(address > 0x17fff)
                                        address -= 0x8000;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
inline u32 CGbaEmu::gfxIncreaseBrightness(u32 color, int coeff)
{
    int r = (color & 0x1F);
    int g = ((color >> 5) & 0x1F);
    int b = ((color >> 10) & 0x1F);
    r = r + (((31 - r) * coeff) >> 4);
    g = g + (((31 - g) * coeff) >> 4);
    b = b + (((31 - b) * coeff) >> 4);
    if(r > 31)
        r = 31;
    if(g > 31)
        g = 31;
    if(b > 31)
        b = 31;
    color = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
    return color;
}
inline void CGbaEmu::gfxIncreaseBrightness(u32 *line, int coeff)
{
    for(int x = 0; x < 240; x++) 
    {
        u32 color = *line;
        int r = (color & 0x1F);
        int g = ((color >> 5) & 0x1F);
        int b = ((color >> 10) & 0x1F);
        r = r + (((31 - r) * coeff) >> 4);
        g = g + (((31 - g) * coeff) >> 4);
        b = b + (((31 - b) * coeff) >> 4);
        if(r > 31)
            r = 31;
        if(g > 31)
            g = 31;
        if(b > 31)
            b = 31;
        *line++ = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
    }
}
inline u32 CGbaEmu::gfxDecreaseBrightness(u32 color, int coeff)
{
    int r = (color & 0x1F);
    int g = ((color >> 5) & 0x1F);
    int b = ((color >> 10) & 0x1F);
    r = r - ((r * coeff) >> 4);
    g = g - ((g * coeff) >> 4);
    b = b - ((b * coeff) >> 4);
    if(r < 0)
        r = 0;
    if(g < 0)
        g = 0;
    if(b < 0)
        b = 0;
    color = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
    return color;
}
inline void CGbaEmu::gfxDecreaseBrightness(u32 *line, int coeff)
{
    for(int x = 0; x < 240; x++) 
    {
        u32 color = *line;
        int r = (color & 0x1F);
        int g = ((color >> 5) & 0x1F);
        int b = ((color >> 10) & 0x1F);
        r = r - ((r * coeff) >> 4);
        g = g - ((g * coeff) >> 4);
        b = b - ((b * coeff) >> 4);
        if(r < 0)
            r = 0;
        if(g < 0)
            g = 0;
        if(b < 0)
            b = 0;
        *line++ = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
    }
}
inline u32 CGbaEmu::gfxAlphaBlend(u32 color, u32 color2, int ca, int cb)
{
    if(color < 0x80000000) 
    {
        int r = (color & 0x1F);
        int g = ((color >> 5) & 0x1F);
        int b = ((color >> 10) & 0x1F);
        int r0 = (color2 & 0x1F);
        int g0 = ((color2 >> 5) & 0x1F);
        int b0 = ((color2 >> 10) & 0x1F);
        r = ((r * ca) >> 4) + ((r0 * cb) >> 4);
        g = ((g * ca) >> 4) + ((g0 * cb) >> 4);
        b = ((b * ca) >> 4) + ((b0 * cb) >> 4);
        if(r > 31)
            r = 31;
        if(g > 31)
            g = 31;
        if(b > 31)
            b = 31;
        return (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
    }
    return color;
}
inline void CGbaEmu::gfxAlphaBlend(u32 *ta, u32 *tb, int ca, int cb)
{
    for(int x = 0; x < 240; x++) 
    {
        u32 color = *ta;
        if(color < 0x80000000) 
        {
            int r = (color & 0x1F);
            int g = ((color >> 5) & 0x1F);
            int b = ((color >> 10) & 0x1F);
            u32 color2 = (*tb++);
            int r0 = (color2 & 0x1F);
            int g0 = ((color2 >> 5) & 0x1F);
            int b0 = ((color2 >> 10) & 0x1F);
            r = ((r * ca) >> 4) + ((r0 * cb) >> 4);
            g = ((g * ca) >> 4) + ((g0 * cb) >> 4);
            b = ((b * ca) >> 4) + ((b0 * cb) >> 4);
            if(r > 31)
                r = 31;
            if(g > 31)
                g = 31;
            if(b > 31)
                b = 31;
            *ta++ = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
        }
        else 
        {
            ta++;
            tb++;
        }
    }
}

void CGbaEmu::mode0RenderLine()
{
    int x;
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        return;
    }
    if(layerEnable & 0x0100) 
    {
        gfxDrawTextScreen(BG0CNT, BG0HOFS, BG0VOFS, line0);
    }
    if(layerEnable & 0x0200) 
    {
        gfxDrawTextScreen(BG1CNT, BG1HOFS, BG1VOFS, line1);
    }
    if(layerEnable & 0x0400) 
    {
        gfxDrawTextScreen(BG2CNT, BG2HOFS, BG2VOFS, line2);
    }
    if(layerEnable & 0x0800) 
    {
        gfxDrawTextScreen(BG3CNT, BG3HOFS, BG3VOFS, line3);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    for( x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if(line0[x] < color) 
        {
            color = line0[x];
            top = 0x01;
        }
        if((u8)(line1[x]>>24) < (u8)(color >> 24)) 
        {
            color = line1[x];
            top = 0x02;
        }
        if((u8)(line2[x]>>24) < (u8)(color >> 24)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(line3[x]>>24) < (u8)(color >> 24)) 
        {
            color = line3[x];
            top = 0x08;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if((top & 0x10) && (color & 0x00010000)) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((u8)(line0[x]>>24) < (u8)(back >> 24)) 
            {
                back = line0[x];
                top2 = 0x01;
            }
            if((u8)(line1[x]>>24) < (u8)(back >> 24)) 
            {
                back = line1[x];
                top2 = 0x02;
            }
            if((u8)(line2[x]>>24) < (u8)(back >> 24)) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if((u8)(line3[x]>>24) < (u8)(back >> 24)) 
            {
                back = line3[x];
                top2 = 0x08;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
}
void CGbaEmu::mode0RenderLineNoWindow()
{
    int x;
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        return;
    }
    if(layerEnable & 0x0100) 
    {
        gfxDrawTextScreen(BG0CNT, BG0HOFS, BG0VOFS, line0);
    }
    if(layerEnable & 0x0200) 
    {
        gfxDrawTextScreen(BG1CNT, BG1HOFS, BG1VOFS, line1);
    }
    if(layerEnable & 0x0400) 
    {
        gfxDrawTextScreen(BG2CNT, BG2HOFS, BG2VOFS, line2);
    }
    if(layerEnable & 0x0800) 
    {
        gfxDrawTextScreen(BG3CNT, BG3HOFS, BG3VOFS, line3);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    int effect = (BLDMOD >> 6) & 3;
    for(x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if(line0[x] < color) 
        {
            color = line0[x];
            top = 0x01;
        }
        if(line1[x] < (color & 0xFF000000)) 
        {
            color = line1[x];
            top = 0x02;
        }
        if(line2[x] < (color & 0xFF000000)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if(line3[x] < (color & 0xFF000000)) 
        {
            color = line3[x];
            top = 0x08;
        }
        if(lineOBJ[x] < (color & 0xFF000000)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(!(color & 0x00010000)) 
        {
            switch(effect) 
            {
                case 0:
                    break;
                case 1:
                    {
                        if(top & BLDMOD) 
                        {
                            u32 back = backdrop;
                            u8 top2 = 0x20;
                            if(line0[x] < back) 
                            {
                                if(top != 0x01) 
                                {
                                    back = line0[x];
                                    top2 = 0x01;
                                }
                            }
                            if(line1[x] < (back & 0xFF000000)) 
                            {
                                if(top != 0x02) 
                                {
                                    back = line1[x];
                                    top2 = 0x02;
                                }
                            }
                            if(line2[x] < (back & 0xFF000000)) 
                            {
                                if(top != 0x04) 
                                {
                                    back = line2[x];
                                    top2 = 0x04;
                                }
                            }
                            if(line3[x] < (back & 0xFF000000)) 
                            {
                                if(top != 0x08) 
                                {
                                    back = line3[x];
                                    top2 = 0x08;
                                }
                            }
                            if(lineOBJ[x] < (back & 0xFF000000)) 
                            {
                                if(top != 0x10) 
                                {
                                    back = lineOBJ[x];
                                    top2 = 0x10;
                                }
                            }
                            if(top2 & (BLDMOD>>8))
                                color = gfxAlphaBlend(color, back,
                                coeff[COLEV & 0x1F],
                                coeff[(COLEV >> 8) & 0x1F]);
                        }
                    }
                    break;
                case 2:
                    if(BLDMOD & top)
                        color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
                case 3:
                    if(BLDMOD & top)
                        color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
            }
        }
        else 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if(line0[x] < back) 
            {
                back = line0[x];
                top2 = 0x01;
            }
            if(line1[x] < (back & 0xFF000000)) 
            {
                back = line1[x];
                top2 = 0x02;
            }
            if(line2[x] < (back & 0xFF000000)) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(line3[x] < (back & 0xFF000000)) 
            {
                back = line3[x];
                top2 = 0x08;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
}
void CGbaEmu::mode0RenderLineAll()
{
    int x;
    u16 *palette = (u16 *)paletteRAM;

    if(DISPCNT & 0x80) 
    {
        for(x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        return;
    }
    
    bool inWindow0 = false;
    bool inWindow1 = false;
    if(layerEnable & 0x2000) 
    {
        u8 v0 = WIN0V >> 8;
        u8 v1 = WIN0V & 255;
        inWindow0 = ((v0 == v1) && (v0 >= 0xe8));
        if(v1 >= v0)
            inWindow0 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow0 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x4000) 
    {
        u8 v0 = WIN1V >> 8;
        u8 v1 = WIN1V & 255;
        inWindow1 = ((v0 == v1) && (v0 >= 0xe8));    
        if(v1 >= v0)
            inWindow1 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow1 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    
    if((layerEnable & 0x0100)) 
    {
        gfxDrawTextScreen(BG0CNT, BG0HOFS, BG0VOFS, line0);
    }
    if((layerEnable & 0x0200)) 
    {
        gfxDrawTextScreen(BG1CNT, BG1HOFS, BG1VOFS, line1);
    }
    if((layerEnable & 0x0400)) 
    {
        gfxDrawTextScreen(BG2CNT, BG2HOFS, BG2VOFS, line2);
    }
    if((layerEnable & 0x0800)) 
    {
        gfxDrawTextScreen(BG3CNT, BG3HOFS, BG3VOFS, line3);
    }
    gfxDrawSprites(lineOBJ);
    gfxDrawOBJWin(lineOBJWin);  
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    u8 inWin0Mask = WININ & 0xFF;
    u8 inWin1Mask = WININ >> 8;
    u8 outMask = WINOUT & 0xFF;
    for(x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        u8 mask = outMask;
        if(!(lineOBJWin[x] & 0x80000000)) 
        {
            mask = WINOUT >> 8;
        }
        if(inWindow1) 
        {
            if(gfxInWin1[x])
                mask = inWin1Mask;
        }
        if(inWindow0) 
        {
            if(gfxInWin0[x]) 
            {
                mask = inWin0Mask;
            }
        }
        if((mask & 1) && (line0[x] < color)) 
        {
            color = line0[x];
            top = 0x01;
        }
        if((mask & 2) && ((u8)(line1[x]>>24) < (u8)(color >> 24))) 
        {
            color = line1[x];
            top = 0x02;
        }
        if((mask & 4) && ((u8)(line2[x]>>24) < (u8)(color >> 24))) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((mask & 8) && ((u8)(line3[x]>>24) < (u8)(color >> 24))) 
        {                                   if(top != 0x01) 
            color = line3[x];
            top = 0x08;
        }
        if((mask & 16) && ((u8)(lineOBJ[x]>>24) < (u8)(color >> 24))) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        // special FX on in the window
        if(mask & 32) 
        {
            if(!(color & 0x00010000)) 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 0:
                        break;
                    case 1:
                        {
                            if(top & BLDMOD) 
                            {
                                u32 back = backdrop;
                                u8 top2 = 0x20;
                                if((mask & 1) && (u8)(line0[x]>>24) < (u8)(back >> 24)) 
                                {

                                    {
                                        back = line0[x];
                                        top2 = 0x01;
                                    }
                                }
                                if((mask & 2) && (u8)(line1[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x02) 
                                    {
                                        back = line1[x];
                                        top2 = 0x02;
                                    }
                                }
                                if((mask & 4) && (u8)(line2[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x04) 
                                    {
                                        back = line2[x];
                                        top2 = 0x04;
                                    }
                                }
                                if((mask & 8) && (u8)(line3[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x08) 
                                    {
                                        back = line3[x];
                                        top2 = 0x08;
                                    }
                                }
                                if((mask & 16) && (u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x10) 
                                    {
                                        back = lineOBJ[x];
                                        top2 = 0x10;
                                    }
                                }
                                if(top2 & (BLDMOD>>8))
                                    color = gfxAlphaBlend(color, back,
                                    coeff[COLEV & 0x1F],
                                    coeff[(COLEV >> 8) & 0x1F]);
                            }
                        }
                        break;
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
            else 
            {
                // semi-transparent OBJ
                u32 back = backdrop;
                u8 top2 = 0x20;
                if((mask & 1) && ((u8)(line0[x]>>24) < (u8)(back >> 24))) 
                {
                    back = line0[x];
                    top2 = 0x01;
                }
                if((mask & 2) && ((u8)(line1[x]>>24) < (u8)(back >> 24))) 
                {
                    back = line1[x];
                    top2 = 0x02;
                }
                if((mask & 4) && ((u8)(line2[x]>>24) < (u8)(back >> 24))) 
                {
                    back = line2[x];
                    top2 = 0x04;
                }
                if((mask & 8) && ((u8)(line3[x]>>24) < (u8)(back >> 24))) 
                {
                    back = line3[x];
                    top2 = 0x08;
                }
                if(top2 & (BLDMOD>>8))
                    color = gfxAlphaBlend(color, back,
                    coeff[COLEV & 0x1F],
                    coeff[(COLEV >> 8) & 0x1F]);
                else 
                {
                    switch((BLDMOD >> 6) & 3) 
                    {
                        case 2:
                            if(BLDMOD & top)
                                color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                        case 3:
                            if(BLDMOD & top)
                                color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                    }
                }
            }
        }
        else if(color & 0x00010000) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((mask & 1) && ((u8)(line0[x]>>24) < (u8)(back >> 24))) 
            {
                back = line0[x];
                top2 = 0x01;
            }
            if((mask & 2) && ((u8)(line1[x]>>24) < (u8)(back >> 24))) 
            {
                back = line1[x];
                top2 = 0x02;
            }
            if((mask & 4) && ((u8)(line2[x]>>24) < (u8)(back >> 24))) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if((mask & 8) && ((u8)(line3[x]>>24) < (u8)(back >> 24))) 
            {
                back = line3[x];
                top2 = 0x08;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
}
void CGbaEmu::mode1RenderLine()
{
    int x;
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    if(layerEnable & 0x0100) 
    {
        gfxDrawTextScreen(BG0CNT, BG0HOFS, BG0VOFS, line0);
    }
    if(layerEnable & 0x0200) 
    {
        gfxDrawTextScreen(BG1CNT, BG1HOFS, BG1VOFS, line1);
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed, line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    for(x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if(line0[x] < color) 
        {
            color = line0[x];
            top = 0x01;
        }
        if((u8)(line1[x]>>24) < (u8)(color >> 24)) 
        {
            color = line1[x];
            top = 0x02;
        }
        if((u8)(line2[x]>>24) < (u8)(color >> 24)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if((top & 0x10) && (color & 0x00010000)) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((u8)(line0[x]>>24) < (u8)(back >> 24)) 
            {
                back = line0[x];
                top2 = 0x01;
            }
            if((u8)(line1[x]>>24) < (u8)(back >> 24)) 
            {
                back = line1[x];
                top2 = 0x02;
            }
            if((u8)(line2[x]>>24) < (u8)(back >> 24)) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT; 
}
void CGbaEmu::mode1RenderLineNoWindow()
{
    int x;
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    if(layerEnable & 0x0100) 
    {
        gfxDrawTextScreen(BG0CNT, BG0HOFS, BG0VOFS, line0);
    }
    if(layerEnable & 0x0200) 
    {
        gfxDrawTextScreen(BG1CNT, BG1HOFS, BG1VOFS, line1);
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed, line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    for(x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if(line0[x] < color) 
        {
            color = line0[x];
            top = 0x01;
        }
        if((u8)(line1[x]>>24) < (u8)(color >> 24)) 
        {
            color = line1[x];
            top = 0x02;
        }
        if((u8)(line2[x]>>24) < (u8)(color >> 24)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(!(color & 0x00010000)) 
        {
            switch((BLDMOD >> 6) & 3) 
            {
                case 0:
                    break;
                case 1:
                    {
                        if(top & BLDMOD) 
                        {
                            u32 back = backdrop;
                            u8 top2 = 0x20;
                            if((u8)(line0[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x01) 
                                {
                                    back = line0[x];
                                    top2 = 0x01;
                                }
                            }
                            if((u8)(line1[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x02) 
                                {
                                    back = line1[x];
                                    top2 = 0x02;
                                }
                            }
                            if((u8)(line2[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x04) 
                                {
                                    back = line2[x];
                                    top2 = 0x04;
                                }
                            }
                            if((u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x10) 
                                {
                                    back = lineOBJ[x];
                                    top2 = 0x10;
                                }
                            }
                            if(top2 & (BLDMOD>>8))
                                color = gfxAlphaBlend(color, back,
                                coeff[COLEV & 0x1F],
                                coeff[(COLEV >> 8) & 0x1F]);
                        }
                    }
                    break;
                case 2:
                    if(BLDMOD & top)
                        color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
                case 3:
                    if(BLDMOD & top)
                        color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
            }
        }
        else 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((u8)(line0[x]>>24) < (u8)(back >> 24)) 
            {
                back = line0[x];
                top2 = 0x01;
            }
            if((u8)(line1[x]>>24) < (u8)(back >> 24)) 
            {
                back = line1[x];
                top2 = 0x02;
            }
            if((u8)(line2[x]>>24) < (u8)(back >> 24)) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::mode1RenderLineAll()
{
    int x;
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    bool inWindow0 = false;
    bool inWindow1 = false;
    if(layerEnable & 0x2000) 
    {
        u8 v0 = WIN0V >> 8;
        u8 v1 = WIN0V & 255;
        inWindow0 = ((v0 == v1) && (v0 >= 0xe8));
        if(v1 >= v0)
            inWindow0 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow0 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x4000) 
    {
        u8 v0 = WIN1V >> 8;
        u8 v1 = WIN1V & 255;
        inWindow1 = ((v0 == v1) && (v0 >= 0xe8));    
        if(v1 >= v0)
            inWindow1 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow1 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x0100) 
    {
        gfxDrawTextScreen(BG0CNT, BG0HOFS, BG0VOFS, line0);
    }
    if(layerEnable & 0x0200) 
    {
        gfxDrawTextScreen(BG1CNT, BG1HOFS, BG1VOFS, line1);
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed, line2);
    }
    gfxDrawSprites(lineOBJ);
    gfxDrawOBJWin(lineOBJWin);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    u8 inWin0Mask = WININ & 0xFF;
    u8 inWin1Mask = WININ >> 8;
    u8 outMask = WINOUT & 0xFF;
    for(x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        u8 mask = outMask;
        if(!(lineOBJWin[x] & 0x80000000)) 
        {
            mask = WINOUT >> 8;
        }
        if(inWindow1) 
        {
            if(gfxInWin1[x])
                mask = inWin1Mask;
        }
        if(inWindow0) 
        {
            if(gfxInWin0[x]) 
            {
                mask = inWin0Mask;
            }
        }
        if(line0[x] < color && (mask & 1)) 
        {
            color = line0[x];
            top = 0x01;
        }
        if((u8)(line1[x]>>24) < (u8)(color >> 24) && (mask & 2)) 
        {
            color = line1[x];
            top = 0x02;
        }
        if((u8)(line2[x]>>24) < (u8)(color >> 24) && (mask & 4)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24) && (mask & 16)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        // special FX on the window
        if(mask & 32) 
        {
            if(!(color & 0x00010000)) 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 0:
                        break;
                    case 1:
                        {
                            if(top & BLDMOD) 
                            {
                                u32 back = backdrop;
                                u8 top2 = 0x20;
                                if((mask & 1) && (u8)(line0[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x01) 
                                    {
                                        back = line0[x];
                                        top2 = 0x01;
                                    }
                                }
                                if((mask & 2) && (u8)(line1[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x02) 
                                    {
                                        back = line1[x];
                                        top2 = 0x02;
                                    }
                                }
                                if((mask & 4) && (u8)(line2[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x04) 
                                    {
                                        back = line2[x];
                                        top2 = 0x04;
                                    }
                                }
                                if((mask & 16) && (u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x10) 
                                    {
                                        back = lineOBJ[x];
                                        top2 = 0x10;
                                    }
                                }
                                if(top2 & (BLDMOD>>8))
                                    color = gfxAlphaBlend(color, back,
                                    coeff[COLEV & 0x1F],
                                    coeff[(COLEV >> 8) & 0x1F]);
                            }
                        }
                        break;
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
            else 
            {
                // semi-transparent OBJ
                u32 back = backdrop;
                u8 top2 = 0x20;
                if((mask & 1) && (u8)(line0[x]>>24) < (u8)(back >> 24)) 
                {
                    back = line0[x];
                    top2 = 0x01;
                }
                if((mask & 2) && (u8)(line1[x]>>24) < (u8)(back >> 24)) 
                {
                    back = line1[x];
                    top2 = 0x02;
                }
                if((mask & 4) && (u8)(line2[x]>>24) < (u8)(back >> 24)) 
                {
                    back = line2[x];
                    top2 = 0x04;
                }
                if(top2 & (BLDMOD>>8))
                    color = gfxAlphaBlend(color, back,
                    coeff[COLEV & 0x1F],
                    coeff[(COLEV >> 8) & 0x1F]);
                else 
                {
                    switch((BLDMOD >> 6) & 3) 
                    {
                        case 2:
                            if(BLDMOD & top)
                                color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                        case 3:
                            if(BLDMOD & top)
                                color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                    }
                }
            }
        }
        else if(color & 0x00010000) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((mask & 1) && (u8)(line0[x]>>24) < (u8)(back >> 24)) 
            {
                back = line0[x];
                top2 = 0x01;
            }
            if((mask & 2) && (u8)(line1[x]>>24) < (u8)(back >> 24)) 
            {
                back = line1[x];
                top2 = 0x02;
            }
            if((mask & 4) && (u8)(line2[x]>>24) < (u8)(back >> 24)) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}

void CGbaEmu::mode2RenderLine()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;
        return;
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD, gfxBG2X, gfxBG2Y,
        changed, line2);
    }
    if(layerEnable & 0x0800) 
    {
        int changed = gfxBG3Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG3CNT, BG3X_L, BG3X_H, BG3Y_L, BG3Y_H,
        BG3PA, BG3PB, BG3PC, BG3PD, gfxBG3X, gfxBG3Y,
        changed, line3);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if((u8)(line2[x]>>24) < (u8)(color >> 24)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(line3[x]>>24) < (u8)(color >> 24)) 
        {
            color = line3[x];
            top = 0x08;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if((top & 0x10) && (color & 0x00010000)) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((u8)(line2[x]>>24) < (u8)(back >> 24)) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if((u8)(line3[x]>>24) < (u8)(back >> 24)) 
            {
                back = line3[x];
                top2 = 0x08;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxBG3Changed = 0;
    gfxLastVCOUNT = VCOUNT;    
}
void CGbaEmu::mode2RenderLineNoWindow()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;
        return;
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD, gfxBG2X, gfxBG2Y,
        changed, line2);
    }
    if(layerEnable & 0x0800) 
    {
        int changed = gfxBG3Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG3CNT, BG3X_L, BG3X_H, BG3Y_L, BG3Y_H,
        BG3PA, BG3PB, BG3PC, BG3PD, gfxBG3X, gfxBG3Y,
        changed, line3);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if((u8)(line2[x]>>24) < (u8)(color >> 24)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(line3[x]>>24) < (u8)(color >> 24)) 
        {
            color = line3[x];
            top = 0x08;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(!(color & 0x00010000)) 
        {
            switch((BLDMOD >> 6) & 3) 
            {
                case 0:
                    break;
                case 1:
                    {
                        if(top & BLDMOD) 
                        {
                            u32 back = backdrop;
                            u8 top2 = 0x20;
                            if((u8)(line2[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x04) 
                                {
                                    back = line2[x];
                                    top2 = 0x04;
                                }
                            }
                            if((u8)(line3[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x08) 
                                {
                                    back = line3[x];
                                    top2 = 0x08;
                                }
                            }
                            if((u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x10) 
                                {
                                    back = lineOBJ[x];
                                    top2 = 0x10;
                                }
                            }
                            if(top2 & (BLDMOD>>8))
                                color = gfxAlphaBlend(color, back,
                                coeff[COLEV & 0x1F],
                                coeff[(COLEV >> 8) & 0x1F]);
                        }
                    }
                    break;
                case 2:
                    if(BLDMOD & top)
                        color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
                case 3:
                    if(BLDMOD & top)
                        color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
            }
        }
        else 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((u8)(line2[x]>>24) < (u8)(back >> 24)) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if((u8)(line3[x]>>24) < (u8)(back >> 24)) 
            {
                back = line3[x];
                top2 = 0x08;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxBG3Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::mode2RenderLineAll()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;
        return;
    }
    bool inWindow0 = false;
    bool inWindow1 = false;
    if(layerEnable & 0x2000) 
    {
        u8 v0 = WIN0V >> 8;
        u8 v1 = WIN0V & 255;
        inWindow0 = ((v0 == v1) && (v0 >= 0xe8));
        if(v1 >= v0)
            inWindow0 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow0 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x4000) 
    {
        u8 v0 = WIN1V >> 8;
        u8 v1 = WIN1V & 255;
        inWindow1 = ((v0 == v1) && (v0 >= 0xe8));    
        if(v1 >= v0)
            inWindow1 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow1 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD, gfxBG2X, gfxBG2Y,
        changed, line2);
    }
    if(layerEnable & 0x0800) 
    {
        int changed = gfxBG3Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen(BG3CNT, BG3X_L, BG3X_H, BG3Y_L, BG3Y_H,
        BG3PA, BG3PB, BG3PC, BG3PD, gfxBG3X, gfxBG3Y,
        changed, line3);
    }
    gfxDrawSprites(lineOBJ);
    gfxDrawOBJWin(lineOBJWin);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    u8 inWin0Mask = WININ & 0xFF;
    u8 inWin1Mask = WININ >> 8;
    u8 outMask = WINOUT & 0xFF;
    for(int x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        u8 mask = outMask;    
        if(!(lineOBJWin[x] & 0x80000000)) 
        {
            mask = WINOUT >> 8;
        }
        if(inWindow1) 
        {
            if(gfxInWin1[x])
                mask = inWin1Mask;
        }
        if(inWindow0) 
        {
            if(gfxInWin0[x]) 
            {
                mask = inWin0Mask;
            }
        }
        if(line2[x] < color && (mask & 4)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(line3[x]>>24) < (u8)(color >> 24) && (mask & 8)) 
        {
            color = line3[x];
            top = 0x08;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24) && (mask & 16)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(mask & 32) 
        {
            if(!(color & 0x00010000)) 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 0:
                        break;
                    case 1:
                        {
                            if(top & BLDMOD) 
                            {
                                u32 back = backdrop;
                                u8 top2 = 0x20;
                                if((mask & 4) && line2[x] < back) 
                                {
                                    if(top != 0x04) 
                                    {
                                        back = line2[x];
                                        top2 = 0x04;
                                    }
                                }
                                if((mask & 8) && (u8)(line3[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x08) 
                                    {
                                        back = line3[x];
                                        top2 = 0x08;
                                    }
                                }
                                if((mask & 16) && (u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x10) 
                                    {
                                        back = lineOBJ[x];
                                        top2 = 0x10;
                                    }
                                }
                                if(top2 & (BLDMOD>>8))
                                    color = gfxAlphaBlend(color, back,
                                    coeff[COLEV & 0x1F],
                                    coeff[(COLEV >> 8) & 0x1F]); 
                            }
                        }
                        break;
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
            else 
            {
                // semi-transparent OBJ
                u32 back = backdrop;
                u8 top2 = 0x20;
                if((mask & 4) && line2[x] < back) 
                {
                    back = line2[x];
                    top2 = 0x04;
                }
                if((mask & 8) && (u8)(line3[x]>>24) < (u8)(back >> 24)) 
                {
                    back = line3[x];
                    top2 = 0x08;
                }
                if(top2 & (BLDMOD>>8))
                    color = gfxAlphaBlend(color, back,
                    coeff[COLEV & 0x1F],
                    coeff[(COLEV >> 8) & 0x1F]);
                else 
                {
                    switch((BLDMOD >> 6) & 3) 
                    {
                        case 2:
                            if(BLDMOD & top)
                                color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                        case 3:
                            if(BLDMOD & top)
                                color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                    }
                }
            }
        }
        else if(color & 0x00010000) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((mask & 4) && line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if((mask & 8) && (u8)(line3[x]>>24) < (u8)(back >> 24)) 
            {
                back = line3[x];
                top2 = 0x08;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxBG3Changed = 0;
    gfxLastVCOUNT = VCOUNT;      
}
void CGbaEmu::mode3RenderLine()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen16Bit(BG2CNT, BG2X_L, BG2X_H,
        BG2Y_L, BG2Y_H, BG2PA, BG2PB,
        BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,                      
        line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 background = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = background;
        u8 top = 0x20;
        if(line2[x] < color) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >>24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if((top & 0x10) && (color & 0x00010000)) 
        {
            // semi-transparent OBJ
            u32 back = background;
            u8 top2 = 0x20;
            if(line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;      
}
void CGbaEmu::mode3RenderLineNoWindow()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen16Bit(BG2CNT, BG2X_L, BG2X_H,
        BG2Y_L, BG2Y_H, BG2PA, BG2PB,
        BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,                      
        line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 background = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = background;
        u8 top = 0x20;
        if(line2[x] < color) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >>24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(!(color & 0x00010000)) 
        {
            switch((BLDMOD >> 6) & 3) 
            {
                case 0:
                    break;
                case 1:
                    {
                        if(top & BLDMOD) 
                        {
                            u32 back = background;
                            u8 top2 = 0x20;
                            if(line2[x] < back) 
                            {
                                if(top != 0x04) 
                                {
                                    back = line2[x];
                                    top2 = 0x04;
                                }
                            }
                            if((u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x10) 
                                {
                                    back = lineOBJ[x];
                                    top2 = 0x10;
                                }
                            }
                            if(top2 & (BLDMOD>>8))
                                color = gfxAlphaBlend(color, back,
                                coeff[COLEV & 0x1F],
                                coeff[(COLEV >> 8) & 0x1F]);
                        }
                    }
                    break;
                case 2:
                    if(BLDMOD & top)
                        color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
                case 3:
                    if(BLDMOD & top)
                        color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
            }
        }
        else 
        {
            // semi-transparent OBJ
            u32 back = background;
            u8 top2 = 0x20;
            if(line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::mode3RenderLineAll()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x80) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    bool inWindow0 = false;
    bool inWindow1 = false;
    if(layerEnable & 0x2000) 
    {
        u8 v0 = WIN0V >> 8;
        u8 v1 = WIN0V & 255;
        inWindow0 = ((v0 == v1) && (v0 >= 0xe8));
        if(v1 >= v0)
            inWindow0 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow0 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x4000) 
    {
        u8 v0 = WIN1V >> 8;
        u8 v1 = WIN1V & 255;
        inWindow1 = ((v0 == v1) && (v0 >= 0xe8));    
        if(v1 >= v0)
            inWindow1 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow1 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen16Bit(BG2CNT, BG2X_L, BG2X_H,
        BG2Y_L, BG2Y_H, BG2PA, BG2PB,
        BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,                      
        line2);
    }
    gfxDrawSprites(lineOBJ);
    gfxDrawOBJWin(lineOBJWin);
    u8 inWin0Mask = WININ & 0xFF;
    u8 inWin1Mask = WININ >> 8;
    u8 outMask = WINOUT & 0xFF;
    u32 background = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = background;
        u8 top = 0x20;
        u8 mask = outMask;
        if(!(lineOBJWin[x] & 0x80000000)) 
        {
            mask = WINOUT >> 8;
        }
        if(inWindow1) 
        {
            if(gfxInWin1[x])
                mask = inWin1Mask;
        }
        if(inWindow0) 
        {
            if(gfxInWin0[x]) 
            {
                mask = inWin0Mask;
            }
        }
        if((mask & 4) && (line2[x] < color)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((mask & 16) && ((u8)(lineOBJ[x]>>24) < (u8)(color >>24))) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(mask & 32) 
        {
            if(!(color & 0x00010000)) 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 0:
                        break;
                    case 1:
                        {
                            if(top & BLDMOD) 
                            {
                                u32 back = background;
                                u8 top2 = 0x20;
                                if((mask & 4) && line2[x] < back) 
                                {
                                    if(top != 0x04) 
                                    {
                                        back = line2[x];
                                        top2 = 0x04;
                                    }
                                }
                                if((mask & 16) && (u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x10) 
                                    {
                                        back = lineOBJ[x];
                                        top2 = 0x10;
                                    }
                                }
                                if(top2 & (BLDMOD>>8))
                                    color = gfxAlphaBlend(color, back,
                                    coeff[COLEV & 0x1F],
                                    coeff[(COLEV >> 8) & 0x1F]);
                            }
                        }
                        break;
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
            else 
            {
                // semi-transparent OBJ
                u32 back = background;
                u8 top2 = 0x20;
                if((mask & 4) && line2[x] < back) 
                {
                    back = line2[x];
                    top2 = 0x04;
                }
                if(top2 & (BLDMOD>>8))
                    color = gfxAlphaBlend(color, back,
                    coeff[COLEV & 0x1F],
                    coeff[(COLEV >> 8) & 0x1F]);
                else 
                {
                    switch((BLDMOD >> 6) & 3) 
                    {
                        case 2:
                            if(BLDMOD & top)
                                color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                        case 3:
                            if(BLDMOD & top)
                                color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                    }
                }
            }
        }
        else if(color & 0x00010000) 
        {
            // semi-transparent OBJ
            u32 back = background;
            u8 top2 = 0x20;
            if((mask & 4) && line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}

void CGbaEmu::mode4RenderLine()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x0080) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    if(layerEnable & 0x400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen256(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,
        line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if(line2[x] < color) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if((top & 0x10) && (color & 0x00010000)) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if(line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::mode4RenderLineNoWindow()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x0080) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;        
        return;
    }
    if(layerEnable & 0x400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen256(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,
        line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        if(line2[x] < color) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >> 24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(!(color & 0x00010000)) 
        {
            switch((BLDMOD >> 6) & 3) 
            {
                case 0:
                    break;
                case 1:
                    {
                        if(top & BLDMOD) 
                        {
                            u32 back = backdrop;
                            u8 top2 = 0x20;
                            if(line2[x] < back) 
                            {
                                if(top != 0x04) 
                                {
                                    back = line2[x];
                                    top2 = 0x04;
                                }
                            }
                            if((u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x10) 
                                {
                                    back = lineOBJ[x];
                                    top2 = 0x10;
                                }
                            }
                            if(top2 & (BLDMOD>>8))
                                color = gfxAlphaBlend(color, back,
                                coeff[COLEV & 0x1F],
                                coeff[(COLEV >> 8) & 0x1F]);
                        }
                    }
                    break;
                case 2:
                    if(BLDMOD & top)
                        color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
                case 3:
                    if(BLDMOD & top)
                        color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
            }
        }
        else 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if(line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::mode4RenderLineAll()
{
    u16 *palette = (u16 *)paletteRAM;
    if(DISPCNT & 0x0080) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;        
        return;
    }
    bool inWindow0 = false;
    bool inWindow1 = false;
    if(layerEnable & 0x2000) 
    {
        u8 v0 = WIN0V >> 8;
        u8 v1 = WIN0V & 255;
        inWindow0 = ((v0 == v1) && (v0 >= 0xe8));
        if(v1 >= v0)
            inWindow0 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow0 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x4000) 
    {
        u8 v0 = WIN1V >> 8;
        u8 v1 = WIN1V & 255;
        inWindow1 = ((v0 == v1) && (v0 >= 0xe8));    
        if(v1 >= v0)
            inWindow1 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow1 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen256(BG2CNT, BG2X_L, BG2X_H, BG2Y_L, BG2Y_H,
        BG2PA, BG2PB, BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,
        line2);
    }
    gfxDrawSprites(lineOBJ);
    gfxDrawOBJWin(lineOBJWin);  
    u32 backdrop = (READ16LE(&palette[0]) | 0x30000000);
    u8 inWin0Mask = WININ & 0xFF;
    u8 inWin1Mask = WININ >> 8;
    u8 outMask = WINOUT & 0xFF;
    for(int x = 0; x < 240; x++) 
    {
        u32 color = backdrop;
        u8 top = 0x20;
        u8 mask = outMask;    
        if(!(lineOBJWin[x] & 0x80000000)) 
        {
            mask = WINOUT >> 8;
        }
        if(inWindow1) 
        {
            if(gfxInWin1[x])
                mask = inWin1Mask;
        }
        if(inWindow0) 
        {
            if(gfxInWin0[x]) 
            {
                mask = inWin0Mask;
            }
        }
        if((mask & 4) && (line2[x] < color)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((mask & 16) && ((u8)(lineOBJ[x]>>24) < (u8)(color >>24))) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(mask & 32) 
        {
            if(!(color & 0x00010000)) 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 0:
                        break;
                    case 1:
                        {
                            if(top & BLDMOD) 
                            {
                                u32 back = backdrop;
                                u8 top2 = 0x20;
                                if((mask & 4) && line2[x] < back) 
                                {
                                    if(top != 0x04) 
                                    {
                                        back = line2[x];
                                        top2 = 0x04;
                                    }
                                }
                                if((mask & 16) && (u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x10) 
                                    {
                                        back = lineOBJ[x];
                                        top2 = 0x10;
                                    }
                                }
                                if(top2 & (BLDMOD>>8))
                                    color = gfxAlphaBlend(color, back,
                                    coeff[COLEV & 0x1F],
                                    coeff[(COLEV >> 8) & 0x1F]);
                            }
                        }
                        break;
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
            else 
            {
                // semi-transparent OBJ
                u32 back = backdrop;
                u8 top2 = 0x20;
                if((mask & 4) && line2[x] < back) 
                {
                    back = line2[x];
                    top2 = 0x04;
                }
                if(top2 & (BLDMOD>>8))
                    color = gfxAlphaBlend(color, back,
                    coeff[COLEV & 0x1F],
                    coeff[(COLEV >> 8) & 0x1F]);
                else 
                {
                    switch((BLDMOD >> 6) & 3) 
                    {
                        case 2:
                            if(BLDMOD & top)
                                color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                        case 3:
                            if(BLDMOD & top)
                                color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                    }
                }
            }
        }
        else if(color & 0x00010000) 
        {
            // semi-transparent OBJ
            u32 back = backdrop;
            u8 top2 = 0x20;
            if((mask & 4) && line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}

void CGbaEmu::mode5RenderLine()
{
    if(DISPCNT & 0x0080) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    u16 *palette = (u16 *)paletteRAM;
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen16Bit160(BG2CNT, BG2X_L, BG2X_H,
        BG2Y_L, BG2Y_H, BG2PA, BG2PB,
        BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,
        line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 background = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = background;
        u8 top = 0x20;
        if(line2[x] < color) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >>24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if((top & 0x10) && (color & 0x00010000)) 
        {
            // semi-transparent OBJ
            u32 back = background;
            u8 top2 = 0x20;
            if(line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::mode5RenderLineNoWindow()
{
    if(DISPCNT & 0x0080) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    u16 *palette = (u16 *)paletteRAM;
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen16Bit160(BG2CNT, BG2X_L, BG2X_H,
        BG2Y_L, BG2Y_H, BG2PA, BG2PB,
        BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,
        line2);
    }
    gfxDrawSprites(lineOBJ);
    u32 background = ( READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = background;
        u8 top = 0x20;
        if(line2[x] < color) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((u8)(lineOBJ[x]>>24) < (u8)(color >>24)) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(!(color & 0x00010000)) 
        {
            switch((BLDMOD >> 6) & 3) 
            {
                case 0:
                    break;
                case 1:
                    {
                        if(top & BLDMOD) 
                        {
                            u32 back = background;
                            u8 top2 = 0x20;
                            if(line2[x] < back) 
                            {
                                if(top != 0x04) 
                                {
                                    back = line2[x];
                                    top2 = 0x04;
                                }
                            }
                            if((u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                            {
                                if(top != 0x10) 
                                {
                                    back = lineOBJ[x];
                                    top2 = 0x10;
                                }
                            }
                            if(top2 & (BLDMOD>>8))
                                color = gfxAlphaBlend(color, back,
                                coeff[COLEV & 0x1F],
                                coeff[(COLEV >> 8) & 0x1F]);
                        }
                    }
                    break;
                case 2:
                    if(BLDMOD & top)
                        color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
                case 3:
                    if(BLDMOD & top)
                        color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                    break;
            }
        }
        else 
        {
            // semi-transparent OBJ
            u32 back = background;
            u8 top2 = 0x20;
            if(line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::mode5RenderLineAll()
{
    if(DISPCNT & 0x0080) 
    {
        for(int x = 0; x < 240; x++) 
        {
            lineMix[x] = 0x7fff;
        }
        gfxLastVCOUNT = VCOUNT;    
        return;
    }
    u16 *palette = (u16 *)paletteRAM;
    if(layerEnable & 0x0400) 
    {
        int changed = gfxBG2Changed;
        if(gfxLastVCOUNT > VCOUNT)
            changed = 3;
        gfxDrawRotScreen16Bit160(BG2CNT, BG2X_L, BG2X_H,
        BG2Y_L, BG2Y_H, BG2PA, BG2PB,
        BG2PC, BG2PD,
        gfxBG2X, gfxBG2Y, changed,
        line2);
    }
    gfxDrawSprites(lineOBJ);
    gfxDrawOBJWin(lineOBJWin);
    bool inWindow0 = false;
    bool inWindow1 = false;
    if(layerEnable & 0x2000) 
    {
        u8 v0 = WIN0V >> 8;
        u8 v1 = WIN0V & 255;
        inWindow0 = ((v0 == v1) && (v0 >= 0xe8));
        if(v1 >= v0)
            inWindow0 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow0 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    if(layerEnable & 0x4000) 
    {
        u8 v0 = WIN1V >> 8;
        u8 v1 = WIN1V & 255;
        inWindow1 = ((v0 == v1) && (v0 >= 0xe8));    
        if(v1 >= v0)
            inWindow1 |= (VCOUNT >= v0 && VCOUNT < v1);
        else
            inWindow1 |= (VCOUNT >= v0 || VCOUNT < v1);
    }
    u8 inWin0Mask = WININ & 0xFF;
    u8 inWin1Mask = WININ >> 8;
    u8 outMask = WINOUT & 0xFF;
    u32 background = (READ16LE(&palette[0]) | 0x30000000);
    for(int x = 0; x < 240; x++) 
    {
        u32 color = background;
        u8 top = 0x20;
        u8 mask = outMask;
        if(!(lineOBJWin[x] & 0x80000000)) 
        {
            mask = WINOUT >> 8;
        }
        if(inWindow1) 
        {
            if(gfxInWin1[x])
                mask = inWin1Mask;
        }
        if(inWindow0) 
        {
            if(gfxInWin0[x]) 
            {
                mask = inWin0Mask;
            }
        }
        if((mask & 4) && (line2[x] < color)) 
        {
            color = line2[x];
            top = 0x04;
        }
        if((mask & 16) && ((u8)(lineOBJ[x]>>24) < (u8)(color >>24))) 
        {
            color = lineOBJ[x];
            top = 0x10;
        }
        if(mask & 32) 
        {
            if(!(color & 0x00010000)) 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 0:
                        break;
                    case 1:
                        {
                            if(top & BLDMOD) 
                            {
                                u32 back = background;
                                u8 top2 = 0x20;
                                if((mask & 4) && line2[x] < back) 
                                {
                                    if(top != 0x04) 
                                    {
                                        back = line2[x];
                                        top2 = 0x04;
                                    }
                                }
                                if((mask & 16) && (u8)(lineOBJ[x]>>24) < (u8)(back >> 24)) 
                                {
                                    if(top != 0x10) 
                                    {
                                        back = lineOBJ[x];
                                        top2 = 0x10;
                                    }
                                }
                                if(top2 & (BLDMOD>>8))
                                    color = gfxAlphaBlend(color, back,
                                    coeff[COLEV & 0x1F],
                                    coeff[(COLEV >> 8) & 0x1F]);
                            }
                        }
                        break;
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
            else 
            {
                // semi-transparent OBJ
                u32 back = background;
                u8 top2 = 0x20;
                if((mask & 4) && line2[x] < back) 
                {
                    back = line2[x];
                    top2 = 0x04;
                }
                if(top2 & (BLDMOD>>8))
                    color = gfxAlphaBlend(color, back,
                    coeff[COLEV & 0x1F],
                    coeff[(COLEV >> 8) & 0x1F]);
                else 
                {
                    switch((BLDMOD >> 6) & 3) 
                    {
                        case 2:
                            if(BLDMOD & top)
                                color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                        case 3:
                            if(BLDMOD & top)
                                color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                            break;
                    }
                }
            }
        }
        else if(color & 0x00010000) 
        {
            // semi-transparent OBJ
            u32 back = background;
            u8 top2 = 0x20;
            if((mask & 4) && line2[x] < back) 
            {
                back = line2[x];
                top2 = 0x04;
            }
            if(top2 & (BLDMOD>>8))
                color = gfxAlphaBlend(color, back,
                coeff[COLEV & 0x1F],
                coeff[(COLEV >> 8) & 0x1F]);
            else 
            {
                switch((BLDMOD >> 6) & 3) 
                {
                    case 2:
                        if(BLDMOD & top)
                            color = gfxIncreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                    case 3:
                        if(BLDMOD & top)
                            color = gfxDecreaseBrightness(color, coeff[COLY & 0x1F]);
                        break;
                }
            }
        }
        lineMix[x] = color;
    }
    gfxBG2Changed = 0;
    gfxLastVCOUNT = VCOUNT;  
}
void CGbaEmu::flashSaveDecide(u32 address, u8 byte)
{
    if(address == 0x0e005555) 
    {
        saveType = 2;
        cpuSaveGameFunc = &CGbaEmu::flashWrite;
    }
    else 
    {
        saveType = 1;
        cpuSaveGameFunc = &CGbaEmu::sramWrite;
    }
    
    (this->*cpuSaveGameFunc)(address, byte);
}

void CGbaEmu::flashWrite(u32 address, u8 byte)
{
    address &= 0xFFFF;
    switch(flashState) 
    {
        case FLASH_READ_ARRAY:
            if(address == 0x5555 && byte == 0xAA)
                flashState = FLASH_CMD_1;
            break;
        case FLASH_CMD_1:
            if(address == 0x2AAA && byte == 0x55)
                flashState = FLASH_CMD_2;
            else
                flashState = FLASH_READ_ARRAY;
            break;
        case FLASH_CMD_2:
            if(address == 0x5555) 
            {
                if(byte == 0x90) 
                {
                    flashState = FLASH_AUTOSELECT;
                    flashReadState = FLASH_AUTOSELECT;
                }
                else if(byte == 0x80) 
                {
                    flashState = FLASH_CMD_3;
                }
                else if(byte == 0xF0) 
                {
                    flashState = FLASH_READ_ARRAY;
                    flashReadState = FLASH_READ_ARRAY;
                }
                else if(byte == 0xA0) 
                {
                    flashState = FLASH_PROGRAM;
                }
                else if(byte == 0xB0 && flashSize == 0x20000) 
                {
                    flashState = FLASH_SETBANK;
                }
                else 
                {
                    flashState = FLASH_READ_ARRAY;
                    flashReadState = FLASH_READ_ARRAY;
                }
            }
            else 
            {
                flashState = FLASH_READ_ARRAY;
                flashReadState = FLASH_READ_ARRAY;
            }
            break;
        case FLASH_CMD_3:
            if(address == 0x5555 && byte == 0xAA) 
            {
                flashState = FLASH_CMD_4;
            }
            else 
            {
                flashState = FLASH_READ_ARRAY;
                flashReadState = FLASH_READ_ARRAY;
            }
            break;
        case FLASH_CMD_4:
            if(address == 0x2AAA && byte == 0x55) 
            {
                flashState = FLASH_CMD_5;
            }
            else 
            {
                flashState = FLASH_READ_ARRAY;
                flashReadState = FLASH_READ_ARRAY;
            }
            break;
        case FLASH_CMD_5:
            if(byte == 0x30) 
            {
                // SECTOR ERASE
                memset(&flashSaveMemory[(flashBank << 16) + (address & 0xF000)],
                0,
                0x1000);
                systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
                flashReadState = FLASH_ERASE_COMPLETE;
            }
            else if(byte == 0x10) 
            {
                // CHIP ERASE
                memset(flashSaveMemory, 0, flashSize);
                systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
                flashReadState = FLASH_ERASE_COMPLETE;
            }
            else 
            {
                flashState = FLASH_READ_ARRAY;
                flashReadState = FLASH_READ_ARRAY;
            }
            break;
        case FLASH_AUTOSELECT:
            if(byte == 0xF0) 
            {
                flashState = FLASH_READ_ARRAY;
                flashReadState = FLASH_READ_ARRAY;
            }
            else if(address == 0x5555 && byte == 0xAA)
                flashState = FLASH_CMD_1;
            else 
            {
                flashState = FLASH_READ_ARRAY;
                flashReadState = FLASH_READ_ARRAY;
            }
            break;
        case FLASH_PROGRAM:
            flashSaveMemory[(flashBank<<16)+address] = byte;
            systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
            flashState = FLASH_READ_ARRAY;
            flashReadState = FLASH_READ_ARRAY;
            break;
        case FLASH_SETBANK:
            if(address == 0) 
            {
                flashBank = (byte & 1);
            }
            flashState = FLASH_READ_ARRAY;
            flashReadState = FLASH_READ_ARRAY;
            break;
    }
}

u8 CGbaEmu::sramRead(u32 address)
{
  return flashSaveMemory[address & 0xFFFF];
}

void CGbaEmu::sramWrite(u32 address, u8 byte)
{
  flashSaveMemory[address & 0xFFFF] = byte;
  systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
}
void CGbaEmu::CPUUpdateCPSR()
{
    u32 CPSR = reg[16].I & 0x40;
    if(N_FLAG)
        CPSR |= 0x80000000;
    if(Z_FLAG)
        CPSR |= 0x40000000;
    if(C_FLAG)
        CPSR |= 0x20000000;
    if(V_FLAG)
        CPSR |= 0x10000000;
    if(!armState)
        CPSR |= 0x00000020;
    if(!armIrqEnable)
        CPSR |= 0x80;
    CPSR |= (armMode & 0x1F);
    reg[16].I = CPSR;
}
void CGbaEmu::CPUUpdateFlags(bool breakLoop)
{
    u32 CPSR = reg[16].I;
    N_FLAG = (CPSR & 0x80000000) ? true: false;
    Z_FLAG = (CPSR & 0x40000000) ? true: false;
    C_FLAG = (CPSR & 0x20000000) ? true: false;
    V_FLAG = (CPSR & 0x10000000) ? true: false;
    armState = (CPSR & 0x20) ? false : true;
    armIrqEnable = (CPSR & 0x80) ? false : true;
    if(breakLoop) 
    {
        if(armIrqEnable && (IF & IE) && (IME & 1)) 
        {
            cpuSavedTicks = cpuSavedTicks - *extCpuLoopTicks;
            *extCpuLoopTicks = *extClockTicks;
            *extTicks = *extClockTicks;
        }
    }
}
void CGbaEmu::CPUUpdateFlags()
{
    CPUUpdateFlags(true);
}

inline void CGbaEmu::CPUSwap(u32 *a, u32 *b)
{
    u32 c = *b;
    *b = *a;
    *a = c;
}

void CGbaEmu::CPUSwitchMode(int mode, bool saveState, bool breakLoop)
{
    CPUUpdateCPSR();
    switch(armMode) 
    {
        case 0x10:
            case 0x1F:
            reg[R13_USR].I = reg[13].I;
            reg[R14_USR].I = reg[14].I;
            reg[17].I = reg[16].I;
            break;
        case 0x11:
            CPUSwap(&reg[R8_FIQ].I, &reg[8].I);
            CPUSwap(&reg[R9_FIQ].I, &reg[9].I);
            CPUSwap(&reg[R10_FIQ].I, &reg[10].I);
            CPUSwap(&reg[R11_FIQ].I, &reg[11].I);
            CPUSwap(&reg[R12_FIQ].I, &reg[12].I);
            reg[R13_FIQ].I = reg[13].I;
            reg[R14_FIQ].I = reg[14].I;
            reg[SPSR_FIQ].I = reg[17].I;
            break;
        case 0x12:
            reg[R13_IRQ].I  = reg[13].I;
            reg[R14_IRQ].I  = reg[14].I;
            reg[SPSR_IRQ].I =  reg[17].I;
            break;
        case 0x13:
            reg[R13_SVC].I  = reg[13].I;
            reg[R14_SVC].I  = reg[14].I;
            reg[SPSR_SVC].I =  reg[17].I;
            break;
        case 0x17:
            reg[R13_ABT].I  = reg[13].I;
            reg[R14_ABT].I  = reg[14].I;
            reg[SPSR_ABT].I =  reg[17].I;
            break;
        case 0x1b:
            reg[R13_UND].I  = reg[13].I;
            reg[R14_UND].I  = reg[14].I;
            reg[SPSR_UND].I =  reg[17].I;
            break;
    }
    u32 CPSR = reg[16].I;
    u32 SPSR = reg[17].I;
    switch(mode) 
    {
        case 0x10:
            case 0x1F:
            reg[13].I = reg[R13_USR].I;
            reg[14].I = reg[R14_USR].I;
            reg[16].I = SPSR;
            break;
        case 0x11:
            CPUSwap(&reg[8].I, &reg[R8_FIQ].I);
            CPUSwap(&reg[9].I, &reg[R9_FIQ].I);
            CPUSwap(&reg[10].I, &reg[R10_FIQ].I);
            CPUSwap(&reg[11].I, &reg[R11_FIQ].I);
            CPUSwap(&reg[12].I, &reg[R12_FIQ].I);
            reg[13].I = reg[R13_FIQ].I;
            reg[14].I = reg[R14_FIQ].I;
            if(saveState)
                reg[17].I = CPSR;
            else
                reg[17].I = reg[SPSR_FIQ].I;
            break;
        case 0x12:
            reg[13].I = reg[R13_IRQ].I;
            reg[14].I = reg[R14_IRQ].I;
            reg[16].I = SPSR;
            if(saveState)
                reg[17].I = CPSR;
            else
                reg[17].I = reg[SPSR_IRQ].I;
            break;
        case 0x13:
            reg[13].I = reg[R13_SVC].I;
            reg[14].I = reg[R14_SVC].I;
            reg[16].I = SPSR;
            if(saveState)
                reg[17].I = CPSR;
            else
                reg[17].I = reg[SPSR_SVC].I;
            break;
        case 0x17:
            reg[13].I = reg[R13_ABT].I;
            reg[14].I = reg[R14_ABT].I;
            reg[16].I = SPSR;
            if(saveState)
                reg[17].I = CPSR;
            else
                reg[17].I = reg[SPSR_ABT].I;
            break;    
        case 0x1b:
            reg[13].I = reg[R13_UND].I;
            reg[14].I = reg[R14_UND].I;
            reg[16].I = SPSR;
            if(saveState)
                reg[17].I = CPSR;
            else
                reg[17].I = reg[SPSR_UND].I;
            break;    
        default:
            LOG("Unsupported ARM mode %02x\n", mode);
            break;
    }
    armMode = mode;
    CPUUpdateFlags(breakLoop);
    CPUUpdateCPSR();
}
void CGbaEmu::CPUSwitchMode(int mode, bool saveState)
{
    CPUSwitchMode(mode, saveState, true);
}
void CGbaEmu::CPUUndefinedException()
{
    u32 PC = reg[15].I;
    bool savedArmState = armState;
    CPUSwitchMode(0x1b, true, false);
    reg[14].I = PC - (savedArmState ? 4 : 2);
    reg[15].I = 0x04;
    armState = true;
    armIrqEnable = false;
    armNextPC = 0x04;
    reg[15].I += 4;  
}
void CGbaEmu::CPUSoftwareInterrupt()
{
    u32 PC = reg[15].I;
    bool savedArmState = armState;
    CPUSwitchMode(0x13, true, false);
    reg[14].I = PC - (savedArmState ? 4 : 2);
    reg[15].I = 0x08;
    armState = true;
    armIrqEnable = false;
    armNextPC = 0x08;
    reg[15].I += 4;
}

inline u32 CGbaEmu::CPUReadMemory(u32 address)
{
    u32 value;
    switch(address >> 24) 
    {
    case 0:
        if(reg[15].I >> 24) 
        {
            if(address < 0x4000) 
            {
                value = READ32LE(((u32 *)&biosProtected));
            }
            else goto unreadable;
        }
        else
            value = READ32LE(((u32 *)&bios[address & 0x3FFC]));
        break;
    case 2:
        value = READ32LE(((u32 *)&workRAM[address & 0x3FFFC]));
        break;
    case 3:
        value = READ32LE(((u32 *)&internalRAM[address & 0x7ffC]));
        break;
    case 4:
        if((address < 0x4000400) && ioReadable[address & 0x3fc]) 
        {
            if(ioReadable[(address & 0x3fc) + 2])
                value = READ32LE(((u32 *)&ioMem[address & 0x3fC]));
            else
                value = READ16LE(((u16 *)&ioMem[address & 0x3fc]));
        }
        else goto unreadable;
        break;
    case 5:
        value = READ32LE(((u32 *)&paletteRAM[address & 0x3fC]));
        break;
    case 6:
        value = READ32LE(((u32 *)&vram[address & 0x1fffc]));
        break;
    case 7:
        value = READ32LE(((u32 *)&oam[address & 0x3FC]));
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        value = READ32LE(((u32 *)&rom[address&0x1FFFFFC]));
        break;    
    case 13:
        if(cpuEEPROMEnabled)
            return eepromRead(address);
        goto unreadable;
    case 14:
        if(cpuFlashEnabled | cpuSramEnabled)
            return flashRead(address);
    default:
unreadable:
        if(armState) 
        {
            value = CPUReadMemoryQuick(reg[15].I);
        }
        else 
        {
            value = CPUReadHalfWordQuick(reg[15].I) |
                CPUReadHalfWordQuick(reg[15].I) << 16;
        }
    }
    if(address & 3) 
    {
        int shift = (address & 3) << 3;
        value = (value >> shift) | (value << (32 - shift));
    }
    return value;
}

inline u32 CGbaEmu::CPUReadHalfWord(u32 address)
{
    u32 value;
    switch(address >> 24) 
    {
    case 0:
        if (reg[15].I >> 24) 
        {
            if(address < 0x4000) 
            {
                value = READ16LE(((u16 *)&biosProtected[address&2]));
            }
            else goto unreadable;
        }
        else
            value = READ16LE(((u16 *)&bios[address & 0x3FFE]));
        break;
    case 2:
        value = READ16LE(((u16 *)&workRAM[address & 0x3FFFE]));
        break;
    case 3:
        value = READ16LE(((u16 *)&internalRAM[address & 0x7ffe]));
        break;
    case 4:
        if((address < 0x4000400) && ioReadable[address & 0x3fe])
            value =  READ16LE(((u16 *)&ioMem[address & 0x3fe]));
        else goto unreadable;
        break;
    case 5:
        value = READ16LE(((u16 *)&paletteRAM[address & 0x3fe]));
        break;
    case 6:
        value = READ16LE(((u16 *)&vram[address & 0x1fffe]));
        break;
    case 7:
        value = READ16LE(((u16 *)&oam[address & 0x3fe]));
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8)
            value = rtcRead(address);
        else
            value = READ16LE(((u16 *)&rom[address & 0x1FFFFFE]));
        break;    
    case 13:
        if(cpuEEPROMEnabled)
            // no need to swap this
            return  eepromRead(address);
        goto unreadable;
    case 14:
        if(cpuFlashEnabled | cpuSramEnabled)
            // no need to swap this
            return flashRead(address);
        // default
    default:
unreadable:
        if(cpuDmaHack && cpuDmaCount) 
        {
            value = (u16)cpuDmaLast;
        }
        else 
        {
            if(armState) 
            {
                value = CPUReadHalfWordQuick(reg[15].I + (address & 2));
            }
            else 
            {
                value = CPUReadHalfWordQuick(reg[15].I);
            }
        }
        break;
    }
    if(address & 1) 
    {
        value = (value >> 8) | (value << 24);
    }
    return value;
}
u16 CGbaEmu::CPUReadHalfWordSigned(u32 address)
{
    u16 value = CPUReadHalfWord(address);
    if((address & 1))
        value = (s8)value;
    return value;
}
u8 CGbaEmu::CPUReadByte(u32 address)
{
    switch(address >> 24) 
    {
    case 0:
        if (reg[15].I >> 24) 
        {
            if(address < 0x4000) 
            {
                return biosProtected[address & 3];
            }
            else goto unreadable;
        }
        return bios[address & 0x3FFF];
    case 2:
        return workRAM[address & 0x3FFFF];
    case 3:
        return internalRAM[address & 0x7fff];
    case 4:
        if((address < 0x4000400) && ioReadable[address & 0x3ff])
            return ioMem[address & 0x3ff];
        else goto unreadable;
    case 5:
        return paletteRAM[address & 0x3ff];
    case 6:
        return vram[address & 0x1ffff];
    case 7:
        return oam[address & 0x3ff];
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        return rom[address & 0x1FFFFFF];        
    case 13:
        if(cpuEEPROMEnabled)
            return eepromRead(address);
        goto unreadable;
    case 14:
        if(cpuSramEnabled | cpuFlashEnabled)
            return flashRead(address);
        if(cpuEEPROMSensorEnabled) 
        {
            switch(address & 0x00008f00) 
            {
            case 0x8200:
                return systemGetSensorX() & 255;
            case 0x8300:
                return (systemGetSensorX() >> 8)|0x80;
            case 0x8400:
                return systemGetSensorY() & 255;
            case 0x8500:
                return systemGetSensorY() >> 8;
            }
        }
        // default
    default:
unreadable:
        if(armState) 
        {
            return CPUReadByteQuick(reg[15].I+(address & 3));
        }
        else 
        {
            return CPUReadByteQuick(reg[15].I+(address & 1));
        }
        break;
    }
}
void CGbaEmu::CPUWriteMemory(u32 address, u32 value)
{
    switch(address >> 24) 
    {
    case 0x02:
        WRITE32LE(((u32 *)&workRAM[address & 0x3FFFC]), value);
        break;
    case 0x03:
        WRITE32LE(((u32 *)&internalRAM[address & 0x7ffC]), value);
        break;
    case 0x04:
        CPUUpdateRegister((address & 0x3FC), value & 0xFFFF);
        CPUUpdateRegister((address & 0x3FC) + 2, (value >> 16));
        break;
    case 0x05:
        WRITE32LE(((u32 *)&paletteRAM[address & 0x3FC]), value);
        break;
    case 0x06:
        if(address & 0x10000)
            WRITE32LE(((u32 *)&vram[address & 0x17ffc]), value);
        else
            WRITE32LE(((u32 *)&vram[address & 0x1fffc]), value);
        break;
    case 0x07:
        WRITE32LE(((u32 *)&oam[address & 0x3fc]), value);
        break;
#if _GBA_VM
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        WRITE32LE(((u32 *)&rom[address & 0x1ffffff]), value);
        break;
#endif
    case 0x0D:
        if(cpuEEPROMEnabled) 
        {
            eepromWrite(address, value);
            break;
        }
        goto unwritable;
    case 0x0E:
        if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) 
        {
            (this->*cpuSaveGameFunc)(address, (u8)value);
            break;
        }
        // default
    default:
unwritable:
        break;
    }
}

int CGbaEmu::eepromRead(u32 /* address */)
{
    switch(eepromMode) 
    {
    case EEPROM_IDLE:
    case EEPROM_READADDRESS:
    case EEPROM_WRITEDATA:
        return 1;
    case EEPROM_READDATA:
        {
            eepromBits++;
            if(eepromBits == 4) 
            {
                eepromMode = EEPROM_READDATA2;
                eepromBits = 0;
                eepromByte = 0;
            }
            return 0;
        }
    case EEPROM_READDATA2:
        {
            int data = 0;
            int address = eepromAddress << 3;
            int mask = 1 << (7 - (eepromBits & 7));
            data = (eepromData[address+eepromByte] & mask) ? 1 : 0;
            eepromBits++;
            if((eepromBits & 7) == 0)
                eepromByte++;
            if(eepromBits == 0x40)
                eepromMode = EEPROM_IDLE;
            return data;
        }
    default:
        return 0;
    }
    return 1;
}

void CGbaEmu::eepromWrite(u32 /* address */, u8 value)
{
    if(cpuDmaCount == 0)
        return;
    int bit = value & 1;
    switch(eepromMode) 
    {
    case EEPROM_IDLE:
        eepromByte = 0;
        eepromBits = 1;
        eepromBuffer[eepromByte] = bit;
        eepromMode = EEPROM_READADDRESS;
        break;
    case EEPROM_READADDRESS:
        eepromBuffer[eepromByte] <<= 1;
        eepromBuffer[eepromByte] |= bit;
        eepromBits++;
        if((eepromBits & 7) == 0) 
        {
            eepromByte++;
        }
        if(cpuDmaCount == 0x11 || cpuDmaCount == 0x51) 
        {
            if(eepromBits == 0x11) 
            {
                eepromInUse = true;
                eepromSize = 0x2000;
                eepromAddress = ((eepromBuffer[0] & 0x3F) << 8) | ((eepromBuffer[1] & 0xFF));
                if(!(eepromBuffer[0] & 0x40)) 
                {
                    eepromBuffer[0] = bit;          
                    eepromBits = 1;
                    eepromByte = 0;
                    eepromMode = EEPROM_WRITEDATA;
                }
                else 
                {
                    eepromMode = EEPROM_READDATA;
                    eepromByte = 0;
                    eepromBits = 0;
                }
            }
        }
        else 
        {
            if(eepromBits == 9) 
            {
                eepromInUse = true;
                eepromAddress = (eepromBuffer[0] & 0x3F);
                if(!(eepromBuffer[0] & 0x40)) 
                {
                    eepromBuffer[0] = bit;
                    eepromBits = 1;
                    eepromByte = 0;         
                    eepromMode = EEPROM_WRITEDATA;
                }
                else 
                {
                    eepromMode = EEPROM_READDATA;
                    eepromByte = 0;
                    eepromBits = 0;
                }
            }
        }
        break;
    case EEPROM_READDATA:
        case EEPROM_READDATA2:
        // should we reset here?
        eepromMode = EEPROM_IDLE;
        break;
    case EEPROM_WRITEDATA:
        eepromBuffer[eepromByte] <<= 1;
        eepromBuffer[eepromByte] |= bit;
        eepromBits++;
        if((eepromBits & 7) == 0) 
        {
            eepromByte++;
        }
        if(eepromBits == 0x40) 
        {
            eepromInUse = true;
            // write data;
            for(int i = 0; i < 8; i++) 
            {
                eepromData[(eepromAddress << 3) + i] = eepromBuffer[i];
            }
            systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
        }
        else if(eepromBits == 0x41) 
        {
            eepromMode = EEPROM_IDLE;
            eepromByte = 0;
            eepromBits = 0;
        }
        break;
    }
}

u8 CGbaEmu::flashRead(u32 address)
{
    address &= 0xFFFF;
    switch(flashReadState) 
    {
    case FLASH_READ_ARRAY:
        return flashSaveMemory[(flashBank << 16) + address];
    case FLASH_AUTOSELECT:
        switch(address & 0xFF) 
        {
        case 0:
            return flashManufacturerID;
        case 1:
            return flashDeviceID;
        }
        break;
    case FLASH_ERASE_COMPLETE:
        flashState = FLASH_READ_ARRAY;
        flashReadState = FLASH_READ_ARRAY;
        return 0xFF;
    }

    return 0;
}
u16 CGbaEmu::rtcRead(u32 address)
{
    if(rtcEnabled) 
    {
        if(address == 0x80000c8)
            return rtcClockData->byte2;
        else if(address == 0x80000c6)
            return rtcClockData->byte1;
        else if(address == 0x80000c4) 
        {
            return rtcClockData->byte0;
        }
    }
    return READ16LE((u16*)(&rom[address & 0x1FFFFFE]));
}
int CGbaEmu::systemGetSensorX()
{
    return 0;
}
int CGbaEmu::systemGetSensorY()
{
    return 0;
}
void CGbaEmu::CPUUpdateRegister(u32 address, u16 value)
{
    switch(address) 
    {
        case 0x00:
            {
                bool change = ((DISPCNT ^ value) & 0x80) ? true : false;
                bool changeBG = ((DISPCNT ^ value) & 0x0F00) ? true : false;
                DISPCNT = (value & 0xFFF7);
                UPDATE_REG(0x00, DISPCNT);
                layerEnable = layerSettings & value;
                windowOn = (layerEnable & 0x6000) ? true : false;
                if(change && !((value & 0x80))) 
                {
                    if(!(DISPSTAT & 1)) 
                    {
                        lcdTicks = 960;                     
                        DISPSTAT &= 0xFFFC;
                        UPDATE_REG(0x04, DISPSTAT);
                        CPUCompareVCOUNT();
                    }
                    
                }
                CPUUpdateRender();
                
                if(changeBG)
                    CPUUpdateRenderBuffers(false);
                
            }
            break;
        case 0x04:
            DISPSTAT = (value & 0xFF38) | (DISPSTAT & 7);
            UPDATE_REG(0x04, DISPSTAT);
            break;
        case 0x06:
            
            break;
        case 0x08:
            BG0CNT = (value & 0xDFCF);
            UPDATE_REG(0x08, BG0CNT);
            break;
        case 0x0A:
            BG1CNT = (value & 0xDFCF);
            UPDATE_REG(0x0A, BG1CNT);
            break;
        case 0x0C:
            BG2CNT = (value & 0xFFCF);
            UPDATE_REG(0x0C, BG2CNT);
            break;
        case 0x0E:
            BG3CNT = (value & 0xFFCF);
            UPDATE_REG(0x0E, BG3CNT);
            break;
        case 0x10:
            BG0HOFS = value & 511;
            UPDATE_REG(0x10, BG0HOFS);
            break;
        case 0x12:
            BG0VOFS = value & 511;
            UPDATE_REG(0x12, BG0VOFS);
            break;
        case 0x14:
            BG1HOFS = value & 511;
            UPDATE_REG(0x14, BG1HOFS);
            break;
        case 0x16:
            BG1VOFS = value & 511;
            UPDATE_REG(0x16, BG1VOFS);
            break;      
        case 0x18:
            BG2HOFS = value & 511;
            UPDATE_REG(0x18, BG2HOFS);
            break;
        case 0x1A:
            BG2VOFS = value & 511;
            UPDATE_REG(0x1A, BG2VOFS);
            break;
        case 0x1C:
            BG3HOFS = value & 511;
            UPDATE_REG(0x1C, BG3HOFS);
            break;
        case 0x1E:
            BG3VOFS = value & 511;
            UPDATE_REG(0x1E, BG3VOFS);
            break;      
        case 0x20:
            BG2PA = value;
            UPDATE_REG(0x20, BG2PA);
            break;
        case 0x22:
            BG2PB = value;
            UPDATE_REG(0x22, BG2PB);
            break;
        case 0x24:
            BG2PC = value;
            UPDATE_REG(0x24, BG2PC);
            break;
        case 0x26:
            BG2PD = value;
            UPDATE_REG(0x26, BG2PD);
            break;
        case 0x28:
            BG2X_L = value;
            UPDATE_REG(0x28, BG2X_L);
            gfxBG2Changed |= 1;
            break;
        case 0x2A:
            BG2X_H = (value & 0xFFF);
            UPDATE_REG(0x2A, BG2X_H);
            gfxBG2Changed |= 1;    
            break;
        case 0x2C:
            BG2Y_L = value;
            UPDATE_REG(0x2C, BG2Y_L);
            gfxBG2Changed |= 2;    
            break;
        case 0x2E:
            BG2Y_H = value & 0xFFF;
            UPDATE_REG(0x2E, BG2Y_H);
            gfxBG2Changed |= 2;    
            break;
        case 0x30:
            BG3PA = value;
            UPDATE_REG(0x30, BG3PA);
            break;
        case 0x32:
            BG3PB = value;
            UPDATE_REG(0x32, BG3PB);
            break;
        case 0x34:
            BG3PC = value;
            UPDATE_REG(0x34, BG3PC);
            break;
        case 0x36:
            BG3PD = value;
            UPDATE_REG(0x36, BG3PD);
            break;
        case 0x38:
            BG3X_L = value;
            UPDATE_REG(0x38, BG3X_L);
            gfxBG3Changed |= 1;
            break;
        case 0x3A:
            BG3X_H = value & 0xFFF;
            UPDATE_REG(0x3A, BG3X_H);
            gfxBG3Changed |= 1;    
            break;
        case 0x3C:
            BG3Y_L = value;
            UPDATE_REG(0x3C, BG3Y_L);
            gfxBG3Changed |= 2;    
            break;
        case 0x3E:
            BG3Y_H = value & 0xFFF;
            UPDATE_REG(0x3E, BG3Y_H);
            gfxBG3Changed |= 2;    
            break;
        case 0x40:
            WIN0H = value;
            UPDATE_REG(0x40, WIN0H);
            CPUUpdateWindow0();
            break;
        case 0x42:
            WIN1H = value;
            UPDATE_REG(0x42, WIN1H);
            CPUUpdateWindow1();    
            break;      
        case 0x44:
            WIN0V = value;
            UPDATE_REG(0x44, WIN0V);
            break;
        case 0x46:
            WIN1V = value;
            UPDATE_REG(0x46, WIN1V);
            break;
        case 0x48:
            WININ = value & 0x3F3F;
            UPDATE_REG(0x48, WININ);
            break;
        case 0x4A:
            WINOUT = value & 0x3F3F;
            UPDATE_REG(0x4A, WINOUT);
            break;
        case 0x4C:
            MOSAIC = value;
            UPDATE_REG(0x4C, MOSAIC);
            break;
        case 0x50:
            BLDMOD = value & 0x3FFF;
            UPDATE_REG(0x50, BLDMOD);
            fxOn = ((BLDMOD>>6)&3) != 0;
            CPUUpdateRender();
            break;
        case 0x52:
            COLEV = value & 0x1F1F;
            UPDATE_REG(0x52, COLEV);
            break;
        case 0x54:
            COLY = value & 0x1F;
            UPDATE_REG(0x54, COLY);
            break;
        case 0x60:
            case 0x62:
            case 0x64:
            case 0x68:
            case 0x6c:
            case 0x70:
            case 0x72:
            case 0x74:
            case 0x78:
            case 0x7c:
            case 0x80:
            case 0x84:
            soundEvent(address&0xFF, (u8)(value & 0xFF));
            soundEvent((address&0xFF)+1, (u8)(value>>8));
            break;
        case 0x82:
            case 0x88:
            case 0xa0:
            case 0xa2:
            case 0xa4:
            case 0xa6:
            case 0x90:
            case 0x92:
            case 0x94:
            case 0x96:
            case 0x98:
            case 0x9a:
            case 0x9c:
            case 0x9e:    
            soundEvent(address&0xFF, (u8)value);
            break;
        case 0xB0:
            DM0SAD_L = value;
            UPDATE_REG(0xB0, DM0SAD_L);
            break;
        case 0xB2:
            DM0SAD_H = value & 0x07FF;
            UPDATE_REG(0xB2, DM0SAD_H);
            break;
        case 0xB4:
            DM0DAD_L = value;
            UPDATE_REG(0xB4, DM0DAD_L);
            break;
        case 0xB6:
            DM0DAD_H = value & 0x07FF;
            UPDATE_REG(0xB6, DM0DAD_H);
            break;
        case 0xB8:
            DM0CNT_L = value & 0x3FFF;
            UPDATE_REG(0xB8, 0);
            break;
        case 0xBA:
            {
                bool start = ((DM0CNT_H ^ value) & 0x8000) ? true : false;
                value &= 0xF7E0;
                DM0CNT_H = value;
                UPDATE_REG(0xBA, DM0CNT_H);    
                if(start && (value & 0x8000)) 
                {
                    dma0Source = DM0SAD_L | (DM0SAD_H << 16);
                    dma0Dest = DM0DAD_L | (DM0DAD_H << 16);
                    CPUCheckDMA(0, 1);
                }
            }
            break;      
        case 0xBC:
            DM1SAD_L = value;
            UPDATE_REG(0xBC, DM1SAD_L);
            break;
        case 0xBE:
            DM1SAD_H = value & 0x0FFF;
            UPDATE_REG(0xBE, DM1SAD_H);
            break;
        case 0xC0:
            DM1DAD_L = value;
            UPDATE_REG(0xC0, DM1DAD_L);
            break;
        case 0xC2:
            DM1DAD_H = value & 0x07FF;
            UPDATE_REG(0xC2, DM1DAD_H);
            break;
        case 0xC4:
            DM1CNT_L = value & 0x3FFF;
            UPDATE_REG(0xC4, 0);
            break;
        case 0xC6:
            {
                bool start = ((DM1CNT_H ^ value) & 0x8000) ? true : false;
                value &= 0xF7E0;
                DM1CNT_H = value;
                UPDATE_REG(0xC6, DM1CNT_H);
                if(start && (value & 0x8000)) 
                {
                    dma1Source = DM1SAD_L | (DM1SAD_H << 16);
                    dma1Dest = DM1DAD_L | (DM1DAD_H << 16);
                    CPUCheckDMA(0, 2);
                }
            }
            break;
        case 0xC8:
            DM2SAD_L = value;
            UPDATE_REG(0xC8, DM2SAD_L);
            break;
        case 0xCA:
            DM2SAD_H = value & 0x0FFF;
            UPDATE_REG(0xCA, DM2SAD_H);
            break;
        case 0xCC:
            DM2DAD_L = value;
            UPDATE_REG(0xCC, DM2DAD_L);
            break;
        case 0xCE:
            DM2DAD_H = value & 0x07FF;
            UPDATE_REG(0xCE, DM2DAD_H);
            break;
        case 0xD0:
            DM2CNT_L = value & 0x3FFF;
            UPDATE_REG(0xD0, 0);
            break;
        case 0xD2:
            {
                bool start = ((DM2CNT_H ^ value) & 0x8000) ? true : false;
                value &= 0xF7E0;
                DM2CNT_H = value;
                UPDATE_REG(0xD2, DM2CNT_H);
                if(start && (value & 0x8000)) 
                {
                    dma2Source = DM2SAD_L | (DM2SAD_H << 16);
                    dma2Dest = DM2DAD_L | (DM2DAD_H << 16);
                    CPUCheckDMA(0, 4);
                }
            }
            break;
        case 0xD4:
            DM3SAD_L = value;
            UPDATE_REG(0xD4, DM3SAD_L);
            break;
        case 0xD6:
            DM3SAD_H = value & 0x0FFF;
            UPDATE_REG(0xD6, DM3SAD_H);
            break;
        case 0xD8:
            DM3DAD_L = value;
            UPDATE_REG(0xD8, DM3DAD_L);
            break;
        case 0xDA:
            DM3DAD_H = value & 0x0FFF;
            UPDATE_REG(0xDA, DM3DAD_H);
            break;
        case 0xDC:
            DM3CNT_L = value;
            UPDATE_REG(0xDC, 0);
            break;
        case 0xDE:
            {
                bool start = ((DM3CNT_H ^ value) & 0x8000) ? true : false;
                value &= 0xFFE0;
                DM3CNT_H = value;
                UPDATE_REG(0xDE, DM3CNT_H);
                if(start && (value & 0x8000)) 
                {
                    dma3Source = DM3SAD_L | (DM3SAD_H << 16);
                    dma3Dest = DM3DAD_L | (DM3DAD_H << 16);
                    CPUCheckDMA(0,8);
                }
            }
            break;
        case 0x100:
            timer0Reload = value;
            break;
        case 0x102:
            timer0Ticks = timer0ClockReload = TIMER_TICKS[value & 3];        
            if(!timer0On && (value & 0x80)) 
            {
                
                TM0D = timer0Reload;      
                if(timer0ClockReload == 1)
                    timer0Ticks = 0x10000 - TM0D;
                UPDATE_REG(0x100, TM0D);
            }
            timer0On = value & 0x80 ? true : false;
            TM0CNT = value & 0xC7;
            UPDATE_REG(0x102, TM0CNT);
            
            break;
        case 0x104:
            timer1Reload = value;
            break;
        case 0x106:
            timer1Ticks = timer1ClockReload = TIMER_TICKS[value & 3];        
            if(!timer1On && (value & 0x80)) 
            {
                
                TM1D = timer1Reload;      
                if(timer1ClockReload == 1)
                    timer1Ticks = 0x10000 - TM1D;
                UPDATE_REG(0x104, TM1D);
            }
            timer1On = value & 0x80 ? true : false;
            TM1CNT = value & 0xC7;
            UPDATE_REG(0x106, TM1CNT);
            break;
        case 0x108:
            timer2Reload = value;
            break;
        case 0x10A:
            timer2Ticks = timer2ClockReload = TIMER_TICKS[value & 3];        
            if(!timer2On && (value & 0x80)) 
            {
                
                TM2D = timer2Reload;      
                if(timer2ClockReload == 1)
                    timer2Ticks = 0x10000 - TM2D;
                UPDATE_REG(0x108, TM2D);
            }
            timer2On = value & 0x80 ? true : false;
            TM2CNT = value & 0xC7;
            UPDATE_REG(0x10A, TM2CNT);
            break;
        case 0x10C:
            timer3Reload = value;
            break;
        case 0x10E:
            timer3Ticks = timer3ClockReload = TIMER_TICKS[value & 3];        
            if(!timer3On && (value & 0x80)) 
            {
                
                TM3D = timer3Reload;      
                if(timer3ClockReload == 1)
                    timer3Ticks = 0x10000 - TM3D;
                UPDATE_REG(0x10C, TM3D);
            }
            timer3On = value & 0x80 ? true : false;
            TM3CNT = value & 0xC7;
            UPDATE_REG(0x10E, TM3CNT);
            break;
        case 0x128:
            if(value & 0x80) 
            {
                value &= 0xff7f;
                if(value & 1 && (value & 0x4000)) 
                {
                    UPDATE_REG(0x12a, 0xFF);
                    IF |= 0x80;
                    UPDATE_REG(0x202, IF);
                    value &= 0x7f7f;
                }
            }
            UPDATE_REG(0x128, value);
            break;
        case 0x130:
            P1 |= (value & 0x3FF);
            UPDATE_REG(0x130, P1);
            break;
        case 0x132:
            UPDATE_REG(0x132, value & 0xC3FF);
            break;
        case 0x200:
            IE = value & 0x3FFF;
            UPDATE_REG(0x200, IE);
            if((IME & 1) && (IF & IE) && armIrqEnable) 
            {
                CPU_BREAK_LOOP_2;
            }
            break;
        case 0x202:
            IF ^= (value & IF);
            UPDATE_REG(0x202, IF);
            break;
        case 0x204:
            {
                int i;
                memoryWait[0x0e] = memoryWaitSeq[0x0e] = gamepakRamWaitState[value & 3];
                if(!speedHack) 
                {
                    memoryWait[0x08] = memoryWait[0x09] = gamepakWaitState[(value >> 2) & 7];
                    memoryWaitSeq[0x08] = memoryWaitSeq[0x09] =
                    gamepakWaitState0[(value >> 2) & 7];
                    memoryWait[0x0a] = memoryWait[0x0b] = gamepakWaitState[(value >> 5) & 7];
                    memoryWaitSeq[0x0a] = memoryWaitSeq[0x0b] =
                    gamepakWaitState1[(value >> 5) & 7];
                    memoryWait[0x0c] = memoryWait[0x0d] = gamepakWaitState[(value >> 8) & 7];
                    memoryWaitSeq[0x0c] = memoryWaitSeq[0x0d] =
                    gamepakWaitState2[(value >> 8) & 7];
                }
                else 
                {
                    memoryWait[0x08] = memoryWait[0x09] = 4;
                    memoryWaitSeq[0x08] = memoryWaitSeq[0x09] = 2;
                    memoryWait[0x0a] = memoryWait[0x0b] = 4;
                    memoryWaitSeq[0x0a] = memoryWaitSeq[0x0b] = 4;
                    memoryWait[0x0c] = memoryWait[0x0d] = 4;
                    memoryWaitSeq[0x0c] = memoryWaitSeq[0x0d] = 8;
                }
                for(i = 0; i < 16; i++) 
                {
                    memoryWaitFetch32[i] = memoryWait32[i] = memoryWait[i] *
                    (memory32[i] ? 1 : 2);
                    memoryWaitFetch[i] = memoryWait[i];
                }
                memoryWaitFetch32[3] += 1;
                memoryWaitFetch32[2] += 3;
                if(value & 0x4000) 
                {
                    for(i = 8; i < 16; i++) 
                    {
                        memoryWaitFetch32[i] = 2*cpuMemoryWait[i];
                        memoryWaitFetch[i] = cpuMemoryWait[i];
                    }
                }
                UPDATE_REG(0x204, value);
            }
            break;
        case 0x208:
            IME = value & 1;
            UPDATE_REG(0x208, IME);
            if((IME & 1) && (IF & IE) && armIrqEnable) 
            {
                CPU_BREAK_LOOP_2;
            }
            break;
        case 0x300:
            if(value != 0)
                value &= 0xFFFE;
            UPDATE_REG(0x300, value);
            break;
        default:
        UPDATE_REG(address&0x3FE, value);
        break;
    }
}

void CGbaEmu::soundEvent(u32 address, u8 data)
{
    // no sound event
}

void CGbaEmu::CPUCompareVCOUNT()
{
    if(VCOUNT == (DISPSTAT >> 8)) 
    {
        DISPSTAT |= 4;
        UPDATE_REG(0x04, DISPSTAT);
        if(DISPSTAT & 0x20) 
        {
            IF |= 4;
            UPDATE_REG(0x202, IF);
        }
    }
    else 
    {
        DISPSTAT &= 0xFFFB;
        UPDATE_REG(0x4, DISPSTAT);
    }
}
void CGbaEmu::CPUUpdateRender()
{
    switch(DISPCNT & 7) 
    {
        case 0:
            if((!fxOn && !windowOn && !(layerEnable & 0x8000)) || cpuDisableSfx)
                renderLine = &CGbaEmu::mode0RenderLine;
            else if(fxOn && !windowOn && !(layerEnable & 0x8000))
                renderLine = &CGbaEmu::mode0RenderLineNoWindow;
            else 
                renderLine = &CGbaEmu::mode0RenderLineAll;
            break;
        case 1:
            if((!fxOn && !windowOn && !(layerEnable & 0x8000)) || cpuDisableSfx)
                renderLine = &CGbaEmu::mode1RenderLine;
            else if(fxOn && !windowOn && !(layerEnable & 0x8000))
                renderLine = &CGbaEmu::mode1RenderLineNoWindow;
            else
                renderLine = &CGbaEmu::mode1RenderLineAll;
            break;
        case 2:
            if((!fxOn && !windowOn && !(layerEnable & 0x8000)) || cpuDisableSfx)
                renderLine = &CGbaEmu::mode2RenderLine;
            else if(fxOn && !windowOn && !(layerEnable & 0x8000))
                renderLine = &CGbaEmu::mode2RenderLineNoWindow;
            else
                renderLine = &CGbaEmu::mode2RenderLineAll;
            break;
        case 3:
            if((!fxOn && !windowOn && !(layerEnable & 0x8000)) || cpuDisableSfx)
                renderLine = &CGbaEmu::mode3RenderLine;
            else if(fxOn && !windowOn && !(layerEnable & 0x8000))
                renderLine = &CGbaEmu::mode3RenderLineNoWindow;
            else
                renderLine = &CGbaEmu::mode3RenderLineAll;
            break;
        case 4:
            if((!fxOn && !windowOn && !(layerEnable & 0x8000)) || cpuDisableSfx)
                renderLine = &CGbaEmu::mode4RenderLine;
            else if(fxOn && !windowOn && !(layerEnable & 0x8000))
                renderLine = &CGbaEmu::mode4RenderLineNoWindow;
            else
                renderLine = &CGbaEmu::mode4RenderLineAll;
            break;
        case 5:
            if((!fxOn && !windowOn && !(layerEnable & 0x8000)) || cpuDisableSfx)
                renderLine = &CGbaEmu::mode5RenderLine;
            else if(fxOn && !windowOn && !(layerEnable & 0x8000))
                renderLine = &CGbaEmu::mode5RenderLineNoWindow;
            else
                renderLine = &CGbaEmu::mode5RenderLineAll;
            default:
            break;
    }
}
void CGbaEmu::CPUCheckDMA(int reason, int dmamask)
{
    cpuDmaHack = 0;
    // DMA 0
    if((DM0CNT_H & 0x8000) && (dmamask & 1)) 
    {
        if(((DM0CNT_H >> 12) & 3) == reason) 
        {
            u32 sourceIncrement = 4;
            u32 destIncrement = 4;
            switch((DM0CNT_H >> 7) & 3) 
            {
                case 0:
                    break;
                case 1:
                    sourceIncrement = (u32)-4;
                    break;
                case 2:
                    sourceIncrement = 0;
                    break;
            }
            switch((DM0CNT_H >> 5) & 3) 
            {
                case 0:
                    break;
                case 1:
                    destIncrement = (u32)-4;
                    break;
                case 2:
                    destIncrement = 0;
                    break;
            }
            doDMA(dma0Source, dma0Dest, sourceIncrement, destIncrement,
            DM0CNT_L ? DM0CNT_L : 0x4000,
            DM0CNT_H & 0x0400);
            cpuDmaHack = 1;
            if(DM0CNT_H & 0x4000) 
            {
                IF |= 0x0100;
                UPDATE_REG(0x202, IF);
            }
            if(((DM0CNT_H >> 5) & 3) == 3) 
            {
                dma0Dest = DM0DAD_L | (DM0DAD_H << 16);
            }
            if(!(DM0CNT_H & 0x0200) || (reason == 0)) 
            {
                DM0CNT_H &= 0x7FFF;
                UPDATE_REG(0xBA, DM0CNT_H);
            }
        }
    }
    // DMA 1
    if((DM1CNT_H & 0x8000) && (dmamask & 2)) 
    {
        if(((DM1CNT_H >> 12) & 3) == reason) 
        {
            u32 sourceIncrement = 4;
            u32 destIncrement = 4;
            switch((DM1CNT_H >> 7) & 3) 
            {
                case 0:
                    break;
                case 1:
                    sourceIncrement = (u32)-4;
                    break;
                case 2:
                    sourceIncrement = 0;
                    break;
            }
            switch((DM1CNT_H >> 5) & 3) 
            {
                case 0:
                    break;
                case 1:
                    destIncrement = (u32)-4;
                    break;
                case 2:
                    destIncrement = 0;
                    break;
            }
            if(reason == 3) 
            {
                doDMA(dma1Source, dma1Dest, sourceIncrement, 0, 4,
                0x0400);
            }
            else 
            {       
                doDMA(dma1Source, dma1Dest, sourceIncrement, destIncrement,
                DM1CNT_L ? DM1CNT_L : 0x4000,
                DM1CNT_H & 0x0400);
            }
            cpuDmaHack = 1;
            if(DM1CNT_H & 0x4000) 
            {
                IF |= 0x0200;
                UPDATE_REG(0x202, IF);
            }
            if(((DM1CNT_H >> 5) & 3) == 3) 
            {
                dma1Dest = DM1DAD_L | (DM1DAD_H << 16);
            }
            if(!(DM1CNT_H & 0x0200) || (reason == 0)) 
            {
                DM1CNT_H &= 0x7FFF;
                UPDATE_REG(0xC6, DM1CNT_H);
            }
        }
    }
    // DMA 2
    if((DM2CNT_H & 0x8000) && (dmamask & 4)) 
    {
        if(((DM2CNT_H >> 12) & 3) == reason) 
        {
            u32 sourceIncrement = 4;
            u32 destIncrement = 4;
            switch((DM2CNT_H >> 7) & 3) 
            {
                case 0:
                    break;
                case 1:
                    sourceIncrement = (u32)-4;
                    break;
                case 2:
                    sourceIncrement = 0;
                    break;
            }
            switch((DM2CNT_H >> 5) & 3) 
            {
                case 0:
                    break;
                case 1:
                    destIncrement = (u32)-4;
                    break;
                case 2:
                    destIncrement = 0;
                    break;
            }
            if(reason == 3) 
            {
                doDMA(dma2Source, dma2Dest, sourceIncrement, 0, 4,
                0x0400);
            }
            else 
            {
                doDMA(dma2Source, dma2Dest, sourceIncrement, destIncrement,
                DM2CNT_L ? DM2CNT_L : 0x4000,
                DM2CNT_H & 0x0400);
            }
            cpuDmaHack = 1;
            if(DM2CNT_H & 0x4000) 
            {
                IF |= 0x0400;
                UPDATE_REG(0x202, IF);
            }
            if(((DM2CNT_H >> 5) & 3) == 3) 
            {
                dma2Dest = DM2DAD_L | (DM2DAD_H << 16);
            }
            if(!(DM2CNT_H & 0x0200) || (reason == 0)) 
            {
                DM2CNT_H &= 0x7FFF;
                UPDATE_REG(0xD2, DM2CNT_H);
            }
        }
    }
    // DMA 3
    if((DM3CNT_H & 0x8000) && (dmamask & 8)) 
    {
        if(((DM3CNT_H >> 12) & 3) == reason) 
        {
            u32 sourceIncrement = 4;
            u32 destIncrement = 4;
            switch((DM3CNT_H >> 7) & 3) 
            {
                case 0:
                    break;
                case 1:
                    sourceIncrement = (u32)-4;
                    break;
                case 2:
                    sourceIncrement = 0;
                    break;
            }
            switch((DM3CNT_H >> 5) & 3) 
            {
                case 0:
                    break;
                case 1:
                    destIncrement = (u32)-4;
                    break;
                case 2:
                    destIncrement = 0;
                    break;
            }
            doDMA(dma3Source, dma3Dest, sourceIncrement, destIncrement,
            DM3CNT_L ? DM3CNT_L : 0x10000,
            DM3CNT_H & 0x0400);
            if(DM3CNT_H & 0x4000) 
            {
                IF |= 0x0800;
                UPDATE_REG(0x202, IF);
            }
            if(((DM3CNT_H >> 5) & 3) == 3) 
            {
                dma3Dest = DM3DAD_L | (DM3DAD_H << 16);
            }
            if(!(DM3CNT_H & 0x0200) || (reason == 0)) 
            {
                DM3CNT_H &= 0x7FFF;
                UPDATE_REG(0xDE, DM3CNT_H);
            }
        }
    }
    cpuDmaHack = 0;
}
void CGbaEmu::doDMA(u32 &s, u32 &d, u32 si, u32 di, u32 c, int transfer32)
{
    int sm = s >> 24;
    int dm = d >> 24;
    int sc = c;
    cpuDmaCount = c;
    if(transfer32) 
    {
        s &= 0xFFFFFFFC;
        if(s < 0x02000000 && (reg[15].I >> 24)) 
        {
            while(c != 0) 
            {
                CPUWriteMemory(d, 0);
                d += di;
                c--;
            }
        }
        else 
        {
            while(c != 0) 
            {
                CPUWriteMemory(d, CPUReadMemory(s));
                d += di;
                s += si;
                c--;
            }
        }
    }
    else 
    {
        s &= 0xFFFFFFFE;
        si = (int)si >> 1;
        di = (int)di >> 1;
        if(s < 0x02000000 && (reg[15].I >> 24)) 
        {
            while(c != 0) 
            {
                CPUWriteHalfWord(d, 0);
                d += di;
                c--;
            }
        }
        else 
        {
            while(c != 0) 
            {
                cpuDmaLast = CPUReadHalfWord(s);
                CPUWriteHalfWord(d, cpuDmaLast);
                d += di;
                s += si;
                c--;
            }
        }
    }
    cpuDmaCount = 0;
    int sw = 1+memoryWaitSeq[sm & 15];
    int dw = 1+memoryWaitSeq[dm & 15];
    int totalTicks = 0;
    if(transfer32) 
    {
        if(!memory32[sm & 15])
            sw <<= 1;
        if(!memory32[dm & 15])
            dw <<= 1;
    }
    totalTicks = (sw+dw)*sc;
    cpuDmaTicksToUpdate += totalTicks;
    if(*extCpuLoopTicks >= 0) 
    {
        CPU_BREAK_LOOP;
    }
}
void CGbaEmu::CPUWriteHalfWord(u32 address, u16 value)
{
    switch(address >> 24) 
    {
        case 2:
            WRITE16LE(((u16 *)&workRAM[address & 0x3FFFE]),value);
            break;
        case 3:
            WRITE16LE(((u16 *)&internalRAM[address & 0x7ffe]), value);
            break;    
        case 4:
            CPUUpdateRegister(address & 0x3fe, value);
            break;
        case 5:
            WRITE16LE(((u16 *)&paletteRAM[address & 0x3fe]), value);
            break;
        case 6:
            if(address & 0x10000)
                WRITE16LE(((u16 *)&vram[address & 0x17ffe]), value);
            else
                WRITE16LE(((u16 *)&vram[address & 0x1fffe]), value);
            break;
        case 7:
            WRITE16LE(((u16 *)&oam[address & 0x3fe]), value);
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            if(address == 0x80000c4 || address == 0x80000c6 || address == 0x80000c8) 
            {
                if(!rtcWrite(address, value))
                    goto unwritable;
            }
            else if(!agbPrintWrite(address, value))
            {
#if _GBA_VM
                WRITE16LE(((u16 *)&rom[address & 0x1ffffff]), value);
#endif
                goto unwritable;
            }
            break;
        case 13:
            if(cpuEEPROMEnabled) 
            {
                eepromWrite(address, (u8)value);
                break;
        }
        goto unwritable;
        case 14:
            if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) 
            {
                (this->*cpuSaveGameFunc)(address, (u8)value);
                break;
        }
        goto unwritable;
        default:
        unwritable:
        break;
    }
}
void CGbaEmu::CPUWriteByte(u32 address, u8 b)
{
    switch(address >> 24) 
    {
        case 2:
            workRAM[address & 0x3FFFF] = b;
            break;
        case 3:
            internalRAM[address & 0x7fff] = b;
            break;
        case 4:
            switch(address & 0x3FF) 
            {
                case 0x301:
                if(b == 0x80)
                    stopState = true;
                holdState = 1;
                holdType = -1;
                break;
            case 0x60:
                case 0x61:
                case 0x62:
                case 0x63:
                case 0x64:
                case 0x65:
                case 0x68:
                case 0x69:
                case 0x6c:
                case 0x6d:
                case 0x70:
                case 0x71:
                case 0x72:
                case 0x73:
                case 0x74:
                case 0x75:
                case 0x78:
                case 0x79:
                case 0x7c:
                case 0x7d:
                case 0x80:
                case 0x81:
                case 0x84:
                case 0x85:
                case 0x90:
                case 0x91:
                case 0x92:
                case 0x93:
                case 0x94:
                case 0x95:
                case 0x96:
                case 0x97:
                case 0x98:
                case 0x99:
                case 0x9a:
                case 0x9b:
                case 0x9c:
                case 0x9d:
                case 0x9e:
                case 0x9f:      
                soundEvent(address&0xFF, b);
                break;
            default:
            if(address & 1)
                CPUUpdateRegister(address & 0x3fe,
            ((READ16LE(((u16 *)&ioMem[address & 0x3fe])))
            & 0x00FF) |
                b<<8);
            else
                CPUUpdateRegister(address & 0x3fe,
            ((READ16LE(((u16 *)&ioMem[address & 0x3fe])) & 0xFF00) | b));
        }
        break;
        case 5:
            // no need to switch
            *((u16 *)&paletteRAM[address & 0x3FE]) = (b << 8) | b;
            break;
        case 6:
            // no need to switch
            if(address & 0x10000)
            *((u16 *)&vram[address & 0x17FFE]) = (b << 8) | b;
            else
            *((u16 *)&vram[address & 0x1FFFE]) = (b << 8) | b;
            break;
        case 7:
            // no need to switch
            *((u16 *)&oam[address & 0x3FE]) = (b << 8) | b;
            break;    
#if _GBA_VM
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            rom[address & 0x1ffffff] = b;
            break;
#endif
        case 13:
            if(cpuEEPROMEnabled) 
            {
                eepromWrite(address, b);
                break;
        }
        goto unwritable;
        case 14:
            if(!eepromInUse | cpuSramEnabled | cpuFlashEnabled) 
            {
                (this->*cpuSaveGameFunc)(address, b);
                break;
        }
        // default
        default:
        unwritable:
        break;
    }
}

bool CGbaEmu::agbPrintWrite(u32 address, u16 value)
{
    if(agbPrintEnabled) 
    {
        if(address == 0x9fe2ffe) 
        {
            // protect
            agbPrintProtect = (value != 0);
            debuggerWriteHalfWord(address, value);
            return true;
        }
        else 
        {
            if(agbPrintProtect &&
            ((address >= 0x9fe20f8 && address <= 0x9fe20ff) // control structure
            || (address >= 0x8fd0000 && address <= 0x8fdffff)
            || (address >= 0x9fd0000 && address <= 0x9fdffff))) 
            {
                debuggerWriteHalfWord(address, value);
                return true;
            }
        }
    }
    return false;
}
void CGbaEmu::agbPrintReset()
{
    agbPrintProtect = false;
}
void CGbaEmu::agbPrintEnable(bool enable)
{
    agbPrintEnabled = enable;
}
bool CGbaEmu::agbPrintIsEnabled()
{
    return agbPrintEnabled;
}

void CGbaEmu::agbPrintFlush()
{
    u16 get = debuggerReadHalfWord(0x9fe20fc);
    u16 put = debuggerReadHalfWord(0x9fe20fe);
    u32 address = (debuggerReadHalfWord(0x9fe20fa) << 16);
    if(address != 0xfd0000 && address != 0x1fd0000) 
    {
        LOG("Did you forget to call AGBPrintInit?\n", 0);
        debuggerWriteHalfWord(0x9fe20fc, put);    
        return;
    }
    u8 *data = &rom[address];
    while(get != put) 
    {
        char c = data[get++];
        char s[2];
        s[0] = c;
        s[1] = 0;
        if(systemVerbose & VERBOSE_AGBPRINT)
            LOG("%s",s);
        if(c == '\n')
            break;
    }
    debuggerWriteHalfWord(0x9fe20fc, get);
}

void CGbaEmu::rtcEnable(bool e)
{
    rtcEnabled = e;
}
bool CGbaEmu::rtcIsEnabled()
{
    return rtcEnabled;
}
u8 CGbaEmu::toBCD(u8 value)
{
    value = value % 100;
    int l = value % 10;
    int h = value / 10;
    return h * 16 + l;
}
bool CGbaEmu::rtcWrite(u32 address, u16 value)
{
    if(!rtcEnabled)
        return false;
    if(address == 0x80000c8) 
    {
        rtcClockData->byte2 = (u8)value; // enable ?
    }
    else if(address == 0x80000c6) 
    {
        rtcClockData->byte1 = (u8)value; // read/write
    }
    else if(address == 0x80000c4) 
    {
        if(rtcClockData->byte2 & 1) 
        {
            if(rtcClockData->state == IDLE && rtcClockData->byte0 == 1 && value == 5) 
            {
                rtcClockData->state = COMMAND;
                rtcClockData->bits = 0;
                rtcClockData->command = 0;
            }
            else if(!(rtcClockData->byte0 & 1) && (value & 1)) 
            {
                // bit transfer
                rtcClockData->byte0 = (u8)value;        
                switch(rtcClockData->state) 
                {
                    case COMMAND:
                        rtcClockData->command |= ((value & 2) >> 1) << (7-rtcClockData->bits);
                        rtcClockData->bits++;
                        if(rtcClockData->bits == 8) 
                        {
                            rtcClockData->bits = 0;
                            switch(rtcClockData->command) 
                            {
                                case 0x60:
                                // not sure what this command does but it doesn't take parameters
                                // maybe it is a reset or stop
                                rtcClockData->state = IDLE;
                                rtcClockData->bits = 0;
                                break;
                            case 0x62:
                                // this sets the control state but not sure what those values are
                                rtcClockData->state = READDATA;
                                rtcClockData->dataLen = 1;
                                break;
                            case 0x63:
                                rtcClockData->dataLen = 1;
                                rtcClockData->data[0] = 0x40;
                                rtcClockData->state = DATA;
                                break;
                            case 0x65:
                                {
                                    struct tm *newtime;
                                    time_t long_time;
                                    time( &long_time );                /* Get time as long integer. */
                                    newtime = localtime( &long_time ); /* Convert to local time. */
                                    rtcClockData->dataLen = 7;
                                    rtcClockData->data[0] = toBCD(newtime->tm_year);
                                    rtcClockData->data[1] = toBCD(newtime->tm_mon+1);
                                    rtcClockData->data[2] = toBCD(newtime->tm_mday);
                                    rtcClockData->data[3] = 0;
                                    rtcClockData->data[4] = toBCD(newtime->tm_hour);
                                    rtcClockData->data[5] = toBCD(newtime->tm_min);
                                    rtcClockData->data[6] = toBCD(newtime->tm_sec);
                                    rtcClockData->state = DATA;
                                }
                                break;              
                            case 0x67:
                                {
                                    struct tm *newtime;
                                    time_t long_time;
                                    time( &long_time );                /* Get time as long integer. */
                                    newtime = localtime( &long_time ); /* Convert to local time. */
                                    rtcClockData->dataLen = 3;
                                    rtcClockData->data[0] = toBCD(newtime->tm_hour);
                                    rtcClockData->data[1] = toBCD(newtime->tm_min);
                                    rtcClockData->data[2] = toBCD(newtime->tm_sec);
                                    rtcClockData->state = DATA;
                                }
                                break;
                            default:
                            LOG("Unknown RTC command %02x\n", rtcClockData->command);
                            rtcClockData->state = IDLE;
                            break;
                        }
                    }
                    break;
                    case DATA:
                        if(rtcClockData->byte1 & 2) 
                        {
                        }
                        else 
                        {
                            rtcClockData->byte0 = (rtcClockData->byte0 & ~2) |
                            ((rtcClockData->data[rtcClockData->bits >> 3] >>
                            (rtcClockData->bits & 7)) & 1)*2;
                            rtcClockData->bits++;
                            if(rtcClockData->bits == 8*rtcClockData->dataLen) 
                            {
                                rtcClockData->bits = 0;
                                rtcClockData->state = IDLE;
                            }
                        }
                        break;
                    case READDATA:
                        if(!(rtcClockData->byte1 & 2)) 
                        {
                        }
                        else 
                        {
                            rtcClockData->data[rtcClockData->bits >> 3] =
                            (rtcClockData->data[rtcClockData->bits >> 3] >> 1) |
                            ((value << 6) & 128);
                            rtcClockData->bits++;
                            if(rtcClockData->bits == 8*rtcClockData->dataLen) 
                            {
                                rtcClockData->bits = 0;
                                rtcClockData->state = IDLE;
                            }
                        }
                        break;
                    default:
                    break;
                }
            }
            else
                rtcClockData->byte0 = (u8)value;
        }
    }
    return true;
}

void CGbaEmu::rtcReset()
{
    memset(rtcClockData, 0, sizeof(rtcClockData));
    rtcClockData->byte0 = 0;
    rtcClockData->byte1 = 0;
    rtcClockData->byte2 = 0;
    rtcClockData->command = 0;
    rtcClockData->dataLen = 0;
    rtcClockData->bits = 0;
    rtcClockData->state = IDLE;
}

int CGbaEmu::CPUInit(CFileBase *bios_file, bool useBiosFile)
{
    gbaSaveType = 0;
    eepromInUse = 0;
    saveType = 0;
#if _GBA_VM
    this->GbaVmResume();
#endif
    
    if(useBiosFile && bios_file) 
    {
        int size = 0x4000;
        ASSERT(bios_file->GetSize() <= size);
        bios_file->Seek(0);
        bios_file->Read(this->bios,size);
    }
    
    if(!useBios) 
        memcpy(bios, myROM, sizeof(myROM));

    int i = 0;
    biosProtected[0] = 0x00;
    biosProtected[1] = 0xf0;
    biosProtected[2] = 0x29;
    biosProtected[3] = 0xe1;
    for(i = 0; i < 256; i++) 
    {
        int count = 0;
        int j;
        for(j = 0; j < 8; j++)
        if(i & (1 << j))
            count++;
        cpuBitsSet[i] = count;
        for(j = 0; j < 8; j++)
            if(i & (1 << j))
                break;
            cpuLowestBitSet[i] = j;
    }
    for(i = 0; i < 0x400; i++)
        ioReadable[i] = true;
    for(i = 0x10; i < 0x48; i++)
        ioReadable[i] = false;
    for(i = 0x4c; i < 0x50; i++)
        ioReadable[i] = false;
    for(i = 0x54; i < 0x60; i++)
        ioReadable[i] = false;
    for(i = 0x8c; i < 0x90; i++)
        ioReadable[i] = false;
    for(i = 0xa0; i < 0xb8; i++)
        ioReadable[i] = false;
    for(i = 0xbc; i < 0xc4; i++)
        ioReadable[i] = false;
    for(i = 0xc8; i < 0xd0; i++)
        ioReadable[i] = false;
    for(i = 0xd4; i < 0xdc; i++)
        ioReadable[i] = false;
    for(i = 0xe0; i < 0x100; i++)
        ioReadable[i] = false;
    for(i = 0x110; i < 0x120; i++)
        ioReadable[i] = false;
    for(i = 0x12c; i < 0x130; i++)
        ioReadable[i] = false;
    for(i = 0x138; i < 0x140; i++)
        ioReadable[i] = false;
    for(i = 0x144; i < 0x150; i++)
        ioReadable[i] = false;
    for(i = 0x15c; i < 0x200; i++)
        ioReadable[i] = false;
    for(i = 0x20c; i < 0x300; i++)
        ioReadable[i] = false;
    for(i = 0x304; i < 0x400; i++)
        ioReadable[i] = false;

    //todo: check array size
//  *((u16 *)&rom[0x1fe209c]) = 0xdffa; // SWI 0xFA
//  *((u16 *)&rom[0x1fe209e]) = 0x4770; // BX LR

    return OK;
}

void CGbaEmu::CPUReset()
{
    int i;

    if(gbaSaveType == 0) 
    {
        if(eepromInUse)
            gbaSaveType = 3;
        else
            switch(saveType) 
        {
            case 1:
                gbaSaveType = 1;
                break;
            case 2:
                gbaSaveType = 2;
                break;
        }
    }

    rtcReset();

    // clen registers
    for(i = 0; i < 45; i++)
        this->reg[i].I = 0;
    for(i = 0; i < 0x400; i++)
        this->paletteRAM[i] = 0;
    for(i = 0; i < 0x20000; i++)
        this->vram[i] = 0;
    for(i = 0; i < 0x400; i++)
        this->oam[i] = 0;
    for(i = 0; i < 4 * 241 * 162; i++)
        this->pix[i] = 0;
    for(i = 0; i < 0x400; i++)
        this->ioMem[i] = 0;

    DISPCNT  = 0x0080;
    DISPSTAT = 0x0000;
    VCOUNT   = 0x0000;
    BG0CNT   = 0x0000;
    BG1CNT   = 0x0000;
    BG2CNT   = 0x0000;
    BG3CNT   = 0x0000;
    BG0HOFS  = 0x0000;
    BG0VOFS  = 0x0000;
    BG1HOFS  = 0x0000;
    BG1VOFS  = 0x0000;
    BG2HOFS  = 0x0000;
    BG2VOFS  = 0x0000;
    BG3HOFS  = 0x0000;
    BG3VOFS  = 0x0000;
    BG2PA    = 0x0100;
    BG2PB    = 0x0000;
    BG2PC    = 0x0000;
    BG2PD    = 0x0100;
    BG2X_L   = 0x0000;
    BG2X_H   = 0x0000;
    BG2Y_L   = 0x0000;
    BG2Y_H   = 0x0000;
    BG3PA    = 0x0100;
    BG3PB    = 0x0000;
    BG3PC    = 0x0000;
    BG3PD    = 0x0100;
    BG3X_L   = 0x0000;
    BG3X_H   = 0x0000;
    BG3Y_L   = 0x0000;
    BG3Y_H   = 0x0000;
    WIN0H    = 0x0000;
    WIN1H    = 0x0000;
    WIN0V    = 0x0000;
    WIN1V    = 0x0000;
    WININ    = 0x0000;
    WINOUT   = 0x0000;
    MOSAIC   = 0x0000;
    BLDMOD   = 0x0000;
    COLEV    = 0x0000;
    COLY     = 0x0000;
    DM0SAD_L = 0x0000;
    DM0SAD_H = 0x0000;
    DM0DAD_L = 0x0000;
    DM0DAD_H = 0x0000;
    DM0CNT_L = 0x0000;
    DM0CNT_H = 0x0000;
    DM1SAD_L = 0x0000;
    DM1SAD_H = 0x0000;
    DM1DAD_L = 0x0000;
    DM1DAD_H = 0x0000;
    DM1CNT_L = 0x0000;
    DM1CNT_H = 0x0000;
    DM2SAD_L = 0x0000;
    DM2SAD_H = 0x0000;
    DM2DAD_L = 0x0000;
    DM2DAD_H = 0x0000;
    DM2CNT_L = 0x0000;
    DM2CNT_H = 0x0000;
    DM3SAD_L = 0x0000;
    DM3SAD_H = 0x0000;
    DM3DAD_L = 0x0000;
    DM3DAD_H = 0x0000;
    DM3CNT_L = 0x0000;
    DM3CNT_H = 0x0000;
    TM0D     = 0x0000;
    TM0CNT   = 0x0000;
    TM1D     = 0x0000;
    TM1CNT   = 0x0000;
    TM2D     = 0x0000;
    TM2CNT   = 0x0000;
    TM3D     = 0x0000;
    TM3CNT   = 0x0000;
    P1       = 0x03FF;
    IE       = 0x0000;
    IF       = 0x0000;
    IME      = 0x0000;
    armMode = 0x1F;

    if(cpuIsMultiBoot) 
    {
        reg[13].I = 0x03007F00;
        reg[15].I = 0x02000000;
        reg[16].I = 0x00000000;
        reg[R13_IRQ].I = 0x03007FA0;
        reg[R13_SVC].I = 0x03007FE0;
        armIrqEnable = true;
    }
    else 
    {
        if(useBios && !skipBios) 
        {
            reg[15].I = 0x00000000;
            armMode = 0x13;
            armIrqEnable = false;      
        }
        else 
        {
            reg[13].I = 0x03007F00;
            reg[15].I = 0x08000000;
            reg[16].I = 0x00000000;
            reg[R13_IRQ].I = 0x03007FA0;
            reg[R13_SVC].I = 0x03007FE0;
            armIrqEnable = true;      
        }
    }
    
    armState = true;
    C_FLAG = V_FLAG = N_FLAG = Z_FLAG = false;
    UPDATE_REG(0x00, DISPCNT);
    UPDATE_REG(0x20, BG2PA);
    UPDATE_REG(0x26, BG2PD);
    UPDATE_REG(0x30, BG3PA);
    UPDATE_REG(0x36, BG3PD);
    UPDATE_REG(0x130, P1);
    UPDATE_REG(0x88, 0x200);
    // disable FIQ
    reg[16].I |= 0x40;
    CPUUpdateCPSR();
    armNextPC = reg[15].I;
    reg[15].I += 4;
    // reset internal state
    holdState = false;
    holdType = 0;
    biosProtected[0] = 0x00;
    biosProtected[1] = 0xf0;
    biosProtected[2] = 0x29;
    biosProtected[3] = 0xe1;
    lcdTicks = 960;
    timer0On = false;
    timer0Ticks = 0;
    timer0Reload = 0;
    timer0ClockReload  = 0;
    timer1On = false;
    timer1Ticks = 0;
    timer1Reload = 0;
    timer1ClockReload  = 0;
    timer2On = false;
    timer2Ticks = 0;
    timer2Reload = 0;
    timer2ClockReload  = 0;
    timer3On = false;
    timer3Ticks = 0;
    timer3Reload = 0;
    timer3ClockReload  = 0;
    dma0Source = 0;
    dma0Dest = 0;
    dma1Source = 0;
    dma1Dest = 0;
    dma2Source = 0;
    dma2Dest = 0;
    dma3Source = 0;
    dma3Dest = 0;
    cpuSaveGameFunc = &CGbaEmu::flashSaveDecide;
    renderLine = &CGbaEmu::mode0RenderLine;
    fxOn = false;
    windowOn = false;
    frameCount = 0;
    saveType = 0;
    layerEnable = DISPCNT & layerSettings;
    CPUUpdateRenderBuffers(true);

    for(i = 0; i < 256; i++) 
    {
        map[i].address = (u8 *)&dummyAddress;
        map[i].mask = 0;
    }
    map[0].address = bios;
    map[0].mask = 0x3FFF;
    map[2].address = workRAM;
    map[2].mask = 0x3FFFF;
    map[3].address = internalRAM;
    map[3].mask = 0x7FFF;
    map[4].address = ioMem;
    map[4].mask = 0x3FF;
    map[5].address = paletteRAM;
    map[5].mask = 0x3FF;
    map[6].address = vram;
    map[6].mask = 0x1FFFF;
    map[7].address = oam;
    map[7].mask = 0x3FF;
    map[8].address = rom;
    map[8].mask = 0x1FFFFFF;
    map[9].address = rom;
    map[9].mask = 0x1FFFFFF;  
    map[10].address = rom;
    map[10].mask = 0x1FFFFFF;
    map[12].address = rom;
    map[12].mask = 0x1FFFFFF;
    map[14].address = flashSaveMemory;
    map[14].mask = 0xFFFF;
    eepromReset();
    flashReset();
    soundReset();
    CPUUpdateWindow0();
    CPUUpdateWindow1();
    // make sure registers are correctly initialized if not using BIOS
    if(!useBios) 
    {
        if(cpuIsMultiBoot)
            BIOS_RegisterRamReset(0xfe);
        else
            BIOS_RegisterRamReset(0xff);
    }
    else 
    {
        if(cpuIsMultiBoot)
            BIOS_RegisterRamReset(0xfe);
    }
    switch(cpuSaveType) 
    {
        case 0: // automatic
            cpuSramEnabled = true;
            cpuFlashEnabled = true;
            cpuEEPROMEnabled = true;
            cpuEEPROMSensorEnabled = false;
            break;
        case 1: // EEPROM
            cpuSramEnabled = false;
            cpuFlashEnabled = false;
            cpuEEPROMEnabled = true;
            cpuEEPROMSensorEnabled = false;
            break;
        case 2: // SRAM
            cpuSramEnabled = true;
            cpuFlashEnabled = false;
            cpuEEPROMEnabled = false;
            cpuEEPROMSensorEnabled = false;
            cpuSaveGameFunc = &CGbaEmu::sramWrite;
            break;
        case 3: // FLASH
            cpuSramEnabled = false;
            cpuFlashEnabled = true;
            cpuEEPROMEnabled = false;
            cpuEEPROMSensorEnabled = false;
            cpuSaveGameFunc = &CGbaEmu::flashWrite;
            break;
        case 4: // EEPROM+Sensor
            cpuSramEnabled = false;
            cpuFlashEnabled = false;
            cpuEEPROMEnabled = true;
            cpuEEPROMSensorEnabled = true;
            break;
        case 5: // NONE
            cpuSramEnabled = false;
            cpuFlashEnabled = false;
            cpuEEPROMEnabled = false;
            cpuEEPROMSensorEnabled = false;
            break;
    }
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    lastTime = systemGetClock();

    this->InitColorMap();
}


void CGbaEmu::soundReset()
{
    //sound is not supported
}

void CGbaEmu::eepromReset()
{
    eepromMode = EEPROM_IDLE;
    eepromByte = 0;
    eepromBits = 0;
    eepromAddress = 0;
    eepromInUse = false;
    eepromSize = 512;
}

void CGbaEmu::flashReset()
{
    flashState = FLASH_READ_ARRAY;
    flashReadState = FLASH_READ_ARRAY;
    flashBank = 0;
}

void CGbaEmu::BIOS_RegisterRamReset(u32 flags)
{
    // no need to trace here. this is only called directly from GBA.cpp
    // to emulate bios initialization
    if(flags) 
    {
        if(flags & 0x01) 
        {
            // clear work RAM
            memset(workRAM, 0, 0x40000);
        }
        if(flags & 0x02) 
        {
            // clear internal RAM
            memset(internalRAM, 0, 0x7e00); // don't clear 0x7e00-0x7fff
        }
        if(flags & 0x04) 
        {
            // clear palette RAM
            memset(paletteRAM, 0, 0x400);
        }
        if(flags & 0x08) 
        {
            // clear VRAM
            memset(vram, 0, 0x18000);
        }
        if(flags & 0x10) 
        {
            // clean OAM
            memset(oam, 0, 0x400);
        }
        if(flags & 0x80) 
        {
            int i;
            for(i = 0; i < 8; i++)
                CPUUpdateRegister(0x200+i*2, 0);
            CPUUpdateRegister(0x202, 0xFFFF);
            for(i = 0; i < 8; i++)
                CPUUpdateRegister(0x4+i*2, 0);
            for(i = 0; i < 16; i++)
                CPUUpdateRegister(0x20+i*2, 0);
            for(i = 0; i < 24; i++)
            CPUUpdateRegister(0xb0+i*2, 0);
            CPUUpdateRegister(0x130, 0);
            CPUUpdateRegister(0x20, 0x100);
            CPUUpdateRegister(0x30, 0x100);
            CPUUpdateRegister(0x26, 0x100);
            CPUUpdateRegister(0x36, 0x100);
        }
        if(flags & 0x20) 
        {
            int i;
            for(i = 0; i < 8; i++)
                CPUUpdateRegister(0x110+i*2, 0);
            CPUUpdateRegister(0x134, 0x8000);
            for(i = 0; i < 7; i++)
                CPUUpdateRegister(0x140+i*2, 0);
        }
        if(flags & 0x40) 
        {
            int i;
            CPUWriteByte(0x4000084, 0);
            CPUWriteByte(0x4000084, 0x80);
            CPUWriteMemory(0x4000080, 0x880e0000);
            CPUUpdateRegister(0x88, CPUReadHalfWord(0x4000088)&0x3ff);
            CPUWriteByte(0x4000070, 0x70);
            for(i = 0; i < 8; i++)
                CPUUpdateRegister(0x90+i*2, 0);
            CPUWriteByte(0x4000070, 0);
            for(i = 0; i < 8; i++)
                CPUUpdateRegister(0x90+i*2, 0);
            CPUWriteByte(0x4000084, 0);
        }
    }
}

u32 CGbaEmu::systemGetClock()
{
    return CMisc::get_sys_time();
}
void CGbaEmu::CPUInterrupt()
{
    u32 PC = reg[15].I;
    bool savedState = armState;
    CPUSwitchMode(0x12, true, false);
    reg[14].I = PC;
    if(!savedState)
        reg[14].I += 2;
    reg[15].I = 0x18;
    armState = true;
    armIrqEnable = false;
    armNextPC = reg[15].I;
    reg[15].I += 4;
    //  if(!holdState)
    biosProtected[0] = 0x02;
    biosProtected[1] = 0xc0;
    biosProtected[2] = 0x5e;
    biosProtected[3] = 0xe5;
}

void CGbaEmu::CPULoop(int ticks)
{
    int clockTicks;
    int cpuLoopTicks = 0;
    int timerOverflow = 0;
    // variables used by the CPU core
    extCpuLoopTicks = &cpuLoopTicks;
    extClockTicks = &clockTicks;
    extTicks = &ticks;
    cpuLoopTicks = CPUUpdateTicks();
    if(cpuLoopTicks > ticks) 
    {
        cpuLoopTicks = ticks;
        cpuSavedTicks = ticks;
    }
    if(intState) 
    {
        cpuLoopTicks = 5;
        cpuSavedTicks = 5;
    }
    for(;;) 
    {
#if _GBA_VM
        if(this->gba_vm_suspend)
            break;
#endif
        if(!holdState) 
        {
            if(armState) 
            {
#include "gba_arm_new.h"
            }
            else 
            {
#include "gba_thumb.h"
            }
        }
        else 
        {
            clockTicks = lcdTicks;
            if(soundTicks < clockTicks)
                clockTicks = soundTicks;
            if(timer0On && (timer0Ticks < clockTicks)) 
            {
                clockTicks = timer0Ticks;
            }
            if(timer1On && (timer1Ticks < clockTicks)) 
            {
                clockTicks = timer1Ticks;
            }
            if(timer2On && (timer2Ticks < clockTicks)) 
            {
                clockTicks = timer2Ticks;
            }
            if(timer3On && (timer3Ticks < clockTicks)) 
            {
                clockTicks = timer3Ticks;
            }
        }
        cpuLoopTicks -= clockTicks;
        if((cpuLoopTicks <= 0)) 
        {
            if(cpuSavedTicks) 
            {
                clockTicks = cpuSavedTicks;// + cpuLoopTicks;
            }
            cpuDmaTicksToUpdate = -cpuLoopTicks;
            updateLoop:
            lcdTicks -= clockTicks;
            if(lcdTicks <= 0) 
            {
                if(DISPSTAT & 1) 
                {
                    // V-BLANK
                    // if in V-Blank mode, keep computing...
                    if(DISPSTAT & 2) 
                    {
                        lcdTicks += 960;
                        VCOUNT++;
                        UPDATE_REG(0x06, VCOUNT);
                        DISPSTAT &= 0xFFFD;
                        UPDATE_REG(0x04, DISPSTAT);
                        CPUCompareVCOUNT();
                    }
                    else 
                    {
                        lcdTicks += 272;
                        DISPSTAT |= 2;
                        UPDATE_REG(0x04, DISPSTAT);
                        if(DISPSTAT & 16) 
                        {
                            IF |= 2;
                            UPDATE_REG(0x202, IF);
                        }
                    }
                    if(VCOUNT >= 228) 
                    {
                        DISPSTAT &= 0xFFFC;
                        UPDATE_REG(0x04, DISPSTAT);
                        VCOUNT = 0;
                        UPDATE_REG(0x06, VCOUNT);
                        CPUCompareVCOUNT();
                    }
                }
                else 
                {
                    int framesToSkip = systemFrameSkip;
                    if(speedup)
                        framesToSkip = 9; // try 6 FPS during speedup
                    if(DISPSTAT & 2) 
                    {
                        // if in H-Blank, leave it and move to drawing mode
                        VCOUNT++;
                        UPDATE_REG(0x06, VCOUNT);
                        lcdTicks += (960);
                        DISPSTAT &= 0xFFFD;
                        if(VCOUNT == 160) 
                        {
                            count++;
                            systemFrame();
                            if((count % 10) == 0) 
                            {
                                system10Frames(60);
                            }
                            if(count == 60) 
                            {
                                u32 time = systemGetClock();
                                if(time != lastTime) 
                                {
                                    u32 t = 100000/(time - lastTime);
                                    systemShowSpeed(t);
                                }
                                else
                                    systemShowSpeed(0);
                                lastTime = time;
                                count = 0;
                            }
                            u32 joy = 0;
                            // update joystick information
                            if(systemReadJoypads())
                            // read default joystick
                                joy = systemReadJoypad(-1);
                            P1 = 0x03FF ^ (joy & 0x3FF);
                            if(cpuEEPROMSensorEnabled)
                                systemUpdateMotionSensor();              
                            UPDATE_REG(0x130, P1);
                            u16 P1CNT = READ16LE(((u16 *)&ioMem[0x132]));
                            // this seems wrong, but there are cases where the game
                            // can enter the stop state without requesting an IRQ from
                            // the joypad.
                            if((P1CNT & 0x4000) || stopState) 
                            {
                                u16 p1 = (0x3FF ^ P1) & 0x3FF;
                                if(P1CNT & 0x8000) 
                                {
                                    if(p1 == (P1CNT & 0x3FF)) 
                                    {
                                        IF |= 0x1000;
                                        UPDATE_REG(0x202, IF);
                                    }
                                }
                                else 
                                {
                                    if(p1 & P1CNT) 
                                    {
                                        IF |= 0x1000;
                                        UPDATE_REG(0x202, IF);
                                    }
                                }
                            }
                            u32 ext = (joy >> 10);
                            int cheatTicks = 0;
                            if(cheatsEnabled)
                                cheatsCheckKeys(P1^0x3FF, ext);
                            cpuDmaTicksToUpdate += cheatTicks;
                            speedup = (ext & 1) ? true : false;
                            capture = (ext & 2) ? true : false;
                            if(capture && !capturePrevious) 
                            {
                                captureNumber++;
                                systemScreenCapture(captureNumber);
                            }
                            capturePrevious = capture;
                            DISPSTAT |= 1;
                            DISPSTAT &= 0xFFFD;
                            UPDATE_REG(0x04, DISPSTAT);
                            if(DISPSTAT & 0x0008) 
                            {
                                IF |= 1;
                                UPDATE_REG(0x202, IF);
                            }
                            CPUCheckDMA(1, 0x0f);
                            if(frameCount >= framesToSkip) 
                            {
                                systemDrawScreen();
                                frameCount = 0;
                            }
                            else 
                                frameCount++;
                            if(systemPauseOnFrame())
                                ticks = 0;
                        }
                        UPDATE_REG(0x04, DISPSTAT);
                        CPUCompareVCOUNT(); 
                    }
                    else 
                    {
                        if(frameCount >= framesToSkip) 
                        {
                            (this->*renderLine)();
                            switch(systemColorDepth) 
                            {
                                case 16:
                                    {
                                        u16 *dest = (u16 *)pix + 242 * (VCOUNT+1);
                                        for(int x = 0; x < 240;) 
                                        {
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                            *dest++ = systemColorMap16[lineMix[x++]&0xFFFF];
                                        }
                                        // for filters that read past the screen
                                        *dest++ = 0;
                                    }
                                    break;
                                case 24:
                                    {
                                        u8 *dest = (u8 *)pix + 240 * VCOUNT * 3;
                                        for(int x = 0; x < 240;) 
                                        {
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;
                                            *((u32 *)dest) = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            dest += 3;              
                                        }
                                    }
                                    break;
                                case 32:
                                    {
                                        u32 *dest = (u32 *)pix + 241 * (VCOUNT+1);
                                        for(int x = 0; x < 240; ) 
                                        {
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                            *dest++ = systemColorMap32[lineMix[x++] & 0xFFFF];
                                        }
                                    }
                                    break;
                            }
                        }
                        // entering H-Blank
                        DISPSTAT |= 2;
                        UPDATE_REG(0x04, DISPSTAT);
                        lcdTicks += 272;
                        CPUCheckDMA(2, 0x0f);
                        if(DISPSTAT & 16) 
                        {
                            IF |= 2;
                            UPDATE_REG(0x202, IF);
                        }
                    }
                }
            }
            if(!stopState) 
            {
                if(timer0On) 
                {
                    if(timer0ClockReload == 1) 
                    {
                        u32 tm0d = TM0D + clockTicks;
                        if(tm0d > 0xffff) 
                        {
                            tm0d += timer0Reload;
                            timerOverflow |= 1;
                            soundTimerOverflow(0);
                            if(TM0CNT & 0x40) 
                            {
                                IF |= 0x08;
                                UPDATE_REG(0x202, IF);
                            }
                        }
                        TM0D = tm0d;
                        timer0Ticks = 0x10000 - TM0D;
                        UPDATE_REG(0x100, TM0D);            
                    }
                    else 
                    {
                        timer0Ticks -= clockTicks;    
                        if(timer0Ticks <= 0) 
                        {
                            timer0Ticks += timer0ClockReload;
                            TM0D++;
                            if(TM0D == 0) 
                            {
                                TM0D = timer0Reload;
                                timerOverflow |= 1;
                                soundTimerOverflow(0);
                                if(TM0CNT & 0x40) 
                                {
                                    IF |= 0x08;
                                    UPDATE_REG(0x202, IF);
                                }
                            }
                            UPDATE_REG(0x100, TM0D);  
                        }
                    }
                }
                if(timer1On) 
                {
                    if(TM1CNT & 4) 
                    {
                        if(timerOverflow & 1) 
                        {
                            TM1D++;
                            if(TM1D == 0) 
                            {
                                TM1D += timer1Reload;
                                timerOverflow |= 2;
                                soundTimerOverflow(1);
                                if(TM1CNT & 0x40) 
                                {
                                    IF |= 0x10;
                                    UPDATE_REG(0x202, IF);
                                }
                            }
                            UPDATE_REG(0x104, TM1D);
                        }
                    }
                    else 
                    {
                        if(timer1ClockReload == 1) 
                        {
                            u32 tm1d = TM1D + clockTicks;
                            if(tm1d > 0xffff) 
                            {
                                tm1d += timer1Reload;
                                timerOverflow |= 2;           
                                soundTimerOverflow(1);
                                if(TM1CNT & 0x40) 
                                {
                                    IF |= 0x10;
                                    UPDATE_REG(0x202, IF);
                                }
                            }
                            TM1D = tm1d;
                            timer1Ticks = 0x10000 - TM1D;
                            UPDATE_REG(0x104, TM1D);                    
                        }
                        else 
                        {
                            timer1Ticks -= clockTicks;          
                            if(timer1Ticks <= 0) 
                            {
                                timer1Ticks += timer1ClockReload;
                                TM1D++;
                                if(TM1D == 0) 
                                {
                                    TM1D = timer1Reload;
                                    timerOverflow |= 2;           
                                    soundTimerOverflow(1);
                                    if(TM1CNT & 0x40) 
                                    {
                                        IF |= 0x10;
                                        UPDATE_REG(0x202, IF);
                                    }
                                }
                                UPDATE_REG(0x104, TM1D);        
                            }
                        }
                    }
                }
                if(timer2On) 
                {
                    if(TM2CNT & 4) 
                    {
                        if(timerOverflow & 2) 
                        {
                            TM2D++;
                            if(TM2D == 0) 
                            {
                                TM2D += timer2Reload;
                                timerOverflow |= 4;
                                if(TM2CNT & 0x40) 
                                {
                                    IF |= 0x20;
                                    UPDATE_REG(0x202, IF);
                                }
                            }
                            UPDATE_REG(0x108, TM2D);
                        }
                    }
                    else 
                    {
                        if(timer2ClockReload == 1) 
                        {
                            u32 tm2d = TM2D + clockTicks;
                            if(tm2d > 0xffff) 
                            {
                                tm2d += timer2Reload;
                                timerOverflow |= 4;
                                if(TM2CNT & 0x40) 
                                {
                                    IF |= 0x20;
                                    UPDATE_REG(0x202, IF);
                                }
                            }
                            TM2D = tm2d;
                            timer2Ticks = 0x10000 - TM2D;
                            UPDATE_REG(0x108, TM2D);                    
                        }
                        else 
                        {
                            timer2Ticks -= clockTicks;          
                            if(timer2Ticks <= 0) 
                            {
                                timer2Ticks += timer2ClockReload;
                                TM2D++;
                                if(TM2D == 0) 
                                {
                                    TM2D = timer2Reload;
                                    timerOverflow |= 4;
                                    if(TM2CNT & 0x40) 
                                    {
                                        IF |= 0x20;
                                        UPDATE_REG(0x202, IF);
                                    }
                                }
                                UPDATE_REG(0x108, TM2D);        
                            }
                        }
                    }
                }
                if(timer3On) 
                {
                    if(TM3CNT & 4) 
                    {
                        if(timerOverflow & 4) 
                        {
                            TM3D++;
                            if(TM3D == 0) 
                            {
                                TM3D += timer3Reload;
                                if(TM3CNT & 0x40) 
                                {
                                    IF |= 0x40;
                                    UPDATE_REG(0x202, IF);
                                }
                            }
                            UPDATE_REG(0x10c, TM3D);
                        }
                    }
                    else 
                    {
                        if(timer3ClockReload == 1) 
                        {
                            u32 tm3d = TM3D + clockTicks;
                            if(tm3d > 0xffff) 
                            {
                                tm3d += timer3Reload;
                                if(TM3CNT & 0x40) 
                                {
                                    IF |= 0x40;
                                    UPDATE_REG(0x202, IF);
                                }
                            }
                            TM3D = tm3d;
                            timer3Ticks = 0x10000 - TM3D;
                            UPDATE_REG(0x10C, TM3D);                            
                        }
                        else 
                        {
                            timer3Ticks -= clockTicks;          
                            if(timer3Ticks <= 0) 
                            {
                                timer3Ticks += timer3ClockReload;
                                TM3D++;
                                if(TM3D == 0) 
                                {
                                    TM3D = timer3Reload;
                                    if(TM3CNT & 0x40) 
                                    {
                                        IF |= 0x40;
                                        UPDATE_REG(0x202, IF);
                                    }
                                }
                                UPDATE_REG(0x10C, TM3D);
                            }
                        }
                    }
                }
            }
            // we shouldn't be doing sound in stop state, but we lose synchronization
            // if sound is disabled, so in stop state, soundTick will just produce
            // mute sound
            soundTicks -= clockTicks;
            if(soundTicks <= 0) 
            {
                soundTick();
                soundTicks += SOUND_CLOCK_TICKS;
            }
            timerOverflow = 0;
            ticks -= clockTicks;
            cpuLoopTicks = CPUUpdateTicks();
            if(cpuDmaTicksToUpdate > 0) 
            {
                clockTicks = cpuSavedTicks;
                if(clockTicks > cpuDmaTicksToUpdate)
                    clockTicks = cpuDmaTicksToUpdate;
                cpuDmaTicksToUpdate -= clockTicks;
                if(cpuDmaTicksToUpdate < 0)
                    cpuDmaTicksToUpdate = 0;
                goto updateLoop;
            }
            if(IF && (IME & 1) && armIrqEnable) 
            {
                int res = IF & IE;
                if(stopState)
                    res &= 0x3080;
                if(res) 
                {
                    if(intState) 
                    {
                        CPUInterrupt();         
                        intState = false;
                        if(holdState) 
                        {
                            holdState = false;
                            stopState = false;
                        }
                    }
                    else 
                    {
                        if(!holdState) 
                        {
                            intState = true;
                            cpuLoopTicks = 5;
                            cpuSavedTicks = 5;
                        }
                        else 
                        {
                            CPUInterrupt();         
                            if(holdState) 
                            {
                                holdState = false;
                                stopState = false;
                            }
                        }
                    }
                }
            }
            if(ticks <= 0)
                break;
        }
    }
}

void CGbaEmu::systemFrame()
{
    //nothing
}

void CGbaEmu::system10Frames(int _iRate)
{
    //nothing
}

void CGbaEmu::systemShowSpeed(int _iSpeed)
{
    //nothing
}

u32 CGbaEmu::systemReadJoypad(int option)
{
    return this->joypad;
}

bool CGbaEmu::systemReadJoypads()
{
    return true;
}

void CGbaEmu::systemUpdateMotionSensor()
{
    //nothing
}

int CGbaEmu::cheatsCheckKeys(u32 keys, u32 extended)
{
    return ERROR;
}

void CGbaEmu::systemScreenCapture(int _iNum)
{
    //nothing   
}

void CGbaEmu::systemDrawScreen()
{
    systemRenderedFrames++;
}

bool CGbaEmu::systemPauseOnFrame()
{
    return false;
}

void CGbaEmu::soundTimerOverflow(int timer)
{
    //nothing
}

void CGbaEmu::soundTick()
{
    //nothing
}

void CGbaEmu::CPUSoftwareInterrupt(int comment)
{
    static bool disableMessage = false;
    if(armState) comment >>= 16;

    if(comment == 0xfa) 
    {
        agbPrintFlush();
        return;
    }
    if(useBios) 
    {
        CPUSoftwareInterrupt();
        return;
    }   
    switch(comment) 
    {
    case 0x00:
        BIOS_SoftReset();
        break;
    case 0x01:
        BIOS_RegisterRamReset();
        break;
    case 0x02:
        holdState = true;
        holdType = -1;
        break;
    case 0x03:
        holdState = true;
        holdType = -1;
        stopState = true;
        break;
    case 0x04:
        CPUSoftwareInterrupt();
        break;    
    case 0x05:
        CPUSoftwareInterrupt();
        break;
    case 0x06:
        CPUSoftwareInterrupt();
        break;
    case 0x07:
        CPUSoftwareInterrupt();
        break;
    case 0x08:
        BIOS_Sqrt();
        break;
    case 0x09:
        BIOS_ArcTan();
        break;
    case 0x0A:
        BIOS_ArcTan2();
        break;
    case 0x0B:
        BIOS_CpuSet();
        break;
    case 0x0C:
        BIOS_CpuFastSet();
        break;
    case 0x0E:
        BIOS_BgAffineSet();
        break;
    case 0x0F:
        BIOS_ObjAffineSet();
        break;
    case 0x10:
        BIOS_BitUnPack();
        break;
    case 0x11:
        BIOS_LZ77UnCompWram();
        break;
    case 0x12:
        BIOS_LZ77UnCompVram();
        break;
    case 0x13:
        BIOS_HuffUnComp();
        break;
    case 0x14:
        BIOS_RLUnCompWram();
        break;
    case 0x15:
        BIOS_RLUnCompVram();
        break;
    case 0x16:
        BIOS_Diff8bitUnFilterWram();
        break;
    case 0x17:
        BIOS_Diff8bitUnFilterVram();
        break;
    case 0x18:
        BIOS_Diff16bitUnFilter();
        break;
    case 0x19:
        if(reg[0].I)
            systemSoundPause();
        else
            systemSoundResume();
        break;
    case 0x1F:
        BIOS_MidiKey2Freq();
        break;
#if _GBA_VM
    case _GBA_VM_BIOS_INT:
        if(this->callback_vm_bios_int)
        {
            this->callback_vm_bios_int(this,this->param_vm_bios_int);
        }
        break;
#endif

    case 0x2A:
        BIOS_SndDriverJmpTableCopy();
        // let it go, because we don't really emulate this function
    default:
        if(!disableMessage) 
        {
            LOG("Unsupported BIOS function %02x called from %08x. A BIOS file is needed in order to get correct behaviour.",
                comment, armMode ? armNextPC - 4: armNextPC - 2);
            disableMessage = true;
        }
        break;
    }
}

void CGbaEmu::systemSoundPause()
{
    //mothing
}

void CGbaEmu::systemSoundResume()
{
    //nothing
}

void CGbaEmu::BIOS_ArcTan()
{
    s32 a = -((s32)(reg[0].I * reg[0].I)) >> 14; 
    s32 b = ((0xA9 * a) >> 14) + 0x390;
    b = ((b * a) >> 14) + 0x91C;
    b = ((b * a) >> 14) + 0xFB6;
    b = ((b * a) >> 14) + 0x16AA;
    b = ((b * a) >> 14) + 0x2081;
    b = ((b * a) >> 14) + 0x3651;
    b = ((b * a) >> 14) + 0xA2F9;
    reg[0].I = (reg[0].I * b) >> 16;
}
void CGbaEmu::BIOS_ArcTan2()
{
    s16 x = reg[0].I;
    s16 y = reg[1].I;
    if (y == 0) 
    {
        reg[0].I = 0x8000 & x;
        reg[3].I = 0x170;
    }
    else 
    {
        if (x == 0) 
        {
            reg[0].I = (0x8000 & y) + 0x4000;
            reg[3].I = 0x170;
        }
        else 
        {
            if (abs(x) > abs(y)) 
            {
                reg[1].I = x;
                reg[0].I = y << 14;
                BIOS_Div();
                BIOS_ArcTan();
                if (x < 0)
                    reg[0].I = 0x8000 + reg[0].I;
                else
                    reg[0].I = ((y & 0x8000) << 1 ) + reg[0].I;
                reg[3].I = 0x170;
            }
            else 
            {
                reg[0].I = x << 14;
                BIOS_Div();
                BIOS_ArcTan();
                reg[0].I = (0x4000 + (y & 0x8000)) - reg[0].I;
                reg[3].I = 0x170;
            }
        }
    }
}
void CGbaEmu::BIOS_BitUnPack()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = reg[2].I;
    int len = CPUReadHalfWord(header);
    // check address
    int bits = CPUReadByte(header+2);
    int revbits = 8 - bits; 
    // u32 value = 0;
    u32 base = CPUReadMemory(header+4);
    bool addBase = (base & 0x80000000) ? true : false;
    base &= 0x7fffffff;
    int dataSize = CPUReadByte(header+3);
    int data = 0; 
    int bitwritecount = 0; 
    while(1) 
    {
        len -= 1;
        if(len < 0)
            break;
        int mask = 0xff >> revbits; 
        u8 b = CPUReadByte(source); 
        source++;
        int bitcount = 0;
        while(1) 
        {
            if(bitcount >= 8)
                break;
            u32 d = b & mask;
            u32 temp = d >> bitcount;
            if(!temp && addBase) 
            {
                temp += base;
            }
            data |= temp << bitwritecount;
            bitwritecount += dataSize;
            if(bitwritecount >= 32) 
            {
                CPUWriteMemory(dest, data);
                dest += 4;
                data = 0;
                bitwritecount = 0;
            }
            mask <<= bits;
            bitcount += bits;
        }
    }
}
void CGbaEmu::BIOS_BgAffineSet()
{
    u32 src = reg[0].I;
    u32 dest = reg[1].I;
    int num = reg[2].I;
    for(int i = 0; i < num; i++) 
    {
        s32 cx = CPUReadMemory(src);
        src+=4;
        s32 cy = CPUReadMemory(src);
        src+=4;
        s16 dispx = CPUReadHalfWord(src);
        src+=2;
        s16 dispy = CPUReadHalfWord(src);
        src+=2;
        s16 rx = CPUReadHalfWord(src);
        src+=2;
        s16 ry = CPUReadHalfWord(src);
        src+=2;
        u16 theta = CPUReadHalfWord(src)>>8;
        src+=4; // keep structure alignment
        s32 a = sineTable[(theta+0x40)&255];
        s32 b = sineTable[theta];
        s16 dx =  (rx * a)>>14;
        s16 dmx = (rx * b)>>14;
        s16 dy =  (ry * b)>>14;
        s16 dmy = (ry * a)>>14;
        CPUWriteHalfWord(dest, dx);
        dest += 2;
        CPUWriteHalfWord(dest, -dmx);
        dest += 2;
        CPUWriteHalfWord(dest, dy);
        dest += 2;
        CPUWriteHalfWord(dest, dmy);
        dest += 2;
        s32 startx = cx - dx * dispx + dmx * dispy;
        s32 starty = cy - dy * dispx - dmy * dispy;
        CPUWriteMemory(dest, startx);
        dest += 4;
        CPUWriteMemory(dest, starty);
        dest += 4;
    }
}
void CGbaEmu::BIOS_CpuSet()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 cnt = reg[2].I;
    if(((source & 0xe000000) == 0) ||
    ((source + (((cnt << 11)>>9) & 0x1fffff)) & 0xe000000) == 0)
        return;
    int count = cnt & 0x1FFFFF;
    // 32-bit ?
    if((cnt >> 26) & 1) 
    {
        // needed for 32-bit mode!
        source &= 0xFFFFFFFC;
        dest &= 0xFFFFFFFC;
        // fill ?
        if((cnt >> 24) & 1) 
        {
            u32 value = CPUReadMemory(source);
            while(count) 
            {
                CPUWriteMemory(dest, value);
                dest += 4;
                count--;
            }
        }
        else 
        {
            // copy
            while(count) 
            {
                CPUWriteMemory(dest, CPUReadMemory(source));
                source += 4;
                dest += 4;
                count--;
            }
        }
    }
    else 
    {
        // 16-bit fill?
        if((cnt >> 24) & 1) 
        {
            u16 value = CPUReadHalfWord(source);
            while(count) 
            {
                CPUWriteHalfWord(dest, value);
                dest += 2;
                count--;
            }
        }
        else 
        {
            // copy
            while(count) 
            {
                CPUWriteHalfWord(dest, CPUReadHalfWord(source));
                source += 2;
                dest += 2;
                count--;
            }
        }
    }
}
void CGbaEmu::BIOS_CpuFastSet()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 cnt = reg[2].I;
    if(((source & 0xe000000) == 0) ||
    ((source + (((cnt << 11)>>9) & 0x1fffff)) & 0xe000000) == 0)
        return;
    // needed for 32-bit mode!
    source &= 0xFFFFFFFC;
    dest &= 0xFFFFFFFC;
    int count = cnt & 0x1FFFFF;
    // fill?
    if((cnt >> 24) & 1) 
    {
        while(count > 0) 
        {
            // BIOS always transfers 32 bytes at a time
            u32 value = CPUReadMemory(source);
            for(int i = 0; i < 8; i++) 
            {
                CPUWriteMemory(dest, value);
                dest += 4;
            }
            count -= 8;
        }
    }
    else 
    {
        // copy
        while(count > 0) 
        {
            // BIOS always transfers 32 bytes at a time
            for(int i = 0; i < 8; i++) 
            {
                CPUWriteMemory(dest, CPUReadMemory(source));
                source += 4;
                dest += 4;
            }
            count -= 8;
        }
    }
}
void CGbaEmu::BIOS_Diff8bitUnFilterWram()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff) & 0xe000000) == 0))
        return;  
    int len = header >> 8;
    u8 data = CPUReadByte(source++);
    CPUWriteByte(dest++, data);
    len--;
    while(len > 0) 
    {
        u8 diff = CPUReadByte(source++);
        data += diff;
        CPUWriteByte(dest++, data);
        len--;
    }
}
void CGbaEmu::BIOS_Diff8bitUnFilterVram()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff)) & 0xe000000) == 0)
        return;  
    int len = header >> 8;
    u8 data = CPUReadByte(source++);
    u16 writeData = data;
    int shift = 8;
    int bytes = 1;
    while(len >= 2) 
    {
        u8 diff = CPUReadByte(source++);
        data += diff;
        writeData |= (data << shift);
        bytes++;
        shift += 8;
        if(bytes == 2) 
        {
            CPUWriteHalfWord(dest, writeData);
            dest += 2;
            len -= 2;
            bytes = 0;
            writeData = 0;
            shift = 0;
        }
    }
}
void CGbaEmu::BIOS_Diff16bitUnFilter()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff)) & 0xe000000) == 0)
        return;  
    int len = header >> 8;
    u16 data = CPUReadHalfWord(source);
    source += 2;
    CPUWriteHalfWord(dest, data);
    dest += 2;
    len -= 2;
    while(len >= 2) 
    {
        u16 diff = CPUReadHalfWord(source);
        source += 2;
        data += diff;
        CPUWriteHalfWord(dest, data);
        dest += 2;
        len -= 2;
    }
}
void CGbaEmu::BIOS_Div()
{
    int number = reg[0].I;
    int denom = reg[1].I;
    if(denom != 0) 
    {
        reg[0].I = number / denom;
        reg[1].I = number % denom;
        s32 temp = (s32)reg[0].I;
        reg[3].I = temp < 0 ? (u32)-temp : (u32)temp;
    }
}
void CGbaEmu::BIOS_DivARM()
{
    u32 temp = reg[0].I;
    reg[0].I = reg[1].I;
    reg[1].I = temp;
    BIOS_Div();
}
void CGbaEmu::BIOS_HuffUnComp()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff)) & 0xe000000) == 0)
        return;  
    u8 treeSize = CPUReadByte(source++);
    u32 treeStart = source;
    source += (treeSize<<1) + 1;
    int len = header >> 8;
    u32 mask = 0x80000000;
    u32 data = CPUReadMemory(source);
    source += 4;
    int pos = 0;
    u8 rootNode = CPUReadByte(treeStart);
    u8 currentNode = rootNode;
    bool writeData = false;
    int byteShift = 0;
    int byteCount = 0;
    u32 writeValue = 0;
    if((header & 0x0F) == 8) 
    {
        while(len > 0) 
        {
            // take left
            if(pos == 0)
                pos++;
            else
                pos += (((currentNode & 0x3F)+1)<<1);
            if(data & mask) 
            {
                // right
                if(currentNode & 0x40)
                    writeData = true;
                currentNode = CPUReadByte(treeStart+pos+1);
            }
            else 
            {
                // left
                if(currentNode & 0x80)
                    writeData = true;
                currentNode = CPUReadByte(treeStart+pos);
            }
            if(writeData) 
            {
                writeValue |= (currentNode << byteShift);
                byteCount++;
                byteShift += 8;
                pos = 0;
                currentNode = rootNode;
                writeData = false;
                if(byteCount == 4) 
                {
                    byteCount = 0;
                    byteShift = 0;
                    CPUWriteMemory(dest, writeValue);
                    writeValue = 0;
                    dest += 4;
                    len -= 4;
                }
            }
            mask >>= 1;
            if(mask == 0) 
            {
                mask = 0x80000000;
                data = CPUReadMemory(source);
                source += 4;
            }
        }
    }
    else 
    {
        int halfLen = 0;
        int value = 0;
        while(len > 0) 
        {
            // take left
            if(pos == 0)
                pos++;
            else
                pos += (((currentNode & 0x3F)+1)<<1);
            if((data & mask)) 
            {
                // right
                if(currentNode & 0x40)
                    writeData = true;
                currentNode = CPUReadByte(treeStart+pos+1);
            }
            else 
            {
                // left
                if(currentNode & 0x80)
                    writeData = true;
                currentNode = CPUReadByte(treeStart+pos);
            }
            if(writeData) 
            {
                if(halfLen == 0)
                    value |= currentNode;
                else
                    value |= (currentNode<<4);
                halfLen += 4;
                if(halfLen == 8) 
                {
                    writeValue |= (value << byteShift);
                    byteCount++;
                    byteShift += 8;
                    halfLen = 0;
                    value = 0;
                    if(byteCount == 4) 
                    {
                        byteCount = 0;
                        byteShift = 0;
                        CPUWriteMemory(dest, writeValue);
                        dest += 4;
                        writeValue = 0;
                        len -= 4;
                    }
                }
                pos = 0;
                currentNode = rootNode;
                writeData = false;
            }
            mask >>= 1;
            if(mask == 0) 
            {
                mask = 0x80000000;
                data = CPUReadMemory(source);
                source += 4;
            }
        }
    }
}
void CGbaEmu::BIOS_LZ77UnCompVram()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff)) & 0xe000000) == 0)
        return;    
    int byteCount = 0;
    int byteShift = 0;
    u32 writeValue = 0;
    int len = header >> 8;
    while(len > 0) 
    {
        u8 d = CPUReadByte(source++);
        if(d) 
        {
            for(int i = 0; i < 8; i++) 
            {
                if(d & 0x80) 
                {
                    u16 data = CPUReadByte(source++) << 8;
                    data |= CPUReadByte(source++);
                    int length = (data >> 12) + 3;
                    int offset = (data & 0x0FFF);
                    u32 windowOffset = dest + byteCount - offset - 1;
                    for(int i = 0; i < length; i++) 
                    {
                        writeValue |= (CPUReadByte(windowOffset++) << byteShift);
                        byteShift += 8;
                        byteCount++;
                        if(byteCount == 2) 
                        {
                            CPUWriteHalfWord(dest, writeValue);
                            dest += 2;
                            byteCount = 0;
                            byteShift = 0;
                            writeValue = 0;
                        }
                        len--;
                        if(len == 0)
                            return;
                    }
                }
                else 
                {
                    writeValue |= (CPUReadByte(source++) << byteShift);
                    byteShift += 8;
                    byteCount++;
                    if(byteCount == 2) 
                    {
                        CPUWriteHalfWord(dest, writeValue);
                        dest += 2;
                        byteCount = 0;
                        byteShift = 0;
                        writeValue = 0;
                    }
                    len--;
                    if(len == 0)
                        return;
                }
                d <<= 1;
            }
        }
        else 
        {
            for(int i = 0; i < 8; i++) 
            {
                writeValue |= (CPUReadByte(source++) << byteShift);
                byteShift += 8;
                byteCount++;
                if(byteCount == 2) 
                {
                    CPUWriteHalfWord(dest, writeValue);
                    dest += 2;      
                    byteShift = 0;
                    byteCount = 0;
                    writeValue = 0;
                }
                len--;
                if(len == 0)
                    return;
            }
        }
    }
}
void CGbaEmu::BIOS_LZ77UnCompWram()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff)) & 0xe000000) == 0)
        return;  
    int len = header >> 8;
    while(len > 0) 
    {
        u8 d = CPUReadByte(source++);
        if(d) 
        {
            for(int i = 0; i < 8; i++) 
            {
                if(d & 0x80) 
                {
                    u16 data = CPUReadByte(source++) << 8;
                    data |= CPUReadByte(source++);
                    int length = (data >> 12) + 3;
                    int offset = (data & 0x0FFF);
                    u32 windowOffset = dest - offset - 1;
                    for(int i = 0; i < length; i++) 
                    {
                        CPUWriteByte(dest++, CPUReadByte(windowOffset++));
                        len--;
                        if(len == 0)
                            return;
                    }
                }
                else 
                {
                    CPUWriteByte(dest++, CPUReadByte(source++));
                    len--;
                    if(len == 0)
                        return;
                }
                d <<= 1;
            }
        }
        else 
        {
            for(int i = 0; i < 8; i++) 
            {
                CPUWriteByte(dest++, CPUReadByte(source++));
                len--;
                if(len == 0)
                    return;
            }
        }
    }
}
void CGbaEmu::BIOS_ObjAffineSet()
{
    u32 src = reg[0].I;
    u32 dest = reg[1].I;
    int num = reg[2].I;
    int offset = reg[3].I;
    for(int i = 0; i < num; i++) 
    {
        s16 rx = CPUReadHalfWord(src);
        src+=2;
        s16 ry = CPUReadHalfWord(src);
        src+=2;
        u16 theta = CPUReadHalfWord(src)>>8;
        src+=4; // keep structure alignment
        s32 a = (s32)sineTable[(theta+0x40)&255];
        s32 b = (s32)sineTable[theta];
        s16 dx =  ((s32)rx * a)>>14;
        s16 dmx = ((s32)rx * b)>>14;
        s16 dy =  ((s32)ry * b)>>14;
        s16 dmy = ((s32)ry * a)>>14;
        CPUWriteHalfWord(dest, dx);
        dest += offset;
        CPUWriteHalfWord(dest, -dmx);
        dest += offset;
        CPUWriteHalfWord(dest, dy);
        dest += offset;
        CPUWriteHalfWord(dest, dmy);
        dest += offset;
    }
}
void CGbaEmu::BIOS_RegisterRamReset()
{
    BIOS_RegisterRamReset(reg[0].I);
}
void CGbaEmu::BIOS_RLUnCompVram()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff)) & 0xe000000) == 0)
        return;  
    int len = header >> 8;
    int byteCount = 0;
    int byteShift = 0;
    u32 writeValue = 0;
    while(len > 0) 
    {
        u8 d = CPUReadByte(source++);
        int l = d & 0x7F;
        if(d & 0x80) 
        {
            u8 data = CPUReadByte(source++);
            l += 3;
            for(int i = 0;i < l; i++) 
            {
                writeValue |= (data << byteShift);
                byteShift += 8;
                byteCount++;
                if(byteCount == 2) 
                {
                    CPUWriteHalfWord(dest, writeValue);
                    dest += 2;
                    byteCount = 0;
                    byteShift = 0;
                    writeValue = 0;
                }
                len--;
                if(len == 0)
                    return;
            }
        }
        else 
        {
            l++;
            for(int i = 0; i < l; i++) 
            {
                writeValue |= (CPUReadByte(source++) << byteShift);
                byteShift += 8;
                byteCount++;
                if(byteCount == 2) 
                {
                    CPUWriteHalfWord(dest, writeValue);
                    dest += 2;
                    byteCount = 0;
                    byteShift = 0;
                    writeValue = 0;
                }
                len--;
                if(len == 0)
                    return;
            }
        }
    }
}
void CGbaEmu::BIOS_RLUnCompWram()
{
    u32 source = reg[0].I;
    u32 dest = reg[1].I;
    u32 header = CPUReadMemory(source);
    source += 4;
    if(((source & 0xe000000) == 0) ||
    ((source + ((header >> 8) & 0x1fffff)) & 0xe000000) == 0)
        return;  
    int len = header >> 8;
    while(len > 0) 
    {
        u8 d = CPUReadByte(source++);
        int l = d & 0x7F;
        if(d & 0x80) 
        {
            u8 data = CPUReadByte(source++);
            l += 3;
            for(int i = 0;i < l; i++) 
            {
                CPUWriteByte(dest++, data);
                len--;
                if(len == 0)
                    return;
            }
        }
        else 
        {
            l++;
            for(int i = 0; i < l; i++) 
            {
                CPUWriteByte(dest++,  CPUReadByte(source++));
                len--;
                if(len == 0)
                    return;
            }
        }
    }
}
void CGbaEmu::BIOS_SoftReset()
{
    armState = true;
    armMode = 0x1F;
    armIrqEnable = false;
    C_FLAG = V_FLAG = N_FLAG = Z_FLAG = false;
    reg[13].I = 0x03007F00;
    reg[14].I = 0x00000000;
    reg[16].I = 0x00000000;
    reg[R13_IRQ].I = 0x03007FA0;
    reg[R14_IRQ].I = 0x00000000;
    reg[SPSR_IRQ].I = 0x00000000;
    reg[R13_SVC].I = 0x03007FE0;  
    reg[R14_SVC].I = 0x00000000;
    reg[SPSR_SVC].I = 0x00000000;
    u8 b = internalRAM[0x7ffa];
    memset(&internalRAM[0x7e00], 0, 0x200);
    if(b) 
    {
        armNextPC = 0x02000000;
        reg[15].I = 0x02000004;
    }
    else 
    {
        armNextPC = 0x08000000;
        reg[15].I = 0x08000004;
    }
}
void CGbaEmu::BIOS_Sqrt()
{
    reg[0].I = (u32)sqrt((double)reg[0].I);
}
void CGbaEmu::BIOS_MidiKey2Freq()
{
    int freq = CPUReadMemory(reg[0].I+4);
    double tmp;
    tmp = ((double)(180 - reg[1].I)) - ((double)reg[2].I / 256.f);
    tmp = pow((double)2.f, tmp / 12.f);
    reg[0].I = (int)((double)freq / tmp);
}
void CGbaEmu::BIOS_SndDriverJmpTableCopy()
{
    for(int i = 0; i < 0x24; i++) 
    {
        CPUWriteMemory(reg[0].I, 0x9c);
        reg[0].I += 4;
    }
}
int CGbaEmu::utilWriteBMPHeader(CFileBase *file, int w, int h)
{
    ASSERT(file);

    u32 t;
    
    file->Putc('B');
    file->Putc('M');
    
    t = 54 + w*h*3; file->Write(&t,4);  
    t = 0; file->Write(&t,4);   //reservered
    t = 0x36; file->Write(&t,4);
    t = 0x28; file->Write(&t,4);
    t = w; file->Write(&t,4);
    t = h; file->Write(&t,4);
    t = 1; file->Write(&t,2);
    t = 24;file->Write(&t,2);
    t = 0; file->Write(&t,4);
    t = 3*w*h; file->Write(&t,4);
    //BMP has 54 bytes header
    file->FillBlock(54 - file->GetOffset(),0);

    return OK;
}

int CGbaEmu::utilWriteBMPBits(CFileBase *file, int w, int h, u8 *pix)
{
    ASSERT(file && pix);

    u8 writeBuffer[512 * 3];
    u8 *b = writeBuffer;
    int sizeX = w;
    int sizeY = h;

    switch(systemColorDepth) 
    {
        case 16:
        {
            u16 *p = (u16 *)(pix+(w+2)*(h)*2); // skip first black line
            for(int y = 0; y < sizeY; y++) 
            {
                for(int x = 0; x < sizeX; x++) 
                {
                    u16 v = *p++;
                    *b++ = (u8)((v >> systemBlueShift) & 0x01f) << 3; // B      
                    *b++ = (u8)((v >> systemGreenShift) & 0x001f) << 3; // G 
                    *b++ = (u8)((v >> systemRedShift) & 0x001f) << 3; // R
                }
                p++; // skip black pixel for filters
                p++; // skip black pixel for filters
                p -= (w+2)<<1;
                file->Write(writeBuffer,3*w);
                b = writeBuffer;
            }
        }
        break;
        case 24:
        {
            u8 *pixU8 = (u8 *)pix+3*w*(h-1);
            for(int y = 0; y < sizeY; y++) 
            {
                for(int x = 0; x < sizeX; x++) 
                {
                    if(systemRedShift > systemBlueShift) 
                    {
                        *b++ = *pixU8++; // B
                        *b++ = *pixU8++; // G
                        *b++ = *pixU8++; // R
                    }
                    else 
                    {
                        int red = *pixU8++;
                        int green = *pixU8++;
                        int blue = *pixU8++;
                        *b++ = blue;
                        *b++ = green;
                        *b++ = red;
                    }
                }
                pixU8 -= 2*3*w;
                file->Write(writeBuffer,3*w);
                b = writeBuffer;
            }
        }
        break;
        case 32:
        {
            u32 *pixU32 = (u32 *)(pix+4*(w+1)*(h));
            for(int y = 0; y < sizeY; y++) 
            {
                for(int x = 0; x < sizeX; x++) 
                {
                    u32 v = *pixU32++;
                    *b++ = (u8)((v >> systemBlueShift) & 0x001f) << 3; // B     
                    *b++ = (u8)((v >> systemGreenShift) & 0x001f) << 3; // G
                    *b++ = (u8)((v >> systemRedShift) & 0x001f) << 3; // R
                }
                pixU32++;
                pixU32 -= 2*(w+1);
                file->Write(writeBuffer,3*w);
                b = writeBuffer;
            }
        }
        break;
    }

    return OK;
}
bool CGbaEmu::utilWriteBMPFile(CFileBase *file, int w, int h, u8 *pix)
{
    ASSERT(file);

    file->SetSize(0);
    this->utilWriteBMPHeader(file,w,h);
    this->utilWriteBMPBits(file,w,h,pix);

    return true;
}
int CGbaEmu::gbaWriteBMPFile(CFileBase *file)
{
    ASSERT(file);
    return utilWriteBMPFile(file,240, 160, pix);
}
int CGbaEmu::gbaWriteBMPFile(char *filename)
{
    ASSERT(filename);
    
    CFile file;
    
    file.Init();
    file.OpenFile(filename,"wb+");
    return this->gbaWriteBMPFile(&file);
}

int CGbaEmu::CPULoadRom(char *fn)
{
    ASSERT(fn);
    CFile file;

    file.Init();
    ASSERT( file.OpenFile(fn,"rb") );
    
    return this->CPULoadRom(&file);
}

int CGbaEmu::CPUWriteBatteryFile(CFileBase *file)
{
    ASSERT(file);

    file->SetSize(0);

    if(gbaSaveType == 0) 
    {
        if(eepromInUse)
            gbaSaveType = 3;
        else switch(saveType) 
        {
        case 1:
            gbaSaveType = 1;
            break;
        case 2:
            gbaSaveType = 2;
            break;
        }
    }
    
    if(gbaSaveType) 
    {
        if(gbaSaveType != 3) 
        {
            if(gbaSaveType == 2) 
                file->Write(flashSaveMemory, flashSize);
            else 
                file->Write(flashSaveMemory, 0x10000);
        }
        else 
        {
            file->Write(eepromData, eepromSize);
        }
    }

    return OK;
}

int CGbaEmu::CPUReadBatteryFile(CFileBase *file)
{
    ASSERT(file);
    file->Seek(0);
    
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    
    if(file->GetSize() == 512 || file->GetSize() == 0x2000) 
    {
        file->Read(this->eepromData,file->GetSize());
    }
    else 
    {
        if(file->GetSize() == 0x20000) 
        {
            file->Read(flashSaveMemory,0x20000);
            flashSetSize(0x20000);
        } 
        else 
        {
            file->Read(flashSaveMemory,0x10000);
            flashSetSize(0x10000);
        }
    }
    return OK;
}

void CGbaEmu::flashSetSize(int size)
{
    flashSize = size;
    if(size == 0x10000) 
    {
        flashDeviceID = 0x1b;
        flashManufacturerID = 0x32;
    }
    else 
    {
        flashDeviceID = 0x13; //0x09;
        flashManufacturerID = 0x62; //0xc2;
    }
}

int CGbaEmu::CPUWriteBatteryFile(char *fn)
{
    ASSERT(fn);

    CFile file;
    file.Init();
    ASSERT( file.OpenFile(fn,"wb+") );

    return this->CPUWriteBatteryFile(&file);
}


int CGbaEmu::CPUReadBatteryFile(char *fn)
{
    ASSERT(fn);

    CFile file;
    file.Init();
    ASSERT( file.OpenFile(fn,"rb") );

    return this->CPUReadBatteryFile(&file);

}

struct tm * CGbaEmu::gbaGetTime()
{
#if _LINUX_ || _WIN32_
    time_t t = time(NULL);
    return localtime(&t);
#endif

#if _WINCE_
    static struct tm t;

    SYSTEMTIME st;
    GetSystemTime( &st);
    
    t.tm_year = st.wYear;
    t.tm_mon = st.wMonth;
    t.tm_mday = st.wMonth;
    t.tm_hour = st.wHour;
    t.tm_min = st.wMinute;
    t.tm_sec = st.wSecond;
    
    return &t;
#endif

    return NULL;
}

int CGbaEmu::CPUWriteState(CFileBase *file)
{
    ASSERT(file);
    file->SetSize(0);

    int t;

    file->Write(&rom[0xa0],16);
    t = useBios; file->Write(&t,4);
    file->Write(this->reg,sizeof(reg_pair)*45);
        
    file->Write(&DISPCNT,sizeof(u16));
    file->Write(&DISPSTAT,sizeof(u16));
    file->Write(&VCOUNT,sizeof(u16));
    file->Write(&BG0CNT,sizeof(u16));
    file->Write(&BG1CNT,sizeof(u16));
    file->Write(&BG2CNT,sizeof(u16));
    file->Write(&BG3CNT,sizeof(u16));
    file->Write(&BG0HOFS,sizeof(u16));
    file->Write(&BG0VOFS,sizeof(u16));
    file->Write(&BG1HOFS,sizeof(u16));
    file->Write(&BG1VOFS,sizeof(u16));
    file->Write(&BG2HOFS,sizeof(u16));
    file->Write(&BG2VOFS,sizeof(u16));
    file->Write(&BG3HOFS,sizeof(u16));
    file->Write(&BG3VOFS,sizeof(u16));
    file->Write(&BG2PA,sizeof(u16));
    file->Write(&BG2PB,sizeof(u16));
    file->Write(&BG2PC,sizeof(u16));
    file->Write(&BG2PD,sizeof(u16));
    file->Write(&BG2X_L,sizeof(u16));
    file->Write(&BG2X_H,sizeof(u16));
    file->Write(&BG2Y_L,sizeof(u16));
    file->Write(&BG2Y_H,sizeof(u16));
    file->Write(&BG3PA,sizeof(u16));
    file->Write(&BG3PB,sizeof(u16));
    file->Write(&BG3PC,sizeof(u16));
    file->Write(&BG3PD,sizeof(u16));
    file->Write(&BG3X_L,sizeof(u16));
    file->Write(&BG3X_H,sizeof(u16));
    file->Write(&BG3Y_L,sizeof(u16));
    file->Write(&BG3Y_H,sizeof(u16));
    file->Write(&WIN0H,sizeof(u16));
    file->Write(&WIN1H,sizeof(u16));
    file->Write(&WIN0V,sizeof(u16));
    file->Write(&WIN1V,sizeof(u16));
    file->Write(&WININ,sizeof(u16));
    file->Write(&WINOUT,sizeof(u16));
    file->Write(&MOSAIC,sizeof(u16));
    file->Write(&BLDMOD,sizeof(u16));
    file->Write(&COLEV,sizeof(u16));
    file->Write(&COLY,sizeof(u16));
    file->Write(&DM0SAD_L,sizeof(u16));
    file->Write(&DM0SAD_H,sizeof(u16));
    file->Write(&DM0DAD_L,sizeof(u16));
    file->Write(&DM0DAD_H,sizeof(u16));
    file->Write(&DM0CNT_L,sizeof(u16));
    file->Write(&DM0CNT_H,sizeof(u16));
    file->Write(&DM1SAD_L,sizeof(u16));
    file->Write(&DM1SAD_H,sizeof(u16));
    file->Write(&DM1DAD_L,sizeof(u16));
    file->Write(&DM1DAD_H,sizeof(u16));
    file->Write(&DM1CNT_L,sizeof(u16));
    file->Write(&DM1CNT_H,sizeof(u16));
    file->Write(&DM2SAD_L,sizeof(u16));
    file->Write(&DM2SAD_H,sizeof(u16));
    file->Write(&DM2DAD_L,sizeof(u16));
    file->Write(&DM2DAD_H,sizeof(u16));
    file->Write(&DM2CNT_L,sizeof(u16));
    file->Write(&DM2CNT_H,sizeof(u16));
    file->Write(&DM3SAD_L,sizeof(u16));
    file->Write(&DM3SAD_H,sizeof(u16));
    file->Write(&DM3DAD_L,sizeof(u16));
    file->Write(&DM3DAD_H,sizeof(u16));
    file->Write(&DM3CNT_L,sizeof(u16));
    file->Write(&DM3CNT_H,sizeof(u16));
    file->Write(&TM0D,sizeof(u16));
    file->Write(&TM0CNT,sizeof(u16));
    file->Write(&TM1D,sizeof(u16));
    file->Write(&TM1CNT,sizeof(u16));
    file->Write(&TM2D,sizeof(u16));
    file->Write(&TM2CNT,sizeof(u16));
    file->Write(&TM3D,sizeof(u16));
    file->Write(&TM3CNT,sizeof(u16));
    file->Write(&P1,sizeof(u16));
    file->Write(&IE,sizeof(u16));
    file->Write(&IF,sizeof(u16));
    file->Write(&IME,sizeof(u16));
    file->Write(&holdState,sizeof(bool));
    file->Write(&holdType,sizeof(int));
    file->Write(&lcdTicks,sizeof(int));
    file->Write(&timer0On,sizeof(bool));
    file->Write(&timer0Ticks,sizeof(int));
    file->Write(&timer0Reload,sizeof(int));
    file->Write(&timer0ClockReload,sizeof(int));
    file->Write(&timer1On,sizeof(bool));
    file->Write(&timer1Ticks,sizeof(int));
    file->Write(&timer1Reload,sizeof(int));
    file->Write(&timer1ClockReload,sizeof(int));
    file->Write(&timer2On,sizeof(bool));
    file->Write(&timer2Ticks,sizeof(int));
    file->Write(&timer2Reload,sizeof(int));
    file->Write(&timer2ClockReload,sizeof(int));
    file->Write(&timer3On,sizeof(bool));
    file->Write(&timer3Ticks,sizeof(int));
    file->Write(&timer3Reload,sizeof(int));
    file->Write(&timer3ClockReload,sizeof(int));
    file->Write(&dma0Source,sizeof(u32));
    file->Write(&dma0Dest,sizeof(u32));
    file->Write(&dma1Source,sizeof(u32));
    file->Write(&dma1Dest,sizeof(u32));
    file->Write(&dma2Source,sizeof(u32));
    file->Write(&dma2Dest,sizeof(u32));
    file->Write(&dma3Source,sizeof(u32));
    file->Write(&dma3Dest,sizeof(u32));
    file->Write(&fxOn,sizeof(bool));
    file->Write(&windowOn,sizeof(bool));
    file->Write(&N_FLAG,sizeof(bool));
    file->Write(&C_FLAG,sizeof(bool));
    file->Write(&Z_FLAG,sizeof(bool));
    file->Write(&V_FLAG,sizeof(bool));
    file->Write(&armState,sizeof(bool));
    file->Write(&armIrqEnable,sizeof(bool));
    file->Write(&armNextPC,sizeof(u32));
    file->Write(&armMode,sizeof(int));
    file->Write(&saveType,sizeof(int));

    t = stopState; file->Write(&t,4);
    t = intState; file->Write(&t,4);
    
    file->Write(internalRAM, 0x8000);
    file->Write(paletteRAM, 0x400);
    file->Write(workRAM, 0x40000);
    file->Write(vram, 0x20000);
    file->Write(oam, 0x400);
    file->Write(ioMem, 0x400);
    
    eepromSaveGame(file);
    flashSaveGame(file);
    rtcSaveGame(file);

    return OK;
    
}

int CGbaEmu::eepromSaveGame(CFileBase *file)
{
    int t;

    ASSERT(file);   
    
    file->Write(&eepromMode,sizeof(int));
    file->Write(&eepromByte,sizeof(int));
    file->Write(&eepromBits,sizeof(int));
    file->Write(&eepromAddress,sizeof(int));
    file->Write(&eepromInUse,sizeof(bool));
    file->Write(&eepromData[0],512);
    file->Write(&eepromBuffer[0],16);

    t = eepromSize; file->Write(&t,4);
    file->Write(eepromData, 0x2000);

    return OK;
}

int CGbaEmu::flashSaveGame(CFileBase *file)
{
    ASSERT(file);
    file->Write(&flashState,sizeof(int));
    file->Write(&flashReadState,sizeof(int));
    file->Write(&flashSize,sizeof(int));
    file->Write(&flashBank,sizeof(int));
    file->Write(&flashSaveMemory[0],0x20000);
    return OK;
}

int CGbaEmu::rtcSaveGame(CFileBase *file)
{
    ASSERT(file);
    file->Write(rtcClockData,sizeof(RTCCLOCKDATA));
    return OK;
}

int CGbaEmu::rtcReadGame(CFileBase *file)
{
    ASSERT(file);
    file->Read(rtcClockData,sizeof(RTCCLOCKDATA));
    return OK;
}

int CGbaEmu::CPUReadState(CFileBase *file)
{
    ASSERT(file);
    file->Seek(0);
    
    int t;
    char romname[17];

    file->Read(romname,16);
    file->Read(&t,4);
    ASSERT( (t!=0) == this->useBios);

    file->Read(this->reg,45*sizeof(reg_pair));

    file->Read(&DISPCNT,sizeof(u16));
    file->Read(&DISPSTAT,sizeof(u16));
    file->Read(&VCOUNT,sizeof(u16));
    file->Read(&BG0CNT,sizeof(u16));
    file->Read(&BG1CNT,sizeof(u16));
    file->Read(&BG2CNT,sizeof(u16));
    file->Read(&BG3CNT,sizeof(u16));
    file->Read(&BG0HOFS,sizeof(u16));
    file->Read(&BG0VOFS,sizeof(u16));
    file->Read(&BG1HOFS,sizeof(u16));
    file->Read(&BG1VOFS,sizeof(u16));
    file->Read(&BG2HOFS,sizeof(u16));
    file->Read(&BG2VOFS,sizeof(u16));
    file->Read(&BG3HOFS,sizeof(u16));
    file->Read(&BG3VOFS,sizeof(u16));
    file->Read(&BG2PA,sizeof(u16));
    file->Read(&BG2PB,sizeof(u16));
    file->Read(&BG2PC,sizeof(u16));
    file->Read(&BG2PD,sizeof(u16));
    file->Read(&BG2X_L,sizeof(u16));
    file->Read(&BG2X_H,sizeof(u16));
    file->Read(&BG2Y_L,sizeof(u16));
    file->Read(&BG2Y_H,sizeof(u16));
    file->Read(&BG3PA,sizeof(u16));
    file->Read(&BG3PB,sizeof(u16));
    file->Read(&BG3PC,sizeof(u16));
    file->Read(&BG3PD,sizeof(u16));
    file->Read(&BG3X_L,sizeof(u16));
    file->Read(&BG3X_H,sizeof(u16));
    file->Read(&BG3Y_L,sizeof(u16));
    file->Read(&BG3Y_H,sizeof(u16));
    file->Read(&WIN0H,sizeof(u16));
    file->Read(&WIN1H,sizeof(u16));
    file->Read(&WIN0V,sizeof(u16));
    file->Read(&WIN1V,sizeof(u16));
    file->Read(&WININ,sizeof(u16));
    file->Read(&WINOUT,sizeof(u16));
    file->Read(&MOSAIC,sizeof(u16));
    file->Read(&BLDMOD,sizeof(u16));
    file->Read(&COLEV,sizeof(u16));
    file->Read(&COLY,sizeof(u16));
    file->Read(&DM0SAD_L,sizeof(u16));
    file->Read(&DM0SAD_H,sizeof(u16));
    file->Read(&DM0DAD_L,sizeof(u16));
    file->Read(&DM0DAD_H,sizeof(u16));
    file->Read(&DM0CNT_L,sizeof(u16));
    file->Read(&DM0CNT_H,sizeof(u16));
    file->Read(&DM1SAD_L,sizeof(u16));
    file->Read(&DM1SAD_H,sizeof(u16));
    file->Read(&DM1DAD_L,sizeof(u16));
    file->Read(&DM1DAD_H,sizeof(u16));
    file->Read(&DM1CNT_L,sizeof(u16));
    file->Read(&DM1CNT_H,sizeof(u16));
    file->Read(&DM2SAD_L,sizeof(u16));
    file->Read(&DM2SAD_H,sizeof(u16));
    file->Read(&DM2DAD_L,sizeof(u16));
    file->Read(&DM2DAD_H,sizeof(u16));
    file->Read(&DM2CNT_L,sizeof(u16));
    file->Read(&DM2CNT_H,sizeof(u16));
    file->Read(&DM3SAD_L,sizeof(u16));
    file->Read(&DM3SAD_H,sizeof(u16));
    file->Read(&DM3DAD_L,sizeof(u16));
    file->Read(&DM3DAD_H,sizeof(u16));
    file->Read(&DM3CNT_L,sizeof(u16));
    file->Read(&DM3CNT_H,sizeof(u16));
    file->Read(&TM0D,sizeof(u16));
    file->Read(&TM0CNT,sizeof(u16));
    file->Read(&TM1D,sizeof(u16));
    file->Read(&TM1CNT,sizeof(u16));
    file->Read(&TM2D,sizeof(u16));
    file->Read(&TM2CNT,sizeof(u16));
    file->Read(&TM3D,sizeof(u16));
    file->Read(&TM3CNT,sizeof(u16));
    file->Read(&P1,sizeof(u16));
    file->Read(&IE,sizeof(u16));
    file->Read(&IF,sizeof(u16));
    file->Read(&IME,sizeof(u16));
    file->Read(&holdState,sizeof(bool));
    file->Read(&holdType,sizeof(int));
    file->Read(&lcdTicks,sizeof(int));
    file->Read(&timer0On,sizeof(bool));
    file->Read(&timer0Ticks,sizeof(int));
    file->Read(&timer0Reload,sizeof(int));
    file->Read(&timer0ClockReload,sizeof(int));
    file->Read(&timer1On,sizeof(bool));
    file->Read(&timer1Ticks,sizeof(int));
    file->Read(&timer1Reload,sizeof(int));
    file->Read(&timer1ClockReload,sizeof(int));
    file->Read(&timer2On,sizeof(bool));
    file->Read(&timer2Ticks,sizeof(int));
    file->Read(&timer2Reload,sizeof(int));
    file->Read(&timer2ClockReload,sizeof(int));
    file->Read(&timer3On,sizeof(bool));
    file->Read(&timer3Ticks,sizeof(int));
    file->Read(&timer3Reload,sizeof(int));
    file->Read(&timer3ClockReload,sizeof(int));
    file->Read(&dma0Source,sizeof(u32));
    file->Read(&dma0Dest,sizeof(u32));
    file->Read(&dma1Source,sizeof(u32));
    file->Read(&dma1Dest,sizeof(u32));
    file->Read(&dma2Source,sizeof(u32));
    file->Read(&dma2Dest,sizeof(u32));
    file->Read(&dma3Source,sizeof(u32));
    file->Read(&dma3Dest,sizeof(u32));
    file->Read(&fxOn,sizeof(bool));
    file->Read(&windowOn,sizeof(bool));
    file->Read(&N_FLAG,sizeof(bool));
    file->Read(&C_FLAG,sizeof(bool));
    file->Read(&Z_FLAG,sizeof(bool));
    file->Read(&V_FLAG,sizeof(bool));
    file->Read(&armState,sizeof(bool));
    file->Read(&armIrqEnable,sizeof(bool));
    file->Read(&armNextPC,sizeof(u32));
    file->Read(&armMode,sizeof(int));
    file->Read(&saveType,sizeof(int));

    file->Read(&t,4);  stopState = (t!=0);
    file->Read(&t,4);  intState = (t!=0);

    file->Read(internalRAM, 0x8000);
    file->Read(paletteRAM, 0x400);
    file->Read(workRAM, 0x40000);
    file->Read(vram, 0x20000);
    file->Read(oam, 0x400);
    file->Read(ioMem, 0x400);
    
    eepromReadGame(file);
    flashReadGame(file);
    rtcReadGame(file);
    
    layerEnable = layerSettings & DISPCNT;
  
    CPUUpdateRender();
    CPUUpdateRenderBuffers(true);
    CPUUpdateWindow0();
    CPUUpdateWindow1();
    gbaSaveType = 0;

    switch(saveType) 
    {
    case 0:
        cpuSaveGameFunc = &CGbaEmu::flashSaveDecide;
        break;
    case 1:
        cpuSaveGameFunc = &CGbaEmu::sramWrite;
        gbaSaveType = 1;
        break;
    case 2:
        cpuSaveGameFunc = &CGbaEmu::flashWrite;
        gbaSaveType = 2;
        break;
    default:
        LOG("Unsupported save type %d\n", saveType);
        break;
    }

    if(eepromInUse)
        gbaSaveType = 3;

    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
  
    return OK;
}

int CGbaEmu::eepromReadGame(CFileBase *file)
{
    int t;

    ASSERT(file);   
    
    file->Read(&eepromMode,sizeof(int));
    file->Read(&eepromByte,sizeof(int));
    file->Read(&eepromBits,sizeof(int));
    file->Read(&eepromAddress,sizeof(int));
    file->Read(&eepromInUse,sizeof(bool));
    file->Read(&eepromData[0],512);
    file->Read(&eepromBuffer[0],16);
    file->Read(&t,4);eepromSize = t; 
    file->Read(eepromData, 0x2000);

    return OK;

}

int CGbaEmu::flashReadGame(CFileBase *file)
{
    ASSERT(file);
    file->Read(&flashState,sizeof(int));
    file->Read(&flashReadState,sizeof(int));
    file->Read(&flashSize,sizeof(int));
    file->Read(&flashBank,sizeof(int));
    file->Read(&flashSaveMemory[0],0x20000);
    return OK;
}


#if _GBA_VM

void * CGbaEmu::GbaVmMapMemory(u32 addr, int max_size)
{
    ASSERT(max_size >= 0);
    
    switch(addr >> 24) 
    {
        case 2:
            ASSERT(addr >= 0x02000000 && (addr+max_size) <= 0x02040000);
            return &this->workRAM[addr - 0x02000000];
        break;

        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            ASSERT(addr >= 0x08000000 && (addr+max_size) <= 0x10000000);
            return &rom[addr - 0x08000000];
        break;
    }

    return NULL;
}


int CGbaEmu::GbaVmSuspend()
{
    this->gba_vm_suspend = 1;
    return OK;
}

int CGbaEmu::GbaVmResume()
{
    this->gba_vm_suspend = 0;
    return OK;
}
#endif //_GBA_VM
