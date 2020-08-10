// NesRom.cpp: implementation of the CNesRom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NesRom.h"
#include "mem_tool.h"
#include "mem.h"
#include "nescrc.h"

//////////////////////////////////////////////////////////////////////
// CNesHeader
//////////////////////////////////////////////////////////////////////

CNesHeader::CNesHeader()
{
    this->InitBasic();
}
CNesHeader::~CNesHeader()
{   
}
int CNesHeader::InitBasic()
{
    NES_ZERO_ARRAY(ID,4);
    NES_ZERO_ARRAY(this->reserved,8);

    this->CHR_PAGE_SIZE = 0;
    this->control1 = 0;
    this->control2 = 0;

    return OK;
}
int CNesHeader::LoadHeader(CFileBase *file)
{
    ASSERT(file);

    file->Seek(0);
    //read 16 byte header
    file->Read(this->ID,4);
    file->Read(&this->PRG_PAGE_SIZE,1);
    file->Read(&this->CHR_PAGE_SIZE,1);
    file->Read(&this->control1,1);
    file->Read(&this->control2,1);
    file->Read(this->reserved,8);

    return OK;
}
int CNesHeader::HeaderSize()
{
    return 16;
}
int CNesHeader::WriteToMem(void *buf)
{
    ASSERT(buf);

    CMem mem;

    mem.Init();
    //at least 16 bytes of buf size
    mem.SetP((char*)buf,this->HeaderSize());
    mem.Seek(0);
    mem.Write(ID,4);
    mem.Write(&this->PRG_PAGE_SIZE,1);
    mem.Write(&this->CHR_PAGE_SIZE,1);
    mem.Write(&this->control1,1);
    mem.Write(&this->control2,1);
    mem.Write(this->reserved,8);
    
    return OK;
}

//////////////////////////////////////////////////////////////////////
// CNesNsfHeader
//////////////////////////////////////////////////////////////////////
CNesNsfHeader::CNesNsfHeader()
{
    this->InitBasic();
}
CNesNsfHeader::~CNesNsfHeader()
{
}
int CNesNsfHeader::InitBasic()
{
    this->Version = 0;
    this->TotalSong = 0;
    this->StartSong = 0;
    this->LoadAddress = 0;
    this->InitAddress = 0;
    this->PlayAddress = 0;
    this->SpeedNTSC = 0;
    this->SpeedPAL = 0;
    this->NTSC_PALbits = 0;
    this->ExtraChipSelect = 0;
    
    NES_ZERO_ARRAY(ID,5);
    NES_ZERO_ARRAY(SongName,32);
    NES_ZERO_ARRAY(ArtistName,32);
    NES_ZERO_ARRAY(CopyrightName,32);
    NES_ZERO_ARRAY(BankSwitch,8);
    NES_ZERO_ARRAY(Expansion,4);

    return OK;
}

int CNesNsfHeader::LoadHeader(CFileBase *file)
{
    ASSERT(file);

    file->Seek(0);
    file->Read(this->ID,5);
    file->Read(&this->Version,1);
    file->Read(&this->TotalSong,1);
    file->Read(&this->StartSong,1);
    file->Read(&this->LoadAddress,2);
    file->Read(&this->InitAddress,2);
    file->Read(&this->PlayAddress,2);
    file->Read(this->SongName,32);
    file->Read(this->ArtistName,32);
    file->Read(this->CopyrightName,32);
    file->Read(&this->SpeedNTSC,2);
    file->Read(this->BankSwitch,8);
    file->Read(&this->SpeedPAL,2);
    file->Read(&this->NTSC_PALbits,1);
    file->Read(&this->ExtraChipSelect,1);
    file->Read(this->Expansion,4);

    return OK;
}


int CNesNsfHeader::HeaderSize()
{
    return 128;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNesRom::CNesRom()
{
    this->InitBasic();
}
CNesRom::~CNesRom()
{
    this->Destroy();
}
int CNesRom::InitBasic()
{
    this->header = 0;
    this->nsfheader = 0;
    this->lpPRG = 0;
    this->lpCHR = 0;
    this->lpTrainer = 0;
    this->lpDiskBios = 0;
    this->lpDisk = 0;
    this->crc = 0;
    this->crcall = 0;
    this->crcvrom = 0;
    this->fdsmakerID = 0;
    this->fdsgameID = 0;
    this->mapper = 0;
    this->diskno = 0;
    this->bPAL = 0;
    this->bNSF = 0;
    this->NSF_PAGE_SIZE = 0;

    return OK;
}
int CNesRom::Init()
{
    this->InitBasic();
    
    NEW(this->header,CNesHeader);
    NEW(this->nsfheader,CNesNsfHeader);

    return OK;
}
int CNesRom::Destroy()
{
    DEL(this->header);
    DEL(this->nsfheader);

    FREE(this->lpCHR);
    FREE(this->lpPRG);
    FREE(this->lpTrainer);

    this->InitBasic();
    return OK;
}
int CNesRom::Copy(CNesRom *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
int CNesRom::Comp(CNesRom *p)
{
    return 0;
}
int CNesRom::Print()
{
    //add your code
    return TRUE;
}
int CNesRom::CheckLinkPtr()
{
    return TRUE;
}

int CNesRom::LoadRom(CFileBase *file)
{
    DWORD   PRGoffset, CHRoffset;
    LONG    PRGsize, CHRsize, FileSize;
    CNesCrc nes_crc;

    FileSize = file->GetSize();
    ASSERT(FileSize > 17);

    nes_crc.Init();

    this->header->LoadHeader(file);
    
    if( header->ID[0] == 'N' && header->ID[1] == 'E' && header->ID[2] == 'S' && header->ID[3] == 0x1A ) 
    {   
        PRGsize = (LONG)header->PRG_PAGE_SIZE*0x4000;
        CHRsize = (LONG)header->CHR_PAGE_SIZE*0x2000;
        PRGoffset = header->HeaderSize();
        CHRoffset = PRGoffset + PRGsize;
        
        if( IsTRAINER() ) 
        {
            PRGoffset += 512;
            CHRoffset += 512;
        }
        
        ASSERT( PRGsize > 0 && (PRGsize+CHRsize) <= FileSize ) 

        MALLOC(lpPRG,BYTE,PRGsize);
        file->Seek(PRGoffset);
        file->Read(lpPRG,PRGsize);      
        
        if( CHRsize > 0 ) 
        {
            MALLOC(lpCHR,BYTE,CHRsize);

            if( FileSize < (LONG)(CHRoffset+CHRsize) ) 
            {
                CHRsize -= (CHRoffset+CHRsize - FileSize);      
            }   
            file->Seek(CHRoffset);
            file->Read(lpCHR,CHRsize);
        }
        else 
        {
            lpCHR = NULL;
        }
        
        if( IsTRAINER() ) 
        {
            MALLOC(lpTrainer,BYTE,512);
            file->Seek(header->HeaderSize()); //nes header size
            file->Read(lpTrainer,512);
        } 
        else 
        {
            lpTrainer = NULL;
        }
    }
    
    if( header->ID[0] == 'F' && header->ID[1] == 'D' && header->ID[2] == 'S' && header->ID[3] == 0x1A )
    {
        LOG("unsupport disk rom\n");
    }

    if( header->ID[0] == 'N' && header->ID[1] == 'E'&& header->ID[2] == 'S' && header->ID[3] == 'M') 
    {
        bNSF = TRUE;
        header->InitBasic();
        
        this->nsfheader->LoadHeader(file);
        
        PRGsize = FileSize-this->nsfheader->HeaderSize();
        PRGsize = (PRGsize+0x0FFF) & (~0x0FFF);
        
        MALLOC(lpPRG,BYTE,PRGsize);         
        ::memset( lpPRG, 0, PRGsize );
        
        file->Seek(nsfheader->HeaderSize());
        file->Write(lpPRG, FileSize - nsfheader->HeaderSize());         
        
        NSF_PAGE_SIZE = PRGsize>>12;
    }

    if( !bNSF ) 
    {
        mapper = (header->control1>>4)|(header->control2&0xF0);
        crc = crcall = crcvrom = 0;
        if( mapper != 20 ) 
        {           
            if( IsTRAINER() ) 
            {                   
                crcall = nes_crc.CrcRev(file,nsfheader->HeaderSize(),512+PRGsize+CHRsize);
                crc    = nes_crc.CrcRev(file,nsfheader->HeaderSize(),512+PRGsize);
                if( CHRsize )
                    crcvrom = nes_crc.CrcRev(file,PRGsize+512+header->HeaderSize(),CHRsize);
            } 
            else 
            {
                crcall = nes_crc.CrcRev(file,header->HeaderSize(),PRGsize+CHRsize);
                crc    = nes_crc.CrcRev(file,header->HeaderSize(),PRGsize);
                if( CHRsize )
                    crcvrom = nes_crc.CrcRev(file,PRGsize+header->HeaderSize(),CHRsize);
            }
        } 
    }
    return OK;
}

int CNesRom::IsTRAINER()
{
    return header->control1 & ROM_TRAINER;
}

int CNesRom::IsVSUNISYSTEM()
{
    return header->control2 & ROM_VSUNISYSTEM;
}

int CNesRom::IsNSF()
{
    return this->bNSF;
}

CNesNsfHeader * CNesRom::GetNsfHeader()
{
    return this->nsfheader;
}

int CNesRom::GetMapperNo()
{
    return this->mapper;
}

int CNesRom::IsSAVERAM()
{
    return header->control1 & ROM_SAVERAM;
}

DWORD CNesRom::GetPROM_CRC()
{
    return this->crc;
}

BOOL CNesRom::IsPAL()
{
    return bPAL;
}

BYTE * CNesRom::GetPROM()
{
    return lpPRG; 
}

BYTE * CNesRom::GetVROM()
{
    return lpCHR;
}

BYTE CNesRom::GetPROM_SIZE()    
{
    return header->PRG_PAGE_SIZE; 
}
BYTE CNesRom::GetVROM_SIZE()    
{
    return header->CHR_PAGE_SIZE; 
}

BOOL CNesRom::Is4SCREEN()
{
    return header->control1 & ROM_4SCREEN;
}

BOOL CNesRom::IsVMIRROR()
{
    return header->control1 & ROM_VMIRROR;
}

BYTE * CNesRom::GetTRAINER()
{
    return lpTrainer;
}

BYTE * CNesRom::GetDISK()
{
    return this->lpDisk;
}

int CNesRom::GetDiskNo()
{
    return this->diskno;
}
