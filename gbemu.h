// GbEmu.h: interface for the CGbEmu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GBEMU_H__3E8A5D45_AA89_4ACC_AF00_A1E2448601E6__INCLUDED_)
#define AFX_GBEMU_H__3E8A5D45_AA89_4ACC_AF00_A1E2448601E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gb_common.h"
#include "filebase.h"

typedef union {
    struct {
#if GB_WORDS_BIGENDIAN
        u8 B1, B0;
#else
        u8 B0,B1;
#endif
    } B;
    u16 W;
} gbRegister;

struct mapperMBC1 {
  int mapperRAMEnable;
  int mapperROMBank;
  int mapperRAMBank;
  int mapperMemoryModel;
  int mapperROMHighAddress;
  int mapperRAMAddress;
};

struct mapperMBC2 {
  int mapperRAMEnable;
  int mapperROMBank;
};

struct mapperMBC3 {
  int mapperRAMEnable;
  int mapperROMBank;
  int mapperRAMBank;
  int mapperRAMAddress;
  int mapperClockLatch;
  int mapperClockRegister;
  int mapperSeconds;
  int mapperMinutes;
  int mapperHours;
  int mapperDays;
  int mapperControl;
  int mapperLSeconds;
  int mapperLMinutes;
  int mapperLHours;
  int mapperLDays;
  int mapperLControl;
  time_t mapperLastTime;
};

struct mapperMBC5 {
  int mapperRAMEnable;
  int mapperROMBank;
  int mapperRAMBank;
  int mapperROMHighAddress;
  int mapperRAMAddress;
  int isRumbleCartridge;
};

struct mapperMBC7 {
  int mapperRAMEnable;
  int mapperROMBank;
  int mapperRAMBank;
  int mapperRAMAddress;
  int cs;
  int sk;
  int state;
  int buffer;
  int idle;
  int count;
  int code;
  int address;
  int writeEnable;
  int value;
};

struct mapperHuC1 {
  int mapperRAMEnable;
  int mapperROMBank;
  int mapperRAMBank;
  int mapperMemoryModel;
  int mapperROMHighAddress;
  int mapperRAMAddress;
};

struct mapperHuC3 {
  int mapperRAMEnable;
  int mapperROMBank;
  int mapperRAMBank;
  int mapperRAMAddress;
  int mapperAddress;
  int mapperRAMFlag;
  int mapperRAMValue;
  int mapperRegister1;
  int mapperRegister2;
  int mapperRegister3;
  int mapperRegister4;
  int mapperRegister5;
  int mapperRegister6;
  int mapperRegister7;
  int mapperRegister8;
};


class CGbEmu{
public:
    gbRegister PC;
    gbRegister SP;
    gbRegister AF;
    gbRegister BC;
    gbRegister DE;
    gbRegister HL;
    u16 IFF;
    // 0xff04
    u8 register_DIV;
    // 0xff05
    u8 register_TIMA;
    // 0xff06
    u8 register_TMA;
    // 0xff07
    u8 register_TAC;
    // 0xff0f
    u8 register_IF;
    // 0xff40
    u8 register_LCDC;
    // 0xff41
    u8 register_STAT;
    // 0xff42
    u8 register_SCY;
    // 0xff43
    u8 register_SCX;
    // 0xff44
    u8 register_LY;
    // 0xff45
    u8 register_LYC;
    // 0xff46
    u8 register_DMA;
    // 0xff4a
    u8 register_WY;
    // 0xff4b
    u8 register_WX;
    // 0xff4f
    u8 register_VBK;
    // 0xff51
    u8 register_HDMA1;
    // 0xff52
    u8 register_HDMA2;
    // 0xff53
    u8 register_HDMA3;
    // 0xff54
    u8 register_HDMA4;
    // 0xff55
    u8 register_HDMA5;
    // 0xff70
    u8 register_SVBK;
    // 0xffff
    u8 register_IE;
    // ticks definition
    int GBDIV_CLOCK_TICKS;
    int GBLCD_MODE_0_CLOCK_TICKS;
    int GBLCD_MODE_1_CLOCK_TICKS;
    int GBLCD_MODE_2_CLOCK_TICKS;
    int GBLCD_MODE_3_CLOCK_TICKS;
    int GBLY_INCREMENT_CLOCK_TICKS;
    int GBTIMER_MODE_0_CLOCK_TICKS;
    int GBTIMER_MODE_1_CLOCK_TICKS;
    int GBTIMER_MODE_2_CLOCK_TICKS;
    int GBTIMER_MODE_3_CLOCK_TICKS;
    int GBSERIAL_CLOCK_TICKS;
    int GBSYNCHRONIZE_CLOCK_TICKS;
    // state variables
    // interrupt
    int gbInterrupt;
    int gbInterruptWait;
    // serial
    int gbSerialOn;
    int gbSerialTicks;
    int gbSerialBits;
    // timer
    int gbTimerOn;
    int gbTimerTicks;
    int gbTimerClockTicks;
    int gbTimerMode;
    // lcd
    int gbLcdMode;
    int gbLcdTicks;
    int gbLcdLYIncrementTicks;
    // div
    int gbDivTicks;
    // cgb
    int gbVramBank;
    int gbWramBank;
    int gbHdmaSource;
    int gbHdmaDestination;
    int gbHdmaBytes;
    int gbHdmaOn;
    int gbSpeed;
    // frame counting
    int gbFrameCount;
    int gbFrameSkip;
    int gbFrameSkipCount;
    // timing
    u32 gbLastTime;
    u32 gbElapsedTime;
    u32 gbTimeNow;
    int gbSynchronizeTicks;
    // emulator features
    int gbBattery;
    int gbCaptureNumber;
    bool gbCapture;
    bool gbCapturePrevious;

    void (CGbEmu::*mapper)(u16,u8);
    void (CGbEmu::*mapperRAM)(u16,u8);
    u8 (CGbEmu::*mapperReadRAM)(u16);

    int gbJoymask[4];

    //from globals.h
    u16 *gbColorFilter;
    u8 **gbMemoryMap;
    u8 *gbMemory;
    u8 *gbVram;
    u8 *gbRom;
    u8 *gbRam;
    u8 *gbWram;
    u16 *gbLineBuffer;
    u8 gbBgp[4];
    u8 gbObp0[4];
    u8 gbObp1[4];
    u16 *gbPalette;
    int gbWindowLine;
    int gbCgbMode;
    int gbColorOption;
    int gbPaletteOption;
    int gbEmulatorType;
    int gbBorderOn;
    int gbBorderAutomatic;
    int gbBorderLineSkip;
    int gbBorderRowSkip;
    int gbBorderColumnSkip;
    int gbDmaTicks;
    u8 *bios;
    u8 *rom;
    u8 *internalRAM;
    u8 *workRAM;
    u8 *paletteRAM;
    u8 *vram;
    u8 *pix;
    u8 *oam;
    u8 *ioMem;
    int gbRomSize;
    int gbRomSizeMask ;
    int gbRamSizeMask ;
    int gbRamSize ;
    int emulating;
    int speedup;

    //from sgb.h
    u8 *gbSgbBorderChar;
    u8 *gbSgbBorder;
    int gbSgbCGBSupport;
    int gbSgbMask;
    int gbSgbMode;
    int gbSgbPacketState;
    int gbSgbBit;
    int gbSgbPacketTimeout;
    int GBSGB_PACKET_TIMEOUT;
    u8  *gbSgbPacket;
    int gbSgbPacketNBits;
    int gbSgbPacketByte;
    int gbSgbPacketNumber;
    int gbSgbMultiplayer;
    int gbSgbFourPlayers;
    u8  gbSgbNextController;
    u8  gbSgbReadingController;
    u16 *gbSgbSCPPalette;
    u8  *gbSgbATF;
    u8  *gbSgbATFList;
    u8  *gbSgbScreenBuffer;

    //from system.h
    u32  *systemColorMap32;
    u16  *systemColorMap16;
    u16  *systemGbPalette;
    int  systemRedShift;
    int  systemGreenShift;
    int  systemBlueShift;
    int  systemColorDepth;
    int  systemDebug;
    int  systemVerbose;
    int  systemSaveUpdateCounter;
    int  systemFrameSkip;
    int  systemRenderedFrames;
    //from memory.h
    mapperMBC1 gbDataMBC1;
    mapperMBC2 gbDataMBC2;
    mapperMBC3 gbDataMBC3;
    mapperMBC5 gbDataMBC5;
    mapperMBC7 gbDataMBC7;
    mapperHuC1 gbDataHuC1;
    mapperHuC3 gbDataHuC3;
    
    //from sound.h
    int soundQuality;
    int soundTicks;
    int SOUND_CLOCK_TICKS;

    //from gfx.h
    u16 *gbLineMix;
    int layerSettings;

    //callbacks
    int *param_read_joypad;
    int (*call_back_read_joypad)(int *p,int which);
public:
    int gbReadSgbSaveStructV3(CFileBase *file);
    int gbSgbReadGame(CFileBase *file);
    int gbReadSaveGameStruct(CFileBase *file);
    int gbReadSaveState(CFileBase *file);
    int gbWriteSgbSaveStructV3(CFileBase *file);
    int gbSgbSaveGame(CFileBase *file);
    int gbWriteSaveState(CFileBase *file);
    int gbWriteSaveGameStruct(CFileBase *file);
    int gbGetScreenSize(int *w,int *h);
    int utilWriteBMPBits(CFileBase *file,int w,int h,u8 *pix);
    int utilWriteBMPHeader(CFileBase *file,int w,int h);
    int gbWriteBatteryFile(char *filename);
    int gbReadBatteryFile(char *filename);
    int gbWriteSaveMBC5(CFileBase *file);
    int gbWriteSaveMBC7(CFileBase *file);
    int gbWriteSaveMBC3(CFileBase *file, int extendedSave);
    int gbWriteSaveMBC2(CFileBase *file);
    int gbWriteSaveMBC1(CFileBase *file);
    int gbWriteBatteryFile(CFileBase *file,int extendedSave);
    int gbReadSaveMBC7(CFileBase *file);
    int gbReadSaveMBC5(CFileBase *file);
    int gbReadSaveMBC3(CFileBase *file);
    int gbReadSaveMBC2(CFileBase *file);
    int gbReadSaveMBC1(CFileBase *file);
    int gbReadBatteryFile(CFileBase *file);
    int gbWriteBMPFile(CFileBase *file);
    int gbLoadRom(char *filename);
    int gbWriteBMPFile(char *filename);
    int InitColorMap();
    void gbSoundTick();
    void gbRenderLine();
    void gbDrawSpriteTile(int tile, int x,int y,int t, int flags, int size,int spriteNumber);
    void gbDrawSprites();
    void systemDrawScreen();
    void systemScreenCapture(int _iNum);
    void systemUpdateMotionSensor();
    int systemReadJoypad(int i);
    int systemReadJoypads();
    void systemShowSpeed(int _iSpeed);
    void system10Frames(int _iRate);
    void systemFrame();
    void gbEmulate(int ticksToStop);
    bool utilWriteBMPFile(CFileBase *file, int w, int h, u8 *pix);
    void WRITE16LE(u16 *addr,u16 value);
    int systemGetSensorX();
    int systemGetSensorY();
    int gbInit();
    void mapperMBC1ROM(u16 address, u8 value);
    void mapperMBC1RAM(u16 address, u8 value);
    void memoryUpdateMapMBC1();
    void mapperMBC2ROM(u16 address, u8 value);
    void mapperMBC2RAM(u16 address, u8 value);
    void memoryUpdateMapMBC2();
    void memoryUpdateMBC3Clock();
    void mapperMBC3ROM(u16 address, u8 value);
    void mapperMBC3RAM(u16 address, u8 value);
    u8 mapperMBC3ReadRAM(u16 address);
    void memoryUpdateMapMBC3();
    void mapperMBC5ROM(u16 address, u8 value);
    void mapperMBC5RAM(u16 address, u8 value);
    void memoryUpdateMapMBC5();
    void mapperMBC7ROM(u16 address, u8 value);
    u8 mapperMBC7ReadRAM(u16 address);
    void mapperMBC7RAM(u16 address, u8 value);
    void memoryUpdateMapMBC7();
    void mapperHuC1ROM(u16 address, u8 value);
    void mapperHuC1RAM(u16 address, u8 value);
    void memoryUpdateMapHuC1();
    void mapperHuC3ROM(u16 address, u8 value);
    u8 mapperHuC3ReadRAM(u16 address);
    void mapperHuC3RAM(u16 address, u8 value);
    void memoryUpdateMapHuC3();
    bool gbUpdateSizes();
    int utilGetSize(int size);
    int gbLoadRom(CFileBase *rom);
    void gbLcd_interrupt();
    void gbTimer_interrupt();
    void gbSerial_interrupt();
    void gbJoypad_interrupt();
    void gbVblank_interrupt();
    u8 gbReadMemory(u16 address);
    u8 gbReadOpcode(u16 address);
    u16 READ16LE(u16* addr);
    void gbSoundEvent(u16 address,int data);
    u32 systemGetClock();
    void gbSoundReset();
    void gbSpeedSwitch();
    void gbReset();
    void systemGbBorderOn();
    inline void gbSgbDraw24Bit(u8 *p, u16 v);
    inline void gbSgbDraw32Bit(u32 *p, u16 v);
    inline void gbSgbDraw16Bit(u16 *p, u16 v);
    void gbSgbReset();
    int gbSgbInit();
    int gbSgbShutdown();
    void gbSgbFillScreen(u16 color);
    void gbSgbRenderScreenToBuffer();
    void gbSgbDrawBorderTile(int x, int y, int tile, int attr);
    void gbSgbRenderBorder();
    void gbSgbPicture();
    void gbSgbSetPalette(int a,int b,u16 *p);
    void gbSgbScpPalette();
    void gbSgbSetATF(int n);
    void gbSgbSetPalette();
    void gbSgbAttributeBlock();
    void gbSgbSetColumnPalette(u8 col, u8 p);
    void gbSgbSetRowPalette(u8 row, u8 p);
    void gbSgbAttributeDivide();
    void gbSgbAttributeLine();
    void gbSgbAttributeCharacter();
    void gbSgbSetATFList();
    void gbSgbMaskEnable();
    void gbSgbChrTransfer();
    void gbSgbMultiRequest();
    void gbSgbCommand();
    void gbSgbResetPacketState();
    void gbSgbDoBitTransfer(u8 value);
    int gbWriteMemory(u16 address,u8 value);
    int gbCompareLYToLYC();
    int gbDoHdma();
    int gbCopyMemory(u16 d, u16 s, int count);
    int gbIsGameBoyRom(char *file_name);
    int gbGenFilter();
    int gbGetValue(int min,int max,int v);
    CGbEmu();
    virtual ~CGbEmu();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_GBEMU_H__3E8A5D45_AA89_4ACC_AF00_A1E2448601E6__INCLUDED_)
