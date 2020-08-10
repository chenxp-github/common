// NesRom.h: interface for the CNesRom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NESROM_H__92E2E398_F2EE_48B3_A880_F0648EC78E71__INCLUDED_)
#define AFX_NESROM_H__92E2E398_F2EE_48B3_A880_F0648EC78E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nes_common.h"
#include "filebase.h"
#include "common.h" // Added by ClassView

#define ROM_VMIRROR  0x01
#define ROM_SAVERAM  0x02
#define ROM_TRAINER  0x04
#define ROM_4SCREEN  0x08
#define ROM_VSUNISYSTEM 0x01

class CNesHeader{
public:
    BYTE    ID[4];
    BYTE    PRG_PAGE_SIZE;
    BYTE    CHR_PAGE_SIZE;
    BYTE    control1;
    BYTE    control2;
    BYTE    reserved[8];
public:
    int WriteToMem(void *buf);
    int HeaderSize();
    int LoadHeader(CFileBase *file);
    CNesHeader();
    virtual ~CNesHeader();
    int InitBasic();
};

class CNesNsfHeader{
public:
    BYTE    ID[5];
    BYTE    Version;
    BYTE    TotalSong;
    BYTE    StartSong;
    WORD    LoadAddress;
    WORD    InitAddress;
    WORD    PlayAddress;
    BYTE    SongName[32];
    BYTE    ArtistName[32];
    BYTE    CopyrightName[32];
    WORD    SpeedNTSC;
    BYTE    BankSwitch[8];
    WORD    SpeedPAL;
    BYTE    NTSC_PALbits;
    BYTE    ExtraChipSelect;
    BYTE    Expansion[4];       // must be 0
public:
    int HeaderSize();
    int LoadHeader(CFileBase *file);
    CNesNsfHeader();
    virtual ~CNesNsfHeader();
    int InitBasic();
};

class CNesRom{
public:
    CNesHeader      *header;
    CNesNsfHeader   *nsfheader;
    BYTE        *lpPRG;
    BYTE        *lpCHR;
    BYTE        *lpTrainer;
    BYTE        *lpDiskBios;
    BYTE        *lpDisk;

    // PROM CRC
    DWORD       crc;
    DWORD       crcall;
    DWORD       crcvrom;

    DWORD       fdsmakerID;
    DWORD       fdsgameID;

    int     mapper;
    int     diskno;

    // For PAL(Database)
    BOOL    bPAL;

    // For NSF
    BOOL    bNSF;
    int     NSF_PAGE_SIZE;
public:
    int GetDiskNo();
    BYTE * GetDISK();
    BYTE * GetTRAINER();
    BOOL IsVMIRROR();
    BOOL Is4SCREEN();
    BYTE GetPROM_SIZE();    
    BYTE GetVROM_SIZE();
    BYTE * GetVROM();
    BYTE * GetPROM();
    BOOL IsPAL();
    DWORD GetPROM_CRC();
    int IsSAVERAM();
    int GetMapperNo();
    CNesNsfHeader * GetNsfHeader();
    int IsNSF();
    int IsVSUNISYSTEM();
    int IsTRAINER();
    int LoadRom(CFileBase *file);
    CNesRom();
    virtual ~CNesRom();
    int Init();
    int Destroy();
    int Copy(CNesRom *p);
    int Comp(CNesRom *p);
    int Print();
    int InitBasic();
    int CheckLinkPtr();
};

#endif // !defined(AFX_NESROM_H__92E2E398_F2EE_48B3_A880_F0648EC78E71__INCLUDED_)
