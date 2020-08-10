// NesPpu.cpp: implementation of the CNesPpu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nesppu.h"
#include "mem_tool.h"
#include "nesmmu.h"
#include "nesmapper.h"
#include "nesemu.h"
#include "nescpu.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNesPpu::CNesPpu()
{
    this->InitBasic();
}
CNesPpu::~CNesPpu()
{
    this->Destroy();
}
int CNesPpu::InitBasic()
{
    this->mmu = 0;
    this->nes = 0;
    this->bExtLatch = 0;
    this->bChrLatch = 0;
    this->bExtNameTable = 0;
    this->bExtMono = 0;
    this->loopy_y = 0;
    this->loopy_shift = 0;
    this->lpScreen = 0;
    this->lpScanline = 0;
    this->ScanlineNo = 0;
    this->lpColormode = 0;
    this->Bit2Rev = 0;
    this->bVSMode = 0;
    this->VSSecurityData = 0;
    this->nVSColorMap = 0;

    this->VSColorMap[0] = 0;
    this->VSColorMap[1] = 0;
    this->VSColorMap[2] = 0;
    this->VSColorMap[3] = 0;
    this->VSColorMap[4] = 0;

    return OK;
}
int CNesPpu::Init()
{
    this->InitBasic();
    
    MALLOC(this->Bit2Rev,BYTE,256);
    MALLOC(this->VSColorMap[0],BYTE,64);
    MALLOC(this->VSColorMap[1],BYTE,64);
    MALLOC(this->VSColorMap[2],BYTE,64);
    MALLOC(this->VSColorMap[3],BYTE,64);
    MALLOC(this->VSColorMap[4],BYTE,64);
    
    int i,j;
    BYTE m,c;

    for(  i = 0; i < 256; i++ ) 
    {
        m = 0x80;
        c = 0;
        for( j = 0; j < 8; j++ ) 
        {
            if( i&(1<<j) ) 
            {
                c |= m;
            }
            m >>= 1;
        }
        Bit2Rev[i] = c;
    }

    return OK;
}

int CNesPpu::Destroy()
{
    FREE(this->Bit2Rev);
    FREE(this->VSColorMap[0]);
    FREE(this->VSColorMap[1]);
    FREE(this->VSColorMap[2]);
    FREE(this->VSColorMap[3]);
    FREE(this->VSColorMap[4]);

    this->InitBasic();
    return OK;
}
int CNesPpu::Copy(CNesPpu *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
int CNesPpu::Comp(CNesPpu *p)
{
    return 0;
}
int CNesPpu::Print()
{
    //add your code
    return TRUE;
}
int CNesPpu::CheckLinkPtr()
{
    ASSERT(this->mmu);
    ASSERT(this->nes);
    ASSERT(this->lpScreen);
    ASSERT(this->lpColormode);

    return TRUE;
}

int CNesPpu::Reset()
{
    bExtLatch = FALSE;
    bChrLatch = FALSE;
    bExtNameTable = FALSE;
    bExtMono = FALSE;

    mmu->PPUREG[0] = mmu->PPUREG[1] = 0;

    mmu->PPU56Toggle = 0;

    mmu->PPU7_Temp = 0xFF;  // VS Excitebike

    mmu->loopy_v = mmu->loopy_t = 0;
    mmu->loopy_x = loopy_y = 0;
    loopy_shift = 0;

    if( lpScreen )
        ::memset( lpScreen, 0x3F, NES_SCREEN_WIDTH*NES_SCREEN_HEIGHT*sizeof(BYTE) );

    if( lpColormode )
        ::memset( lpColormode, 0, NES_SCREEN_HEIGHT*sizeof(BYTE) );

    return OK;
}

BYTE CNesPpu::Read(WORD addr)
{
    BYTE    data = 0x00;
    
    switch( addr ) 
    {
    // Write only Register
    case    0x2000: // PPU Control Register #1(W)
    case    0x2001: // PPU Control Register #2(W)
    case    0x2003: // SPR-RAM Address Register(W)
    case    0x2005: // PPU Scroll Register(W2)
    case    0x2006: // VRAM Address Register(W2)
        data = mmu->PPU7_Temp;  
        break;
        // Read/Write Register
    case    0x2002: // PPU Status Register(R)
        data = mmu->PPUREG[2] | VSSecurityData;
        mmu->PPU56Toggle = 0;
        mmu->PPUREG[2] &= ~PPU_VBLANK_FLAG;
        break;
    case    0x2004: // SPR_RAM I/O Register(RW)
        data = mmu->SPRAM[ mmu->PPUREG[3]++ ];
        break;
    case    0x2007: // VRAM I/O Register(RW)
        WORD    addr = mmu->loopy_v & 0x3FFF;
        data = mmu->PPU7_Temp;
        if( mmu->PPUREG[0] & PPU_INC32_BIT ) mmu->loopy_v+=32;
        else                mmu->loopy_v++;
        if( addr >= 0x3000 ) {
            if( addr >= 0x3F00 ) {
                //                  data &= 0x3F;
                if( !(addr&0x0010) ) {
                    return  mmu->BGPAL[addr&0x000F];
                } else {
                    return  mmu->SPPAL[addr&0x000F];
                }
            }
            addr &= 0xEFFF;
        }
        mmu->PPU7_Temp = mmu->PPU_MEM_BANK[addr>>10][addr&0x03FF];
    }
    
    return  data;
}

int CNesPpu::Write(WORD addr, BYTE data)
{
    if( bVSMode && VSSecurityData ) {
        if( addr == 0x2000 ) {
            addr = 0x2001;
        } else if( addr == 0x2001 ){
            addr = 0x2000;
        }
    }

    switch( addr ) {
        // Read only Register
        case    0x2002: // PPU Status register(R)
            break;
        // Write Register
        case    0x2000: // PPU Control Register #1(W)
            // NameTable select
            // t:0000110000000000=d:00000011
            mmu->loopy_t = (mmu->loopy_t & 0xF3FF)|(((WORD)data & 0x03)<<10);

            if( (data & 0x80) && !(mmu->PPUREG[0] & 0x80) && (mmu->PPUREG[2] & 0x80) ) {
                nes->cpu->NMI();    // hmm..
            }
            mmu->PPUREG[0] = data;
            break;
        case    0x2001: // PPU Control Register #2(W)
            mmu->PPUREG[1] = data;
            break;
        case    0x2003: // SPR-RAM Address Register(W)
            mmu->PPUREG[3] = data;
            break;
        case    0x2004: // SPR_RAM I/O Register(RW)
            mmu->SPRAM[ mmu->PPUREG[3]++ ] = data;
            break;

        case    0x2005: // PPU Scroll Register(W2)
            if( !mmu->PPU56Toggle ) {
            // First write
                // tile X t:0000000000011111=d:11111000
                mmu->loopy_t = (mmu->loopy_t & 0xFFE0)|(((WORD)data)>>3);
                // scroll offset X x=d:00000111
                mmu->loopy_x = data & 0x07;
            } else {
            // Second write
                // tile Y t:0000001111100000=d:11111000
                mmu->loopy_t = (mmu->loopy_t & 0xFC1F)|((((WORD)data) & 0xF8)<<2);
                // scroll offset Y t:0111000000000000=d:00000111
                mmu->loopy_t = (mmu->loopy_t & 0x8FFF)|((((WORD)data) & 0x07)<<12);
            }
            mmu->PPU56Toggle = !mmu->PPU56Toggle;
            break;
        case    0x2006: // VRAM Address Register(W2)
            if( !mmu->PPU56Toggle ) {
            // First write
                // t:0011111100000000=d:00111111
                // t:1100000000000000=0
                mmu->loopy_t = (mmu->loopy_t & 0x00FF)|((((WORD)data) & 0x3F)<<8);
            } else {
            // Second write
                // t:0000000011111111=d:11111111
                mmu->loopy_t = (mmu->loopy_t & 0xFF00)|(WORD)data;
                // v=t
                mmu->loopy_v = mmu->loopy_t;

                nes->mapper->PPU_Latch( mmu->loopy_v );
            }
            mmu->PPU56Toggle = !mmu->PPU56Toggle;
            break;

        case    0x2007: // VRAM I/O Register(RW)
            WORD    vaddr = mmu->loopy_v & 0x3FFF;
            if( mmu->PPUREG[0] & PPU_INC32_BIT ) mmu->loopy_v+=32;
            else                mmu->loopy_v++;

            if( vaddr >= 0x3000 ) {
                if( vaddr >= 0x3F00 ) {
                    data &= 0x3F;
                    if( bVSMode && nVSColorMap != -1 ) {
                        BYTE    temp = VSColorMap[nVSColorMap][data];
                        if( temp != 0xFF ) {
                            data = temp & 0x3F;
                        }
                    }

                    if( !(vaddr&0x000F) ) {
                        mmu->BGPAL[0] = mmu->SPPAL[0] = data;
                    } else if( !(vaddr&0x0010) ) {
                        mmu->BGPAL[vaddr&0x000F] = data;
                    } else {
                        mmu->SPPAL[vaddr&0x000F] = data;
                    }
                    mmu->BGPAL[0x04] = mmu->BGPAL[0x08] = mmu->BGPAL[0x0C] = mmu->BGPAL[0x00];
                    mmu->SPPAL[0x00] = mmu->SPPAL[0x04] = mmu->SPPAL[0x08] = mmu->SPPAL[0x0C] = mmu->BGPAL[0x00];
                    return OK;
                }
                vaddr &= 0xEFFF;
            }
            if( mmu->PPU_MEM_TYPE[vaddr>>10] != BANKTYPE_VROM ) {
                mmu->PPU_MEM_BANK[vaddr>>10][vaddr&0x03FF] = data;
            }
            break;
    }

    return OK;
}

int CNesPpu::DMA(BYTE data)
{
    int i;
    WORD addr = data<<8;

    for( i = 0; i < 256; i++ ) 
    {
        mmu->SPRAM[i] = nes->Read( addr+i );
    }

    return OK;
}

int CNesPpu::VBlankStart()
{
    mmu->PPUREG[2] |= PPU_VBLANK_FLAG;
    return OK;
}

int CNesPpu::VBlankEnd()
{
    mmu->PPUREG[2] &= ~PPU_VBLANK_FLAG;
    mmu->PPUREG[2] &= ~PPU_SPHIT_FLAG;
    return OK;
}

int CNesPpu::FrameStart()
{
    if( mmu->PPUREG[1] & (PPU_SPDISP_BIT|PPU_BGDISP_BIT) ) 
    {
        mmu->loopy_v = mmu->loopy_t;
        loopy_shift = mmu->loopy_x;
        loopy_y = (mmu->loopy_v&0x7000)>>12;
    }

    if( lpScreen ) 
    {
        ::memset( lpScreen, 0x3F, NES_SCREEN_WIDTH*sizeof(BYTE) );
    }
    
    if( lpColormode ) 
    {
        lpColormode[0] = 0;
    }

    return OK;
}

int CNesPpu::FrameEnd()
{
    return OK;
}

int CNesPpu::SetRenderScanline(int scanline)
{
    ScanlineNo = scanline;
    if( scanline < 240 ) 
    {
        lpScanline = lpScreen+NES_SCREEN_WIDTH*scanline;
    }

    return OK;
}

int CNesPpu::ScanlineStart()
{
    if( mmu->PPUREG[1] & (PPU_BGDISP_BIT|PPU_SPDISP_BIT) ) 
    {
        mmu->loopy_v = (mmu->loopy_v & 0xFBE0)|(mmu->loopy_t & 0x041F);
        loopy_shift = mmu->loopy_x;
        loopy_y = (mmu->loopy_v&0x7000)>>12;
        nes->mapper->PPU_Latch( 0x2000 + (mmu->loopy_v & 0x0FFF) );
    }

    return OK;
}

int CNesPpu::ScanlineNext()
{
    if( mmu->PPUREG[1] & (PPU_BGDISP_BIT|PPU_SPDISP_BIT) ) 
    {
        if( (mmu->loopy_v & 0x7000) == 0x7000 ) 
        {
            mmu->loopy_v &= 0x8FFF;
            if( (mmu->loopy_v & 0x03E0) == 0x03A0 ) 
            {
                mmu->loopy_v ^= 0x0800;
                mmu->loopy_v &= 0xFC1F;
            }
            else 
            {
                if( (mmu->loopy_v & 0x03E0) == 0x03E0 ) 
                {
                    mmu->loopy_v &= 0xFC1F;
                }
                else 
                {
                    mmu->loopy_v += 0x0020;
                }
            }
        } 
        else 
        {
            mmu->loopy_v += 0x1000;
        }
        
        loopy_y = (mmu->loopy_v&0x7000)>>12;
    }
    return OK;
}

int CNesPpu::Scanline(int scanline, BOOL bMax, BOOL bLeftClip)
{
    BYTE    BGwrite[33+1];
    BYTE    BGmono[33+1];

    // Linecolor mode
    
    lpColormode[scanline] = ((mmu->PPUREG[1]&PPU_BGCOLOR_BIT)>>5)|((mmu->PPUREG[1]&PPU_COLORMODE_BIT)<<7);
    ::memset(BGwrite,0,sizeof(BGwrite));
    ::memset(BGmono,0,sizeof(BGmono));

    // Render BG
    if( !(mmu->PPUREG[1]&PPU_BGDISP_BIT) ) 
    {
        ::memset( lpScanline, mmu->BGPAL[0], NES_SCREEN_WIDTH );
        
        if( nes->GetRenderMethod() == TILE_RENDER ) 
        {
            nes->EmulationCPU( FETCH_CYCLES*4*32 );
        }
    }
    else 
    {
        if( nes->GetRenderMethod() != TILE_RENDER ) 
        {
            if( !bExtLatch ) 
            {
                            
                // Without Extension Latch
                BYTE    *pScn = lpScanline+(8-loopy_shift);
                BYTE    *pBGw = BGwrite;

                int tileofs = (mmu->PPUREG[0]&PPU_BGTBL_BIT)<<8;
                int ntbladr = 0x2000+(mmu->loopy_v&0x0FFF);
                int attradr = 0x23C0+(mmu->loopy_v&0x0C00)+((mmu->loopy_v&0x0380)>>4);
                int ntbl_x  = ntbladr&0x001F;
                int attrsft = (ntbladr&0x0040)>>4;
                BYTE *pNTBL = mmu->PPU_MEM_BANK[ntbladr>>10];

                int tileadr;
                int cache_tile = 0xFFFF0000;
                BYTE    cache_attr = 0xFF;

                BYTE    chr_h, chr_l, attr;

                attradr &= 0x3FF;

                for( int i = 0; i < 33; i++ ) 
                {
                    tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;
                    attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;

                    if( cache_tile == tileadr && cache_attr == attr ) 
                    {
                        ::memcpy(pScn+0,pScn-8,4);
                        ::memcpy(pScn+4,pScn-4,4);

                        *(pBGw+0) = *(pBGw-1);
                    }
                    else 
                    {
                        cache_tile = tileadr;
                        cache_attr = attr;
                        chr_l = mmu->PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
                        chr_h = mmu->PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
                        *pBGw = chr_h|chr_l;

                        BYTE *  pBGPAL = &mmu->BGPAL[attr];
                        
                        {
                        register int c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
                        register int c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
                        pScn[0] = pBGPAL[(c1>>6)];
                        pScn[4] = pBGPAL[(c1>>2)&3];
                        pScn[1] = pBGPAL[(c2>>6)];
                        pScn[5] = pBGPAL[(c2>>2)&3];
                        pScn[2] = pBGPAL[(c1>>4)&3];
                        pScn[6] = pBGPAL[c1&3];
                        pScn[3] = pBGPAL[(c2>>4)&3];
                        pScn[7] = pBGPAL[c2&3];
                        }
                    }
                    pScn+=8;
                    pBGw++;

                    // Character latch(For MMC2/MMC4)
                    if( bChrLatch ) 
                    {
                        nes->mapper->PPU_ChrLatch( tileadr );
                    }

                    if( ++ntbl_x == 32 ) 
                    {
                        ntbl_x = 0;
                        ntbladr ^= 0x41F;
                        attradr = 0x03C0+((ntbladr&0x0380)>>4);
                        pNTBL = mmu->PPU_MEM_BANK[ntbladr>>10];
                    } 
                    else 
                    {
                        ntbladr++;
                    }
                }
            } 
            else 
            {
                // With Extension Latch(For MMC5)
                BYTE *  pScn = lpScanline+(8-loopy_shift);
                BYTE *  pBGw = BGwrite;

                int ntbladr = 0x2000+(mmu->loopy_v&0x0FFF);
                int ntbl_x  = ntbladr & 0x1F;

                int cache_tile = 0xFFFF0000;
                BYTE    cache_attr = 0xFF;

                BYTE    chr_h, chr_l, attr, exattr;

                for( int i = 0; i < 33; i++ ) 
                {
                    nes->mapper->PPU_ExtLatchX( i );
                    nes->mapper->PPU_ExtLatch( ntbladr, chr_l, chr_h, exattr );
                    attr = exattr&0x0C;

                    if( cache_tile != (((int)chr_h<<8)+(int)chr_l) || cache_attr != attr ) 
                    {
                        cache_tile = (((int)chr_h<<8)+(int)chr_l);
                        cache_attr = attr;
                        *pBGw = chr_h|chr_l;

                        BYTE *  pBGPAL = &mmu->BGPAL[attr];
                        {
                        register int    c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
                        register int    c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
                        pScn[0] = pBGPAL[(c1>>6)];
                        pScn[4] = pBGPAL[(c1>>2)&3];
                        pScn[1] = pBGPAL[(c2>>6)];
                        pScn[5] = pBGPAL[(c2>>2)&3];
                        pScn[2] = pBGPAL[(c1>>4)&3];
                        pScn[6] = pBGPAL[c1&3];
                        pScn[3] = pBGPAL[(c2>>4)&3];
                        pScn[7] = pBGPAL[c2&3];
                        }
                    } 
                    else 
                    {
                        ::memcpy(pScn+0,pScn-8,4);
                        ::memcpy(pScn+4,pScn-4,4);

                        *(pBGw+0) = *(pBGw-1);
                    }
                    pScn+=8;
                    pBGw++;

                    if( ++ntbl_x == 32 ) 
                    {
                        ntbl_x = 0;
                        ntbladr ^= 0x41F;
                    }
                    else 
                    {
                        ntbladr++;
                    }
                }
            }
        } 
        else 
        {
            if( !bExtLatch ) {
                // Without Extension Latch
                if( !bExtNameTable ) 
                {
                    BYTE *  pScn = lpScanline+(8-loopy_shift);
                    BYTE *  pBGw = BGwrite;
                    int ntbladr = 0x2000+(mmu->loopy_v&0x0FFF);
                    int attradr = 0x03C0+((mmu->loopy_v&0x0380)>>4);
                    int ntbl_x  = ntbladr&0x001F;
                    int attrsft = (ntbladr&0x0040)>>4;
                    BYTE *  pNTBL = mmu->PPU_MEM_BANK[ntbladr>>10];

                    int tileadr;
                    int cache_tile = 0xFFFF0000;
                    BYTE    cache_attr = 0xFF;
                    BYTE    cache_mono = 0x00;

                    BYTE    chr_h, chr_l, attr;

                    for( int i = 0; i < 33; i++ ) 
                    {
                        tileadr = ((mmu->PPUREG[0]&PPU_BGTBL_BIT)<<8)+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

                        if( i != 0 ) 
                        {
                            nes->EmulationCPU( FETCH_CYCLES*4 );
                        }

                        attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;

                        if( cache_tile != tileadr || cache_attr != attr ) 
                        {
                            cache_tile = tileadr;
                            cache_attr = attr;

                            chr_l = mmu->PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
                            chr_h = mmu->PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
                            *pBGw = chr_l|chr_h;

                            BYTE *  pBGPAL = &mmu->BGPAL[attr];
                            {
                            register int    c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
                            register int    c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
                            pScn[0] = pBGPAL[(c1>>6)];
                            pScn[4] = pBGPAL[(c1>>2)&3];
                            pScn[1] = pBGPAL[(c2>>6)];
                            pScn[5] = pBGPAL[(c2>>2)&3];
                            pScn[2] = pBGPAL[(c1>>4)&3];
                            pScn[6] = pBGPAL[c1&3];
                            pScn[3] = pBGPAL[(c2>>4)&3];
                            pScn[7] = pBGPAL[c2&3];
                            }
                        } 
                        else 
                        {
                            ::memcpy(pScn+0,pScn-8,4);
                            ::memcpy(pScn+4,pScn-4,4);

                            *(pBGw+0) = *(pBGw-1);
                        }
                        pScn+=8;
                        pBGw++;

                        // Character latch(For MMC2/MMC4)
                        if( bChrLatch ) 
                        {
                            nes->mapper->PPU_ChrLatch( tileadr );
                        }

                        if( ++ntbl_x == 32 ) 
                        {
                            ntbl_x = 0;
                            ntbladr ^= 0x41F;
                            attradr = 0x03C0+((ntbladr&0x0380)>>4);
                            pNTBL = mmu->PPU_MEM_BANK[ntbladr>>10];
                        } 
                        else 
                        {
                            ntbladr++;
                        }
                    }
                } 
                else 
                {
                    BYTE *  pScn = lpScanline+(8-loopy_shift);
                    BYTE *  pBGw = BGwrite;

                    int ntbladr;
                    int tileadr;
                    int cache_tile = 0xFFFF0000;
                    BYTE    cache_attr = 0xFF;
                    BYTE    cache_mono = 0x00;

                    BYTE    chr_h, chr_l, attr;

                    WORD    loopy_v_tmp = mmu->loopy_v;

                    for( int i = 0; i < 33; i++ ) 
                    {
                        if( i != 0 ) 
                        {
                            nes->EmulationCPU( FETCH_CYCLES*4 );
                        }

                        ntbladr = 0x2000+(mmu->loopy_v&0x0FFF);
                        tileadr = ((mmu->PPUREG[0]&PPU_BGTBL_BIT)<<8)+mmu->PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+((mmu->loopy_v&0x7000)>>12);
                        attr = ((mmu->PPU_MEM_BANK[ntbladr>>10][0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2)]>>(((ntbladr&0x40)>>4)+(ntbladr&0x02)))&3)<<2;

                        if( cache_tile != tileadr || cache_attr != attr ) 
                        {
                            cache_tile = tileadr;
                            cache_attr = attr;

                            chr_l = mmu->PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
                            chr_h = mmu->PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
                            *pBGw = chr_l|chr_h;

                            BYTE *  pBGPAL = &mmu->BGPAL[attr];
                            {
                            register int    c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
                            register int    c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
                            pScn[0] = pBGPAL[(c1>>6)];
                            pScn[4] = pBGPAL[(c1>>2)&3];
                            pScn[1] = pBGPAL[(c2>>6)];
                            pScn[5] = pBGPAL[(c2>>2)&3];
                            pScn[2] = pBGPAL[(c1>>4)&3];
                            pScn[6] = pBGPAL[c1&3];
                            pScn[3] = pBGPAL[(c2>>4)&3];
                            pScn[7] = pBGPAL[c2&3];
                            }
                        } 
                        else 
                        {
                            ::memcpy(pScn+0,pScn-8,4);
                            ::memcpy(pScn+4,pScn-4,4);

                            *(pBGw+0) = *(pBGw-1);
                        }
                        pScn+=8;
                        pBGw++;

                        // Character latch(For MMC2/MMC4)
                        if( bChrLatch ) 
                        {
                            nes->mapper->PPU_ChrLatch( tileadr );
                        }

                        if( (mmu->loopy_v & 0x1F) == 0x1F ) 
                        {
                            mmu->loopy_v ^= 0x041F;
                        }
                        else 
                        {
                            mmu->loopy_v++;
                        }
                    }
                    mmu->loopy_v = loopy_v_tmp;
                }
            } 
            else 
            {
                // With Extension Latch(For MMC5)
                BYTE *  pScn = lpScanline+(8-loopy_shift);
                BYTE *  pBGw = BGwrite;

                int ntbladr = 0x2000+(mmu->loopy_v&0x0FFF);
                int ntbl_x  = ntbladr & 0x1F;

                int cache_tile = 0xFFFF0000;
                BYTE    cache_attr = 0xFF;

                BYTE    chr_h, chr_l, attr, exattr;

                for( int i = 0; i < 33; i++ ) 
                {
                    if( i != 0 ) 
                    {
                        nes->EmulationCPU( FETCH_CYCLES*4 );
                    }
                    nes->mapper->PPU_ExtLatchX( i );
                    nes->mapper->PPU_ExtLatch( ntbladr, chr_l, chr_h, exattr );
                    attr = exattr&0x0C;

                    if( cache_tile != (((int)chr_h<<8)+(int)chr_l) || cache_attr != attr ) 
                    {
                        cache_tile = (((int)chr_h<<8)+(int)chr_l);
                        cache_attr = attr;
                        *pBGw = chr_l|chr_h;

                        BYTE *  pBGPAL   = &mmu->BGPAL[attr];
                        {
                        register int    c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
                        register int    c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
                        pScn[0] = pBGPAL[(c1>>6)];
                        pScn[4] = pBGPAL[(c1>>2)&3];
                        pScn[1] = pBGPAL[(c2>>6)];
                        pScn[5] = pBGPAL[(c2>>2)&3];
                        pScn[2] = pBGPAL[(c1>>4)&3];
                        pScn[6] = pBGPAL[c1&3];
                        pScn[3] = pBGPAL[(c2>>4)&3];
                        pScn[7] = pBGPAL[c2&3];
                        }
                    } 
                    else 
                    {
                        ::memcpy(pScn+0,pScn-8,4);
                        ::memcpy(pScn+4,pScn-4,4);
                        *(pBGw+0) = *(pBGw-1);
                    }
                    pScn+=8;
                    pBGw++;

                    if( ++ntbl_x == 32 ) 
                    {
                        ntbl_x = 0;
                        ntbladr ^= 0x41F;
                    }
                    else 
                    {
                        ntbladr++;
                    }
                }
            }
        }
        if( !(mmu->PPUREG[1]&PPU_BGCLIP_BIT) && bLeftClip ) 
        {
            BYTE *  pScn = lpScanline+8;
            for( int i = 0; i < 8; i++ ) 
            {
                pScn[i] = mmu->BGPAL[0];
            }
        }
    }

    // Render sprites
    mmu->PPUREG[2] &= ~PPU_SPMAX_FLAG;

    if( scanline > 239 )
        return OK;

    if( !(mmu->PPUREG[1]&PPU_SPDISP_BIT) ) 
    {
        return OK;
    }

    BYTE SPwrite[33+1];
    int spmax;
    int spraddr, sp_y, sp_h;
    BYTE    chr_h, chr_l;
    CNesSprite *sp;

    BYTE *  pBGw = BGwrite;
    BYTE *  pSPw = SPwrite;
    BYTE *  pBit2Rev = Bit2Rev;

    memset( SPwrite, 0,sizeof(SPwrite) );

    spmax = 0;
    sp = (CNesSprite *)mmu->SPRAM;
    sp_h = (mmu->PPUREG[0]&PPU_SP16_BIT)?15:7;

    // Left clip
    if( !(mmu->PPUREG[1]&PPU_SPCLIP_BIT) && bLeftClip ) 
    {
        SPwrite[0] = 0xFF;
    }

    for( int i = 0; i < 64; i++ ) 
    {
        sp = (CNesSprite*)(mmu->SPRAM + (i << 2));
        sp_y = scanline - (sp->y+1);

        if( sp_y != (sp_y & sp_h) )
            continue;

        if( !(mmu->PPUREG[0]&PPU_SP16_BIT) ) 
        {
        // 8x8 Sprite
            spraddr = (((int)mmu->PPUREG[0]&PPU_SPTBL_BIT)<<9)+((int)sp->tile<<4);
            if( !(sp->attr&SP_VMIRROR_BIT) )
                spraddr += sp_y;
            else
                spraddr += 7-sp_y;
        }
        else 
        {
        // 8x16 Sprite
            spraddr = (((int)sp->tile&1)<<12)+(((int)sp->tile&0xFE)<<4);
            if( !(sp->attr&SP_VMIRROR_BIT) )
                spraddr += ((sp_y&8)<<1)+(sp_y&7);
            else
                spraddr += ((~sp_y&8)<<1)+(7-(sp_y&7));
        }
        // Character pattern
        chr_l = mmu->PPU_MEM_BANK[spraddr>>10][ spraddr&0x3FF   ];
        chr_h = mmu->PPU_MEM_BANK[spraddr>>10][(spraddr&0x3FF)+8];

        // Character latch(For MMC2/MMC4)
        if( bChrLatch ) 
        {
            nes->mapper->PPU_ChrLatch( spraddr );
        }

        // pattern mask
        if( sp->attr&SP_HMIRROR_BIT ) 
        {
            chr_l = pBit2Rev[chr_l];
            chr_h = pBit2Rev[chr_h];
        }
        BYTE    SPpat = chr_l|chr_h;

        // Sprite hitcheck
        if( i == 0 && !(mmu->PPUREG[2]&PPU_SPHIT_FLAG) ) 
        {
            int BGpos = ((sp->x&0xF8)+((loopy_shift+(sp->x&7))&8))>>3;
            int BGsft = 8-((loopy_shift+sp->x)&7);
            BYTE    BGmsk = (((WORD)pBGw[BGpos+0]<<8)|(WORD)pBGw[BGpos+1])>>BGsft;

            if( SPpat & BGmsk ) 
            {
                mmu->PPUREG[2] |= PPU_SPHIT_FLAG;
            }
        }

        // Sprite mask
        int SPpos = sp->x/8;
        int SPsft = 8-(sp->x&7);
        BYTE    SPmsk = (((WORD)pSPw[SPpos+0]<<8)|(WORD)pSPw[SPpos+1])>>SPsft;
        WORD    SPwrt = (WORD)SPpat<<SPsft;
        pSPw[SPpos+0] |= SPwrt >> 8;
        pSPw[SPpos+1] |= SPwrt & 0xFF;
        SPpat &= ~SPmsk;

        if( sp->attr&SP_PRIORITY_BIT ) 
        {
        // BG > SP priority
            int BGpos = ((sp->x&0xF8)+((loopy_shift+(sp->x&7))&8))>>3;
            int BGsft = 8-((loopy_shift+sp->x)&7);
            BYTE    BGmsk = (((WORD)pBGw[BGpos+0]<<8)|(WORD)pBGw[BGpos+1])>>BGsft;

            SPpat &= ~BGmsk;
        }

        // Attribute
        BYTE *  pSPPAL = &mmu->SPPAL[(sp->attr&SP_COLOR_BIT)<<2];
        // Ptr
        BYTE *  pScn   = lpScanline+sp->x+8;

        if( !bExtMono ) 
        {
            register int    c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
            register int    c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
            if( SPpat&0x80 ) pScn[0] = pSPPAL[(c1>>6)];
            if( SPpat&0x08 ) pScn[4] = pSPPAL[(c1>>2)&3];
            if( SPpat&0x40 ) pScn[1] = pSPPAL[(c2>>6)];
            if( SPpat&0x04 ) pScn[5] = pSPPAL[(c2>>2)&3];
            if( SPpat&0x20 ) pScn[2] = pSPPAL[(c1>>4)&3];
            if( SPpat&0x02 ) pScn[6] = pSPPAL[c1&3];
            if( SPpat&0x10 ) pScn[3] = pSPPAL[(c2>>4)&3];
            if( SPpat&0x01 ) pScn[7] = pSPPAL[c2&3];
        } 
        else 
        {
        // Monocrome effect (for Final Fantasy)
            BYTE    mono = BGmono[((sp->x&0xF8)+((loopy_shift+(sp->x&7))&8))>>3];

            register int    c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA);
            register int    c2 = (chr_l&0x55)|((chr_h<<1)&0xAA);
            if( SPpat&0x80 ) pScn[0] = pSPPAL[c1>>6]    |mono;
            if( SPpat&0x08 ) pScn[4] = pSPPAL[(c1>>2)&3]    |mono;
            if( SPpat&0x40 ) pScn[1] = pSPPAL[c2>>6]    |mono;
            if( SPpat&0x04 ) pScn[5] = pSPPAL[(c2>>2)&3]    |mono;
            if( SPpat&0x20 ) pScn[2] = pSPPAL[(c1>>4)&3]    |mono;
            if( SPpat&0x02 ) pScn[6] = pSPPAL[c1&3]     |mono;
            if( SPpat&0x10 ) pScn[3] = pSPPAL[(c2>>4)&3]    |mono;
            if( SPpat&0x01 ) pScn[7] = pSPPAL[c2&3]     |mono;
        }

        if( ++spmax > 8-1 ) 
        {
            if( !bMax )
                break;
        }
    }
    if( spmax > 8-1 ) 
    {
        mmu->PPUREG[2] |= PPU_SPMAX_FLAG;
    }

    return OK;
}


int CNesPpu::IsSprite0(int scanline)
{
    if( (mmu->PPUREG[1]&(PPU_SPDISP_BIT|PPU_BGDISP_BIT)) != (PPU_SPDISP_BIT|PPU_BGDISP_BIT) )
        return  FALSE;

    if( mmu->PPUREG[2]&PPU_SPHIT_FLAG )
        return  FALSE;

    if( !(mmu->PPUREG[0]&PPU_SP16_BIT) ) {
    // 8x8
        if( (scanline < (int)mmu->SPRAM[0]+1) || (scanline > ((int)mmu->SPRAM[0]+7+1)) )
            return  FALSE;
    } else {
    // 8x16
        if( (scanline < (int)mmu->SPRAM[0]+1) || (scanline > ((int)mmu->SPRAM[0]+15+1)) )
            return  FALSE;
    }

    return  TRUE;
}

int CNesPpu::DummyScanline(int scanline)
{
    int i;
    int spmax;
    int sp_h;
    CNesSprite *sp;

    mmu->PPUREG[2] &= ~PPU_SPMAX_FLAG;

    if( !(mmu->PPUREG[1]&PPU_SPDISP_BIT) )
        return OK;

    if( scanline < 0 || scanline > 239 )
        return OK;

    sp = (CNesSprite*)mmu->SPRAM;
    sp_h = (mmu->PPUREG[0]&PPU_SP16_BIT)?15:7;

    spmax = 0;
    // Sprite Max check
    for( i = 0; i < 64; i++) {
        sp = (CNesSprite*)(mmu->SPRAM + (i << 2));
        if( (scanline < (int)sp->y+1) || (scanline > ((int)sp->y+sp_h+1)) ) {
            continue;
        }

        if( ++spmax > 8-1 ) {
            mmu->PPUREG[2] |= PPU_SPMAX_FLAG;
            break;
        }
    }

    return OK;
}

int CNesPpu::SetScreenPtr(BYTE *lpScn, BYTE *lpMode)
{
    lpScreen = lpScn; lpColormode = lpMode;
    return OK;
}

int CNesPpu::SetExtNameTableMode(BOOL bMode)
{
    bExtNameTable = bMode;
    return OK;
}

BOOL CNesPpu::IsDispON()
{
    return mmu->PPUREG[1]&(PPU_BGDISP_BIT|PPU_SPDISP_BIT);
}

int CNesPpu::SetExtMonoMode(BOOL bMode)
{
    this->bExtMono = bMode;
    return OK;
}

WORD CNesPpu::GetPPUADDR()
{
    return mmu->loopy_v;
}

WORD CNesPpu::GetTILEY()
{
    return this->loopy_y;
}

int CNesPpu::SetExtLatchMode(BOOL bMode)
{
    bExtLatch = bMode;
    return OK;
}
