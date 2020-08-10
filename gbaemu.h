// GbaEmu.h: interface for the CGbaEmu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GBAEMU_H__DD960224_B14A_4AF4_9F6F_74C5845A6D21__INCLUDED_)
#define AFX_GBAEMU_H__DD960224_B14A_4AF4_9F6F_74C5845A6D21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "filebase.h"

#define _GBA_VM          1
#define _GBA_VM_BIOS_INT 0x3f   

#define USE_TICKS_AS    380
#define SOUND_MAGIC     0x60000000
#define SOUND_MAGIC_2   0x30000000
#define NOISE_MAGIC     5

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef QWORD u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

#if _WINCE_ || _WIN32_
typedef __int64 s64;
#endif

#if _LINUE_
typedef long long s64;
#endif

typedef struct {
  u8 *address;
  u32 mask;
} memoryMap;

typedef union {
    struct{
        u8 B0;
        u8 B1;
        u8 B2;
        u8 B3;
    } B;
    
    struct{
        u16 W0;
        u16 W1;
    } W;

    u32 I;
} reg_pair;

enum RTCSTATE { IDLE, COMMAND, DATA, READDATA };

typedef struct {
  u8 byte0;
  u8 byte1;
  u8 byte2;
  u8 command;
  int dataLen;
  int bits;
  RTCSTATE state;
  u8 data[12];
  u8 reserved[12];
  bool reserved2;
  u32 reserved3;
} RTCCLOCKDATA;

class CGbaEmu{
public:
    //come from gfx.cpp
    u32 *line0;
    u32 *line1;
    u32 *line2;
    u32 *line3;
    u32 *lineOBJ;
    u32 *lineOBJWin;
    u32 *lineMix;
    bool *gfxInWin0;
    bool *gfxInWin1;
    int gfxBG2Changed;
    int gfxBG3Changed;
    int gfxBG2X;
    int gfxBG2Y;
    int gfxBG2LastX;
    int gfxBG2LastY;
    int gfxBG3X;
    int gfxBG3Y;
    int gfxBG3LastX;
    int gfxBG3LastY;
    int gfxLastVCOUNT;
    //come from global.cpp
    reg_pair *reg;
    memoryMap *map;
    bool *ioReadable;
    bool N_FLAG;
    bool C_FLAG;
    bool Z_FLAG;
    bool V_FLAG;
    bool armState;
    bool armIrqEnable;
    u32 armNextPC;
    int armMode;
    u32 stop;
    int saveType;
    bool useBios;
    bool skipBios;
    int frameSkip;
    bool speedup;
    bool synchronize;
    bool cpuDisableSfx;
    bool cpuIsMultiBoot;
    bool parseDebug;
    int layerSettings;
    int layerEnable;
    bool speedHack;
    int cpuSaveType;
    bool cpuEnhancedDetection;
    bool cheatsEnabled;
    u8 *bios;
    u8 *rom;
    u8 *internalRAM;
    u8 *workRAM;
    u8 *paletteRAM;
    u8 *vram;
    u8 *pix;
    u8 *oam;
    u8 *ioMem;
    u16 DISPCNT;
    u16 DISPSTAT;
    u16 VCOUNT;
    u16 BG0CNT;
    u16 BG1CNT;
    u16 BG2CNT;
    u16 BG3CNT;
    u16 BG0HOFS;
    u16 BG0VOFS;
    u16 BG1HOFS;
    u16 BG1VOFS;
    u16 BG2HOFS;
    u16 BG2VOFS;
    u16 BG3HOFS;
    u16 BG3VOFS;
    u16 BG2PA;
    u16 BG2PB;
    u16 BG2PC;
    u16 BG2PD;
    u16 BG2X_L;
    u16 BG2X_H;
    u16 BG2Y_L;
    u16 BG2Y_H;
    u16 BG3PA;
    u16 BG3PB;
    u16 BG3PC;
    u16 BG3PD;
    u16 BG3X_L;
    u16 BG3X_H;
    u16 BG3Y_L;
    u16 BG3Y_H;
    u16 WIN0H;
    u16 WIN1H;
    u16 WIN0V;
    u16 WIN1V;
    u16 WININ;
    u16 WINOUT;
    u16 MOSAIC;
    u16 BLDMOD;
    u16 COLEV;
    u16 COLY;
    u16 DM0SAD_L;
    u16 DM0SAD_H;
    u16 DM0DAD_L;
    u16 DM0DAD_H;
    u16 DM0CNT_L;
    u16 DM0CNT_H;
    u16 DM1SAD_L;
    u16 DM1SAD_H;
    u16 DM1DAD_L;
    u16 DM1DAD_H;
    u16 DM1CNT_L;
    u16 DM1CNT_H;
    u16 DM2SAD_L;
    u16 DM2SAD_H;
    u16 DM2DAD_L;
    u16 DM2DAD_H;
    u16 DM2CNT_L;
    u16 DM2CNT_H;
    u16 DM3SAD_L;
    u16 DM3SAD_H;
    u16 DM3DAD_L;
    u16 DM3DAD_H;
    u16 DM3CNT_L;
    u16 DM3CNT_H;
    u16 TM0D;
    u16 TM0CNT;
    u16 TM1D;
    u16 TM1CNT;
    u16 TM2D;
    u16 TM2CNT;
    u16 TM3D;
    u16 TM3CNT;
    u16 P1;
    u16 IE;
    u16 IF;
    u16 IME;
    //come from gba.cpp
    int cpuDmaTicksToUpdate;
    int cpuDmaCount;
    bool cpuDmaHack;
    u32 cpuDmaLast;
    int dummyAddress;
    int *extCpuLoopTicks;
    int *extClockTicks;
    int *extTicks;
    int gbaSaveType;
    bool intState;
    bool stopState;
    bool holdState;
    int holdType;
    bool cpuSramEnabled;
    bool cpuFlashEnabled;
    bool cpuEEPROMEnabled;
    bool cpuEEPROMSensorEnabled;
    bool *freezeWorkRAM;
    bool *freezeInternalRAM;
    int lcdTicks;
    bool timer0On;
    int timer0Ticks;
    int timer0Reload;
    int timer0ClockReload;
    bool timer1On;
    int timer1Ticks;
    int timer1Reload;
    int timer1ClockReload;
    bool timer2On;
    int timer2Ticks;
    int timer2Reload;
    int timer2ClockReload;
    bool timer3On;
    int timer3Ticks;
    int timer3Reload;
    int timer3ClockReload;
    u32 dma0Source;
    u32 dma0Dest;
    u32 dma1Source;
    u32 dma1Dest;
    u32 dma2Source;
    u32 dma2Dest;
    u32 dma3Source;
    u32 dma3Dest;
    bool fxOn;
    bool windowOn;
    int frameCount;
    char *buffer;
    u32 lastTime;
    int count;
    int capture;
    int capturePrevious;
    int captureNumber;
    int cpuSavedTicks;
    void (CGbaEmu::*cpuSaveGameFunc)(u32,u8);
    void (CGbaEmu::*renderLine)();
    u8 *biosProtected;
    u8 *cpuBitsSet;
    u8 *cpuLowestBitSet;
    //come from sound.cpp
    int soundTicks;
    int soundQuality;
    int SOUND_CLOCK_TICKS;
    //come from vba.cpp     
    int emulating;
    //come from flash.cpp
    u8 *flashSaveMemory;
    int flashState;
    int flashReadState;
    int flashSize;
    int flashDeviceID;
    int flashManufacturerID;
    int flashBank;
    //come from eeprom.cpp
    int eepromMode;
    int eepromByte;
    int eepromBits;
    int eepromAddress;
    u8 *eepromData;
    u8 *eepromBuffer;
    bool eepromInUse;
    int eepromSize;
    //come from rtc.cpp
    RTCCLOCKDATA *rtcClockData;
    bool rtcEnabled;
    //come from agbprint.cpp
    bool agbPrintEnabled;
    bool agbPrintProtect;
    //come from system.cpp
    int  systemRedShift;
    int  systemGreenShift;
    int  systemBlueShift;
    int  systemColorDepth;
    int  systemDebug;
    int  systemVerbose;
    int  systemSaveUpdateCounter;
    int  systemFrameSkip;
    u32  *systemColorMap32;
    u16  *systemColorMap16;
    u16  *systemGbPalette;
    bool systemSoundOn;
    int systemRenderedFrames;
    //added by cxp
    u32 joypad;

#if _GBA_VM
    int gba_vm_suspend;
    int *param_vm_bios_int;
    int (*callback_vm_bios_int)(CGbaEmu *gba_emu,int *param);
#endif

public:

#if _GBA_VM
    int GbaVmSuspend();
    int GbaVmResume();
    void * GbaVmMapMemory(u32 addr,int max_size);
#endif

    int eepromReadGame(CFileBase *file);
    int flashReadGame(CFileBase *file);
    int CPUReadState(CFileBase *file);
    int rtcReadGame(CFileBase *file);
    int rtcSaveGame(CFileBase *file);
    int flashSaveGame(CFileBase *file);
    int eepromSaveGame(CFileBase *file);
    int CPUWriteState(CFileBase *file);
    struct tm *gbaGetTime();
    int gbaGetScreenSize(int *w,int *h);
    int CPUReadBatteryFile(char *fn);
    int CPUWriteBatteryFile(char *fn);
    void flashSetSize(int size);
    int CPUReadBatteryFile(CFileBase *file);
    int CPUWriteBatteryFile(CFileBase *file);
    int CPULoadRom(char *fn);
    int gbaWriteBMPFile(CFileBase *file);
    int gbaWriteBMPFile(char *filename);
    int utilWriteBMPBits(CFileBase *file,int w,int h,u8 *pix);
    int utilWriteBMPHeader(CFileBase *file,int w,int h);
    bool utilWriteBMPFile(CFileBase *file, int w, int h, u8 *pix);
    int InitColorMap();
    void BIOS_ArcTan();
    void BIOS_ArcTan2();
    void BIOS_BitUnPack();
    void BIOS_BgAffineSet();
    void BIOS_CpuSet();
    void BIOS_CpuFastSet();
    void BIOS_Diff8bitUnFilterWram();
    void BIOS_Diff8bitUnFilterVram();
    void BIOS_Diff16bitUnFilter();
    void BIOS_Div();
    void BIOS_DivARM();
    void BIOS_HuffUnComp();
    void BIOS_LZ77UnCompVram();
    void BIOS_LZ77UnCompWram();
    void BIOS_ObjAffineSet();
    void BIOS_RegisterRamReset();
    void BIOS_RLUnCompVram();
    void BIOS_RLUnCompWram();
    void BIOS_SoftReset();
    void BIOS_Sqrt();
    void BIOS_MidiKey2Freq();
    void BIOS_SndDriverJmpTableCopy();
    void systemSoundResume();
    void systemSoundPause();
    void CPUSoftwareInterrupt(int comment);
    void soundTick();
    void soundTimerOverflow(int timer);
    bool systemPauseOnFrame();
    void systemDrawScreen();
    void systemScreenCapture(int _iNum);
    int cheatsCheckKeys(u32 keys, u32 extended);
    void systemUpdateMotionSensor();
    bool systemReadJoypads();
    u32 systemReadJoypad(int option);
    void systemShowSpeed(int _iSpeed);
    void system10Frames(int _iRate);
    void systemFrame();
    void CPULoop(int ticks);
    void CPUInterrupt();
    u32 systemGetClock();
    void BIOS_RegisterRamReset(u32 flags);
    void flashReset();
    void eepromReset();
    void soundReset();
    void CPUReset();
    int CPUInit(CFileBase *bios_file, bool useBiosFile);
    void rtcEnable(bool e);
    bool rtcIsEnabled();
    u16 rtcRead(u32 address);
    u8 toBCD(u8 value);
    bool rtcWrite(u32 address, u16 value);
    void rtcReset();
    bool agbPrintWrite(u32 address, u16 value);
    void agbPrintReset();
    void agbPrintEnable(bool enable);
    bool agbPrintIsEnabled();
    void agbPrintFlush();
    void CPUWriteHalfWord(u32 address, u16 value);
    void CPUWriteByte(u32 address, u8 b);
    void doDMA(u32 &s, u32 &d, u32 si, u32 di, u32 c, int transfer32);
    void CPUCheckDMA(int reason, int dmamask);
    void CPUUpdateRender();
    void CPUCompareVCOUNT();
    void soundEvent(u32 address, u8 data);
    void CPUUpdateRegister(u32 address, u16 value);
    int systemGetSensorX();
    int systemGetSensorY();
    u8 flashRead(u32 address);
    int eepromRead(u32 /* address */);
    void eepromWrite(u32 /* address */, u8 value);
    u32 CPUReadMemory(u32 address);
    u32 CPUReadHalfWord(u32 address);
    u16 CPUReadHalfWordSigned(u32 address);
    u8 CPUReadByte(u32 address);
    void CPUWriteMemory(u32 address, u32 value);
    void CPUSwitchMode(int mode, bool saveState);
    void CPUUndefinedException();
    void CPUSoftwareInterrupt();
    void CPUSwitchMode(int mode, bool saveState, bool breakLoop);
    void CPUSwap(u32 *a, u32 *b);
    void CPUUpdateCPSR();
    void CPUUpdateFlags(bool breakLoop);
    void CPUUpdateFlags();
    u8 sramRead(u32 address);
    void sramWrite(u32 address, u8 byte);
    void flashWrite(u32 address, u8 byte);
    void flashSaveDecide(u32 address, u8 byte);
    void mode5RenderLine();
    void mode5RenderLineNoWindow();
    void mode5RenderLineAll();
    void mode4RenderLine();
    void mode4RenderLineNoWindow();
    void mode4RenderLineAll();
    void mode3RenderLine();
    void mode3RenderLineNoWindow();
    void mode3RenderLineAll();
    void mode2RenderLine();
    void mode2RenderLineNoWindow();
    void mode2RenderLineAll();
    void mode1RenderLine();
    void mode1RenderLineNoWindow();
    void mode1RenderLineAll();
    void mode0RenderLine();
    void mode0RenderLineNoWindow();
    void mode0RenderLineAll();
    void gfxClearArray(u32 *array);
    void gfxDrawTextScreen(u16 control, u16 hofs, u16 vofs, u32 *line);
    void gfxDrawRotScreen(u16 control, u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int& currentX, int& currentY,int changed,u32 *line);
    void gfxDrawRotScreen16Bit(u16 control,u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int& currentX, int& currentY,int changed,u32 *line);
    void gfxDrawRotScreen256(u16 control, u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int &currentX, int& currentY,int changed,u32 *line);
    void gfxDrawRotScreen16Bit160(u16 control,u16 x_l, u16 x_h,u16 y_l, u16 y_h,u16 pa,  u16 pb,u16 pc,  u16 pd,int& currentX, int& currentY, int changed,u32 *line);
    void gfxDrawSprites(u32 *lineOBJ);
    void gfxDrawOBJWin(u32 *lineOBJWin);
    u32 gfxIncreaseBrightness(u32 color, int coeff);
    void gfxIncreaseBrightness(u32 *line, int coeff);
    u32 gfxDecreaseBrightness(u32 color, int coeff);
    void gfxDecreaseBrightness(u32 *line, int coeff);
    u32 gfxAlphaBlend(u32 color, u32 color2, int ca, int cb);
    void gfxAlphaBlend(u32 *ta, u32 *tb, int ca, int cb);   
    void WRITE32LE(u32 *addr, u32 value);
    u32 READ32LE(u32 *addr);
    u16 READ16LE(u16 *addr);
    void WRITE16LE(u16 *addr, u16 value);
    int CPULoadRom(CFileBase *file);
    void CPUCleanUp();
    void CPUUpdateRenderBuffers(bool force);
    void CPUUpdateWindow1();
    void CPUUpdateWindow0();
    int CPUUpdateTicksAccess32(u32 address);
    int CPUUpdateTicksAccess16(u32 address);
    int CPUUpdateTicksAccessSeq32(u32 address);
    int CPUUpdateTicksAccessSeq16(u32 address);
    int CPUUpdateTicks();
    CGbaEmu();
    virtual ~CGbaEmu();
    int Init();
    int Destroy();
    int InitBasic();
};

#endif // !defined(AFX_GBAEMU_H__DD960224_B14A_4AF4_9F6F_74C5845A6D21__INCLUDED_)
