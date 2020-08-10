// GbEmu.cpp: implementation of the CGbEmu class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GbEmu.h"
#include "file.h"
#include "mem.h"

#if _WIN32_
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

#define SOUND_EVENT gbSoundEvent

int gbRomSizes[] = 
{
    0x00008000, // 32K
    0x00010000, // 64K
    0x00020000, // 128K
    0x00040000, // 256K
    0x00080000, // 512K
    0x00100000, // 1024K
    0x00200000, // 2048K
    0x00400000, // 4096K
    0x00800000  // 8192K
}
;
int gbRomSizesMasks[] = 
{
    0x00007fff,
    0x0000ffff,
    0x0001ffff,
    0x0003ffff,
    0x0007ffff,
    0x000fffff,
    0x001fffff,
    0x003fffff,
    0x007fffff
}
;
int gbRamSizes[6] = 
{
    0x00000000, // 0K
    0x00000800, // 2K
    0x00002000, // 8K
    0x00008000, // 32K
    0x00020000, // 128K
    0x00010000  // 64K
}
;
int gbRamSizesMasks[6] = 
{
    0x00000000,
    0x000007ff,
    0x00001fff,
    0x00007fff,
    0x0001ffff,
    0x0000ffff
}
;
int gbCycles[] = 
{
    //  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,  // 0
    1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,  // 1
    2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1,  // 2
    2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1,  // 3
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 4
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 5
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 6
    2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,  // 7
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 8
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 9
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // a
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // b
    2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 2, 3, 6, 2, 4,  // c
    2, 3, 3, 0, 3, 4, 2, 4, 2, 4, 3, 0, 3, 0, 2, 4,  // d
    3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,  // e
    3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4   // f
}
;
int gbCyclesCB[] = 
{
    //  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f   
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 1
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 2
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 3
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 4
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 5
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 6
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 7
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 8
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 9
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // a
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // b
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // c
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // d
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // e
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2   // f
}
;
u16 DAATable[] = 
{
    0x0080,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700,
    0x0800,0x0900,0x1020,0x1120,0x1220,0x1320,0x1420,0x1520,
    0x1000,0x1100,0x1200,0x1300,0x1400,0x1500,0x1600,0x1700,
    0x1800,0x1900,0x2020,0x2120,0x2220,0x2320,0x2420,0x2520,
    0x2000,0x2100,0x2200,0x2300,0x2400,0x2500,0x2600,0x2700,
    0x2800,0x2900,0x3020,0x3120,0x3220,0x3320,0x3420,0x3520,
    0x3000,0x3100,0x3200,0x3300,0x3400,0x3500,0x3600,0x3700,
    0x3800,0x3900,0x4020,0x4120,0x4220,0x4320,0x4420,0x4520,
    0x4000,0x4100,0x4200,0x4300,0x4400,0x4500,0x4600,0x4700,
    0x4800,0x4900,0x5020,0x5120,0x5220,0x5320,0x5420,0x5520,
    0x5000,0x5100,0x5200,0x5300,0x5400,0x5500,0x5600,0x5700,
    0x5800,0x5900,0x6020,0x6120,0x6220,0x6320,0x6420,0x6520,
    0x6000,0x6100,0x6200,0x6300,0x6400,0x6500,0x6600,0x6700,
    0x6800,0x6900,0x7020,0x7120,0x7220,0x7320,0x7420,0x7520,
    0x7000,0x7100,0x7200,0x7300,0x7400,0x7500,0x7600,0x7700,
    0x7800,0x7900,0x8020,0x8120,0x8220,0x8320,0x8420,0x8520,
    0x8000,0x8100,0x8200,0x8300,0x8400,0x8500,0x8600,0x8700,
    0x8800,0x8900,0x9020,0x9120,0x9220,0x9320,0x9420,0x9520,
    0x9000,0x9100,0x9200,0x9300,0x9400,0x9500,0x9600,0x9700,
    0x9800,0x9900,0x00B0,0x0130,0x0230,0x0330,0x0430,0x0530,
    0x0090,0x0110,0x0210,0x0310,0x0410,0x0510,0x0610,0x0710,
    0x0810,0x0910,0x1030,0x1130,0x1230,0x1330,0x1430,0x1530,
    0x1010,0x1110,0x1210,0x1310,0x1410,0x1510,0x1610,0x1710,
    0x1810,0x1910,0x2030,0x2130,0x2230,0x2330,0x2430,0x2530,
    0x2010,0x2110,0x2210,0x2310,0x2410,0x2510,0x2610,0x2710,
    0x2810,0x2910,0x3030,0x3130,0x3230,0x3330,0x3430,0x3530,
    0x3010,0x3110,0x3210,0x3310,0x3410,0x3510,0x3610,0x3710,
    0x3810,0x3910,0x4030,0x4130,0x4230,0x4330,0x4430,0x4530,
    0x4010,0x4110,0x4210,0x4310,0x4410,0x4510,0x4610,0x4710,
    0x4810,0x4910,0x5030,0x5130,0x5230,0x5330,0x5430,0x5530,
    0x5010,0x5110,0x5210,0x5310,0x5410,0x5510,0x5610,0x5710,
    0x5810,0x5910,0x6030,0x6130,0x6230,0x6330,0x6430,0x6530,
    0x6010,0x6110,0x6210,0x6310,0x6410,0x6510,0x6610,0x6710,
    0x6810,0x6910,0x7030,0x7130,0x7230,0x7330,0x7430,0x7530,
    0x7010,0x7110,0x7210,0x7310,0x7410,0x7510,0x7610,0x7710,
    0x7810,0x7910,0x8030,0x8130,0x8230,0x8330,0x8430,0x8530,
    0x8010,0x8110,0x8210,0x8310,0x8410,0x8510,0x8610,0x8710,
    0x8810,0x8910,0x9030,0x9130,0x9230,0x9330,0x9430,0x9530,
    0x9010,0x9110,0x9210,0x9310,0x9410,0x9510,0x9610,0x9710,
    0x9810,0x9910,0xA030,0xA130,0xA230,0xA330,0xA430,0xA530,
    0xA010,0xA110,0xA210,0xA310,0xA410,0xA510,0xA610,0xA710,
    0xA810,0xA910,0xB030,0xB130,0xB230,0xB330,0xB430,0xB530,
    0xB010,0xB110,0xB210,0xB310,0xB410,0xB510,0xB610,0xB710,
    0xB810,0xB910,0xC030,0xC130,0xC230,0xC330,0xC430,0xC530,
    0xC010,0xC110,0xC210,0xC310,0xC410,0xC510,0xC610,0xC710,
    0xC810,0xC910,0xD030,0xD130,0xD230,0xD330,0xD430,0xD530,
    0xD010,0xD110,0xD210,0xD310,0xD410,0xD510,0xD610,0xD710,
    0xD810,0xD910,0xE030,0xE130,0xE230,0xE330,0xE430,0xE530,
    0xE010,0xE110,0xE210,0xE310,0xE410,0xE510,0xE610,0xE710,
    0xE810,0xE910,0xF030,0xF130,0xF230,0xF330,0xF430,0xF530,
    0xF010,0xF110,0xF210,0xF310,0xF410,0xF510,0xF610,0xF710,
    0xF810,0xF910,0x00B0,0x0130,0x0230,0x0330,0x0430,0x0530,
    0x0090,0x0110,0x0210,0x0310,0x0410,0x0510,0x0610,0x0710,
    0x0810,0x0910,0x1030,0x1130,0x1230,0x1330,0x1430,0x1530,
    0x1010,0x1110,0x1210,0x1310,0x1410,0x1510,0x1610,0x1710,
    0x1810,0x1910,0x2030,0x2130,0x2230,0x2330,0x2430,0x2530,
    0x2010,0x2110,0x2210,0x2310,0x2410,0x2510,0x2610,0x2710,
    0x2810,0x2910,0x3030,0x3130,0x3230,0x3330,0x3430,0x3530,
    0x3010,0x3110,0x3210,0x3310,0x3410,0x3510,0x3610,0x3710,
    0x3810,0x3910,0x4030,0x4130,0x4230,0x4330,0x4430,0x4530,
    0x4010,0x4110,0x4210,0x4310,0x4410,0x4510,0x4610,0x4710,
    0x4810,0x4910,0x5030,0x5130,0x5230,0x5330,0x5430,0x5530,
    0x5010,0x5110,0x5210,0x5310,0x5410,0x5510,0x5610,0x5710,
    0x5810,0x5910,0x6030,0x6130,0x6230,0x6330,0x6430,0x6530,
    0x0600,0x0700,0x0800,0x0900,0x0A00,0x0B00,0x0C00,0x0D00,
    0x0E00,0x0F00,0x1020,0x1120,0x1220,0x1320,0x1420,0x1520,
    0x1600,0x1700,0x1800,0x1900,0x1A00,0x1B00,0x1C00,0x1D00,
    0x1E00,0x1F00,0x2020,0x2120,0x2220,0x2320,0x2420,0x2520,
    0x2600,0x2700,0x2800,0x2900,0x2A00,0x2B00,0x2C00,0x2D00,
    0x2E00,0x2F00,0x3020,0x3120,0x3220,0x3320,0x3420,0x3520,
    0x3600,0x3700,0x3800,0x3900,0x3A00,0x3B00,0x3C00,0x3D00,
    0x3E00,0x3F00,0x4020,0x4120,0x4220,0x4320,0x4420,0x4520,
    0x4600,0x4700,0x4800,0x4900,0x4A00,0x4B00,0x4C00,0x4D00,
    0x4E00,0x4F00,0x5020,0x5120,0x5220,0x5320,0x5420,0x5520,
    0x5600,0x5700,0x5800,0x5900,0x5A00,0x5B00,0x5C00,0x5D00,
    0x5E00,0x5F00,0x6020,0x6120,0x6220,0x6320,0x6420,0x6520,
    0x6600,0x6700,0x6800,0x6900,0x6A00,0x6B00,0x6C00,0x6D00,
    0x6E00,0x6F00,0x7020,0x7120,0x7220,0x7320,0x7420,0x7520,
    0x7600,0x7700,0x7800,0x7900,0x7A00,0x7B00,0x7C00,0x7D00,
    0x7E00,0x7F00,0x8020,0x8120,0x8220,0x8320,0x8420,0x8520,
    0x8600,0x8700,0x8800,0x8900,0x8A00,0x8B00,0x8C00,0x8D00,
    0x8E00,0x8F00,0x9020,0x9120,0x9220,0x9320,0x9420,0x9520,
    0x9600,0x9700,0x9800,0x9900,0x9A00,0x9B00,0x9C00,0x9D00,
    0x9E00,0x9F00,0x00B0,0x0130,0x0230,0x0330,0x0430,0x0530,
    0x0610,0x0710,0x0810,0x0910,0x0A10,0x0B10,0x0C10,0x0D10,
    0x0E10,0x0F10,0x1030,0x1130,0x1230,0x1330,0x1430,0x1530,
    0x1610,0x1710,0x1810,0x1910,0x1A10,0x1B10,0x1C10,0x1D10,
    0x1E10,0x1F10,0x2030,0x2130,0x2230,0x2330,0x2430,0x2530,
    0x2610,0x2710,0x2810,0x2910,0x2A10,0x2B10,0x2C10,0x2D10,
    0x2E10,0x2F10,0x3030,0x3130,0x3230,0x3330,0x3430,0x3530,
    0x3610,0x3710,0x3810,0x3910,0x3A10,0x3B10,0x3C10,0x3D10,
    0x3E10,0x3F10,0x4030,0x4130,0x4230,0x4330,0x4430,0x4530,
    0x4610,0x4710,0x4810,0x4910,0x4A10,0x4B10,0x4C10,0x4D10,
    0x4E10,0x4F10,0x5030,0x5130,0x5230,0x5330,0x5430,0x5530,
    0x5610,0x5710,0x5810,0x5910,0x5A10,0x5B10,0x5C10,0x5D10,
    0x5E10,0x5F10,0x6030,0x6130,0x6230,0x6330,0x6430,0x6530,
    0x6610,0x6710,0x6810,0x6910,0x6A10,0x6B10,0x6C10,0x6D10,
    0x6E10,0x6F10,0x7030,0x7130,0x7230,0x7330,0x7430,0x7530,
    0x7610,0x7710,0x7810,0x7910,0x7A10,0x7B10,0x7C10,0x7D10,
    0x7E10,0x7F10,0x8030,0x8130,0x8230,0x8330,0x8430,0x8530,
    0x8610,0x8710,0x8810,0x8910,0x8A10,0x8B10,0x8C10,0x8D10,
    0x8E10,0x8F10,0x9030,0x9130,0x9230,0x9330,0x9430,0x9530,
    0x9610,0x9710,0x9810,0x9910,0x9A10,0x9B10,0x9C10,0x9D10,
    0x9E10,0x9F10,0xA030,0xA130,0xA230,0xA330,0xA430,0xA530,
    0xA610,0xA710,0xA810,0xA910,0xAA10,0xAB10,0xAC10,0xAD10,
    0xAE10,0xAF10,0xB030,0xB130,0xB230,0xB330,0xB430,0xB530,
    0xB610,0xB710,0xB810,0xB910,0xBA10,0xBB10,0xBC10,0xBD10,
    0xBE10,0xBF10,0xC030,0xC130,0xC230,0xC330,0xC430,0xC530,
    0xC610,0xC710,0xC810,0xC910,0xCA10,0xCB10,0xCC10,0xCD10,
    0xCE10,0xCF10,0xD030,0xD130,0xD230,0xD330,0xD430,0xD530,
    0xD610,0xD710,0xD810,0xD910,0xDA10,0xDB10,0xDC10,0xDD10,
    0xDE10,0xDF10,0xE030,0xE130,0xE230,0xE330,0xE430,0xE530,
    0xE610,0xE710,0xE810,0xE910,0xEA10,0xEB10,0xEC10,0xED10,
    0xEE10,0xEF10,0xF030,0xF130,0xF230,0xF330,0xF430,0xF530,
    0xF610,0xF710,0xF810,0xF910,0xFA10,0xFB10,0xFC10,0xFD10,
    0xFE10,0xFF10,0x00B0,0x0130,0x0230,0x0330,0x0430,0x0530,
    0x0610,0x0710,0x0810,0x0910,0x0A10,0x0B10,0x0C10,0x0D10,
    0x0E10,0x0F10,0x1030,0x1130,0x1230,0x1330,0x1430,0x1530,
    0x1610,0x1710,0x1810,0x1910,0x1A10,0x1B10,0x1C10,0x1D10,
    0x1E10,0x1F10,0x2030,0x2130,0x2230,0x2330,0x2430,0x2530,
    0x2610,0x2710,0x2810,0x2910,0x2A10,0x2B10,0x2C10,0x2D10,
    0x2E10,0x2F10,0x3030,0x3130,0x3230,0x3330,0x3430,0x3530,
    0x3610,0x3710,0x3810,0x3910,0x3A10,0x3B10,0x3C10,0x3D10,
    0x3E10,0x3F10,0x4030,0x4130,0x4230,0x4330,0x4430,0x4530,
    0x4610,0x4710,0x4810,0x4910,0x4A10,0x4B10,0x4C10,0x4D10,
    0x4E10,0x4F10,0x5030,0x5130,0x5230,0x5330,0x5430,0x5530,
    0x5610,0x5710,0x5810,0x5910,0x5A10,0x5B10,0x5C10,0x5D10,
    0x5E10,0x5F10,0x6030,0x6130,0x6230,0x6330,0x6430,0x6530,
    0x00C0,0x0140,0x0240,0x0340,0x0440,0x0540,0x0640,0x0740,
    0x0840,0x0940,0x0440,0x0540,0x0640,0x0740,0x0840,0x0940,
    0x1040,0x1140,0x1240,0x1340,0x1440,0x1540,0x1640,0x1740,
    0x1840,0x1940,0x1440,0x1540,0x1640,0x1740,0x1840,0x1940,
    0x2040,0x2140,0x2240,0x2340,0x2440,0x2540,0x2640,0x2740,
    0x2840,0x2940,0x2440,0x2540,0x2640,0x2740,0x2840,0x2940,
    0x3040,0x3140,0x3240,0x3340,0x3440,0x3540,0x3640,0x3740,
    0x3840,0x3940,0x3440,0x3540,0x3640,0x3740,0x3840,0x3940,
    0x4040,0x4140,0x4240,0x4340,0x4440,0x4540,0x4640,0x4740,
    0x4840,0x4940,0x4440,0x4540,0x4640,0x4740,0x4840,0x4940,
    0x5040,0x5140,0x5240,0x5340,0x5440,0x5540,0x5640,0x5740,
    0x5840,0x5940,0x5440,0x5540,0x5640,0x5740,0x5840,0x5940,
    0x6040,0x6140,0x6240,0x6340,0x6440,0x6540,0x6640,0x6740,
    0x6840,0x6940,0x6440,0x6540,0x6640,0x6740,0x6840,0x6940,
    0x7040,0x7140,0x7240,0x7340,0x7440,0x7540,0x7640,0x7740,
    0x7840,0x7940,0x7440,0x7540,0x7640,0x7740,0x7840,0x7940,
    0x8040,0x8140,0x8240,0x8340,0x8440,0x8540,0x8640,0x8740,
    0x8840,0x8940,0x8440,0x8540,0x8640,0x8740,0x8840,0x8940,
    0x9040,0x9140,0x9240,0x9340,0x9440,0x9540,0x9640,0x9740,
    0x9840,0x9940,0x3450,0x3550,0x3650,0x3750,0x3850,0x3950,
    0x4050,0x4150,0x4250,0x4350,0x4450,0x4550,0x4650,0x4750,
    0x4850,0x4950,0x4450,0x4550,0x4650,0x4750,0x4850,0x4950,
    0x5050,0x5150,0x5250,0x5350,0x5450,0x5550,0x5650,0x5750,
    0x5850,0x5950,0x5450,0x5550,0x5650,0x5750,0x5850,0x5950,
    0x6050,0x6150,0x6250,0x6350,0x6450,0x6550,0x6650,0x6750,
    0x6850,0x6950,0x6450,0x6550,0x6650,0x6750,0x6850,0x6950,
    0x7050,0x7150,0x7250,0x7350,0x7450,0x7550,0x7650,0x7750,
    0x7850,0x7950,0x7450,0x7550,0x7650,0x7750,0x7850,0x7950,
    0x8050,0x8150,0x8250,0x8350,0x8450,0x8550,0x8650,0x8750,
    0x8850,0x8950,0x8450,0x8550,0x8650,0x8750,0x8850,0x8950,
    0x9050,0x9150,0x9250,0x9350,0x9450,0x9550,0x9650,0x9750,
    0x9850,0x9950,0x9450,0x9550,0x9650,0x9750,0x9850,0x9950,
    0xA050,0xA150,0xA250,0xA350,0xA450,0xA550,0xA650,0xA750,
    0xA850,0xA950,0xA450,0xA550,0xA650,0xA750,0xA850,0xA950,
    0xB050,0xB150,0xB250,0xB350,0xB450,0xB550,0xB650,0xB750,
    0xB850,0xB950,0xB450,0xB550,0xB650,0xB750,0xB850,0xB950,
    0xC050,0xC150,0xC250,0xC350,0xC450,0xC550,0xC650,0xC750,
    0xC850,0xC950,0xC450,0xC550,0xC650,0xC750,0xC850,0xC950,
    0xD050,0xD150,0xD250,0xD350,0xD450,0xD550,0xD650,0xD750,
    0xD850,0xD950,0xD450,0xD550,0xD650,0xD750,0xD850,0xD950,
    0xE050,0xE150,0xE250,0xE350,0xE450,0xE550,0xE650,0xE750,
    0xE850,0xE950,0xE450,0xE550,0xE650,0xE750,0xE850,0xE950,
    0xF050,0xF150,0xF250,0xF350,0xF450,0xF550,0xF650,0xF750,
    0xF850,0xF950,0xF450,0xF550,0xF650,0xF750,0xF850,0xF950,
    0x00D0,0x0150,0x0250,0x0350,0x0450,0x0550,0x0650,0x0750,
    0x0850,0x0950,0x0450,0x0550,0x0650,0x0750,0x0850,0x0950,
    0x1050,0x1150,0x1250,0x1350,0x1450,0x1550,0x1650,0x1750,
    0x1850,0x1950,0x1450,0x1550,0x1650,0x1750,0x1850,0x1950,
    0x2050,0x2150,0x2250,0x2350,0x2450,0x2550,0x2650,0x2750,
    0x2850,0x2950,0x2450,0x2550,0x2650,0x2750,0x2850,0x2950,
    0x3050,0x3150,0x3250,0x3350,0x3450,0x3550,0x3650,0x3750,
    0x3850,0x3950,0x3450,0x3550,0x3650,0x3750,0x3850,0x3950,
    0x4050,0x4150,0x4250,0x4350,0x4450,0x4550,0x4650,0x4750,
    0x4850,0x4950,0x4450,0x4550,0x4650,0x4750,0x4850,0x4950,
    0x5050,0x5150,0x5250,0x5350,0x5450,0x5550,0x5650,0x5750,
    0x5850,0x5950,0x5450,0x5550,0x5650,0x5750,0x5850,0x5950,
    0x6050,0x6150,0x6250,0x6350,0x6450,0x6550,0x6650,0x6750,
    0x6850,0x6950,0x6450,0x6550,0x6650,0x6750,0x6850,0x6950,
    0x7050,0x7150,0x7250,0x7350,0x7450,0x7550,0x7650,0x7750,
    0x7850,0x7950,0x7450,0x7550,0x7650,0x7750,0x7850,0x7950,
    0x8050,0x8150,0x8250,0x8350,0x8450,0x8550,0x8650,0x8750,
    0x8850,0x8950,0x8450,0x8550,0x8650,0x8750,0x8850,0x8950,
    0x9050,0x9150,0x9250,0x9350,0x9450,0x9550,0x9650,0x9750,
    0x9850,0x9950,0x9450,0x9550,0x9650,0x9750,0x9850,0x9950,
    0xFA60,0xFB60,0xFC60,0xFD60,0xFE60,0xFF60,0x00C0,0x0140,
    0x0240,0x0340,0x0440,0x0540,0x0640,0x0740,0x0840,0x0940,
    0x0A60,0x0B60,0x0C60,0x0D60,0x0E60,0x0F60,0x1040,0x1140,
    0x1240,0x1340,0x1440,0x1540,0x1640,0x1740,0x1840,0x1940,
    0x1A60,0x1B60,0x1C60,0x1D60,0x1E60,0x1F60,0x2040,0x2140,
    0x2240,0x2340,0x2440,0x2540,0x2640,0x2740,0x2840,0x2940,
    0x2A60,0x2B60,0x2C60,0x2D60,0x2E60,0x2F60,0x3040,0x3140,
    0x3240,0x3340,0x3440,0x3540,0x3640,0x3740,0x3840,0x3940,
    0x3A60,0x3B60,0x3C60,0x3D60,0x3E60,0x3F60,0x4040,0x4140,
    0x4240,0x4340,0x4440,0x4540,0x4640,0x4740,0x4840,0x4940,
    0x4A60,0x4B60,0x4C60,0x4D60,0x4E60,0x4F60,0x5040,0x5140,
    0x5240,0x5340,0x5440,0x5540,0x5640,0x5740,0x5840,0x5940,
    0x5A60,0x5B60,0x5C60,0x5D60,0x5E60,0x5F60,0x6040,0x6140,
    0x6240,0x6340,0x6440,0x6540,0x6640,0x6740,0x6840,0x6940,
    0x6A60,0x6B60,0x6C60,0x6D60,0x6E60,0x6F60,0x7040,0x7140,
    0x7240,0x7340,0x7440,0x7540,0x7640,0x7740,0x7840,0x7940,
    0x7A60,0x7B60,0x7C60,0x7D60,0x7E60,0x7F60,0x8040,0x8140,
    0x8240,0x8340,0x8440,0x8540,0x8640,0x8740,0x8840,0x8940,
    0x8A60,0x8B60,0x8C60,0x8D60,0x8E60,0x8F60,0x9040,0x9140,
    0x9240,0x9340,0x3450,0x3550,0x3650,0x3750,0x3850,0x3950,
    0x3A70,0x3B70,0x3C70,0x3D70,0x3E70,0x3F70,0x4050,0x4150,
    0x4250,0x4350,0x4450,0x4550,0x4650,0x4750,0x4850,0x4950,
    0x4A70,0x4B70,0x4C70,0x4D70,0x4E70,0x4F70,0x5050,0x5150,
    0x5250,0x5350,0x5450,0x5550,0x5650,0x5750,0x5850,0x5950,
    0x5A70,0x5B70,0x5C70,0x5D70,0x5E70,0x5F70,0x6050,0x6150,
    0x6250,0x6350,0x6450,0x6550,0x6650,0x6750,0x6850,0x6950,
    0x6A70,0x6B70,0x6C70,0x6D70,0x6E70,0x6F70,0x7050,0x7150,
    0x7250,0x7350,0x7450,0x7550,0x7650,0x7750,0x7850,0x7950,
    0x7A70,0x7B70,0x7C70,0x7D70,0x7E70,0x7F70,0x8050,0x8150,
    0x8250,0x8350,0x8450,0x8550,0x8650,0x8750,0x8850,0x8950,
    0x8A70,0x8B70,0x8C70,0x8D70,0x8E70,0x8F70,0x9050,0x9150,
    0x9250,0x9350,0x9450,0x9550,0x9650,0x9750,0x9850,0x9950,
    0x9A70,0x9B70,0x9C70,0x9D70,0x9E70,0x9F70,0xA050,0xA150,
    0xA250,0xA350,0xA450,0xA550,0xA650,0xA750,0xA850,0xA950,
    0xAA70,0xAB70,0xAC70,0xAD70,0xAE70,0xAF70,0xB050,0xB150,
    0xB250,0xB350,0xB450,0xB550,0xB650,0xB750,0xB850,0xB950,
    0xBA70,0xBB70,0xBC70,0xBD70,0xBE70,0xBF70,0xC050,0xC150,
    0xC250,0xC350,0xC450,0xC550,0xC650,0xC750,0xC850,0xC950,
    0xCA70,0xCB70,0xCC70,0xCD70,0xCE70,0xCF70,0xD050,0xD150,
    0xD250,0xD350,0xD450,0xD550,0xD650,0xD750,0xD850,0xD950,
    0xDA70,0xDB70,0xDC70,0xDD70,0xDE70,0xDF70,0xE050,0xE150,
    0xE250,0xE350,0xE450,0xE550,0xE650,0xE750,0xE850,0xE950,
    0xEA70,0xEB70,0xEC70,0xED70,0xEE70,0xEF70,0xF050,0xF150,
    0xF250,0xF350,0xF450,0xF550,0xF650,0xF750,0xF850,0xF950,
    0xFA70,0xFB70,0xFC70,0xFD70,0xFE70,0xFF70,0x00D0,0x0150,
    0x0250,0x0350,0x0450,0x0550,0x0650,0x0750,0x0850,0x0950,
    0x0A70,0x0B70,0x0C70,0x0D70,0x0E70,0x0F70,0x1050,0x1150,
    0x1250,0x1350,0x1450,0x1550,0x1650,0x1750,0x1850,0x1950,
    0x1A70,0x1B70,0x1C70,0x1D70,0x1E70,0x1F70,0x2050,0x2150,
    0x2250,0x2350,0x2450,0x2550,0x2650,0x2750,0x2850,0x2950,
    0x2A70,0x2B70,0x2C70,0x2D70,0x2E70,0x2F70,0x3050,0x3150,
    0x3250,0x3350,0x3450,0x3550,0x3650,0x3750,0x3850,0x3950,
    0x3A70,0x3B70,0x3C70,0x3D70,0x3E70,0x3F70,0x4050,0x4150,
    0x4250,0x4350,0x4450,0x4550,0x4650,0x4750,0x4850,0x4950,
    0x4A70,0x4B70,0x4C70,0x4D70,0x4E70,0x4F70,0x5050,0x5150,
    0x5250,0x5350,0x5450,0x5550,0x5650,0x5750,0x5850,0x5950,
    0x5A70,0x5B70,0x5C70,0x5D70,0x5E70,0x5F70,0x6050,0x6150,
    0x6250,0x6350,0x6450,0x6550,0x6650,0x6750,0x6850,0x6950,
    0x6A70,0x6B70,0x6C70,0x6D70,0x6E70,0x6F70,0x7050,0x7150,
    0x7250,0x7350,0x7450,0x7550,0x7650,0x7750,0x7850,0x7950,
    0x7A70,0x7B70,0x7C70,0x7D70,0x7E70,0x7F70,0x8050,0x8150,
    0x8250,0x8350,0x8450,0x8550,0x8650,0x8750,0x8850,0x8950,
    0x8A70,0x8B70,0x8C70,0x8D70,0x8E70,0x8F70,0x9050,0x9150,
    0x9250,0x9350,0x9450,0x9550,0x9650,0x9750,0x9850,0x9950,
}
;
u8 ZeroTable[] = 
{
    0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
}
;
u8 gbInvertTab[256] = 
{
    0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,
    0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
    0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,
    0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
    0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,
    0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
    0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,
    0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
    0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,
    0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
    0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,
    0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
    0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,
    0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
    0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,
    0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
    0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,
    0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
    0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,
    0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
    0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,
    0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
    0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,
    0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
    0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,
    0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
    0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,
    0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
    0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,
    0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
    0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,
    0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
}
;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGbEmu::CGbEmu()
{
    this->InitBasic();
}
CGbEmu::~CGbEmu()
{
    this->Destroy();
}
int CGbEmu::InitBasic()
{
    PC.W = 0;
    SP.W = 0;
    AF.W = 0;
    BC.W = 0;
    DE.W = 0;
    HL.W = 0;
    IFF = 0;
    register_DIV = 0;
    register_TIMA = 0;
    register_TMA = 0;
    register_TAC = 0;
    register_IF = 0;
    register_LCDC = 0;
    register_STAT = 0;
    register_SCY = 0;
    register_SCX = 0;
    register_LY = 0;
    register_LYC = 0;
    register_DMA = 0;
    register_WY = 0;
    register_WX = 0;
    register_VBK = 0;
    register_HDMA1 = 0;
    register_HDMA2 = 0;
    register_HDMA3 = 0;
    register_HDMA4 = 0;
    register_HDMA5 = 0;
    register_SVBK = 0;
    register_IE = 0;
    GBDIV_CLOCK_TICKS = 64;
    GBLCD_MODE_0_CLOCK_TICKS = 51;
    GBLCD_MODE_1_CLOCK_TICKS = 1140;
    GBLCD_MODE_2_CLOCK_TICKS = 20;
    GBLCD_MODE_3_CLOCK_TICKS = 43;
    GBLY_INCREMENT_CLOCK_TICKS = 114;
    GBTIMER_MODE_0_CLOCK_TICKS = 256;
    GBTIMER_MODE_1_CLOCK_TICKS = 4;
    GBTIMER_MODE_2_CLOCK_TICKS = 16;
    GBTIMER_MODE_3_CLOCK_TICKS = 64;
    GBSERIAL_CLOCK_TICKS = 128;
    GBSYNCHRONIZE_CLOCK_TICKS = 52920;
    gbInterrupt = 0;
    gbInterruptWait = 0;
    gbSerialOn = 0;
    gbSerialTicks = 0;
    gbSerialBits = 0;
    gbTimerOn = 0;
    gbTimerTicks = 0;
    gbTimerClockTicks = 0;
    gbTimerMode = 0;
    gbLcdMode = 2;
    gbLcdTicks = GBLCD_MODE_2_CLOCK_TICKS;
    gbLcdLYIncrementTicks = 0;
    gbDivTicks = GBDIV_CLOCK_TICKS;
    gbVramBank = 0;
    gbWramBank = 1;
    gbHdmaSource = 0x0000;
    gbHdmaDestination = 0x8000;
    gbHdmaBytes = 0x0000;
    gbHdmaOn = 0;
    gbSpeed = 0;
    gbFrameCount = 0;
    gbFrameSkip = 0;
    gbFrameSkipCount = 0;
    gbLastTime = 0;
    gbElapsedTime = 0;
    gbTimeNow = 0;
    gbSynchronizeTicks = GBSYNCHRONIZE_CLOCK_TICKS;
    gbBattery = 0;
    gbCaptureNumber = 0;
    gbCapture = false;
    gbCapturePrevious = false;
    this->mapper = NULL;
    this->mapperRAM = NULL;
    this->mapperReadRAM = NULL;
    this->gbColorFilter = NULL;
    this->gbMemoryMap = NULL;
    this->gbMemory = NULL;
    this->gbVram = NULL;
    this->gbRom = NULL;
    this->gbRam = NULL;
    this->gbWram = NULL;
    this->gbLineBuffer = NULL;
    this->gbSgbBorderChar = 0;
    this->gbSgbBorder = 0;
    this->gbSgbCGBSupport = 0;
    this->gbSgbMask = 0;
    this->gbSgbMode = 0;
    this->gbSgbPacketState = 0;
    this->gbSgbBit = 0;
    this->gbSgbPacketTimeout = 0;
    this->GBSGB_PACKET_TIMEOUT = 0;
    this->gbSgbPacket = 0;
    this->gbSgbPacketNBits = 0;
    this->gbSgbPacketByte = 0;
    this->gbSgbPacketNumber = 0;
    this->gbSgbMultiplayer = 0;
    this->gbSgbFourPlayers = 0;
    this->gbSgbNextController = 0;
    this->gbSgbReadingController = 0;
    this->gbSgbSCPPalette = 0;
    this->gbSgbATF = 0;
    this->gbSgbATFList = 0;
    this->gbSgbScreenBuffer = 0;
    this->gbBgp[0] = 0;
    this->gbBgp[1] = 1;
    this->gbBgp[2] = 2;
    this->gbBgp[3] = 3;
    this->gbObp0[0] = 0;
    this->gbObp0[1] = 1;
    this->gbObp0[2] = 2;
    this->gbObp0[3] = 3;
    this->gbObp1[0] = 0;
    this->gbObp1[1] = 1;
    this->gbObp1[2] = 2;
    this->gbObp1[3] = 3;
    this->gbPalette = 0;
    this->gbWindowLine = 0;
    this->gbCgbMode = 0;
    this->gbColorOption = 0;
    this->gbPaletteOption = 0;
    this->gbEmulatorType = 0;
    this->gbBorderOn = 0;
    this->gbBorderAutomatic = 0;
    this->gbBorderLineSkip = 160;
    this->gbBorderRowSkip = 0;
    this->gbBorderColumnSkip = 0;
    this->gbDmaTicks = 0;
    bios = NULL;
    rom = NULL;
    internalRAM = NULL;
    workRAM = NULL;
    paletteRAM = NULL;
    vram = NULL;
    pix = NULL;
    oam = NULL;
    ioMem = NULL;
    this->systemColorMap32 = 0;
    this->systemColorMap16 = 0;
    this->systemGbPalette = 0;
    this->systemColorDepth = 16;
    this->systemRedShift = 0;
    this->systemGreenShift = 0;
    this->systemBlueShift = 0;
    this->systemDebug = 0;
    this->systemVerbose = 0;
    this->systemSaveUpdateCounter = 0;
    this->systemFrameSkip = 0;
    this->systemRenderedFrames = 0;
    gbJoymask[0] = 0;
    gbJoymask[1] = 0;
    gbJoymask[2] = 0;
    gbJoymask[3] = 0;
    memset(&gbDataMBC1,0,sizeof(gbDataMBC1));
    memset(&gbDataMBC2,0,sizeof(gbDataMBC2));
    memset(&gbDataMBC3,0,sizeof(gbDataMBC3));
    memset(&gbDataMBC5,0,sizeof(gbDataMBC5));
    memset(&gbDataMBC7,0,sizeof(gbDataMBC7));
    memset(&gbDataHuC1,0,sizeof(gbDataHuC1));
    memset(&gbDataHuC3,0,sizeof(gbDataHuC3));
    gbDataMBC1.mapperROMBank = 1;
    gbDataMBC2.mapperROMBank = 1;
    gbDataMBC3.mapperROMBank = 1;
    gbDataMBC3.mapperLastTime = -1;
    gbDataMBC5.mapperROMBank = 1;
    gbDataMBC7.mapperROMBank = 1;
    gbDataHuC1.mapperROMBank = 1;
    gbDataHuC3.mapperROMBank = 1;
    this->soundQuality = 2;
    soundTicks = soundQuality * USE_TICKS_AS;
    this->SOUND_CLOCK_TICKS = soundQuality * USE_TICKS_AS;
    this->gbRomSize = 0;
    this->gbRomSizeMask = 0;
    this->gbRamSizeMask = 0;
    this->gbRamSize = 0;
    this->emulating = 0;
    this->speedup = 0;
    this->gbLineMix = 0;
    this->layerSettings = 0xff00;
    this->call_back_read_joypad = NULL;
    this->param_read_joypad = NULL;

    return OK;
}
int CGbEmu::Init()
{
    int i;
    
    this->InitBasic();

    MALLOC(this->gbColorFilter,u16,32768);
    MALLOC(this->gbMemoryMap,u8*,16);
    MALLOC(this->gbSgbPacket,u8,16*7);
    MALLOC(this->gbSgbSCPPalette,u16,4*512);
    MALLOC(this->gbSgbATF,u8,20 * 18);
    MALLOC(this->gbSgbATFList,u8,45 * 20 * 18);
    MALLOC(this->gbSgbScreenBuffer,u8,4160);
    MALLOC(this->gbPalette,u16,128);
    MALLOC(this->systemColorMap32,u32,0x10000);
    MALLOC(this->systemColorMap16,u16,0x10000);
    MALLOC(this->systemGbPalette,u16,24);
    MALLOC(this->gbLineMix,u16,160);

    for(i = 0; i < 16*7; i++)
        this->gbSgbPacket[i] = 0;
    for(i = 0; i < 4*512; i++)
        this->gbSgbSCPPalette[i] = 0;
    for(i = 0; i < 20 * 18; i++)
        this->gbSgbATF[i] = 0;
    for(i = 0; i < 45 * 20 * 18; i++)
        this->gbSgbATFList[i] = 0;
    for(i = 0; i < 4160; i++)
        this->gbSgbScreenBuffer[i] = 0;
    for(i = 0; i < 128; i++)
        this->gbPalette[i] = 0;
    for(i = 0; i < 0x10000; i++)
        this->systemColorMap32[i] = 0;
    for(i = 0; i < 0x10000; i++)
        this->systemColorMap16[i] = 0;
    for(i = 0; i < 24; i++)
        this->systemGbPalette[i] = 0;
    for(i = 0; i < 160; i++)
        this->gbLineMix[i] = 0;

    return OK;
}
int CGbEmu::Destroy()
{
    FREE(this->gbLineMix);
    FREE(this->gbMemory);
    FREE(this->pix);
    FREE(this->gbLineBuffer);
    FREE(this->systemColorMap32);
    FREE(this->systemColorMap16);
    FREE(this->systemGbPalette);
    FREE(this->gbSgbPacket);
    FREE(this->gbSgbSCPPalette);
    FREE(this->gbSgbATF);
    FREE(this->gbSgbATFList);
    FREE(this->gbSgbScreenBuffer);
    FREE(this->gbMemoryMap);
    FREE(this->gbColorFilter);
    FREE(this->gbPalette);
    FREE(gbRam);
    FREE(gbRom);
    FREE(gbMemory);
    FREE(gbLineBuffer);
    FREE(gbVram);
    FREE(gbWram);
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    gbSgbShutdown();
    this->InitBasic();
    return OK;
}
int CGbEmu::Print()
{
    //add your code
    return TRUE;
}
int CGbEmu::gbGetValue(int min, int max, int v)
{
    return (int)(min+(float)(max-min)*(2.0*(v/31.0)-(v/31.0)*(v/31.0)));
}
int CGbEmu::gbGenFilter()
{
    for (int r=0;r<32;r++) 
    {
        for (int g=0;g<32;g++) 
        {
            for (int b=0;b<32;b++) 
            {
                int nr=gbGetValue(gbGetValue(4,14,g),
                gbGetValue(24,29,g),r)-4;
                int ng=gbGetValue(gbGetValue(4+gbGetValue(0,5,r),
                14+gbGetValue(0,3,r),b),
                gbGetValue(24+gbGetValue(0,3,r),
                29+gbGetValue(0,1,r),b),g)-4;
                int nb=gbGetValue(gbGetValue(4+gbGetValue(0,5,r),
                14+gbGetValue(0,3,r),g),
                gbGetValue(24+gbGetValue(0,3,r),
                29+gbGetValue(0,1,r),g),b)-4;
                gbColorFilter[(b<<10)|(g<<5)|r]=(nb<<10)|(ng<<5)|nr;
            }
        }
    }
    return OK;
}
int CGbEmu::gbIsGameBoyRom(char *file_name)
{
    ASSERT(file_name);
    char *p;
    int i,len;
    len = strlen(file_name);
    for(i = len - 1; i >= 0; i--)
    {
        if(file_name[i] == '.')
            break;
    }
    p = file_name + i + 1;
    if(p != NULL)
    {
        if(stricmp(p, ".gb") == 0)
            return TRUE;
        if(stricmp(p, ".gbc") == 0)
            return TRUE;
        if(stricmp(p, ".cgb") == 0)
            return TRUE;
        if(stricmp(p, ".sgb") == 0)
            return TRUE;      
    }
    return FALSE;
}
int CGbEmu::gbCopyMemory(u16 d, u16 s, int count)
{
    while(count) 
    {
        gbMemoryMap[d>>12][d & 0x0fff] = gbMemoryMap[s>>12][s & 0x0fff];
        s++;
        d++;
        count--;
    }
    return OK;
}
int CGbEmu::gbDoHdma()
{
    gbCopyMemory(gbHdmaDestination,gbHdmaSource,0x10);
    gbHdmaDestination += 0x10;
    gbHdmaSource += 0x10;
    register_HDMA2 += 0x10;
    if(register_HDMA2 == 0x00)
        register_HDMA1++;
    register_HDMA4 += 0x10;
    if(register_HDMA4 == 0x00)
        register_HDMA3++;
    if(gbHdmaDestination == 0x96b0)
        gbHdmaBytes = gbHdmaBytes;
    gbHdmaBytes -= 0x10;
    register_HDMA5--;
    if(register_HDMA5 == 0xff)
        gbHdmaOn = 0;
    return OK;
}
int CGbEmu::gbCompareLYToLYC()
{
    if(register_LY == register_LYC) 
    {
        // mark that we have a match
        register_STAT |= 4;
        // check if we need an interrupt
        if((register_STAT & 0x40) && (register_IE & 2))
            gbInterrupt |= 2;
    }
    else // no match
    {
        register_STAT &= 0xfb;
    }
    return OK;
}
int CGbEmu::gbWriteMemory(u16 address, u8 value)
{
    if(address < 0x8000) 
    {
        if(mapper)
        (this->*mapper)(address, value);
        return OK;
    }
    if(address < 0xa000) 
    {
        gbMemoryMap[address>>12][address&0x0fff] = value;
        return OK;
    }
    if(address < 0xc000) 
    {
        if(mapperRAM)
        (this->*mapperRAM)(address, value);
        return OK;
    }
    if(address < 0xfe00) 
    {
        gbMemoryMap[address>>12][address & 0x0fff] = value;
        return OK;
    }
    if(address < 0xff00) 
    {
        gbMemory[address] = value;
        return OK;
    }
    switch(address & 0x00ff) 
    {
        case 0x00: 
        {
            gbMemory[0xff00] = ((gbMemory[0xff00] & 0xcf) | (value & 0x30));
            if(gbSgbMode) 
            {
                gbSgbDoBitTransfer(value);
            }
            return OK;
        }
        case 0x01: 
        {
            gbMemory[0xff01] = value;
            return OK;
        }
        // serial control
        case 0x02: 
        {
            gbSerialOn = (value & 0x80);
            gbMemory[0xff02] = value;    
            if(gbSerialOn) 
            {
                gbSerialTicks = GBSERIAL_CLOCK_TICKS;
            }
            gbSerialBits = 0;
            return OK;
        }
        // DIV register resets on any write
        case 0x04: 
        {
            register_DIV = 0;
            return OK;
        }
        case 0x05:
        register_TIMA = value;
        return OK;
        case 0x06:
        register_TMA = value;
        return OK;
        // TIMER control
        case 0x07: 
        {
            register_TAC = value;
            gbTimerOn = (value & 4);
            gbTimerMode = value & 3;
            //register_TIMA = register_TMA;
            switch(gbTimerMode) 
            {
                case 0:
                gbTimerClockTicks = gbTimerTicks = GBTIMER_MODE_0_CLOCK_TICKS;
                break;
                case 1:
                gbTimerClockTicks = gbTimerTicks = GBTIMER_MODE_1_CLOCK_TICKS;
                break;
                case 2:
                gbTimerClockTicks = gbTimerTicks = GBTIMER_MODE_2_CLOCK_TICKS;
                break;
                case 3:
                gbTimerClockTicks = gbTimerTicks = GBTIMER_MODE_3_CLOCK_TICKS;
                break;
            }
            return OK;
        }
        case 0x0f: 
        {
            register_IF = value;
            gbInterrupt = value;
            return OK;
        }
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26: 
        {
            SOUND_EVENT(address,value);
            return OK;
        }
        case 0x40: 
        {
            int lcdChange = (register_LCDC & 0x80) ^ (value & 0x80);
            if(lcdChange) 
            {
                if(value & 0x80) 
                {
                    gbLcdTicks = GBLCD_MODE_1_CLOCK_TICKS;
                    gbLcdMode = 0;
                    register_STAT &= 0xfc;
                    register_LY = 0x00;
                }
                else 
                {
                    gbLcdTicks = 0;
                    gbLcdMode = 0;
                    register_STAT &= 0xfc;
                    register_LY = 0x00;
                }
            }
            // don't draw the window if it was not enabled and not being drawn before
            if(!(register_LCDC & 0x20) && (value & 0x20) && gbWindowLine == -1 && register_LY > register_WY)
                gbWindowLine = 144;
            register_LCDC = value;
            return OK;
        }
        // STAT
        case 0x41: 
        {
            register_STAT = (value & 0xf8) | (register_STAT & 0x07); // fix ?
            // GB bug from Devrs FAQ
            if(!gbCgbMode && (register_LCDC & 0x80) && gbLcdMode < 2)
                gbInterrupt |= 2;
            return OK;
        }
        // SCY
        case 0x42: 
        {
            register_SCY = value;
            return OK;
        }
        // SCX
        case 0x43: 
        {
            register_SCX = value;
            return OK;
        }
        // LY
        case 0x44: 
        {
            // read only
            return OK;
        }
        // LYC
        case 0x45: 
        {
            register_LYC = value;
            if((register_LCDC & 0x80)) 
            {
                gbCompareLYToLYC();
            }
            return OK;
        }
        // DMA!
        case 0x46: 
        {
            int source = value * 0x0100;
            gbCopyMemory(0xfe00,source,0xa0);
            register_DMA = value;
            return OK;
        }
        // BGP
        case 0x47: 
        {
            gbBgp[0] = value & 0x03;
            gbBgp[1] = (value & 0x0c)>>2;
            gbBgp[2] = (value & 0x30)>>4;
            gbBgp[3] = (value & 0xc0)>>6;
            break;
        }
        // OBP0
        case 0x48: 
        {
            gbObp0[0] = value & 0x03;
            gbObp0[1] = (value & 0x0c)>>2;
            gbObp0[2] = (value & 0x30)>>4;
            gbObp0[3] = (value & 0xc0)>>6;
            break;
        }
        // OBP1
        case 0x49: 
        {
            gbObp1[0] = value & 0x03;
            gbObp1[1] = (value & 0x0c)>>2;
            gbObp1[2] = (value & 0x30)>>4;
            gbObp1[3] = (value & 0xc0)>>6;
            break;
        }
        case 0x4a:
        register_WY = value;
        return OK;
        case 0x4b:
        register_WX = value;
        return OK;
        // KEY1
        case 0x4d: 
        {
            if(gbCgbMode) 
            {
                gbMemory[0xff4d] = (gbMemory[0xff4d] & 0x80) | (value & 1);
                return OK;
            }
        }
        break;
        // VBK
        case 0x4f: 
        {
            if(gbCgbMode) 
            {
                value = value & 1;
                if(value == gbVramBank)
                    return OK;
                int vramAddress = value * 0x2000;
                gbMemoryMap[0x08] = &gbVram[vramAddress];
                gbMemoryMap[0x09] = &gbVram[vramAddress + 0x1000];
                gbVramBank = value;
                register_VBK = value;
            }
            return OK;
        }
        break;
        // HDMA1
        case 0x51: 
        {
            if(gbCgbMode) 
            {
                if(value > 0x7f && value < 0xa0)
                    value = 0;
                gbHdmaSource = (value << 8) | (register_HDMA2 & 0xf0);
                register_HDMA1 = value;
                return OK;
            }
        }
        break;
        // HDMA2
        case 0x52: 
        {
            if(gbCgbMode) 
            {
                value = value & 0xf0;
                gbHdmaSource = (register_HDMA1 << 8) | (value);
                register_HDMA2 = value;
                return OK;
            }
        }
        break;
        // HDMA3
        case 0x53: 
        {
            if(gbCgbMode) 
            {
                value = value & 0x1f;
                gbHdmaDestination = (value << 8) | (register_HDMA4 & 0xf0);
                gbHdmaDestination += 0x8000;
                register_HDMA3 = value;
                return OK;
            }
        }
        break;
        // HDMA4
        case 0x54: 
        {
            if(gbCgbMode) 
            {
                value = value & 0xf0;
                gbHdmaDestination = ((register_HDMA3 & 0x1f) << 8) | value;
                gbHdmaDestination += 0x8000;
                register_HDMA4 = value;
                return OK;
            }
        }
        break;
        // HDMA5
        case 0x55: 
        {
            if(gbCgbMode) 
            {
                gbHdmaBytes = 16 + (value & 0x7f) * 16;
                if(gbHdmaOn) 
                {
                    if(value & 0x80) 
                    {
                        register_HDMA5 = (value & 0x7f);
                    }
                    else 
                    {
                        register_HDMA5 = 0xff;
                        gbHdmaOn = 0;
                    }
                }
                else 
                {
                    if(value & 0x80) 
                    {
                        gbHdmaOn = 1;
                        register_HDMA5 = value & 0x7f;
                        if(gbLcdMode == 0)
                            gbDoHdma();
                    }
                    else 
                    {
                        // we need to take the time it takes to complete the transfer into
                        // account... according to GB DEV FAQs, the setup time is the same
                        // for single and double speed, but the actual transfer takes the
                        // same time
                        if(gbSpeed)
                            gbDmaTicks = 231 + 16 * (value & 0x7f);
                        else
                            gbDmaTicks = 231 + 8 * (value & 0x7f);
                        gbCopyMemory(gbHdmaDestination, gbHdmaSource, gbHdmaBytes);
                        gbHdmaDestination += gbHdmaBytes;
                        gbHdmaSource += gbHdmaBytes;
                        register_HDMA3 = ((gbHdmaDestination - 0x8000) >> 8) & 0x1f;
                        register_HDMA4 = gbHdmaDestination & 0xf0;
                        register_HDMA1 = (gbHdmaSource >> 8) & 0xff;
                        register_HDMA2 = gbHdmaSource & 0xf0;
                    }
                }
                return OK;
            }
        }
        break;
        // BCPS
        case 0x68: 
        {
            if(gbCgbMode) 
            {
                int paletteIndex = (value & 0x3f) >> 1;
                int paletteHiLo   = (value & 0x01);
                gbMemory[0xff68] = value;
                gbMemory[0xff69] = (paletteHiLo ? (gbPalette[paletteIndex] >> 8) : (gbPalette[paletteIndex] & 0x00ff));
                return OK;
            }
        }
        break;
        // BCPD
        case 0x69: 
        {
            if(gbCgbMode) 
            {
                int v = gbMemory[0xff68];      
                int paletteIndex = (v & 0x3f) >> 1;
                int paletteHiLo  = (v & 0x01);
                gbMemory[0xff69] = value;
                gbPalette[paletteIndex] = (paletteHiLo ? ((value << 8) | (gbPalette[paletteIndex] & 0xff)) : ((gbPalette[paletteIndex] & 0xff00) | (value))) & 0x7fff;
                if(gbMemory[0xff68] & 0x80) 
                {
                    int index = ((gbMemory[0xff68] & 0x3f) + 1) & 0x3f;
                    gbMemory[0xff68] = (gbMemory[0xff68] & 0x80) | index;
                    gbMemory[0xff69] = (index & 1 ? (gbPalette[index>>1] >> 8) : (gbPalette[index>>1] & 0x00ff));
                }
                return OK;
            }
        }
        break;
        // OCPS 
        case 0x6a: 
        {
            if(gbCgbMode) 
            {
                int paletteIndex = (value & 0x3f) >> 1;
                int paletteHiLo   = (value & 0x01);
                paletteIndex += 32;
                gbMemory[0xff6a] = value;
                gbMemory[0xff6b] = (paletteHiLo ? (gbPalette[paletteIndex] >> 8) : (gbPalette[paletteIndex] & 0x00ff));
                return OK;
            }
        }
        break;
        // OCPD
        case 0x6b: 
        {
            if(gbCgbMode) 
            {
                int v = gbMemory[0xff6a];      
                int paletteIndex = (v & 0x3f) >> 1;
                int paletteHiLo  = (v & 0x01);
                paletteIndex += 32;
                gbMemory[0xff6b] = value;
                gbPalette[paletteIndex] = (paletteHiLo ? ((value << 8) | (gbPalette[paletteIndex] & 0xff)) : ((gbPalette[paletteIndex] & 0xff00) | (value))) & 0x7fff;
                if(gbMemory[0xff6a] & 0x80) 
                {
                    int index = ((gbMemory[0xff6a] & 0x3f) + 1) & 0x3f;
                    gbMemory[0xff6a] = (gbMemory[0xff6a] & 0x80) | index;
                    gbMemory[0xff6b] = (index & 1 ? (gbPalette[(index>>1) + 32] >> 8) : (gbPalette[(index>>1) + 32] & 0x00ff));
                }
                return OK;
            }
        }
        break;
        // SVBK
        case 0x70: 
        {
            if(gbCgbMode) 
            {
                value = value & 7;
                int bank = value;
                if(value == 0)
                    bank = 1;
                if(bank == gbWramBank)
                    return OK;
                int wramAddress = bank * 0x1000;
                gbMemoryMap[0x0d] = &gbWram[wramAddress];
                gbWramBank = bank;
                register_SVBK = value;
                return OK;
            }
        }
        break;
        case 0xff: 
        {
            register_IE = value;
            register_IF &= value;
            return OK;
        }
    }
    gbMemory[address] = value;
    return OK;
}
inline void CGbEmu::gbSgbDraw24Bit(u8 *p, u16 v)
{
    *((u32*) p) = systemColorMap32[v];
}
inline void CGbEmu::gbSgbDraw32Bit(u32 *p, u16 v)
{
    *p = systemColorMap32[v];
}
inline void CGbEmu::gbSgbDraw16Bit(u16 *p, u16 v)
{
    *p = systemColorMap16[v];
}
void CGbEmu::gbSgbReset()
{
    gbSgbPacketTimeout = 0;
    gbSgbCGBSupport = 0;
    gbSgbMask = 0;
    gbSgbPacketState = GBSGB_NONE;
    gbSgbBit = 0;
    gbSgbPacketNBits = 0;
    gbSgbPacketNumber = 0;
    gbSgbMultiplayer = 0;
    gbSgbFourPlayers = 0;
    gbSgbNextController = 0x0f;
    gbSgbReadingController = 0;
    memset(gbSgbSCPPalette, 0, 512*4);
    memset(gbSgbATF, 0, 20*18);
    memset(gbSgbATFList, 0, 45 * 20 * 18);
    memset(gbSgbPacket, 0, 16 * 7);
    memset(gbSgbBorderChar, 0, 32*256);
    memset(gbSgbBorder, 0, 2048);
    int i;
    for(i = 1; i < 2048; i+=2) 
    {
        gbSgbBorder[i] = 1 << 2;
    }
    for(i = 0; i < 4; i++) 
    {
        gbPalette[i*4] = (0x1f) | (0x1f << 5) | (0x1f << 10);
        gbPalette[i*4+1] = (0x15) | (0x15 << 5) | (0x15 << 10);
        gbPalette[i*4+2] = (0x0c) | (0x0c << 5) | (0x0c << 10);
        gbPalette[i*4+3] = 0;
    }
}
int CGbEmu::gbSgbInit()
{
    MALLOC(gbSgbBorderChar,u8,32*256);
    MALLOC(gbSgbBorder,u8,2048);
    gbSgbReset();
    return OK;
}
int CGbEmu::gbSgbShutdown()
{
    FREE(gbSgbBorderChar);
    FREE(gbSgbBorder);
    return OK;
}
void CGbEmu::gbSgbFillScreen(u16 color)
{
    switch(systemColorDepth) 
    {
        case 16:
        {
            for(int y = 0; y < 144; y++) 
            {
                int yLine = (y+gbBorderRowSkip+1)*(gbBorderLineSkip+2) +
                gbBorderColumnSkip;
                u16 *dest = (u16*)pix + yLine;
                for(register int x = 0; x < 160; x++)
                gbSgbDraw16Bit(dest++, color);
            }
        }
        break;
        case 24:
        {
            for(int y = 0; y < 144; y++) 
            {
                int yLine = (y+gbBorderRowSkip)*gbBorderLineSkip + gbBorderColumnSkip;
                u8 *dest = (u8 *)pix + yLine*3;
                for(register int x = 0; x < 160; x++) 
                {
                    gbSgbDraw24Bit(dest, color);
                    dest += 3;
                }
            }
        }
        break;
        case 32:
        {
            for(int y = 0; y < 144; y++) 
            {
                int yLine = (y+gbBorderRowSkip+1)*(gbBorderLineSkip+1) + gbBorderColumnSkip;
                u32 *dest = (u32 *)pix + yLine;
                for(register int x = 0; x < 160; x++) 
                {
                    gbSgbDraw32Bit(dest++, color);
                }
            }
        }
        break;
    }
}
#define getmem(x) gbMemoryMap[(x) >> 12][(x) & 0xfff]
void CGbEmu::gbSgbRenderScreenToBuffer()
{
    u16 mapAddress = 0x9800;
    if(register_LCDC & 0x08)
        mapAddress = 0x9c00;
    u16 patternAddress = 0x8800;
    int flag = 1;
    if(register_LCDC & 0x10) 
    {
        patternAddress = 0x8000;
        flag = 0;
    }
    u8 *toAddress = gbSgbScreenBuffer;
    for(int i = 0; i < 13; i++) 
    {
        for(int j = 0; j < 20; j++) 
        {
            int tile = getmem(mapAddress);
            mapAddress++;
            if(flag) 
            {
                if(tile > 127)
                    tile -= 128;
                else
                    tile += 128;
            }
            for(int k = 0; k < 16; k++)
            *toAddress++ = getmem(patternAddress + tile*16 + k);
        }
        mapAddress += 12;
    }
}
void CGbEmu::gbSgbDrawBorderTile(int x, int y, int tile, int attr)
{
    u16 *dest = (u16*)pix + ((y+1) * (256+2)) + x;
    u8 *dest8 = (u8*)pix + ((y*256)+x)*3;
    u32 *dest32 = (u32*)pix + ((y+1)*257) + x;
    u8 *tileAddress = &gbSgbBorderChar[tile * 32];
    u8 *tileAddress2 = &gbSgbBorderChar[tile * 32 + 16];
    u8 l = 8;
    u8 palette = ((attr >> 2 ) & 7);
    if(palette < 4)
        palette += 4;
    palette *= 16;
    u8 xx = 0;
    u8 yy = 0;
    int flipX = attr & 0x40;
    int flipY = attr & 0x80;
    while(l > 0) 
    {
        u8 mask = 0x80;
        u8 a = *tileAddress++;
        u8 b = *tileAddress++;
        u8 c = *tileAddress2++;
        u8 d = *tileAddress2++;
        while(mask > 0) 
        {
            u8 color = 0;
            if(a & mask)
                color++;
            if(b & mask)
                color+=2;
            if(c & mask)
                color+=4;
            if(d & mask)
                color+=8;
            u8 xxx = xx;
            u8 yyy = yy;
            if(flipX)
                xxx = 7 - xx;
            if(flipY)
                yyy = 7 - yy;
            u16 c = gbPalette[palette + color];
            if(!color)
                c = gbPalette[0];
            if((yy < 40 || yy >= 184) || (xx < 48 || xx >= 208)) 
            {
                switch(systemColorDepth) 
                {
                    case 16:
                    gbSgbDraw16Bit(dest + yyy*(256+2) + xxx, c);
                    break;
                    case 24:
                    gbSgbDraw24Bit(dest8 + (yyy*256+xxx)*3, c);
                    break;
                    case 32:
                    gbSgbDraw32Bit(dest32 + yyy*(256+1)+xxx, c);
                    break;
                }
            }
            mask >>= 1;
            xx++;
        }
        yy++;
        xx = 0;
        l--;
        mask = 0x80;
    }
}
void CGbEmu::gbSgbRenderBorder()
{
    if(gbBorderOn) 
    {
        u8 *fromAddress = gbSgbBorder;
        for(u8 y = 0; y < 28; y++) 
        {
            for(u8 x = 0; x< 32; x++) 
            {
                u8 tile = *fromAddress++;
                u8 attr = *fromAddress++;
                gbSgbDrawBorderTile(x*8,y*8,tile,attr);
            }
        }
    }
}
void CGbEmu::gbSgbPicture()
{
    gbSgbRenderScreenToBuffer();
    memcpy(gbSgbBorder, gbSgbScreenBuffer, 2048);
    u16 *paletteAddr = (u16 *)&gbSgbScreenBuffer[2048];
    for(int i = 64; i < 128; i++) 
    {
        gbPalette[i] = READ16LE(paletteAddr++);
    }
    gbSgbCGBSupport |= 4;
    if(gbBorderAutomatic && !gbBorderOn && gbSgbCGBSupport > 4) 
    {
        gbBorderOn = 1;
        systemGbBorderOn();
    }
    if(gbBorderOn && !gbSgbMask) 
        gbSgbRenderBorder();
    if(gbSgbMode && gbCgbMode && gbSgbCGBSupport > 4) 
    {
        gbSgbCGBSupport = 0;
        gbSgbMode = 0;
        gbSgbMask = 0;
        gbSgbRenderBorder();
        gbReset();
    }
    if(gbSgbCGBSupport > 4)
        gbSgbCGBSupport = 0;
}
void CGbEmu::gbSgbSetPalette(int a,int b,u16 *p)
{
    u16 bit00 = READ16LE(p++);
    int i;
    for(i = 1; i < 4; i++) 
    {
        gbPalette[a*4+i] = READ16LE(p++);
    }
    for(i = 1; i < 4; i++) 
    {
        gbPalette[b*4+i] = READ16LE(p++);
    }
    gbPalette[0] = gbPalette[4] = gbPalette[8] = gbPalette[12] = bit00;
    if(gbBorderOn && !gbSgbMask)
        gbSgbRenderBorder();
}
void CGbEmu::gbSgbScpPalette()
{
    gbSgbRenderScreenToBuffer();
    u16 *fromAddress = (u16 *)gbSgbScreenBuffer;
    for(int i = 0; i < 512*4; i++) 
    {
        gbSgbSCPPalette[i] = READ16LE(fromAddress++);
    }
}
void CGbEmu::gbSgbSetATF(int n)
{
    if(n < 0)
        n = 0;
    if(n > 44)
        n = 44;
    memcpy(gbSgbATF,&gbSgbATFList[n * 20 * 18], 20 * 18);
    if(gbSgbPacket[1] & 0x40) 
    {
        gbSgbMask = 0;
        if(gbBorderOn)
            gbSgbRenderBorder();
    }
}
void CGbEmu::gbSgbSetPalette()
{
    u16 pal = READ16LE((((u16 *)&gbSgbPacket[1])))&511;
    memcpy(&gbPalette[0], &gbSgbSCPPalette[pal*4], 4 * sizeof(u16));
    pal = READ16LE((((u16 *)&gbSgbPacket[3])))&511;
    memcpy(&gbPalette[4], &gbSgbSCPPalette[pal*4], 4 * sizeof(u16));
    pal = READ16LE((((u16 *)&gbSgbPacket[5])))&511;
    memcpy(&gbPalette[8], &gbSgbSCPPalette[pal*4], 4 * sizeof(u16));
    pal = READ16LE((((u16 *)&gbSgbPacket[7])))&511;
    memcpy(&gbPalette[12], &gbSgbSCPPalette[pal*4], 4 * sizeof(u16));
    u8 atf = gbSgbPacket[9];
    if(atf & 0x80) 
    {
        gbSgbSetATF(atf & 0x3f);
    }
    if(atf & 0x40) 
    {
        gbSgbMask = 0;
        if(gbBorderOn)
            gbSgbRenderBorder();
    }
}
void CGbEmu::gbSgbAttributeBlock()
{
    u8 *fromAddress = &gbSgbPacket[1];
    u8 nDataSet = *fromAddress++;
    if(nDataSet > 12)
        nDataSet = 12;
    if(nDataSet == 0)
        nDataSet = 1;
    while(nDataSet) 
    {
        u8 controlCode = (*fromAddress++) & 7;    
        u8 paletteDesignation = (*fromAddress++) & 0x3f;
        u8 startH = (*fromAddress++) & 0x1f;
        u8 startV = (*fromAddress++) & 0x1f;
        u8 endH   = (*fromAddress++) & 0x1f;
        u8 endV   = (*fromAddress++) & 0x1f;
        u8 * toAddress = gbSgbATF;
        for(u8 y = 0; y < 18; y++) 
        {
            for(u8 x = 0; x < 20; x++) 
            {
                if(x < startH || y < startV ||
                    x > endH || y > endV) 
                {
                    // outside
                    if(controlCode & 0x04)
                    *toAddress = (paletteDesignation >> 4) & 0x03;
                }
                else if(x > startH && x < endH &&
                    y > startV && y < endV) 
                {
                    // inside
                    if(controlCode & 0x01)
                    *toAddress = paletteDesignation & 0x03;
                }
                else 
                {
                    // surrounding line
                    if(controlCode & 0x02)
                    *toAddress = (paletteDesignation>>2) & 0x03;
                    else if(controlCode == 0x01)
                    *toAddress = paletteDesignation & 0x03;         
                }
                toAddress++;
            }
        }
        nDataSet--;
    }
}
void CGbEmu::gbSgbSetColumnPalette(u8 col, u8 p)
{
    //  if(col < 0)
    //    col = 0;
    if(col > 19)
        col = 19;
    p &= 3;
    u8 *toAddress = &gbSgbATF[col];
    for(u8 y = 0; y < 18; y++) 
    {
        *toAddress = p;
        toAddress += 20;
    }
}
void CGbEmu::gbSgbSetRowPalette(u8 row, u8 p)
{
    //  if(row < 0)
    //    row = 0;
    if(row > 17)
        row = 17;
    p &= 3;
    u8 *toAddress = &gbSgbATF[row*20];
    for(u8 x = 0; x < 20; x++) 
    {
        *toAddress++ = p;
    }
}
void CGbEmu::gbSgbAttributeDivide()
{
    u8 control = gbSgbPacket[1];
    u8 coord   = gbSgbPacket[2];
    u8 colorBR = control & 3;
    u8 colorAL = (control >> 2) & 3;
    u8 colorOL = (control >> 4) & 3;
    if(control & 0x40) 
    {
        if(coord > 17)
            coord = 17;
        for(u8 i = 0; i < 18; i++) 
        {
            if(i < coord)
                gbSgbSetRowPalette(i, colorAL);
            else if ( i > coord)
                gbSgbSetRowPalette(i, colorBR);
            else
                gbSgbSetRowPalette(i, colorOL);
        }
    }
    else 
    {
        if(coord > 19)
            coord = 19;
        for(u8 i = 0; i < 20; i++) 
        {
            if(i < coord)
                gbSgbSetColumnPalette(i, colorAL);
            else if ( i > coord)
                gbSgbSetColumnPalette(i, colorBR);
            else
                gbSgbSetColumnPalette(i, colorOL);      
        }
    }
}
void CGbEmu::gbSgbAttributeLine()
{
    u8 *fromAddress = &gbSgbPacket[1];
    u8 nDataSet = *fromAddress++;
    if(nDataSet > 0x6e)
        nDataSet = 0x6e;
    while(nDataSet) 
    {
        u8 line = *fromAddress++;
        u8 num = line & 0x1f;
        u8 pal = (line >> 5) & 0x03;
        if(line & 0x80) 
        {
            if(num > 17)
                num = 17;
            gbSgbSetRowPalette(num,pal);
        }
        else 
        {
            if(num > 19)
                num = 19;
            gbSgbSetColumnPalette(num,pal);
        }
        nDataSet--;
    }
}
void CGbEmu::gbSgbAttributeCharacter()
{
    u8 startH = gbSgbPacket[1] & 0x1f;
    u8 startV = gbSgbPacket[2] & 0x1f;
    int nDataSet = READ16LE(((u16 *)&gbSgbPacket[3]));
    int style = gbSgbPacket[5] & 1;
    if(startH > 19)
        startH = 19;
    if(startV > 17)
        startV = 17;
    u8 s = 6;
    u8 *fromAddress = &gbSgbPacket[6];
    u8 v = *fromAddress++;
    if(style) 
    {
        while(nDataSet) 
        {
            u8 p = (v >> s) & 3;
            gbSgbATF[startV * 20 + startH] = p;
            startV++;
            if(startV == 18) 
            {
                startV = 0;
                startH++;
                if(startH == 20)
                    break;
            }
            if(s)
                s -= 2;
            else 
            {
                s = 6;
                v = *fromAddress++;
                nDataSet--;
            }
        }
    }
    else 
    {
        while(nDataSet) 
        {
            u8 p = (v >> s) & 3;
            gbSgbATF[startV * 20 + startH] = p;
            startH++;
            if(startH == 20) 
            {
                startH = 0;
                startV++;
                if(startV == 18)
                    break;
            }
            if(s)
                s -= 2;
            else 
            {
                s = 6;
                v = *fromAddress++;
                nDataSet--;
            }
        }
    }
}
void CGbEmu::gbSgbSetATFList()
{
    gbSgbRenderScreenToBuffer();
    u8 *fromAddress = gbSgbScreenBuffer;
    u8 *toAddress   = gbSgbATFList;
    for(int i = 0; i < 45; i++) 
    {
        for(int j = 0; j < 90; j++) 
        {
            u8 v = *fromAddress++;
            u8 s = 6;
            if(i == 2)
                s = 6;
            for(int k = 0; k < 4; k++) 
            {
                *toAddress++ = (v >> s) & 0x03;
                s -= 2;
            }
        }
    }
}
void CGbEmu::gbSgbMaskEnable()
{
    int gbSgbMaskFlag = gbSgbPacket[1] & 3;
    gbSgbMask = gbSgbMaskFlag;
    switch(gbSgbMaskFlag) 
    {
        case 1:
        break;
        case 2:
        gbSgbFillScreen(0x0000);
        //    memset(&gbPalette[0], 0, 128*sizeof(u16));
        break;
        case 3:
        gbSgbFillScreen(gbPalette[0]);
        break;
    }
    if(!gbSgbMask) 
    {
        if(gbBorderOn)
            gbSgbRenderBorder();
    }
}
void CGbEmu::gbSgbChrTransfer()
{
    gbSgbRenderScreenToBuffer();
    int address = (gbSgbPacket[1] & 1) * (128*32);
    if(gbSgbPacket[1] & 1)
        gbSgbCGBSupport |= 2;
    else
        gbSgbCGBSupport |= 1;
    memcpy(&gbSgbBorderChar[address], gbSgbScreenBuffer, 128 * 32);
    if(gbBorderAutomatic && !gbBorderOn && gbSgbCGBSupport > 4) 
    {
        gbBorderOn = 1;
        systemGbBorderOn();
    }
    if(gbBorderOn && !gbSgbMask)
        gbSgbRenderBorder();
    if(gbSgbMode && gbCgbMode && gbSgbCGBSupport == 7) 
    {
        gbSgbCGBSupport = 0;
        gbSgbMode = 0;
        gbSgbMask = 0;
        gbSgbRenderBorder();
        gbReset();
    }
    if(gbSgbCGBSupport > 4)
        gbSgbCGBSupport = 0;
}
void CGbEmu::gbSgbMultiRequest()
{
    if(gbSgbPacket[1] & 1) 
    {
        gbSgbMultiplayer    = 1;
        if(gbSgbPacket[1] & 2)
            gbSgbFourPlayers = 1;
        else
            gbSgbFourPlayers = 0;
        gbSgbNextController = 0x0e;
    }
    else 
    {
        gbSgbFourPlayers    = 0;
        gbSgbMultiplayer    = 0;
        gbSgbNextController = 0x0f;
    }
}
void CGbEmu::gbSgbCommand()
{
    int command = gbSgbPacket[0] >> 3;
    //  int nPacket = gbSgbPacket[0] & 7;
    switch(command) 
    {
        case 0x00:
        gbSgbSetPalette(0,1,(u16 *)&gbSgbPacket[1]);
        break;
        case 0x01:
        gbSgbSetPalette(2,3,(u16 *)&gbSgbPacket[1]);
        break;
        case 0x02:
        gbSgbSetPalette(0,3,(u16 *)&gbSgbPacket[1]);
        break;
        case 0x03:
        gbSgbSetPalette(1,2,(u16 *)&gbSgbPacket[1]);
        break;
        case 0x04:
        gbSgbAttributeBlock();
        break;
        case 0x05:
        gbSgbAttributeLine();
        break;
        case 0x06:
        gbSgbAttributeDivide();
        break;
        case 0x07:
        gbSgbAttributeCharacter();
        break;
        case 0x0a:
        gbSgbSetPalette();
        break;
        case 0x0b:
        gbSgbScpPalette();
        break;
        case 0x11:
        gbSgbMultiRequest();
        break;
        case 0x13:
        gbSgbChrTransfer();
        break;
        case 0x14:
        gbSgbPicture();
        break;
        case 0x15:
        gbSgbSetATFList();
        break;
        case 0x16:
        gbSgbSetATF(gbSgbPacket[1] & 0x3f);
        break;
        case 0x17:
        gbSgbMaskEnable();
        break;
    }
}
void CGbEmu::gbSgbResetPacketState()
{
    gbSgbPacketState = GBSGB_NONE;
    gbSgbPacketTimeout = 0;
}
void CGbEmu::gbSgbDoBitTransfer(u8 value)
{
    value = value & 0x30;
    switch(gbSgbPacketState) 
    {
        case GBSGB_NONE:
        if(value == 0) 
        {
            gbSgbPacketState = GBSGB_RESET;
            gbSgbPacketTimeout = GBSGB_PACKET_TIMEOUT;
        }
        else if (value == 0x30) 
        {
            if(gbSgbMultiplayer) 
            {
                if((gbSgbReadingController & 7) == 7) 
                {
                    gbSgbReadingController = 0;
                    if(gbSgbMultiplayer) 
                    {
                        gbSgbNextController--;
                        if(gbSgbFourPlayers) 
                        {
                            if(gbSgbNextController == 0x0b)
                                gbSgbNextController = 0x0f;
                        }
                        else 
                        {
                            if(gbSgbNextController == 0x0d)
                                gbSgbNextController = 0x0f;
                        }
                    }
                }
                else 
                {
                    gbSgbReadingController &= 3;
                }
            }
            gbSgbPacketTimeout = 0;      
        }
        else 
        {
            if(value == 0x10)
                gbSgbReadingController |= 0x2;
            else if(value == 0x20)
                gbSgbReadingController |= 0x01;
            gbSgbPacketTimeout = 0;      
        }
        gbSgbPacketTimeout = 0;
        break;
        case GBSGB_RESET:
        if(value == 0x30) 
        {
            gbSgbPacketState = GBSGB_PACKET_TRANSMIT;
            gbSgbPacketByte  = 0;
            gbSgbPacketNBits = 0;
            gbSgbPacketTimeout = GBSGB_PACKET_TIMEOUT;
        }
        else if(value == 0x00) 
        {
            gbSgbPacketTimeout = GBSGB_PACKET_TIMEOUT;
            gbSgbPacketState = GBSGB_RESET;
        }
        else 
        {
            gbSgbPacketState = GBSGB_NONE;
            gbSgbPacketTimeout = 0;
        }
        break;
        case GBSGB_PACKET_TRANSMIT:
        if(value == 0) 
        {
            gbSgbPacketState = GBSGB_RESET;
            gbSgbPacketTimeout = 0;
        }
        else if (value == 0x30)
        {
            if(gbSgbPacketNBits == 128) 
            {
                gbSgbPacketNBits = 0;
                gbSgbPacketByte  = 0;
                gbSgbPacketNumber++;
                gbSgbPacketTimeout = 0;
                if(gbSgbPacketNumber == (gbSgbPacket[0] & 7)) 
                {
                    gbSgbCommand();
                    gbSgbPacketNumber = 0;
                    gbSgbPacketState = GBSGB_NONE;
                    gbSgbPacketTimeout = 0;
                }
            }
            else 
            {
                if(gbSgbPacketNBits < 128) 
                {
                    gbSgbPacket[gbSgbPacketNumber * 16 + gbSgbPacketByte] >>= 1;
                    gbSgbPacket[gbSgbPacketNumber * 16 + gbSgbPacketByte] |= gbSgbBit;
                    gbSgbPacketNBits++;
                    if(!(gbSgbPacketNBits & 7)) 
                    {
                        gbSgbPacketByte++;
                    }
                    gbSgbPacketTimeout = GBSGB_PACKET_TIMEOUT;
                }
            }
        }
        else 
        {
            if(value == 0x20)
                gbSgbBit = 0x00;
            else
                gbSgbBit = 0x80;
            gbSgbPacketTimeout = GBSGB_PACKET_TIMEOUT;
        }
        gbSgbReadingController = 0;
        break;
        default:
        gbSgbPacketState = GBSGB_NONE;
        gbSgbPacketTimeout = 0;
        break;
    }
}
void CGbEmu::systemGbBorderOn()
{
    gbBorderLineSkip = 256;
    gbBorderColumnSkip = 48;
    gbBorderRowSkip = 40;
    this->gbBorderOn = 1;
}
void CGbEmu::gbReset()
{
    SP.W = 0xfffe;
    AF.W = 0x01b0;
    BC.W = 0x0013;
    DE.W = 0x00d8;
    HL.W = 0x014d;
    PC.W = 0x0100;
    IFF = 0;
    gbInterrupt = 1;
    gbInterruptWait = 0;
    register_DIV = 0;
    register_TIMA = 0;
    register_TMA = 0;
    register_TAC = 0;
    register_IF = 1;
    register_LCDC = 0x91;
    register_STAT = 0;
    register_SCY = 0;
    register_SCX = 0;  
    register_LY = 0;  
    register_LYC = 0;
    register_DMA = 0;
    register_WY = 0;
    register_WX = 0;
    register_VBK = 0;
    register_HDMA1 = 0;
    register_HDMA2 = 0;
    register_HDMA3 = 0;
    register_HDMA4 = 0;
    register_HDMA5 = 0;
    register_SVBK = 0;
    register_IE = 0;  
    if(gbCgbMode) 
    {
        if(gbSgbMode) 
        {
            if(gbEmulatorType == 5)
                AF.W = 0xffb0;
            else
                AF.W = 0x01b0;
            BC.W = 0x0013;
            DE.W = 0x00d8;
            HL.W = 0x014d;
            for(int i = 0; i < 8; i++)
            gbPalette[i] = systemGbPalette[gbPaletteOption*8+i];
        }
        else 
        {
            AF.W = 0x11b0;
            BC.W = 0x0000;
            DE.W = 0xff56;
            HL.W = 0x000d;
        }
        if(gbEmulatorType == 4)
            BC.B.B1 |= 0x01;
        register_HDMA5 = 0xff;
        gbMemory[0xff68] = 0xc0;
        gbMemory[0xff6a] = 0xc0;    
    }
    else 
    {
        for(int i = 0; i < 8; i++)
        gbPalette[i] = systemGbPalette[gbPaletteOption*8+i];
    }
    if(gbSpeed) 
    {
        gbSpeedSwitch();
        gbMemory[0xff4d] = 0;
    }
    gbDivTicks = GBDIV_CLOCK_TICKS;
    gbLcdMode = 2;
    gbLcdTicks = GBLCD_MODE_2_CLOCK_TICKS;
    gbLcdLYIncrementTicks = 0;
    gbTimerTicks = 0;
    gbTimerClockTicks = 0;
    gbSerialTicks = 0;
    gbSerialBits = 0;
    gbSerialOn = 0;
    gbWindowLine = -1;
    gbTimerOn = 0;
    gbTimerMode = 0;
    //  gbSynchronizeTicks = GBSYNCHRONIZE_CLOCK_TICKS;
    gbSpeed = 0;
    gbJoymask[0] = gbJoymask[1] = gbJoymask[2] = gbJoymask[3] = 0;
    if(gbCgbMode) 
    {
        gbSpeed = 0;
        gbHdmaOn = 0;
        gbHdmaSource = 0x0000;
        gbHdmaDestination = 0x8000;
        gbVramBank = 0;
        gbWramBank = 1;
        register_LY = 0x90;
        gbLcdMode = 1;
        for(int i = 0; i < 64; i++)
        gbPalette[i] = 0x7fff;
    }
    if(gbSgbMode) 
    {
        gbSgbReset();
    }
    for(int i =0; i < 4; i++)
    gbBgp[i] = gbObp0[i] = gbObp1[i] = i;
    memset(&gbDataMBC1,0, sizeof(gbDataMBC1));
    gbDataMBC1.mapperROMBank = 1;
    gbDataMBC2.mapperRAMEnable = 0;
    gbDataMBC2.mapperROMBank = 1;
    memset(&gbDataMBC3,0, 6 * sizeof(int));
    gbDataMBC3.mapperROMBank = 1;
    memset(&gbDataMBC5, 0, sizeof(gbDataMBC5));
    gbDataMBC5.mapperROMBank = 1;
    memset(&gbDataHuC1, 0, sizeof(gbDataHuC1));
    gbDataHuC1.mapperROMBank = 1;
    memset(&gbDataHuC3, 0, sizeof(gbDataHuC3));
    gbDataHuC3.mapperROMBank = 1;
    gbMemoryMap[0x00] = &gbRom[0x0000];
    gbMemoryMap[0x01] = &gbRom[0x1000];
    gbMemoryMap[0x02] = &gbRom[0x2000];
    gbMemoryMap[0x03] = &gbRom[0x3000];
    gbMemoryMap[0x04] = &gbRom[0x4000];
    gbMemoryMap[0x05] = &gbRom[0x5000];
    gbMemoryMap[0x06] = &gbRom[0x6000];
    gbMemoryMap[0x07] = &gbRom[0x7000];
    if(gbCgbMode) 
    {
        gbMemoryMap[0x08] = &gbVram[0x0000];
        gbMemoryMap[0x09] = &gbVram[0x1000];
        gbMemoryMap[0x0a] = &gbMemory[0xa000];
        gbMemoryMap[0x0b] = &gbMemory[0xb000];
        gbMemoryMap[0x0c] = &gbMemory[0xc000];
        gbMemoryMap[0x0d] = &gbWram[0x1000];
        gbMemoryMap[0x0e] = &gbMemory[0xe000];
        gbMemoryMap[0x0f] = &gbMemory[0xf000];        
    }
    else 
    {
        gbMemoryMap[0x08] = &gbMemory[0x8000];
        gbMemoryMap[0x09] = &gbMemory[0x9000];
        gbMemoryMap[0x0a] = &gbMemory[0xa000];
        gbMemoryMap[0x0b] = &gbMemory[0xb000];
        gbMemoryMap[0x0c] = &gbMemory[0xc000];
        gbMemoryMap[0x0d] = &gbMemory[0xd000];
        gbMemoryMap[0x0e] = &gbMemory[0xe000];
        gbMemoryMap[0x0f] = &gbMemory[0xf000];    
    }
    if(gbRam) 
    {
        gbMemoryMap[0x0a] = &gbRam[0x0000];
        gbMemoryMap[0x0b] = &gbRam[0x1000];
    }
    gbSoundReset();
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    gbLastTime = systemGetClock();
    gbFrameCount = 0;
}
void CGbEmu::gbSpeedSwitch()
{
    if(gbSpeed == 0) 
    {
        gbSpeed = 1;
        GBLCD_MODE_0_CLOCK_TICKS = 51 * 2; //127; //51 * 2;
        GBLCD_MODE_1_CLOCK_TICKS = 1140 * 2;
        GBLCD_MODE_2_CLOCK_TICKS = 20 * 2; //52; //20 * 2;
        GBLCD_MODE_3_CLOCK_TICKS = 43 * 2; //99; //43 * 2;
        GBDIV_CLOCK_TICKS = 64 * 2;
        GBLY_INCREMENT_CLOCK_TICKS = 114 * 2;
        GBTIMER_MODE_0_CLOCK_TICKS = 256; //256*2;
        GBTIMER_MODE_1_CLOCK_TICKS = 4; //4*2;
        GBTIMER_MODE_2_CLOCK_TICKS = 16; //16*2;
        GBTIMER_MODE_3_CLOCK_TICKS = 64; //64*2;
        GBSERIAL_CLOCK_TICKS = 128 * 2;
        gbDivTicks *= 2;
        gbLcdTicks *= 2;
        gbLcdLYIncrementTicks *= 2;
        //    timerTicks *= 2;
        //    timerClockTicks *= 2;
        gbSerialTicks *= 2;
        SOUND_CLOCK_TICKS = soundQuality * 24 * 2;
        soundTicks *= 2;
        //    synchronizeTicks *= 2;
        //    SYNCHRONIZE_CLOCK_TICKS *= 2;
    }
    else 
    {
        gbSpeed = 0;
        GBLCD_MODE_0_CLOCK_TICKS = 51;
        GBLCD_MODE_1_CLOCK_TICKS = 1140;
        GBLCD_MODE_2_CLOCK_TICKS = 20;
        GBLCD_MODE_3_CLOCK_TICKS = 43;
        GBDIV_CLOCK_TICKS = 64;
        GBLY_INCREMENT_CLOCK_TICKS = 114;
        GBTIMER_MODE_0_CLOCK_TICKS = 256;
        GBTIMER_MODE_1_CLOCK_TICKS = 4;
        GBTIMER_MODE_2_CLOCK_TICKS = 16;
        GBTIMER_MODE_3_CLOCK_TICKS = 64;
        GBSERIAL_CLOCK_TICKS = 128;
        gbDivTicks /= 2;
        gbLcdTicks /= 2;
        gbLcdLYIncrementTicks /= 2;
        //    timerTicks /= 2;
        //    timerClockTicks /= 2;
        gbSerialTicks /= 2;
        SOUND_CLOCK_TICKS = soundQuality * 24;
        soundTicks /= 2;
        //    synchronizeTicks /= 2;
        //    SYNCHRONIZE_CLOCK_TICKS /= 2;    
    }
}
void CGbEmu::gbSoundReset()
{
    //don't care sound
}
u32 CGbEmu::systemGetClock()
{
    return ::timeGetTime();
}
void CGbEmu::gbSoundEvent(u16 address, int data)
{
}
u16 CGbEmu::READ16LE(u16 *addr)
{
    u8 *p = (u8*)addr;
    return p[0] | (p[1]<<8);
}
void CGbEmu::WRITE16LE(u16 *addr, u16 value)
{
    u8 *p = (u8*)addr;
    
    p[0] = (u8)value;
    p[1] = (u8)(value>>8);
}
u8 CGbEmu::gbReadOpcode(u16 address)
{
    switch(address & 0xf000) 
    {
        case 0x0a:
        case 0x0b:
        if(mapperReadRAM)
            return (this->*mapperReadRAM)(address);
        break;
        case 0x0f:
        if(address > 0xff00) 
        {
            switch(address & 0x00ff) 
            {
                case 0x04:
                return register_DIV;
                case 0x05:
                return register_TIMA;
                case 0x06:
                return register_TMA;
                case 0x07:
                return (0xf8 | register_TAC);
                case 0x0f:
                return (0xe0 | register_IF);
                case 0x40:
                return register_LCDC;
                case 0x41:
                return (0x80 | register_STAT);
                case 0x42:
                return register_SCY;
                case 0x43:
                return register_SCX;
                case 0x44:
                return register_LY;
                case 0x45:
                return register_LYC;
                case 0x46:
                return register_DMA;
                case 0x4a:
                return register_WY;
                case 0x4b:
                return register_WX;
                case 0x4f:
                return (0xfe | register_VBK);
                case 0x51:
                return register_HDMA1;
                case 0x52:
                return register_HDMA2;
                case 0x53:
                return register_HDMA3;
                case 0x54:
                return register_HDMA4;
                case 0x55:
                return register_HDMA5;
                case 0x70:
                return (0xf8 | register_SVBK);
                case 0xff:
                return register_IE;
            }
        }
        break;
    }
    return gbMemoryMap[address>>12][address & 0x0fff];
}
u8 CGbEmu::gbReadMemory(u16 address)
{
    if(address < 0xa000)
        return gbMemoryMap[address>>12][address&0x0fff];
    if(address < 0xc000) 
    {
        if(mapperReadRAM)
            return (this->*mapperReadRAM)(address);
        return gbMemoryMap[address>>12][address & 0x0fff];
    }
    if(address >= 0xff00) 
    {
        switch(address & 0x00ff) 
        {
            case 0x00:
            {
                if(gbSgbMode) 
                {
                    gbSgbReadingController |= 4;
                    gbSgbResetPacketState();
                }
                int b = gbMemory[0xff00];
                if((b & 0x30) == 0x20) 
                {
                    b &= 0xf0;
                    int joy = 0;
                    if(gbSgbMode && gbSgbMultiplayer) 
                    {
                        switch(gbSgbNextController) 
                        {
                            case 0x0f:
                            joy = 0;
                            break;
                            case 0x0e:
                            joy = 1;
                            break;
                            case 0x0d:
                            joy = 2;
                            break;
                            case 0x0c:
                            joy = 3;
                            break;
                            default:
                            joy = 0;
                        }
                    }
                    int joystate = gbJoymask[joy];
                    if(!(joystate & 128))
                        b |= 0x08;
                    if(!(joystate & 64))
                        b |= 0x04;
                    if(!(joystate & 32))
                        b |= 0x02;
                    if(!(joystate & 16))
                        b |= 0x01;
                    gbMemory[0xff00] = b;
                }
                else if((b & 0x30) == 0x10) 
                {
                    b &= 0xf0;
                    int joy = 0;
                    if(gbSgbMode && gbSgbMultiplayer) 
                    {
                        switch(gbSgbNextController) 
                        {
                            case 0x0f:
                            joy = 0;
                            break;
                            case 0x0e:
                            joy = 1;
                            break;
                            case 0x0d:
                            joy = 2;
                            break;
                            case 0x0c:
                            joy = 3;
                            break;
                            default:
                            joy = 0;
                        }
                    }
                    int joystate = gbJoymask[joy];
                    if(!(joystate & 8))
                        b |= 0x08;
                    if(!(joystate & 4))
                        b |= 0x04;
                    if(!(joystate & 2))
                        b |= 0x02;
                    if(!(joystate & 1))
                        b |= 0x01;
                    gbMemory[0xff00] = b;
                }
                else 
                {
                    if(gbSgbMode && gbSgbMultiplayer) 
                    {
                        gbMemory[0xff00] = 0xf0 | gbSgbNextController;
                    }
                    else 
                    {
                        gbMemory[0xff00] = 0xff;
                    }
                }
            }
            return gbMemory[0xff00];
            break;
            case 0x01:
            return gbMemory[0xff01];
            case 0x04:
            return register_DIV;
            case 0x05:
            return register_TIMA;
            case 0x06:
            return register_TMA;
            case 0x07:
            return (0xf8 | register_TAC);
            case 0x0f:
            return (0xe0 | register_IF);
            case 0x40:
            return register_LCDC;
            case 0x41:
            return (0x80 | register_STAT);
            case 0x42:
            return register_SCY;
            case 0x43:
            return register_SCX;
            case 0x44:
            return register_LY;
            case 0x45:
            return register_LYC;
            case 0x46:
            return register_DMA;
            case 0x4a:
            return register_WY;
            case 0x4b:
            return register_WX;
            case 0x4f:
            return (0xfe | register_VBK);
            case 0x51:
            return register_HDMA1;
            case 0x52:
            return register_HDMA2;
            case 0x53:
            return register_HDMA3;
            case 0x54:
            return register_HDMA4;
            case 0x55:
            return register_HDMA5;
            case 0x70:
            return (0xf8 | register_SVBK);
            case 0xff:
            return register_IE;
        }
    }
    return gbMemoryMap[address>>12][address & 0x0fff];
}
void CGbEmu::gbVblank_interrupt()
{
    if(IFF & 0x80) 
    {
        PC.W++;
        IFF &= 0x7f;
    }
    gbInterrupt &= 0xfe;
    IFF &= 0x7e;
    register_IF &= 0xfe;
    gbWriteMemory(--SP.W, PC.B.B1);
    gbWriteMemory(--SP.W, PC.B.B0);
    PC.W = 0x40;
}
void CGbEmu::gbLcd_interrupt()
{
    if(IFF & 0x80) 
    {
        PC.W++;
        IFF &= 0x7f;
    }
    gbInterrupt &= 0xfd;
    IFF &= 0x7e;
    register_IF &= 0xfd;
    gbWriteMemory(--SP.W, PC.B.B1);
    gbWriteMemory(--SP.W, PC.B.B0);
    PC.W = 0x48;
}
void CGbEmu::gbTimer_interrupt()
{
    if(IFF & 0x80) 
    {
        PC.W++;
        IFF &= 0x7f;
    }
    IFF &= 0x7e;
    gbInterrupt &= 0xfb;
    register_IF &= 0xfb;
    gbWriteMemory(--SP.W, PC.B.B1);
    gbWriteMemory(--SP.W, PC.B.B0);
    PC.W = 0x50;
}
void CGbEmu::gbSerial_interrupt()
{
    if(IFF & 0x80) 
    {
        PC.W++;
        IFF &= 0x7f;
    }
    IFF &= 0x7e;
    gbInterrupt &= 0xf7;
    register_IF &= 0xf7;
    gbWriteMemory(--SP.W, PC.B.B1);
    gbWriteMemory(--SP.W, PC.B.B0);
    PC.W = 0x58;
}
void CGbEmu::gbJoypad_interrupt()
{
    if(IFF & 0x80) 
    {
        PC.W++;
        IFF &= 0x7f;
    }
    IFF &= 0x7e;
    gbInterrupt &= 0xef;
    register_IF &= 0xef;
    gbWriteMemory(--SP.W, PC.B.B1);
    gbWriteMemory(--SP.W, PC.B.B0);
    PC.W = 0x60;
}
int CGbEmu::gbLoadRom(CFileBase *rom)
{
    ASSERT(rom);
    ASSERT(this->gbRom == NULL);
    int size = this->utilGetSize(rom->GetSize());
    MALLOC(this->gbRom,u8,size);
    rom->Seek(0);
    rom->Read(this->gbRom,size);
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    gbRomSize = size;
    return gbUpdateSizes();
}
int CGbEmu::utilGetSize(int size)
{
    int res = 1;
    while(res < size)
    res <<= 1;
    return res;
}
bool CGbEmu::gbUpdateSizes()
{
    if(gbRom[0x148] > 8) 
    {
        LOG("Unsupported rom size %02x", gbRom[0x148]);
        return false;
    }
    if(gbRomSize < gbRomSizes[gbRom[0x148]]) 
    {
        REALLOC(gbRom,u8,gbRomSize,gbRom[0x148]);
    }
    gbRomSize = gbRomSizes[gbRom[0x148]];
    gbRomSizeMask = gbRomSizesMasks[gbRom[0x148]];
    if(gbRom[0x149] > 5) 
    {
        LOG("Unsupported ram size %02x", gbRom[0x149]);
        return false;
    }
    gbRamSize = gbRamSizes[gbRom[0x149]];
    gbRamSizeMask = gbRamSizesMasks[gbRom[0x149]];
    if(gbRamSize) 
    {
        MALLOC(gbRam,u8,gbRamSize);
        memset(gbRam, 0xFF, gbRamSize);
    }
    int type = gbRom[0x147];
    mapperReadRAM = NULL;
    switch(type) 
    {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        // MBC 1
        mapper = &CGbEmu::mapperMBC1ROM;
        mapperRAM = &CGbEmu::mapperMBC1RAM;
        break;
        case 0x05:
        case 0x06:
        // MBC2
        mapper = &CGbEmu::mapperMBC2ROM;
        mapperRAM = &CGbEmu::mapperMBC2RAM;
        gbRamSize = 0x200;
        gbRamSizeMask = 0x1ff;
        break;
        case 0x0f:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        // MBC 3
        mapper = &CGbEmu::mapperMBC3ROM;
        mapperRAM = &CGbEmu::mapperMBC3RAM;
        mapperReadRAM = &CGbEmu::mapperMBC3ReadRAM;
        break;
        case 0x19:
        case 0x1a:
        case 0x1b:
        // MBC5
        mapper = &CGbEmu::mapperMBC5ROM;
        mapperRAM = &CGbEmu::mapperMBC5RAM;
        break;
        case 0x1c:
        case 0x1d:
        case 0x1e:
        // MBC 5 Rumble
        mapper = &CGbEmu::mapperMBC5ROM;
        mapperRAM = &CGbEmu::mapperMBC5RAM;
        break;
        case 0x22:
        // MBC 7
        mapper = &CGbEmu::mapperMBC7ROM;
        mapperRAM = &CGbEmu::mapperMBC7RAM;
        mapperReadRAM = &CGbEmu::mapperMBC7ReadRAM;
        break;
        case 0xfe:
        // HuC3
        mapper = &CGbEmu::mapperHuC3ROM;
        mapperRAM = &CGbEmu::mapperHuC3RAM;
        mapperReadRAM = &CGbEmu::mapperHuC3ReadRAM;
        break;
        case 0xff:
        // HuC1
        mapper = &CGbEmu::mapperHuC1ROM;
        mapperRAM = &CGbEmu::mapperHuC1RAM;
        break;
        default:
        LOG("Unknown cartridge type %02x", type);
        return false;
    }
    switch(type) 
    {
        case 0x03:
        case 0x06:
        case 0x0f:
        case 0x10:
        case 0x13:
        case 0x1b:
        case 0x1d:
        case 0x1e:
        case 0x22:
        case 0xff:
        gbBattery = 1;
        break;
    }
    if(gbRom[0x146] == 0x03) 
    {
        if(gbEmulatorType == 0 ||
            gbEmulatorType == 2 ||
            gbEmulatorType == 5)
            gbSgbMode = 1;
        else
            gbSgbMode = 0;
    }
    else
        gbSgbMode = 0;
    if(gbRom[0x143] & 0x80) 
    {
        if(gbEmulatorType == 0 ||
            gbEmulatorType == 1 ||
            gbEmulatorType == 4 ||
            gbEmulatorType == 5) 
        {
            gbCgbMode = 1;
            MALLOC(this->gbVram,u8,0x4000);
            MALLOC(this->gbWram,u8,0x8000);
            memset(gbVram,0,0x4000);
            memset(gbWram,0,0x8000);
            memset(gbPalette,0, 2*128);
        }
        else 
        {
            gbCgbMode = 0;
        }
    }
    else
        gbCgbMode = 0;
    gbInit();
    gbReset();
    switch(type) 
    {
        case 0x1c:
        case 0x1d:
        case 0x1e:
        gbDataMBC5.isRumbleCartridge = 1;
    }
    return true;
}
// MBC1 ROM write registers
void CGbEmu::mapperMBC1ROM(u16 address, u8 value)
{
    int tmpAddress = 0;
    switch(address & 0x6000) 
    {
        case 0x0000: // RAM enable register
        gbDataMBC1.mapperRAMEnable = ( ( value & 0x0a) == 0x0a ? 1 : 0);
        break;
        case 0x2000: // ROM bank select
        //    value = value & 0x1f;
        if(value == 0)
            value = 1;
        if(value == gbDataMBC1.mapperROMBank)
            break;
        tmpAddress = value << 14;
        // check current model
        if(gbDataMBC1.mapperMemoryModel == 0) 
        {
            // model is 16/8, so we have a high address in use
            tmpAddress |= (gbDataMBC1.mapperROMHighAddress) << 19;
        }
        tmpAddress &= gbRomSizeMask;
        gbDataMBC1.mapperROMBank = value;
        gbMemoryMap[0x04] = &gbRom[tmpAddress];
        gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
        gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
        gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        break;
        case 0x4000: // RAM bank select
        if(gbDataMBC1.mapperMemoryModel == 1) 
        {
            // 4/32 model, RAM bank switching provided
            value = value & 0x03;
            if(value == gbDataMBC1.mapperRAMBank)
                break;
            tmpAddress = value << 13;
            tmpAddress &= gbRamSizeMask;
            gbMemoryMap[0x0a] = &gbRam[tmpAddress];
            gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
            gbDataMBC1.mapperRAMBank = value;
            gbDataMBC1.mapperRAMAddress = tmpAddress;
        }
        else 
        {
            // 16/8, set the high address
            gbDataMBC1.mapperROMHighAddress = value & 0x03;
            tmpAddress = gbDataMBC1.mapperROMBank << 14;
            tmpAddress |= (gbDataMBC1.mapperROMHighAddress) << 19;
            tmpAddress &= gbRomSizeMask;
            gbMemoryMap[0x04] = &gbRom[tmpAddress];
            gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
            gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
            gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        }
        break;
        case 0x6000: // memory model select
        gbDataMBC1.mapperMemoryModel = value & 1;
        break;
    }
}
// MBC1 RAM write
void CGbEmu::mapperMBC1RAM(u16 address, u8 value)
{
    if(gbDataMBC1.mapperRAMEnable) 
    {
        if(gbRamSize) 
        {
            gbMemoryMap[address >> 12][address & 0x0fff] = value;
            systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
        }
    }
}
void CGbEmu::memoryUpdateMapMBC1()
{
    int tmpAddress = gbDataMBC1.mapperROMBank << 14;
    // check current model
    if(gbDataMBC1.mapperMemoryModel == 1) 
    {
        // model is 16/8, so we have a high address in use
        tmpAddress |= (gbDataMBC1.mapperROMHighAddress) << 19;
    }
    tmpAddress &= gbRomSizeMask;
    gbMemoryMap[0x04] = &gbRom[tmpAddress];
    gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
    gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
    gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
    if(gbRamSize) 
    {
        gbMemoryMap[0x0a] = &gbRam[gbDataMBC1.mapperRAMAddress];
        gbMemoryMap[0x0b] = &gbRam[gbDataMBC1.mapperRAMAddress + 0x1000];
    }
}
// MBC2 ROM write registers
void CGbEmu::mapperMBC2ROM(u16 address, u8 value)
{
    switch(address & 0x6000) 
    {
        case 0x0000: // RAM enable
        if(!(address & 0x0100)) 
        {
            gbDataMBC2.mapperRAMEnable = (value & 0x0f) == 0x0a;
        }
        break;
        case 0x2000: // ROM bank select
        if(address & 0x0100) 
        {
            value &= 0x0f;
            if(value == 0)
                value = 1;
            if(gbDataMBC2.mapperROMBank != value) 
            {
                gbDataMBC2.mapperROMBank = value;
                int tmpAddress = value << 14;
                tmpAddress &= gbRomSizeMask;
                gbMemoryMap[0x04] = &gbRom[tmpAddress];
                gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
                gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
                gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
            }
        }
        break;
    }
}
// MBC2 RAM write
void CGbEmu::mapperMBC2RAM(u16 address, u8 value)
{
    if(gbDataMBC2.mapperRAMEnable) 
    {
        if(gbRamSize && address < 0xa200) 
        {
            gbMemoryMap[address >> 12][address & 0x0fff] = value;
            systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
        }
    }
}
void CGbEmu::memoryUpdateMapMBC2()
{
    int tmpAddress = gbDataMBC2.mapperROMBank << 14;
    tmpAddress &= gbRomSizeMask;
    gbMemoryMap[0x04] = &gbRom[tmpAddress];
    gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
    gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
    gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
}
void CGbEmu::memoryUpdateMBC3Clock()
{
    time_t now = time(NULL);
    time_t diff = now - gbDataMBC3.mapperLastTime;
    if(diff > 0) 
    {
        // update the clock according to the last update time
        gbDataMBC3.mapperSeconds += diff % 60;
        if(gbDataMBC3.mapperSeconds > 59) 
        {
            gbDataMBC3.mapperSeconds -= 60;
            gbDataMBC3.mapperMinutes++;
        }
        diff /= 60;
        gbDataMBC3.mapperMinutes += diff % 60;
        if(gbDataMBC3.mapperMinutes > 60) 
        {
            gbDataMBC3.mapperMinutes -= 60;
            gbDataMBC3.mapperHours++;
        }
        diff /= 60;
        gbDataMBC3.mapperHours += diff % 24;
        if(gbDataMBC3.mapperHours > 24) 
        {
            gbDataMBC3.mapperHours -= 24;
            gbDataMBC3.mapperDays++;
        }
        diff /= 24;
        gbDataMBC3.mapperDays += diff;
        if(gbDataMBC3.mapperDays > 255) 
        {
            if(gbDataMBC3.mapperDays > 511) 
            {
                gbDataMBC3.mapperDays %= 512;
                gbDataMBC3.mapperControl |= 0x80;
            }
            gbDataMBC3.mapperControl = (gbDataMBC3.mapperControl & 0xfe) |
            (gbDataMBC3.mapperDays>255 ? 1 : 0);
        }
    }
    gbDataMBC3.mapperLastTime = now;
}
// MBC3 ROM write registers
void CGbEmu::mapperMBC3ROM(u16 address, u8 value)
{
    int tmpAddress = 0;
    switch(address & 0x6000) 
    {
        case 0x0000: // RAM enable register
        gbDataMBC3.mapperRAMEnable = ( ( value & 0x0a) == 0x0a ? 1 : 0);
        break;
        case 0x2000: // ROM bank select
        value = value & 0x7f;
        if(value == 0)
            value = 1;
        if(value == gbDataMBC3.mapperROMBank)
            break;
        tmpAddress = value << 14;
        tmpAddress &= gbRomSizeMask;
        gbDataMBC3.mapperROMBank = value;
        gbMemoryMap[0x04] = &gbRom[tmpAddress];
        gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
        gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
        gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        break;
        case 0x4000: // RAM bank select
        if(value < 8) 
        {
            if(value == gbDataMBC3.mapperRAMBank)
                break;
            tmpAddress = value << 13;
            tmpAddress &= gbRamSizeMask;
            gbMemoryMap[0x0a] = &gbRam[tmpAddress];
            gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
            gbDataMBC3.mapperRAMBank = value;
            gbDataMBC3.mapperRAMAddress = tmpAddress;
        }
        else 
        {
            if(gbDataMBC3.mapperRAMEnable) 
            {
                gbDataMBC3.mapperRAMBank = -1;
                gbDataMBC3.mapperClockRegister = value;
            }
        }
        break;
        case 0x6000: // clock latch
        if(gbDataMBC3.mapperClockLatch == 0 && value == 1) 
        {
            memoryUpdateMBC3Clock();
            gbDataMBC3.mapperLSeconds = gbDataMBC3.mapperSeconds;
            gbDataMBC3.mapperLMinutes = gbDataMBC3.mapperMinutes;
            gbDataMBC3.mapperLHours   = gbDataMBC3.mapperHours;
            gbDataMBC3.mapperLDays    = gbDataMBC3.mapperDays;
            gbDataMBC3.mapperLControl = gbDataMBC3.mapperControl;
        }
        if(value == 0x00 || value == 0x01)
            gbDataMBC3.mapperClockLatch = value;
        break;
    }
}
// MBC3 RAM write
void CGbEmu::mapperMBC3RAM(u16 address, u8 value)
{
    if(gbDataMBC3.mapperRAMEnable) 
    {
        if(gbDataMBC3.mapperRAMBank != -1) 
        {
            if(gbRamSize) 
            {
                gbMemoryMap[address>>12][address & 0x0fff] = value;
                systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
            }
        }
        else 
        {
            time(&gbDataMBC3.mapperLastTime);
            switch(gbDataMBC3.mapperClockRegister) 
            {
                case 0x08:
                gbDataMBC3.mapperSeconds = value;
                break;
                case 0x09:
                gbDataMBC3.mapperMinutes = value;
                break;
                case 0x0a:
                gbDataMBC3.mapperHours = value;
                break;
                case 0x0b:
                gbDataMBC3.mapperDays = value;
                break;
                case 0x0c:
                if(gbDataMBC3.mapperControl & 0x80)
                    gbDataMBC3.mapperControl = 0x80 | value;
                else
                    gbDataMBC3.mapperControl = value;
                break;
            }
        }
    }
}
// MBC3 read RAM
u8 CGbEmu::mapperMBC3ReadRAM(u16 address)
{
    if(gbDataMBC3.mapperRAMEnable) 
    {
        if(gbDataMBC3.mapperRAMBank != -1) 
        {
            return gbMemoryMap[address>>12][address & 0x0fff];
        }
        switch(gbDataMBC3.mapperClockRegister) 
        {
            case 0x08:
            return gbDataMBC3.mapperLSeconds;
            break;
            case 0x09:
            return gbDataMBC3.mapperLMinutes;
            break;
            case 0x0a:
            return gbDataMBC3.mapperLHours;
            break;
            case 0x0b:
            return gbDataMBC3.mapperLDays;
            break;
            case 0x0c:
            return gbDataMBC3.mapperLControl;
        }
    }
    return 0;
}
void CGbEmu::memoryUpdateMapMBC3()
{
    int tmpAddress = gbDataMBC3.mapperROMBank << 14;
    tmpAddress &= gbRomSizeMask;
    gbMemoryMap[0x04] = &gbRom[tmpAddress];
    gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
    gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
    gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
    if(gbDataMBC3.mapperRAMBank >= 0 && gbRamSize) 
    {
        tmpAddress = gbDataMBC3.mapperRAMBank << 13;
        tmpAddress &= gbRamSizeMask;
        gbMemoryMap[0x0a] = &gbRam[tmpAddress];
        gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
    }
}
// MBC5 ROM write registers
void CGbEmu::mapperMBC5ROM(u16 address, u8 value)
{
    int tmpAddress = 0;
    switch(address & 0x6000) 
    {
        case 0x0000: // RAM enable register
        gbDataMBC5.mapperRAMEnable = ( ( value & 0x0a) == 0x0a ? 1 : 0);
        break;
        case 0x2000: // ROM bank select
        if(address < 0x3000) 
        {
            value = value & 0xff;
            if(value == gbDataMBC5.mapperROMBank)
                break;
            tmpAddress = (value << 14) | (gbDataMBC5.mapperROMHighAddress << 22) ;
            tmpAddress &= gbRomSizeMask;
            gbDataMBC5.mapperROMBank = value;
            gbMemoryMap[0x04] = &gbRom[tmpAddress];
            gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
            gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
            gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        }
        else 
        {
            value = value & 1;
            if(value == gbDataMBC5.mapperROMHighAddress)
                break;
            tmpAddress = (gbDataMBC5.mapperROMBank << 14) | (value << 22);
            tmpAddress &= gbRomSizeMask;
            gbDataMBC5.mapperROMHighAddress = value;
            gbMemoryMap[0x04] = &gbRom[tmpAddress];
            gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
            gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
            gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        }
        break;
        case 0x4000: // RAM bank select
        if(gbDataMBC5.isRumbleCartridge)
            value &= 0x07;
        else
            value &= 0x0f;
        if(value == gbDataMBC5.mapperRAMBank)
            break;
        tmpAddress = value << 13;
        tmpAddress &= gbRamSizeMask;
        if(gbRamSize) 
        {
            gbMemoryMap[0x0a] = &gbRam[tmpAddress];
            gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
            gbDataMBC5.mapperRAMBank = value;
            gbDataMBC5.mapperRAMAddress = tmpAddress;
        }
        break;
    }
}
// MBC5 RAM write
void CGbEmu::mapperMBC5RAM(u16 address, u8 value)
{
    if(gbDataMBC5.mapperRAMEnable) 
    {
        if(gbRamSize) 
        {
            gbMemoryMap[address >> 12][address & 0x0fff] = value;
            systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
        }
    }
}
void CGbEmu::memoryUpdateMapMBC5()
{
    int tmpAddress = (gbDataMBC5.mapperROMBank << 14) |
    (gbDataMBC5.mapperROMHighAddress << 22) ;
    tmpAddress &= gbRomSizeMask;
    gbMemoryMap[0x04] = &gbRom[tmpAddress];
    gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
    gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
    gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
    if(gbRamSize) 
    {
        tmpAddress = gbDataMBC5.mapperRAMBank << 13;
        tmpAddress &= gbRamSizeMask;
        gbMemoryMap[0x0a] = &gbRam[tmpAddress];
        gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
    }
}
// MBC7 ROM write registers
void CGbEmu::mapperMBC7ROM(u16 address, u8 value)
{
    int tmpAddress = 0;
    switch(address & 0x6000) 
    {
        case 0x0000:
        break;
        case 0x2000: // ROM bank select
        value = value & 0x7f;
        if(value == 0)
            value = 1;
        if(value == gbDataMBC7.mapperROMBank)
            break;
        tmpAddress = (value << 14);
        tmpAddress &= gbRomSizeMask;
        gbDataMBC7.mapperROMBank = value;
        gbMemoryMap[0x04] = &gbRom[tmpAddress];
        gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
        gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
        gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        break;
        case 0x4000: // RAM bank select/enable
        if(value < 8) 
        {
            tmpAddress = (value&3) << 13;
            tmpAddress &= gbRamSizeMask;
            gbMemoryMap[0x0a] = &gbMemory[0xa000];
            gbMemoryMap[0x0b] = &gbMemory[0xb000];
            gbDataMBC7.mapperRAMBank = value;
            gbDataMBC7.mapperRAMAddress = tmpAddress;
            gbDataMBC7.mapperRAMEnable = 0;
        }
        else 
        {
            gbDataMBC7.mapperRAMEnable = 0;
        }
        break;
    }
}
// MBC7 read RAM
u8 CGbEmu::mapperMBC7ReadRAM(u16 address)
{
    switch(address & 0xa0f0) 
    {
        case 0xa000:
        case 0xa010:
        case 0xa060:
        case 0xa070:
        return 0;
        case 0xa020:
        // sensor X low byte
        return systemGetSensorX() & 255;
        case 0xa030:
        // sensor X high byte
        return systemGetSensorX() >> 8;
        case 0xa040:
        // sensor Y low byte
        return systemGetSensorY() & 255;
        case 0xa050:
        // sensor Y high byte
        return systemGetSensorY() >> 8;
        case 0xa080:
        return gbDataMBC7.value;
    }
    return 0xff;
}
// MBC7 RAM write
void CGbEmu::mapperMBC7RAM(u16 address, u8 value)
{
    if(address == 0xa080) 
    {
        // special processing needed
        int oldCs = gbDataMBC7.cs,oldSk=gbDataMBC7.sk;
        gbDataMBC7.cs=value>>7;
        gbDataMBC7.sk=(value>>6)&1;
        if(!oldCs && gbDataMBC7.cs) 
        {
            if(gbDataMBC7.state==5) 
            {
                if(gbDataMBC7.writeEnable) 
                {
                    gbMemory[0xa000+gbDataMBC7.address*2]=gbDataMBC7.buffer>>8;
                    gbMemory[0xa000+gbDataMBC7.address*2+1]=gbDataMBC7.buffer&0xff;
                    systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
                }
                gbDataMBC7.state=0;
                gbDataMBC7.value=1;
            }
            else 
            {
                gbDataMBC7.idle=true;
                gbDataMBC7.state=0;
            }
        }
        if(!oldSk && gbDataMBC7.sk) 
        {
            if(gbDataMBC7.idle) 
            {
                if(value & 0x02) 
                {
                    gbDataMBC7.idle=false;
                    gbDataMBC7.count=0;
                    gbDataMBC7.state=1;
                }
            }
            else 
            {
                switch(gbDataMBC7.state) 
                {
                    case 1:
                    // receiving command
                    gbDataMBC7.buffer <<= 1;
                    gbDataMBC7.buffer |= (value & 0x02)?1:0;
                    gbDataMBC7.count++;
                    if(gbDataMBC7.count==2) 
                    {
                        // finished receiving command
                        gbDataMBC7.state=2;
                        gbDataMBC7.count=0;
                        gbDataMBC7.code=gbDataMBC7.buffer & 3;
                    }
                    break;
                    case 2:
                    // receive address
                    gbDataMBC7.buffer <<= 1;
                    gbDataMBC7.buffer |= (value&0x02)?1:0;
                    gbDataMBC7.count++;
                    if(gbDataMBC7.count==8) 
                    {
                        // finish receiving
                        gbDataMBC7.state=3;
                        gbDataMBC7.count=0;
                        gbDataMBC7.address=gbDataMBC7.buffer&0xff;
                        if(gbDataMBC7.code==0) 
                        {
                            if((gbDataMBC7.address>>6)==0) 
                            {
                                gbDataMBC7.writeEnable=0;
                                gbDataMBC7.state=0;
                            }
                            else if((gbDataMBC7.address>>6) == 3) 
                            {
                                gbDataMBC7.writeEnable=1;
                                gbDataMBC7.state=0;
                            }
                        }
                    }
                    break;
                    case 3:
                    gbDataMBC7.buffer <<= 1;
                    gbDataMBC7.buffer |= (value&0x02)?1:0;
                    gbDataMBC7.count++;
                    switch(gbDataMBC7.code) 
                    {
                        case 0:
                        if(gbDataMBC7.count==16) 
                        {
                            if((gbDataMBC7.address>>6)==0) 
                            {
                                gbDataMBC7.writeEnable = 0;
                                gbDataMBC7.state=0;
                            }
                            else if((gbDataMBC7.address>>6)==1) 
                            {
                                if (gbDataMBC7.writeEnable) 
                                {
                                    for(int i=0;i<256;i++) 
                                    {
                                        gbMemory[0xa000+i*2] = gbDataMBC7.buffer >> 8;
                                        gbMemory[0xa000+i*2+1] = gbDataMBC7.buffer & 0xff;
                                        systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
                                    }
                                }
                                gbDataMBC7.state=5;
                            }
                            else if((gbDataMBC7.address>>6) == 2) 
                            {
                                if (gbDataMBC7.writeEnable) 
                                {
                                    for(int i=0;i<256;i++)
                                    WRITE16LE((u16 *)&gbMemory[0xa000+i*2], 0xffff);
                                    systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
                                }
                                gbDataMBC7.state=5;
                            }
                            else if((gbDataMBC7.address>>6)==3) 
                            {
                                gbDataMBC7.writeEnable = 1;
                                gbDataMBC7.state=0;
                            }
                            gbDataMBC7.count=0;
                        }
                        break;
                        case 1:
                        if(gbDataMBC7.count==16) 
                        {
                            gbDataMBC7.count=0;
                            gbDataMBC7.state=5;
                            gbDataMBC7.value=0;
                        }
                        break;
                        case 2:
                        if(gbDataMBC7.count==1) 
                        {
                            gbDataMBC7.state=4;
                            gbDataMBC7.count=0;
                            gbDataMBC7.buffer = (gbMemory[0xa000+gbDataMBC7.address*2]<<8)|
                            (gbMemory[0xa000+gbDataMBC7.address*2+1]);              
                        }
                        break;
                        case 3:
                        if(gbDataMBC7.count==16) 
                        {
                            gbDataMBC7.count=0;
                            gbDataMBC7.state=5;
                            gbDataMBC7.value=0;
                            gbDataMBC7.buffer=0xffff;
                        }
                        break;
                    }
                    break;
                }
            }
        }
        if (oldSk && !gbDataMBC7.sk) 
        {
            if (gbDataMBC7.state==4) 
            {
                gbDataMBC7.value = (gbDataMBC7.buffer & 0x8000)?1:0;
                gbDataMBC7.buffer <<= 1;
                gbDataMBC7.count++;
                if (gbDataMBC7.count==16) 
                {
                    gbDataMBC7.count=0;
                    gbDataMBC7.state=0;
                }
            }
        }
    }
}
void CGbEmu::memoryUpdateMapMBC7()
{
    int tmpAddress = (gbDataMBC5.mapperROMBank << 14);
    tmpAddress &= gbRomSizeMask;
    gbMemoryMap[0x04] = &gbRom[tmpAddress];
    gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
    gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
    gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
}
// HuC1 ROM write registers
void CGbEmu::mapperHuC1ROM(u16 address, u8 value)
{
    int tmpAddress = 0;
    switch(address & 0x6000) 
    {
        case 0x0000: // RAM enable register
        gbDataHuC1.mapperRAMEnable = ( ( value & 0x0a) == 0x0a ? 1 : 0);
        break;
        case 0x2000: // ROM bank select
        value = value & 0x3f;
        if(value == 0)
            value = 1;
        if(value == gbDataHuC1.mapperROMBank)
            break;
        tmpAddress = value << 14;
        tmpAddress &= gbRomSizeMask;
        gbDataHuC1.mapperROMBank = value;
        gbMemoryMap[0x04] = &gbRom[tmpAddress];
        gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
        gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
        gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        break;
        case 0x4000: // RAM bank select
        if(gbDataHuC1.mapperMemoryModel == 1) 
        {
            // 4/32 model, RAM bank switching provided
            value = value & 0x03;
            if(value == gbDataHuC1.mapperRAMBank)
                break;
            tmpAddress = value << 13;
            tmpAddress &= gbRamSizeMask;
            gbMemoryMap[0x0a] = &gbRam[tmpAddress];
            gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
            gbDataHuC1.mapperRAMBank = value;
            gbDataHuC1.mapperRAMAddress = tmpAddress;
        }
        else 
        {
            // 16/8, set the high address
            gbDataHuC1.mapperROMHighAddress = value & 0x03;
            tmpAddress = gbDataHuC1.mapperROMBank << 14;
            tmpAddress |= (gbDataHuC1.mapperROMHighAddress) << 19;
            tmpAddress &= gbRomSizeMask;
            gbMemoryMap[0x04] = &gbRom[tmpAddress];
            gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
            gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
            gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        }
        break;
        case 0x6000: // memory model select
        gbDataHuC1.mapperMemoryModel = value & 1;
        break;
    }
}
// HuC1 RAM write
void CGbEmu::mapperHuC1RAM(u16 address, u8 value)
{
    if(gbDataHuC1.mapperRAMEnable) 
    {
        if(gbRamSize) 
        {
            gbMemoryMap[address >> 12][address & 0x0fff] = value;
            systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
        }
    }
}
void CGbEmu::memoryUpdateMapHuC1()
{
    int tmpAddress = gbDataHuC1.mapperROMBank << 14;
    tmpAddress &= gbRomSizeMask;
    gbMemoryMap[0x04] = &gbRom[tmpAddress];
    gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
    gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
    gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
    if(gbRamSize) 
    {
        tmpAddress = gbDataHuC1.mapperRAMBank << 13;
        tmpAddress &= gbRamSizeMask;
        gbMemoryMap[0x0a] = &gbRam[tmpAddress];
        gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
    }
}
// HuC3 ROM write registers
void CGbEmu::mapperHuC3ROM(u16 address, u8 value)
{
    int tmpAddress = 0;
    switch(address & 0x6000) 
    {
        case 0x0000: // RAM enable register
        gbDataHuC3.mapperRAMEnable = ( value == 0x0a ? 1 : 0);
        gbDataHuC3.mapperRAMFlag = value;
        if(gbDataHuC3.mapperRAMFlag != 0x0a)
            gbDataHuC3.mapperRAMBank = -1;
        break;
        case 0x2000: // ROM bank select
        value = value & 0x7f;
        if(value == 0)
            value = 1;
        if(value == gbDataHuC3.mapperROMBank)
            break;
        tmpAddress = value << 14;
        tmpAddress &= gbRomSizeMask;
        gbDataHuC3.mapperROMBank = value;
        gbMemoryMap[0x04] = &gbRom[tmpAddress];
        gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
        gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
        gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
        break;
        case 0x4000: // RAM bank select
        value = value & 0x03;
        if(value == gbDataHuC3.mapperRAMBank)
            break;
        tmpAddress = value << 13;
        tmpAddress &= gbRamSizeMask;
        gbMemoryMap[0x0a] = &gbRam[tmpAddress];
        gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
        gbDataHuC3.mapperRAMBank = value;
        gbDataHuC3.mapperRAMAddress = tmpAddress;
        break;
        case 0x6000: // nothing to do!
        break;
    }
}
// HuC3 read RAM
u8 CGbEmu::mapperHuC3ReadRAM(u16 address)
{
    if(gbDataHuC3.mapperRAMFlag > 0x0b &&
        gbDataHuC3.mapperRAMFlag < 0x0e) 
    {
        if(gbDataHuC3.mapperRAMFlag != 0x0c)
            return 1;
        return gbDataHuC3.mapperRAMValue;
    }
    else
        return gbMemoryMap[address >> 12][address & 0x0fff];
}
// HuC3 RAM write
void CGbEmu::mapperHuC3RAM(u16 address, u8 value)
{
    int *p;
    if(gbDataHuC3.mapperRAMFlag < 0x0b ||
        gbDataHuC3.mapperRAMFlag > 0x0e) 
    {
        if(gbDataHuC3.mapperRAMEnable) 
        {
            if(gbRamSize) 
            {
                gbMemoryMap[address >> 12][address & 0x0fff] = value;
                systemSaveUpdateCounter = SYSTEM_SAVE_UPDATED;
            }
        }
    }
    else 
    {
        if(gbDataHuC3.mapperRAMFlag == 0x0b) 
        {
            if(value == 0x62) 
            {
                gbDataHuC3.mapperRAMValue = 1;
            }
            else 
            {
                switch(value & 0xf0) 
                {
                    case 0x10:
                    p = &gbDataHuC3.mapperRegister2;
                    gbDataHuC3.mapperRAMValue = *(p+gbDataHuC3.mapperRegister1++);
                    if(gbDataHuC3.mapperRegister1 > 6)
                        gbDataHuC3.mapperRegister1 = 0;
                    break;
                    case 0x30:
                    p = &gbDataHuC3.mapperRegister2;
                    *(p+gbDataHuC3.mapperRegister1++) = value & 0x0f;
                    if(gbDataHuC3.mapperRegister1 > 6)
                        gbDataHuC3.mapperRegister1 = 0;
                    gbDataHuC3.mapperAddress =
                    (gbDataHuC3.mapperRegister6 << 24) |
                    (gbDataHuC3.mapperRegister5 << 16) |
                    (gbDataHuC3.mapperRegister4 <<  8) |
                    (gbDataHuC3.mapperRegister3 <<  4) |
                    (gbDataHuC3.mapperRegister2);
                    break;
                    case 0x40:
                    gbDataHuC3.mapperRegister1 = (gbDataHuC3.mapperRegister1 & 0xf0) |
                    (value & 0x0f);
                    gbDataHuC3.mapperRegister2 = (gbDataHuC3.mapperAddress & 0x0f);
                    gbDataHuC3.mapperRegister3 = ((gbDataHuC3.mapperAddress>>4)&0x0f);
                    gbDataHuC3.mapperRegister4 = ((gbDataHuC3.mapperAddress>>8)&0x0f);
                    gbDataHuC3.mapperRegister5 = ((gbDataHuC3.mapperAddress>>16)&0x0f);
                    gbDataHuC3.mapperRegister6 = ((gbDataHuC3.mapperAddress>>24)&0x0f);
                    gbDataHuC3.mapperRegister7 = 0;
                    gbDataHuC3.mapperRegister8 = 0;
                    gbDataHuC3.mapperRAMValue = 0;
                    break;
                    case 0x50:
                    gbDataHuC3.mapperRegister1 = (gbDataHuC3.mapperRegister1 & 0x0f) |
                    ((value << 4)&0x0f);
                    break;
                    default:
                    gbDataHuC3.mapperRAMValue = 1;
                    break;
                }
            }
        }
    }
}
void CGbEmu::memoryUpdateMapHuC3()
{
    int tmpAddress = gbDataHuC3.mapperROMBank << 14;
    tmpAddress &= gbRomSizeMask;
    gbMemoryMap[0x04] = &gbRom[tmpAddress];
    gbMemoryMap[0x05] = &gbRom[tmpAddress + 0x1000];
    gbMemoryMap[0x06] = &gbRom[tmpAddress + 0x2000];
    gbMemoryMap[0x07] = &gbRom[tmpAddress + 0x3000];
    if(gbRamSize) 
    {
        tmpAddress = gbDataHuC3.mapperRAMBank << 13;
        tmpAddress &= gbRamSizeMask;
        gbMemoryMap[0x0a] = &gbRam[tmpAddress];
        gbMemoryMap[0x0b] = &gbRam[tmpAddress + 0x1000];
    }
}
int CGbEmu::gbInit()
{
    ASSERT( gbMemory == NULL);
    ASSERT( pix == NULL);
    ASSERT( gbLineBuffer == NULL);
    MALLOC(gbMemory,u8,65536);
    MALLOC(pix,u8,4*257*226);
    MALLOC(gbLineBuffer,u16,160);
    memset(gbMemory,0, 65536);
    gbGenFilter();
    gbSgbInit();
    this->InitColorMap();

    return OK;
}
int CGbEmu::systemGetSensorX()
{
    return 0;
}
int CGbEmu::systemGetSensorY()
{
    return 0;
}
bool CGbEmu::utilWriteBMPFile(CFileBase *file, int w, int h, u8 *pix)
{
    ASSERT(file);

    file->SetSize(0);
    this->utilWriteBMPHeader(file,w,h);
    this->utilWriteBMPBits(file,w,h,pix);

    return true;
}
void CGbEmu::gbEmulate(int ticksToStop)
{
    gbRegister tempRegister;
    u8 tempValue;
    s8 offset;
    int clockTicks = 0;
    gbDmaTicks = 0;
    register int opcode = 0;
    while(1) 
    {
        if(IFF & 0x80) 
        {
            if(register_LCDC & 0x80) 
            {
                clockTicks = gbLcdTicks;
            }
            else
                clockTicks = 100;
            if(gbLcdMode == 1 && (gbLcdLYIncrementTicks < clockTicks))
                clockTicks = gbLcdLYIncrementTicks;
            if(gbSerialOn && (gbSerialTicks < clockTicks))
                clockTicks = gbSerialTicks;
            if(gbTimerOn && (gbTimerTicks < clockTicks))
                clockTicks = gbTimerTicks;
            if(soundTicks && (soundTicks < clockTicks))
                clockTicks = soundTicks;
        }
        else 
        {
            opcode = gbReadOpcode(PC.W++);
            if(IFF & 0x100) 
            {
                IFF &= 0xff;
                PC.W--;
            }
            clockTicks = gbCycles[opcode];
            switch(opcode) 
            {
                case 0xCB:
                // extended opcode
                opcode = gbReadOpcode(PC.W++);
                clockTicks = gbCyclesCB[opcode];
                switch(opcode) 
                {
                    case 0x00:
                    // RLC B
                    AF.B.B0 = (BC.B.B1 & 0x80)?C_FLAG:0;
                    BC.B.B1 = (BC.B.B1<<1) | (BC.B.B1>>7);
                    AF.B.B0 |= ZeroTable[BC.B.B1];
                    break;
                    case 0x01:
                    // RLC C
                    AF.B.B0 = (BC.B.B0 & 0x80)?C_FLAG:0;
                    BC.B.B0 = (BC.B.B0<<1) | (BC.B.B0>>7);
                    AF.B.B0 |= ZeroTable[BC.B.B0];
                    break;
                    case 0x02:
                    // RLC D
                    AF.B.B0 = (DE.B.B1 & 0x80)?C_FLAG:0;
                    DE.B.B1 = (DE.B.B1<<1) | (DE.B.B1>>7);
                    AF.B.B0 |= ZeroTable[DE.B.B1];
                    break;
                    case 0x03:
                    // RLC E
                    AF.B.B0 = (DE.B.B0 & 0x80)?C_FLAG:0;
                    DE.B.B0 = (DE.B.B0<<1) | (DE.B.B0>>7);
                    AF.B.B0 |= ZeroTable[DE.B.B0];
                    break;
                    case 0x04:
                    // RLC H
                    AF.B.B0 = (HL.B.B1 & 0x80)?C_FLAG:0;
                    HL.B.B1 = (HL.B.B1<<1) | (HL.B.B1>>7);
                    AF.B.B0 |= ZeroTable[HL.B.B1];
                    break;
                    case 0x05:
                    // RLC L
                    AF.B.B0 = (HL.B.B0 & 0x80)?C_FLAG:0;
                    HL.B.B0 = (HL.B.B0<<1) | (HL.B.B0>>7);
                    AF.B.B0 |= ZeroTable[HL.B.B0];
                    break;
                    case 0x06:
                    // RLC (HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0 = (tempValue & 0x80)?C_FLAG:0;
                    tempValue = (tempValue<<1) | (tempValue>>7);
                    AF.B.B0 |= ZeroTable[tempValue];
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x07:
                    // RLC A
                    AF.B.B0 = (AF.B.B1 & 0x80)?C_FLAG:0;
                    AF.B.B1 = (AF.B.B1<<1) | (AF.B.B1>>7);
                    AF.B.B0 |= ZeroTable[AF.B.B1];
                    break;
                    case 0x08:
                    // RRC B
                    AF.B.B0=(BC.B.B1&0x01 ? C_FLAG : 0);
                    BC.B.B1=(BC.B.B1>>1)|(BC.B.B1<<7);
                    AF.B.B0|=ZeroTable[BC.B.B1];
                    break;
                    case 0x09:
                    // RRC C
                    AF.B.B0=(BC.B.B0&0x01 ? C_FLAG : 0);
                    BC.B.B0=(BC.B.B0>>1)|(BC.B.B0<<7);
                    AF.B.B0|=ZeroTable[BC.B.B0];
                    break;
                    case 0x0a:
                    // RRC D
                    AF.B.B0=(DE.B.B1&0x01 ? C_FLAG : 0);
                    DE.B.B1=(DE.B.B1>>1)|(DE.B.B1<<7);
                    AF.B.B0|=ZeroTable[DE.B.B1];
                    break;
                    case 0x0b:
                    // RRC E
                    AF.B.B0=(DE.B.B0&0x01 ? C_FLAG : 0);
                    DE.B.B0=(DE.B.B0>>1)|(DE.B.B0<<7);
                    AF.B.B0|=ZeroTable[DE.B.B0];
                    break;
                    case 0x0c:
                    // RRC H
                    AF.B.B0=(HL.B.B1&0x01 ? C_FLAG : 0);
                    HL.B.B1=(HL.B.B1>>1)|(HL.B.B1<<7);
                    AF.B.B0|=ZeroTable[HL.B.B1];
                    break;
                    case 0x0d:
                    // RRC L
                    AF.B.B0=(HL.B.B0&0x01 ? C_FLAG : 0);
                    HL.B.B0=(HL.B.B0>>1)|(HL.B.B0<<7);
                    AF.B.B0|=ZeroTable[HL.B.B0];
                    break;
                    case 0x0e:
                    // RRC (HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(tempValue&0x01 ? C_FLAG : 0);
                    tempValue=(tempValue>>1)|(tempValue<<7);
                    AF.B.B0|=ZeroTable[tempValue];
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x0f:
                    // RRC A
                    AF.B.B0=(AF.B.B1&0x01 ? C_FLAG : 0);
                    AF.B.B1=(AF.B.B1>>1)|(AF.B.B1<<7);
                    AF.B.B0|=ZeroTable[AF.B.B1];
                    break;
                    case 0x10:
                    // RL B
                    if(BC.B.B1&0x80) 
                    {
                        BC.B.B1=(BC.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[BC.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        BC.B.B1=(BC.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[BC.B.B1];
                    }
                    break;
                    case 0x11:
                    // RL C
                    if(BC.B.B0&0x80) 
                    {
                        BC.B.B0=(BC.B.B0<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[BC.B.B0]|C_FLAG;
                    }
                    else 
                    {
                        BC.B.B0=(BC.B.B0<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[BC.B.B0];
                    }
                    break;
                    case 0x12:
                    // RL D
                    if(DE.B.B1&0x80) 
                    {
                        DE.B.B1=(DE.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[DE.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        DE.B.B1=(DE.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[DE.B.B1];
                    }
                    break;
                    case 0x13:
                    // RL E
                    if(DE.B.B0&0x80) 
                    {
                        DE.B.B0=(DE.B.B0<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[DE.B.B0]|C_FLAG;
                    }
                    else 
                    {
                        DE.B.B0=(DE.B.B0<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[DE.B.B0];
                    }
                    break;
                    case 0x14:
                    // RL H
                    if(HL.B.B1&0x80) 
                    {
                        HL.B.B1=(HL.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[HL.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        HL.B.B1=(HL.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[HL.B.B1];
                    }
                    break;
                    case 0x15:
                    // RL L
                    if(HL.B.B0&0x80) 
                    {
                        HL.B.B0=(HL.B.B0<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[HL.B.B0]|C_FLAG;
                    }
                    else 
                    {
                        HL.B.B0=(HL.B.B0<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[HL.B.B0];
                    }
                    break;
                    case 0x16:
                    // RL (HL)
                    tempValue=gbReadMemory(HL.W);
                    if(tempValue&0x80) 
                    {
                        tempValue=(tempValue<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[tempValue]|C_FLAG;
                    }
                    else 
                    {
                        tempValue=(tempValue<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[tempValue];
                    }
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x17:
                    // RL A
                    if(AF.B.B1&0x80) 
                    {
                        AF.B.B1=(AF.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[AF.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        AF.B.B1=(AF.B.B1<<1)|(AF.B.B0&C_FLAG ? 1 : 0);
                        AF.B.B0=ZeroTable[AF.B.B1];
                    }
                    break;
                    case 0x18:
                    // RR B
                    if(BC.B.B1&0x01) 
                    {
                        BC.B.B1=(BC.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[BC.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        BC.B.B1=(BC.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[BC.B.B1];
                    }
                    break;
                    case 0x19:
                    // RR C
                    if(BC.B.B0&0x01) 
                    {
                        BC.B.B0=(BC.B.B0>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[BC.B.B0]|C_FLAG;
                    }
                    else 
                    {
                        BC.B.B0=(BC.B.B0>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[BC.B.B0];
                    }
                    break;
                    case 0x1a:
                    // RR D
                    if(DE.B.B1&0x01) 
                    {
                        DE.B.B1=(DE.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[DE.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        DE.B.B1=(DE.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[DE.B.B1];
                    }
                    break;
                    case 0x1b:
                    // RR E
                    if(DE.B.B0&0x01) 
                    {
                        DE.B.B0=(DE.B.B0>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[DE.B.B0]|C_FLAG;
                    }
                    else 
                    {
                        DE.B.B0=(DE.B.B0>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[DE.B.B0];
                    }
                    break;
                    case 0x1c:
                    // RR H
                    if(HL.B.B1&0x01) 
                    {
                        HL.B.B1=(HL.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[HL.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        HL.B.B1=(HL.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[HL.B.B1];
                    }
                    break;
                    case 0x1d:
                    // RR L
                    if(HL.B.B0&0x01) 
                    {
                        HL.B.B0=(HL.B.B0>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[HL.B.B0]|C_FLAG;
                    }
                    else 
                    {
                        HL.B.B0=(HL.B.B0>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[HL.B.B0];
                    }
                    break;
                    case 0x1e:
                    // RR (HL)
                    tempValue=gbReadMemory(HL.W);
                    if(tempValue&0x01) 
                    {
                        tempValue=(tempValue>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[tempValue]|C_FLAG;
                    }
                    else 
                    {
                        tempValue=(tempValue>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[tempValue];
                    }
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x1f:
                    // RR A
                    if(AF.B.B1&0x01) 
                    {
                        AF.B.B1=(AF.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[AF.B.B1]|C_FLAG;
                    }
                    else 
                    {
                        AF.B.B1=(AF.B.B1>>1)|(AF.B.B0 & C_FLAG ? 0x80:0);
                        AF.B.B0=ZeroTable[AF.B.B1];
                    }
                    break;
                    case 0x20:
                    // SLA B
                    AF.B.B0=(BC.B.B1&0x80?C_FLAG : 0);
                    BC.B.B1<<=1;
                    AF.B.B0|=ZeroTable[BC.B.B1];
                    break;
                    case 0x21:
                    // SLA C
                    AF.B.B0=(BC.B.B0&0x80?C_FLAG : 0);
                    BC.B.B0<<=1;
                    AF.B.B0|=ZeroTable[BC.B.B0];
                    break;
                    case 0x22:
                    // SLA D
                    AF.B.B0=(DE.B.B1&0x80?C_FLAG : 0);
                    DE.B.B1<<=1;
                    AF.B.B0|=ZeroTable[DE.B.B1];
                    break;
                    case 0x23:
                    // SLA E
                    AF.B.B0=(DE.B.B0&0x80?C_FLAG : 0);
                    DE.B.B0<<=1;
                    AF.B.B0|=ZeroTable[DE.B.B0];
                    break;
                    case 0x24:
                    // SLA H
                    AF.B.B0=(HL.B.B1&0x80?C_FLAG : 0);
                    HL.B.B1<<=1;
                    AF.B.B0|=ZeroTable[HL.B.B1];
                    break;
                    case 0x25:
                    // SLA L
                    AF.B.B0=(HL.B.B0&0x80?C_FLAG : 0);
                    HL.B.B0<<=1;
                    AF.B.B0|=ZeroTable[HL.B.B0];
                    break;
                    case 0x26:
                    // SLA (HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(tempValue&0x80?C_FLAG : 0);
                    tempValue<<=1;
                    AF.B.B0|=ZeroTable[tempValue];
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x27:
                    // SLA A
                    AF.B.B0=(AF.B.B1&0x80?C_FLAG : 0);
                    AF.B.B1<<=1;
                    AF.B.B0|=ZeroTable[AF.B.B1];
                    break;
                    case 0x28:
                    // SRA B
                    AF.B.B0=(BC.B.B1&0x01 ? C_FLAG: 0);
                    BC.B.B1=(BC.B.B1>>1)|(BC.B.B1&0x80);
                    AF.B.B0|=ZeroTable[BC.B.B1];
                    break;
                    case 0x29:
                    // SRA C
                    AF.B.B0=(BC.B.B0&0x01 ? C_FLAG: 0);
                    BC.B.B0=(BC.B.B0>>1)|(BC.B.B0&0x80);
                    AF.B.B0|=ZeroTable[BC.B.B0];
                    break;
                    case 0x2a:
                    // SRA D
                    AF.B.B0=(DE.B.B1&0x01 ? C_FLAG: 0);
                    DE.B.B1=(DE.B.B1>>1)|(DE.B.B1&0x80);
                    AF.B.B0|=ZeroTable[DE.B.B1];
                    break;
                    case 0x2b:
                    // SRA E
                    AF.B.B0=(DE.B.B0&0x01 ? C_FLAG: 0);
                    DE.B.B0=(DE.B.B0>>1)|(DE.B.B0&0x80);
                    AF.B.B0|=ZeroTable[DE.B.B0];
                    break;
                    case 0x2c:
                    // SRA H
                    AF.B.B0=(HL.B.B1&0x01 ? C_FLAG: 0);
                    HL.B.B1=(HL.B.B1>>1)|(HL.B.B1&0x80);
                    AF.B.B0|=ZeroTable[HL.B.B1];
                    break;
                    case 0x2d:
                    // SRA L
                    AF.B.B0=(HL.B.B0&0x01 ? C_FLAG: 0);
                    HL.B.B0=(HL.B.B0>>1)|(HL.B.B0&0x80);
                    AF.B.B0|=ZeroTable[HL.B.B0];
                    break;
                    case 0x2e:
                    // SRA (HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(tempValue&0x01 ? C_FLAG: 0);
                    tempValue=(tempValue>>1)|(tempValue&0x80);
                    AF.B.B0|=ZeroTable[tempValue];
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x2f:
                    // SRA A
                    AF.B.B0=(AF.B.B1&0x01 ? C_FLAG: 0);
                    AF.B.B1=(AF.B.B1>>1)|(AF.B.B1&0x80);
                    AF.B.B0|=ZeroTable[AF.B.B1];
                    break;
                    case 0x30:
                    // SWAP B
                    BC.B.B1 = (BC.B.B1&0xf0)>>4 | (BC.B.B1&0x0f)<<4;
                    AF.B.B0 = ZeroTable[BC.B.B1];
                    break;
                    case 0x31:
                    // SWAP C
                    BC.B.B0 = (BC.B.B0&0xf0)>>4 | (BC.B.B0&0x0f)<<4;
                    AF.B.B0 = ZeroTable[BC.B.B0];
                    break;
                    case 0x32:
                    // SWAP D
                    DE.B.B1 = (DE.B.B1&0xf0)>>4 | (DE.B.B1&0x0f)<<4;
                    AF.B.B0 = ZeroTable[DE.B.B1];
                    break;
                    case 0x33:
                    // SWAP E
                    DE.B.B0 = (DE.B.B0&0xf0)>>4 | (DE.B.B0&0x0f)<<4;
                    AF.B.B0 = ZeroTable[DE.B.B0];
                    break;
                    case 0x34:
                    // SWAP H
                    HL.B.B1 = (HL.B.B1&0xf0)>>4 | (HL.B.B1&0x0f)<<4;
                    AF.B.B0 = ZeroTable[HL.B.B1];
                    break;
                    case 0x35:
                    // SWAP L
                    HL.B.B0 = (HL.B.B0&0xf0)>>4 | (HL.B.B0&0x0f)<<4;
                    AF.B.B0 = ZeroTable[HL.B.B0];
                    break;
                    case 0x36:
                    // SWAP (HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue = (tempValue&0xf0)>>4 | (tempValue&0x0f)<<4;
                    AF.B.B0 = ZeroTable[tempValue];
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x37:
                    // SWAP A
                    AF.B.B1 = (AF.B.B1&0xf0)>>4 | (AF.B.B1&0x0f)<<4;
                    AF.B.B0 = ZeroTable[AF.B.B1];
                    break;
                    case 0x38:
                    // SRL B
                    AF.B.B0=(BC.B.B1&0x01)?C_FLAG:0;
                    BC.B.B1>>=1;
                    AF.B.B0|=ZeroTable[BC.B.B1];
                    break;
                    case 0x39:
                    // SRL C
                    AF.B.B0=(BC.B.B0&0x01)?C_FLAG:0;
                    BC.B.B0>>=1;
                    AF.B.B0|=ZeroTable[BC.B.B0];
                    break;
                    case 0x3a:
                    // SRL D
                    AF.B.B0=(DE.B.B1&0x01)?C_FLAG:0;
                    DE.B.B1>>=1;
                    AF.B.B0|=ZeroTable[DE.B.B1];
                    break;
                    case 0x3b:
                    // SRL E
                    AF.B.B0=(DE.B.B0&0x01)?C_FLAG:0;
                    DE.B.B0>>=1;
                    AF.B.B0|=ZeroTable[DE.B.B0];
                    break;
                    case 0x3c:
                    // SRL H
                    AF.B.B0=(HL.B.B1&0x01)?C_FLAG:0;
                    HL.B.B1>>=1;
                    AF.B.B0|=ZeroTable[HL.B.B1];
                    break;
                    case 0x3d:
                    // SRL L
                    AF.B.B0=(HL.B.B0&0x01)?C_FLAG:0;
                    HL.B.B0>>=1;
                    AF.B.B0|=ZeroTable[HL.B.B0];
                    break;
                    case 0x3e:
                    // SRL (HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(tempValue&0x01)?C_FLAG:0;
                    tempValue>>=1;
                    AF.B.B0|=ZeroTable[tempValue];
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x3f:
                    // SRL A
                    AF.B.B0=(AF.B.B1&0x01)?C_FLAG:0;
                    AF.B.B1>>=1;
                    AF.B.B0|=ZeroTable[AF.B.B1];
                    break;
                    case 0x40:
                    // BIT 0,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x41:
                    // BIT 0,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x42:
                    // BIT 0,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x43:
                    // BIT 0,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x44:
                    // BIT 0,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x45:
                    // BIT 0,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x46:
                    // BIT 0,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x47:
                    // BIT 0,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<0)? 0:Z_FLAG);
                    break;
                    case 0x48:
                    // BIT 1,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x49:
                    // BIT 1,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x4a:
                    // BIT 1,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x4b:
                    // BIT 1,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x4c:
                    // BIT 1,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x4d:
                    // BIT 1,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x4e:
                    // BIT 1,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x4f:
                    // BIT 1,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<1)? 0:Z_FLAG);
                    break;
                    case 0x50:
                    // BIT 2,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x51:
                    // BIT 2,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x52:
                    // BIT 2,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x53:
                    // BIT 2,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x54:
                    // BIT 2,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x55:
                    // BIT 2,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x56:
                    // BIT 2,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x57:
                    // BIT 2,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<2)? 0:Z_FLAG);
                    break;
                    case 0x58:
                    // BIT 3,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x59:
                    // BIT 3,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x5a:
                    // BIT 3,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x5b:
                    // BIT 3,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x5c:
                    // BIT 3,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x5d:
                    // BIT 3,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x5e:
                    // BIT 3,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x5f:
                    // BIT 3,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<3)? 0:Z_FLAG);
                    break;
                    case 0x60:
                    // BIT 4,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x61:
                    // BIT 4,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x62:
                    // BIT 4,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x63:
                    // BIT 4,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x64:
                    // BIT 4,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x65:
                    // BIT 4,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x66:
                    // BIT 4,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x67:
                    // BIT 4,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<4)? 0:Z_FLAG);
                    break;
                    case 0x68:
                    // BIT 5,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x69:
                    // BIT 5,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x6a:
                    // BIT 5,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x6b:
                    // BIT 5,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x6c:
                    // BIT 5,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x6d:
                    // BIT 5,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x6e:
                    // BIT 5,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x6f:
                    // BIT 5,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<5)? 0:Z_FLAG);
                    break;
                    case 0x70:
                    // BIT 6,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x71:
                    // BIT 6,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x72:
                    // BIT 6,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x73:
                    // BIT 6,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x74:
                    // BIT 6,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x75:
                    // BIT 6,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x76:
                    // BIT 6,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x77:
                    // BIT 6,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<6)? 0:Z_FLAG);
                    break;
                    case 0x78:
                    // BIT 7,B
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B1&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x79:
                    // BIT 7,C
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(BC.B.B0&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x7a:
                    // BIT 7,D
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B1&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x7b:
                    // BIT 7,E
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(DE.B.B0&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x7c:
                    // BIT 7,H
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B1&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x7d:
                    // BIT 7,L
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(HL.B.B0&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x7e:
                    // BIT 7,(HL)
                    tempValue=gbReadMemory(HL.W);
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(tempValue&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x7f:
                    // BIT 7,A
                    AF.B.B0=(AF.B.B0&C_FLAG)|H_FLAG|(AF.B.B1&(1<<7)? 0:Z_FLAG);
                    break;
                    case 0x80:
                    // RES 0,B
                    BC.B.B1&=~(1<<0);
                    break;
                    case 0x81:
                    // RES 0,C
                    BC.B.B0&=~(1<<0);
                    break;
                    case 0x82:
                    // RES 0,D
                    DE.B.B1&=~(1<<0);
                    break;
                    case 0x83:
                    // RES 0,E
                    DE.B.B0&=~(1<<0);
                    break;
                    case 0x84:
                    // RES 0,H
                    HL.B.B1&=~(1<<0);
                    break;
                    case 0x85:
                    // RES 0,L
                    HL.B.B0&=~(1<<0);
                    break;
                    case 0x86:
                    // RES 0,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<0);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x87:
                    // RES 0,A
                    AF.B.B1&=~(1<<0);
                    break;
                    case 0x88:
                    // RES 1,B
                    BC.B.B1&=~(1<<1);
                    break;
                    case 0x89:
                    // RES 1,C
                    BC.B.B0&=~(1<<1);
                    break;
                    case 0x8a:
                    // RES 1,D
                    DE.B.B1&=~(1<<1);
                    break;
                    case 0x8b:
                    // RES 1,E
                    DE.B.B0&=~(1<<1);
                    break;
                    case 0x8c:
                    // RES 1,H
                    HL.B.B1&=~(1<<1);
                    break;
                    case 0x8d:
                    // RES 1,L
                    HL.B.B0&=~(1<<1);
                    break;
                    case 0x8e:
                    // RES 1,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<1);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x8f:
                    // RES 1,A
                    AF.B.B1&=~(1<<1);
                    break;
                    case 0x90:
                    // RES 2,B
                    BC.B.B1&=~(1<<2);
                    break;
                    case 0x91:
                    // RES 2,C
                    BC.B.B0&=~(1<<2);
                    break;
                    case 0x92:
                    // RES 2,D
                    DE.B.B1&=~(1<<2);
                    break;
                    case 0x93:
                    // RES 2,E
                    DE.B.B0&=~(1<<2);
                    break;
                    case 0x94:
                    // RES 2,H
                    HL.B.B1&=~(1<<2);
                    break;
                    case 0x95:
                    // RES 2,L
                    HL.B.B0&=~(1<<2);
                    break;
                    case 0x96:
                    // RES 2,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<2);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x97:
                    // RES 2,A
                    AF.B.B1&=~(1<<2);
                    break;
                    case 0x98:
                    // RES 3,B
                    BC.B.B1&=~(1<<3);
                    break;
                    case 0x99:
                    // RES 3,C
                    BC.B.B0&=~(1<<3);
                    break;
                    case 0x9a:
                    // RES 3,D
                    DE.B.B1&=~(1<<3);
                    break;
                    case 0x9b:
                    // RES 3,E
                    DE.B.B0&=~(1<<3);
                    break;
                    case 0x9c:
                    // RES 3,H
                    HL.B.B1&=~(1<<3);
                    break;
                    case 0x9d:
                    // RES 3,L
                    HL.B.B0&=~(1<<3);
                    break;
                    case 0x9e:
                    // RES 3,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<3);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0x9f:
                    // RES 3,A
                    AF.B.B1&=~(1<<3);
                    break;
                    case 0xa0:
                    // RES 4,B
                    BC.B.B1&=~(1<<4);
                    break;
                    case 0xa1:
                    // RES 4,C
                    BC.B.B0&=~(1<<4);
                    break;
                    case 0xa2:
                    // RES 4,D
                    DE.B.B1&=~(1<<4);
                    break;
                    case 0xa3:
                    // RES 4,E
                    DE.B.B0&=~(1<<4);
                    break;
                    case 0xa4:
                    // RES 4,H
                    HL.B.B1&=~(1<<4);
                    break;
                    case 0xa5:
                    // RES 4,L
                    HL.B.B0&=~(1<<4);
                    break;
                    case 0xa6:
                    // RES 4,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<4);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xa7:
                    // RES 4,A
                    AF.B.B1&=~(1<<4);
                    break;
                    case 0xa8:
                    // RES 5,B
                    BC.B.B1&=~(1<<5);
                    break;
                    case 0xa9:
                    // RES 5,C
                    BC.B.B0&=~(1<<5);
                    break;
                    case 0xaa:
                    // RES 5,D
                    DE.B.B1&=~(1<<5);
                    break;
                    case 0xab:
                    // RES 5,E
                    DE.B.B0&=~(1<<5);
                    break;
                    case 0xac:
                    // RES 5,H
                    HL.B.B1&=~(1<<5);
                    break;
                    case 0xad:
                    // RES 5,L
                    HL.B.B0&=~(1<<5);
                    break;
                    case 0xae:
                    // RES 5,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<5);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xaf:
                    // RES 5,A
                    AF.B.B1&=~(1<<5);
                    break;
                    case 0xb0:
                    // RES 6,B
                    BC.B.B1&=~(1<<6);
                    break;
                    case 0xb1:
                    // RES 6,C
                    BC.B.B0&=~(1<<6);
                    break;
                    case 0xb2:
                    // RES 6,D
                    DE.B.B1&=~(1<<6);
                    break;
                    case 0xb3:
                    // RES 6,E
                    DE.B.B0&=~(1<<6);
                    break;
                    case 0xb4:
                    // RES 6,H
                    HL.B.B1&=~(1<<6);
                    break;
                    case 0xb5:
                    // RES 6,L
                    HL.B.B0&=~(1<<6);
                    break;
                    case 0xb6:
                    // RES 6,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<6);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xb7:
                    // RES 6,A
                    AF.B.B1&=~(1<<6);
                    break;
                    case 0xb8:
                    // RES 7,B
                    BC.B.B1&=~(1<<7);
                    break;
                    case 0xb9:
                    // RES 7,C
                    BC.B.B0&=~(1<<7);
                    break;
                    case 0xba:
                    // RES 7,D
                    DE.B.B1&=~(1<<7);
                    break;
                    case 0xbb:
                    // RES 7,E
                    DE.B.B0&=~(1<<7);
                    break;
                    case 0xbc:
                    // RES 7,H
                    HL.B.B1&=~(1<<7);
                    break;
                    case 0xbd:
                    // RES 7,L
                    HL.B.B0&=~(1<<7);
                    break;
                    case 0xbe:
                    // RES 7,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue&=~(1<<7);
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xbf:
                    // RES 7,A
                    AF.B.B1&=~(1<<7);
                    break;
                    case 0xc0:
                    // SET 0,B
                    BC.B.B1|=1<<0;
                    break;
                    case 0xc1:
                    // SET 0,C
                    BC.B.B0|=1<<0;
                    break;
                    case 0xc2:
                    // SET 0,D
                    DE.B.B1|=1<<0;
                    break;
                    case 0xc3:
                    // SET 0,E
                    DE.B.B0|=1<<0;
                    break;
                    case 0xc4:
                    // SET 0,H
                    HL.B.B1|=1<<0;
                    break;
                    case 0xc5:
                    // SET 0,L
                    HL.B.B0|=1<<0;
                    break;
                    case 0xc6:
                    // SET 0,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<0;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xc7:
                    // SET 0,A
                    AF.B.B1|=1<<0;
                    break;
                    case 0xc8:
                    // SET 1,B
                    BC.B.B1|=1<<1;
                    break;
                    case 0xc9:
                    // SET 1,C
                    BC.B.B0|=1<<1;
                    break;
                    case 0xca:
                    // SET 1,D
                    DE.B.B1|=1<<1;
                    break;
                    case 0xcb:
                    // SET 1,E
                    DE.B.B0|=1<<1;
                    break;
                    case 0xcc:
                    // SET 1,H
                    HL.B.B1|=1<<1;
                    break;
                    case 0xcd:
                    // SET 1,L
                    HL.B.B0|=1<<1;
                    break;
                    case 0xce:
                    // SET 1,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<1;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xcf:
                    // SET 1,A
                    AF.B.B1|=1<<1;
                    break;
                    case 0xd0:
                    // SET 2,B
                    BC.B.B1|=1<<2;
                    break;
                    case 0xd1:
                    // SET 2,C
                    BC.B.B0|=1<<2;
                    break;
                    case 0xd2:
                    // SET 2,D
                    DE.B.B1|=1<<2;
                    break;
                    case 0xd3:
                    // SET 2,E
                    DE.B.B0|=1<<2;
                    break;
                    case 0xd4:
                    // SET 2,H
                    HL.B.B1|=1<<2;
                    break;
                    case 0xd5:
                    // SET 2,L
                    HL.B.B0|=1<<2;
                    break;
                    case 0xd6:
                    // SET 2,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<2;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xd7:
                    // SET 2,A
                    AF.B.B1|=1<<2;
                    break;
                    case 0xd8:
                    // SET 3,B
                    BC.B.B1|=1<<3;
                    break;
                    case 0xd9:
                    // SET 3,C
                    BC.B.B0|=1<<3;
                    break;
                    case 0xda:
                    // SET 3,D
                    DE.B.B1|=1<<3;
                    break;
                    case 0xdb:
                    // SET 3,E
                    DE.B.B0|=1<<3;
                    break;
                    case 0xdc:
                    // SET 3,H
                    HL.B.B1|=1<<3;
                    break;
                    case 0xdd:
                    // SET 3,L
                    HL.B.B0|=1<<3;
                    break;
                    case 0xde:
                    // SET 3,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<3;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xdf:
                    // SET 3,A
                    AF.B.B1|=1<<3;
                    break;
                    case 0xe0:
                    // SET 4,B
                    BC.B.B1|=1<<4;
                    break;
                    case 0xe1:
                    // SET 4,C
                    BC.B.B0|=1<<4;
                    break;
                    case 0xe2:
                    // SET 4,D
                    DE.B.B1|=1<<4;
                    break;
                    case 0xe3:
                    // SET 4,E
                    DE.B.B0|=1<<4;
                    break;
                    case 0xe4:
                    // SET 4,H
                    HL.B.B1|=1<<4;
                    break;
                    case 0xe5:
                    // SET 4,L
                    HL.B.B0|=1<<4;
                    break;
                    case 0xe6:
                    // SET 4,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<4;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xe7:
                    // SET 4,A
                    AF.B.B1|=1<<4;
                    break;
                    case 0xe8:
                    // SET 5,B
                    BC.B.B1|=1<<5;
                    break;
                    case 0xe9:
                    // SET 5,C
                    BC.B.B0|=1<<5;
                    break;
                    case 0xea:
                    // SET 5,D
                    DE.B.B1|=1<<5;
                    break;
                    case 0xeb:
                    // SET 5,E
                    DE.B.B0|=1<<5;
                    break;
                    case 0xec:
                    // SET 5,H
                    HL.B.B1|=1<<5;
                    break;
                    case 0xed:
                    // SET 5,L
                    HL.B.B0|=1<<5;
                    break;
                    case 0xee:
                    // SET 5,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<5;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xef:
                    // SET 5,A
                    AF.B.B1|=1<<5;
                    break;
                    case 0xf0:
                    // SET 6,B
                    BC.B.B1|=1<<6;
                    break;
                    case 0xf1:
                    // SET 6,C
                    BC.B.B0|=1<<6;
                    break;
                    case 0xf2:
                    // SET 6,D
                    DE.B.B1|=1<<6;
                    break;
                    case 0xf3:
                    // SET 6,E
                    DE.B.B0|=1<<6;
                    break;
                    case 0xf4:
                    // SET 6,H
                    HL.B.B1|=1<<6;
                    break;
                    case 0xf5:
                    // SET 6,L
                    HL.B.B0|=1<<6;
                    break;
                    case 0xf6:
                    // SET 6,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<6;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xf7:
                    // SET 6,A
                    AF.B.B1|=1<<6;
                    break;
                    case 0xf8:
                    // SET 7,B
                    BC.B.B1|=1<<7;
                    break;
                    case 0xf9:
                    // SET 7,C
                    BC.B.B0|=1<<7;
                    break;
                    case 0xfa:
                    // SET 7,D
                    DE.B.B1|=1<<7;
                    break;
                    case 0xfb:
                    // SET 7,E
                    DE.B.B0|=1<<7;
                    break;
                    case 0xfc:
                    // SET 7,H
                    HL.B.B1|=1<<7;
                    break;
                    case 0xfd:
                    // SET 7,L
                    HL.B.B0|=1<<7;
                    break;
                    case 0xfe:
                    // SET 7,(HL)
                    tempValue=gbReadMemory(HL.W);
                    tempValue|=1<<7;
                    gbWriteMemory(HL.W,tempValue);
                    break;
                    case 0xff:
                    // SET 7,A
                    AF.B.B1|=1<<7;
                    break;
                    default:
                    LOG("Unknown opcode %02x at %04x", gbReadOpcode(PC.W-1),PC.W-1);
                    emulating = false;
                    return;                 
                }
                break;
                case 0x00: 
                // NOP
                break;
                case 0x01: 
                // LD BC, NNNN
                BC.B.B0=gbReadMemory(PC.W++);
                BC.B.B1=gbReadMemory(PC.W++);
                break;
                case 0x02:
                // LD (BC),A
                gbWriteMemory(BC.W,AF.B.B1);
                break;
                case 0x03:
                // INC BC
                BC.W++;
                break;
                case 0x04: 
                // INC B
                BC.B.B1++; 
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[BC.B.B1]| (BC.B.B1&0x0F? 0:H_FLAG);
                break;
                case 0x05:
                // DEC B
                BC.B.B1--; 
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[BC.B.B1]|
                ((BC.B.B1&0x0F)==0x0F? H_FLAG:0);
                break;
                case 0x06:
                // LD B, NN
                BC.B.B1=gbReadOpcode(PC.W++);
                break;
                case 0x07:
                // RLCA
                tempValue=AF.B.B1&0x80? C_FLAG:0;
                AF.B.B1=(AF.B.B1<<1)|(AF.B.B1>>7);
                AF.B.B0=tempValue;
                break;
                case 0x08:
                // LD (NNNN), SP
                tempRegister.B.B0=gbReadOpcode(PC.W++);
                tempRegister.B.B1=gbReadOpcode(PC.W++);
                gbWriteMemory(tempRegister.W++,SP.B.B0);
                gbWriteMemory(tempRegister.W,SP.B.B1);
                break;
                case 0x09:
                // ADD HL,BC
                tempRegister.W=(HL.W+BC.W)&0xFFFF;
                AF.B.B0= (AF.B.B0 & Z_FLAG)| ((HL.W^BC.W^tempRegister.W)&0x1000? H_FLAG:0)|
                (((long)HL.W+(long)BC.W)&0x10000? C_FLAG:0);
                HL.W=tempRegister.W;
                break;
                case 0x0a:
                // LD A,(BC)
                AF.B.B1=gbReadMemory(BC.W);
                break;
                case 0x0b:
                // DEC BC
                BC.W--;
                break;   
                case 0x0c:
                // INC C
                BC.B.B0++;
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[BC.B.B0]| (BC.B.B0&0x0F? 0:H_FLAG);
                break;
                case 0x0d:
                // DEC C
                BC.B.B0--;
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[BC.B.B0]|
                ((BC.B.B0&0x0F)==0x0F? H_FLAG:0);
                break;   
                case 0x0e:
                // LD C, NN
                BC.B.B0=gbReadOpcode(PC.W++);
                break;
                case 0x0f:
                // RRCA
                tempValue=AF.B.B1&0x01;
                AF.B.B1=(AF.B.B1>>1)|(tempValue? 0x80:0);
                AF.B.B0=(tempValue<<4);
                break;
                case 0x10:
                // STOP
                opcode = gbReadOpcode(PC.W++);
                if(gbCgbMode) 
                {
                    if(gbMemory[0xff4d] & 1) 
                    {
                        gbSpeedSwitch();
                        if(gbSpeed == 0)
                            gbMemory[0xff4d] = 0x00;
                        else
                            gbMemory[0xff4d] = 0x80;
                    }
                }
                break;
                case 0x11:
                // LD DE, NNNN
                DE.B.B0=gbReadMemory(PC.W++);
                DE.B.B1=gbReadMemory(PC.W++);
                break;
                case 0x12:
                // LD (DE),A
                gbWriteMemory(DE.W,AF.B.B1);
                break;
                case 0x13:
                // INC DE
                DE.W++;
                break;
                case 0x14:
                // INC D
                DE.B.B1++;
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[DE.B.B1]| (DE.B.B1&0x0F? 0:H_FLAG);
                break;
                case 0x15:
                // DEC D
                DE.B.B1--;
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[DE.B.B1]|
                ((DE.B.B1&0x0F)==0x0F? H_FLAG:0);
                break;
                case 0x16:
                //  LD D,NN
                DE.B.B1=gbReadOpcode(PC.W++);
                break;
                case 0x17:
                // RLA
                tempValue=AF.B.B1&0x80? C_FLAG:0;
                AF.B.B1=(AF.B.B1<<1)|((AF.B.B0&C_FLAG)>>4);
                AF.B.B0=tempValue;
                break;
                case 0x18:
                // JR NN
                PC.W+=(s8)gbReadMemory(PC.W)+1;
                break;
                case 0x19:
                // ADD HL,DE
                tempRegister.W=(HL.W+DE.W)&0xFFFF;
                AF.B.B0= (AF.B.B0 & Z_FLAG)| ((HL.W^DE.W^tempRegister.W)&0x1000? H_FLAG:0)|
                (((long)HL.W+(long)DE.W)&0x10000? C_FLAG:0);
                HL.W=tempRegister.W;
                break;
                case 0x1a:
                // LD A,(DE)
                AF.B.B1=gbReadMemory(DE.W);
                break;   
                case 0x1b:
                // DEC DE
                DE.W--;
                break;
                case 0x1c:
                // INC E
                DE.B.B0++;
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[DE.B.B0]| (DE.B.B0&0x0F? 0:H_FLAG);
                break;
                case 0x1d:
                // DEC E
                DE.B.B0--;
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[DE.B.B0]|
                ((DE.B.B0&0x0F)==0x0F? H_FLAG:0);
                break;
                case 0x1e:
                // LD E,NN
                DE.B.B0=gbReadOpcode(PC.W++);
                break;   
                case 0x1f:
                // RRA
                tempValue=AF.B.B1&0x01;
                AF.B.B1=(AF.B.B1>>1)|(AF.B.B0&C_FLAG? 0x80:0);
                AF.B.B0=(tempValue<<4);
                break;
                case 0x20:
                // JR NZ,NN
                if(AF.B.B0&Z_FLAG)
                    PC.W++;
                else 
                {
                    PC.W+=(s8)gbReadMemory(PC.W)+1;
                    clockTicks++;
                }
                break;
                case 0x21:
                // LD HL,NNNN
                HL.B.B0=gbReadMemory(PC.W++);
                HL.B.B1=gbReadMemory(PC.W++);
                break;   
                case 0x22:
                // LDI (HL),A
                gbWriteMemory(HL.W++,AF.B.B1);
                break;
                case 0x23:
                // INC HL
                HL.W++;
                break;
                case 0x24:
                // INC H
                HL.B.B1++;
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[HL.B.B1]| (HL.B.B1&0x0F? 0:H_FLAG);
                break;
                case 0x25:
                // DEC H
                HL.B.B1--;
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[HL.B.B1]|
                ((HL.B.B1&0x0F)==0x0F? H_FLAG:0);
                break;
                case 0x26:
                // LD H,NN
                HL.B.B1=gbReadOpcode(PC.W++);
                break;
                case 0x27:
                // DAA
                tempRegister.W=AF.B.B1;
                if(AF.B.B0&C_FLAG) tempRegister.W|=256;
                if(AF.B.B0&H_FLAG) tempRegister.W|=512;
                if(AF.B.B0&N_FLAG) tempRegister.W|=1024;
                AF.W=DAATable[tempRegister.W];
                break;
                case 0x28:
                // JR Z,NN
                if(AF.B.B0&Z_FLAG) 
                {
                    PC.W+=(s8)gbReadMemory(PC.W)+1;
                    clockTicks++;
                }
                else
                    PC.W++;
                break;
                case 0x29:
                // ADD HL,HL
                tempRegister.W=(HL.W+HL.W)&0xFFFF; AF.B.B0= (AF.B.B0 & Z_FLAG)|
                ((HL.W^HL.W^tempRegister.W)&0x1000? H_FLAG:0)|
                (((long)HL.W+(long)HL.W)&0x10000? C_FLAG:0);
                HL.W=tempRegister.W;
                break;
                case 0x2a:
                // LDI A,(HL)
                AF.B.B1 = gbReadMemory(HL.W++);
                break;
                case 0x2b:
                // DEC HL
                HL.W--;
                break;   
                case 0x2c:
                // INC L
                HL.B.B0++;
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[HL.B.B0]| (HL.B.B0&0x0F? 0:H_FLAG);
                break;
                case 0x2d:
                // DEC L
                HL.B.B0--;
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[HL.B.B0]|
                ((HL.B.B0&0x0F)==0x0F? H_FLAG:0);
                break;
                case 0x2e:
                // LD L,NN
                HL.B.B0=gbReadOpcode(PC.W++);
                break;   
                case 0x2f:
                // CPL
                AF.B.B1 ^= 255;
                AF.B.B0|=N_FLAG|H_FLAG;
                break;
                case 0x30:
                // JR NC,NN
                if(AF.B.B0&C_FLAG)
                    PC.W++;
                else 
                {
                    PC.W+=(s8)gbReadMemory(PC.W)+1;
                    clockTicks++;
                }
                break;
                case 0x31:
                // LD SP,NNNN
                SP.B.B0=gbReadMemory(PC.W++);
                SP.B.B1=gbReadMemory(PC.W++);
                break;
                case 0x32:
                // LDD (HL),A
                gbWriteMemory(HL.W--,AF.B.B1);
                break;
                case 0x33:
                // INC SP
                SP.W++;
                break;
                case 0x34:
                // INC (HL)
                tempValue=gbReadMemory(HL.W)+1;
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[tempValue]| (tempValue&0x0F? 0:H_FLAG);
                gbWriteMemory(HL.W,tempValue);
                break;
                case 0x35:
                // DEC (HL)
                tempValue=gbReadMemory(HL.W)-1;
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[tempValue]|
                ((tempValue&0x0F)==0x0F? H_FLAG:0);gbWriteMemory(HL.W,tempValue);
                break;
                case 0x36:
                // LD (HL),NN
                gbWriteMemory(HL.W,gbReadOpcode(PC.W++));
                break;
                case 0x37:
                // SCF
                AF.B.B0 = AF.B.B0 & Z_FLAG | C_FLAG;
                break;   
                case 0x38:
                // JR C,NN
                if(AF.B.B0&C_FLAG) 
                {
                    PC.W+=(s8)gbReadMemory(PC.W)+1;
                    clockTicks ++;
                }
                else
                    PC.W++;
                break;
                case 0x39:
                // ADD HL,SP
                tempRegister.W=(HL.W+SP.W)&0xFFFF;
                AF.B.B0= (AF.B.B0 & Z_FLAG)| ((HL.W^SP.W^tempRegister.W)&0x1000? H_FLAG:0)|
                (((long)HL.W+(long)SP.W)&0x10000? C_FLAG:0);
                HL.W=tempRegister.W;
                break;
                case 0x3a:
                // LDD A,(HL)
                AF.B.B1 = gbReadMemory(HL.W--);
                break;
                case 0x3b:
                // DEC SP
                SP.W--;
                break;
                case 0x3c:
                // INC A
                AF.B.B1++;
                AF.B.B0= (AF.B.B0 & C_FLAG)|ZeroTable[AF.B.B1]| (AF.B.B1&0x0F? 0:H_FLAG);
                break;
                case 0x3d:
                // DEC A
                AF.B.B1--;
                AF.B.B0= N_FLAG|(AF.B.B0 & C_FLAG)|ZeroTable[AF.B.B1]|
                ((AF.B.B1&0x0F)==0x0F? H_FLAG:0);
                break;
                case 0x3e:
                // LD A,NN
                AF.B.B1=gbReadOpcode(PC.W++);
                break;
                case 0x3f:
                // CCF
                AF.B.B0^=C_FLAG;AF.B.B0&=~(N_FLAG|H_FLAG);
                break;
                case 0x40:
                // LD B,B
                BC.B.B1=BC.B.B1;
                break;
                case 0x41:
                // LD B,C
                BC.B.B1=BC.B.B0;
                break;
                case 0x42:
                // LD B,D
                BC.B.B1=DE.B.B1;
                break;
                case 0x43:
                // LD B,E
                BC.B.B1=DE.B.B0;
                break;
                case 0x44:
                // LD B,H
                BC.B.B1=HL.B.B1;
                break;
                case 0x45:
                // LD B,L
                BC.B.B1=HL.B.B0;
                break;
                case 0x46:
                // LD B,(HL)
                BC.B.B1=gbReadMemory(HL.W);
                break;
                case 0x47:
                // LD B,A
                BC.B.B1=AF.B.B1;
                break;
                case 0x48:
                // LD C,B
                BC.B.B0=BC.B.B1;
                break;
                case 0x49:
                // LD C,C
                BC.B.B0=BC.B.B0;
                break;
                case 0x4a:
                // LD C,D
                BC.B.B0=DE.B.B1;
                break;
                case 0x4b:
                // LD C,E
                BC.B.B0=DE.B.B0;
                break;
                case 0x4c:
                // LD C,H
                BC.B.B0=HL.B.B1;
                break;
                case 0x4d:
                // LD C,L
                BC.B.B0=HL.B.B0;
                break;
                case 0x4e:
                // LD C,(HL)
                BC.B.B0=gbReadMemory(HL.W);
                break;
                case 0x4f:
                // LD C,A
                BC.B.B0=AF.B.B1;
                break;
                case 0x50:
                // LD D,B
                DE.B.B1=BC.B.B1;
                break;
                case 0x51:
                // LD D,C
                DE.B.B1=BC.B.B0;
                break;
                case 0x52:
                // LD D,D
                DE.B.B1=DE.B.B1;
                break;
                case 0x53:
                // LD D,E
                DE.B.B1=DE.B.B0;
                break;
                case 0x54:
                // LD D,H
                DE.B.B1=HL.B.B1;
                break;
                case 0x55:
                // LD D,L
                DE.B.B1=HL.B.B0;
                break;
                case 0x56:
                // LD D,(HL)
                DE.B.B1=gbReadMemory(HL.W);
                break;
                case 0x57:
                // LD D,A
                DE.B.B1=AF.B.B1;
                break;
                case 0x58:
                // LD E,B
                DE.B.B0=BC.B.B1;
                break;
                case 0x59:
                // LD E,C
                DE.B.B0=BC.B.B0;
                break;
                case 0x5a:
                // LD E,D
                DE.B.B0=DE.B.B1;
                break;
                case 0x5b:
                // LD E,E
                DE.B.B0=DE.B.B0;
                break;
                case 0x5c:
                // LD E,H
                DE.B.B0=HL.B.B1;
                break;
                case 0x5d:
                // LD E,L
                DE.B.B0=HL.B.B0;
                break;
                case 0x5e:
                // LD E,(HL)
                DE.B.B0=gbReadMemory(HL.W);
                break;
                case 0x5f:
                // LD E,A
                DE.B.B0=AF.B.B1;
                break;
                case 0x60:
                // LD H,B
                HL.B.B1=BC.B.B1;
                break;
                case 0x61:
                // LD H,C
                HL.B.B1=BC.B.B0;
                break;
                case 0x62:
                // LD H,D
                HL.B.B1=DE.B.B1;
                break;
                case 0x63:
                // LD H,E
                HL.B.B1=DE.B.B0;
                break;
                case 0x64:
                // LD H,H
                HL.B.B1=HL.B.B1;
                break;
                case 0x65:
                // LD H,L
                HL.B.B1=HL.B.B0;
                break;
                case 0x66:
                // LD H,(HL)
                HL.B.B1=gbReadMemory(HL.W);
                break;
                case 0x67:
                // LD H,A
                HL.B.B1=AF.B.B1;
                break;
                case 0x68:
                // LD L,B
                HL.B.B0=BC.B.B1;
                break;
                case 0x69:
                // LD L,C
                HL.B.B0=BC.B.B0;
                break;
                case 0x6a:
                // LD L,D
                HL.B.B0=DE.B.B1;
                break;
                case 0x6b:
                // LD L,E
                HL.B.B0=DE.B.B0;
                break;
                case 0x6c:
                // LD L,H
                HL.B.B0=HL.B.B1;
                break;
                case 0x6d:
                // LD L,L
                HL.B.B0=HL.B.B0;
                break;
                case 0x6e:
                // LD L,(HL)
                HL.B.B0=gbReadMemory(HL.W);
                break;
                case 0x6f:
                // LD L,A
                HL.B.B0=AF.B.B1;
                break;
                case 0x70:
                // LD (HL),B
                gbWriteMemory(HL.W,BC.B.B1);
                break;
                case 0x71:
                // LD (HL),C
                gbWriteMemory(HL.W,BC.B.B0);
                break;
                case 0x72:
                // LD (HL),D
                gbWriteMemory(HL.W,DE.B.B1);
                break;
                case 0x73:
                // LD (HL),E
                gbWriteMemory(HL.W,DE.B.B0);
                break;
                case 0x74:
                // LD (HL),H
                gbWriteMemory(HL.W,HL.B.B1);
                break;
                case 0x75:
                // LD (HL),L
                gbWriteMemory(HL.W,HL.B.B0);
                break;
                case 0x76:
                // HALT
                if(IFF & 1) 
                {
                    PC.W--;
                    IFF |= 0x80;
                }
                else 
                {
                    if((register_IE & register_IF) > 0)
                        IFF |= 0x100;
                    else 
                    {
                        PC.W--;
                        IFF |= 0x81;
                    }
                }
                break;
                case 0x77:
                // LD (HL),A
                gbWriteMemory(HL.W,AF.B.B1);
                break;
                case 0x78:
                // LD A,B
                AF.B.B1=BC.B.B1;
                break;
                case 0x79:
                // LD A,C
                AF.B.B1=BC.B.B0;
                break;
                case 0x7a:
                // LD A,D
                AF.B.B1=DE.B.B1;
                break;
                case 0x7b:
                // LD A,E
                AF.B.B1=DE.B.B0;
                break;
                case 0x7c:
                // LD A,H
                AF.B.B1=HL.B.B1;
                break;
                case 0x7d:
                // LD A,L
                AF.B.B1=HL.B.B0;
                break;
                case 0x7e:
                // LD A,(HL)
                AF.B.B1=gbReadMemory(HL.W);
                break;
                case 0x7f:
                // LD A,A
                AF.B.B1=AF.B.B1;
                break;
                case 0x80:
                // ADD B
                tempRegister.W=AF.B.B1+BC.B.B1;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B1^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x81:
                // ADD C
                tempRegister.W=AF.B.B1+BC.B.B0;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B0^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x82:
                // ADD D
                tempRegister.W=AF.B.B1+DE.B.B1;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B1^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x83:
                // ADD E
                tempRegister.W=AF.B.B1+DE.B.B0;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B0^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x84:
                // ADD H
                tempRegister.W=AF.B.B1+HL.B.B1;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B1^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x85:
                // ADD L
                tempRegister.W=AF.B.B1+HL.B.B0;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B0^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x86:
                // ADD (HL)
                tempValue=gbReadMemory(HL.W);
                tempRegister.W=AF.B.B1+tempValue;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x87:
                // ADD A
                tempRegister.W=AF.B.B1+AF.B.B1;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^AF.B.B1^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x88:
                // ADC B:
                tempRegister.W=AF.B.B1+BC.B.B1+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x89:
                // ADC C
                tempRegister.W=AF.B.B1+BC.B.B0+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x8a:
                // ADC D
                tempRegister.W=AF.B.B1+DE.B.B1+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x8b:
                // ADC E
                tempRegister.W=AF.B.B1+DE.B.B0+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x8c:
                // ADC H
                tempRegister.W=AF.B.B1+HL.B.B1+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0); AF.B.B1=tempRegister.B.B0;
                break;
                case 0x8d:
                // ADC L
                tempRegister.W=AF.B.B1+HL.B.B0+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x8e:
                // ADC (HL)
                tempValue=gbReadMemory(HL.W);
                tempRegister.W=AF.B.B1+tempValue+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x8f:
                // ADC A
                tempRegister.W=AF.B.B1+AF.B.B1+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^AF.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x90:
                // SUB B
                tempRegister.W=AF.B.B1-BC.B.B1;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x91:
                // SUB C
                tempRegister.W=AF.B.B1-BC.B.B0;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x92:
                // SUB D
                tempRegister.W=AF.B.B1-DE.B.B1;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x93:
                // SUB E
                tempRegister.W=AF.B.B1-DE.B.B0;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x94:
                // SUB H
                tempRegister.W=AF.B.B1-HL.B.B1;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x95:
                // SUB L
                tempRegister.W=AF.B.B1-HL.B.B0;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x96:
                // SUB (HL)
                tempValue=gbReadMemory(HL.W);
                tempRegister.W=AF.B.B1-tempValue;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x97:
                // SUB A
                AF.B.B1=0;
                AF.B.B0=N_FLAG|Z_FLAG;
                break;
                case 0x98:
                // SBC B
                tempRegister.W=AF.B.B1-BC.B.B1-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x99:
                // SBC C
                tempRegister.W=AF.B.B1-BC.B.B0-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x9a:
                // SBC D
                tempRegister.W=AF.B.B1-DE.B.B1-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x9b:
                // SBC E
                tempRegister.W=AF.B.B1-DE.B.B0-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x9c:
                // SBC H
                tempRegister.W=AF.B.B1-HL.B.B1-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x9d:
                // SBC L
                tempRegister.W=AF.B.B1-HL.B.B0-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x9e:
                // SBC (HL)
                tempValue=gbReadMemory(HL.W);
                tempRegister.W=AF.B.B1-tempValue-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0x9f:
                // SBC A
                tempRegister.W=AF.B.B1-AF.B.B1-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^AF.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0xa0:
                // AND B
                AF.B.B1&=BC.B.B1;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa1:
                // AND C
                AF.B.B1&=BC.B.B0;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa2:
                // AND_D
                AF.B.B1&=DE.B.B1;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa3:
                // AND E
                AF.B.B1&=DE.B.B0;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa4:
                // AND H
                AF.B.B1&=HL.B.B1;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa5:
                // AND L
                AF.B.B1&=HL.B.B0;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa6:
                // AND (HL)
                tempValue=gbReadMemory(HL.W);
                AF.B.B1&=tempValue;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa7:
                // AND A
                AF.B.B1&=AF.B.B1;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xa8:
                // XOR B
                AF.B.B1^=BC.B.B1;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xa9:
                // XOR C
                AF.B.B1^=BC.B.B0;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xaa:
                // XOR D
                AF.B.B1^=DE.B.B1;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xab:
                // XOR E
                AF.B.B1^=DE.B.B0;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xac:
                // XOR H
                AF.B.B1^=HL.B.B1;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xad:
                // XOR L
                AF.B.B1^=HL.B.B0;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xae:
                // XOR (HL)
                tempValue=gbReadMemory(HL.W);
                AF.B.B1^=tempValue;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xaf:
                // XOR A
                AF.B.B1=0;
                AF.B.B0=Z_FLAG;
                break;
                case 0xb0:
                // OR B
                AF.B.B1|=BC.B.B1;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb1:
                // OR C
                AF.B.B1|=BC.B.B0;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb2:
                // OR D
                AF.B.B1|=DE.B.B1;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb3:
                // OR E
                AF.B.B1|=DE.B.B0;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb4:
                // OR H
                AF.B.B1|=HL.B.B1;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb5:
                // OR L
                AF.B.B1|=HL.B.B0;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb6:
                // OR (HL)
                tempValue=gbReadMemory(HL.W);
                AF.B.B1|=tempValue;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb7:
                // OR A
                AF.B.B1|=AF.B.B1;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xb8:
                // CP B:
                tempRegister.W=AF.B.B1-BC.B.B1;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xb9:
                // CP C
                tempRegister.W=AF.B.B1-BC.B.B0;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^BC.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xba:
                // CP D
                tempRegister.W=AF.B.B1-DE.B.B1;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xbb:
                // CP E
                tempRegister.W=AF.B.B1-DE.B.B0;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^DE.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xbc:
                // CP H
                tempRegister.W=AF.B.B1-HL.B.B1;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B1^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xbd:
                // CP L
                tempRegister.W=AF.B.B1-HL.B.B0;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^HL.B.B0^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xbe:
                // CP (HL)
                tempValue=gbReadMemory(HL.W);
                tempRegister.W=AF.B.B1-tempValue;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xbf:
                // CP A
                AF.B.B0=N_FLAG|Z_FLAG;
                break;
                case 0xc0:
                // RET NZ
                if(!(AF.B.B0&Z_FLAG)) 
                {
                    PC.B.B0=gbReadMemory(SP.W++);
                    PC.B.B1=gbReadMemory(SP.W++);
                    clockTicks += 3;
                }
                break;
                case 0xc1:
                // POP BC
                BC.B.B0=gbReadMemory(SP.W++);
                BC.B.B1=gbReadMemory(SP.W++);
                break;
                case 0xc2:
                // JP NZ,NNNN
                if(AF.B.B0&Z_FLAG)
                    PC.W+=2;
                else 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W);
                    PC.W=tempRegister.W;
                    clockTicks++;
                }
                break;
                case 0xc3:
                // JP NNNN
                tempRegister.B.B0=gbReadMemory(PC.W++);
                tempRegister.B.B1=gbReadMemory(PC.W);
                PC.W=tempRegister.W;
                break;
                case 0xc4:
                // CALL NZ,NNNN
                if(AF.B.B0&Z_FLAG)
                    PC.W+=2;
                else 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W++);
                    gbWriteMemory(--SP.W,PC.B.B1);
                    gbWriteMemory(--SP.W,PC.B.B0);
                    PC.W=tempRegister.W;
                    clockTicks += 3;
                }
                break;
                case 0xc5:
                // PUSH BC
                gbWriteMemory(--SP.W,BC.B.B1);
                gbWriteMemory(--SP.W,BC.B.B0);
                break;
                case 0xc6:
                // ADD NN
                tempValue=gbReadOpcode(PC.W++);
                tempRegister.W=AF.B.B1+tempValue;
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10 ? H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0xc7:
                // RST 00
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0000;
                break;
                case 0xc8:
                // RET Z
                if(AF.B.B0&Z_FLAG) 
                {
                    PC.B.B0=gbReadMemory(SP.W++);
                    PC.B.B1=gbReadMemory(SP.W++);
                    clockTicks += 3;
                }
                break;
                case 0xc9:
                // RET
                PC.B.B0=gbReadMemory(SP.W++);
                PC.B.B1=gbReadMemory(SP.W++);
                break;
                case 0xca:
                // JP Z,NNNN
                if(AF.B.B0&Z_FLAG) 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W);
                    PC.W=tempRegister.W;
                    clockTicks++;
                }
                else
                    PC.W+=2;
                break;
                // CB done outside
                case 0xcc:
                // CALL Z,NNNN
                if(AF.B.B0&Z_FLAG) 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W++);
                    gbWriteMemory(--SP.W,PC.B.B1);
                    gbWriteMemory(--SP.W,PC.B.B0);
                    PC.W=tempRegister.W;
                    clockTicks += 3;
                }
                else
                    PC.W+=2;
                break;
                case 0xcd:
                // CALL NNNN
                tempRegister.B.B0=gbReadMemory(PC.W++);
                tempRegister.B.B1=gbReadMemory(PC.W++);
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=tempRegister.W;
                break;
                case 0xce:
                // ADC NN
                tempValue=gbReadOpcode(PC.W++);
                tempRegister.W=AF.B.B1+tempValue+(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= (tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0xcf:
                // RST 08
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0008;
                break;
                case 0xd0:
                // RET NC
                if(!(AF.B.B0&C_FLAG)) 
                {
                    PC.B.B0=gbReadMemory(SP.W++);
                    PC.B.B1=gbReadMemory(SP.W++);
                    clockTicks += 3;
                }
                break;
                case 0xd1:
                // POP DE
                DE.B.B0=gbReadMemory(SP.W++);
                DE.B.B1=gbReadMemory(SP.W++);
                break;
                case 0xd2:
                // JP NC,NNNN
                if(AF.B.B0&C_FLAG)
                    PC.W+=2;
                else 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W);
                    PC.W=tempRegister.W;
                    clockTicks++;
                }
                break;
                // D3 illegal
                case 0xd4:
                // CALL NC,NNNN
                if(AF.B.B0&C_FLAG)
                    PC.W+=2;
                else 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W++);
                    gbWriteMemory(--SP.W,PC.B.B1);
                    gbWriteMemory(--SP.W,PC.B.B0);
                    PC.W=tempRegister.W;
                    clockTicks += 3;
                }
                break;
                case 0xd5:
                // PUSH DE
                gbWriteMemory(--SP.W,DE.B.B1);
                gbWriteMemory(--SP.W,DE.B.B0);
                break;
                case 0xd6:
                // SUB NN
                tempValue=gbReadOpcode(PC.W++);
                tempRegister.W=AF.B.B1-tempValue;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0xd7:
                // RST 10
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0010;
                break;
                case 0xd8:
                // RET C
                if(AF.B.B0&C_FLAG) 
                {
                    PC.B.B0=gbReadMemory(SP.W++);
                    PC.B.B1=gbReadMemory(SP.W++);
                    clockTicks += 4;
                }
                break;
                case 0xd9:
                // RETI
                PC.B.B0=gbReadMemory(SP.W++);
                PC.B.B1=gbReadMemory(SP.W++);
                IFF |= 0x01;
                break;
                case 0xda:
                // JP C,NNNN
                if(AF.B.B0&C_FLAG) 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W);
                    PC.W=tempRegister.W;
                    clockTicks++;
                }
                else
                    PC.W+=2;
                break;
                // DB illegal
                case 0xdc:
                // CALL C,NNNN
                if(AF.B.B0&C_FLAG) 
                {
                    tempRegister.B.B0=gbReadMemory(PC.W++);
                    tempRegister.B.B1=gbReadMemory(PC.W++);
                    gbWriteMemory(--SP.W,PC.B.B1);
                    gbWriteMemory(--SP.W,PC.B.B0);
                    PC.W=tempRegister.W;
                    clockTicks += 3;
                }
                else
                    PC.W+=2;
                break;
                // DD illegal
                case 0xde:
                // SBC NN
                tempValue=gbReadOpcode(PC.W++);
                tempRegister.W=AF.B.B1-tempValue-(AF.B.B0&C_FLAG ? 1 : 0);
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                AF.B.B1=tempRegister.B.B0;
                break;
                case 0xdf:
                // RST 18
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0018;
                break;
                case 0xe0:
                // LD (FF00+NN),A
                gbWriteMemory(0xff00 + gbReadOpcode(PC.W++),AF.B.B1);
                break;
                case 0xe1:
                // POP HL
                HL.B.B0=gbReadMemory(SP.W++);
                HL.B.B1=gbReadMemory(SP.W++);
                break;
                case 0xe2:
                // LD (FF00+C),A
                gbWriteMemory(0xff00 + BC.B.B0,AF.B.B1);
                break;
                // E3 illegal
                // E4 illegal
                case 0xe5:
                // PUSH HL
                gbWriteMemory(--SP.W,HL.B.B1);
                gbWriteMemory(--SP.W,HL.B.B0);
                break;
                case 0xe6:
                // AND NN
                tempValue=gbReadOpcode(PC.W++);
                AF.B.B1&=tempValue;
                AF.B.B0=H_FLAG|ZeroTable[AF.B.B1];
                break;
                case 0xe7:
                // RST 20
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0020;
                break;
                case 0xe8:
                // ADD SP,NN
                offset = (s8)gbReadOpcode(PC.W++);
                if(offset >= 0) 
                {
                    tempRegister.W = SP.W + offset;
                    AF.B.B0 = (SP.W > tempRegister.W ? C_FLAG : 0) |
                    ((SP.W^offset^tempRegister.W)&0x1000? H_FLAG:0);
                    SP.W = tempRegister.W;
                }
                else 
                {
                    tempRegister.W = SP.W + offset;
                    AF.B.B0 = (SP.W < tempRegister.W ? C_FLAG : 0) |
                    ((SP.W^offset^tempRegister.W)&0x1000?H_FLAG:0);
                    SP.W = tempRegister.W;
                }
                break;
                case 0xe9:
                // LD PC,HL
                PC.W=HL.W;
                break;
                case 0xea:
                // LD (NNNN),A
                tempRegister.B.B0=gbReadOpcode(PC.W++);
                tempRegister.B.B1=gbReadOpcode(PC.W++);
                gbWriteMemory(tempRegister.W,AF.B.B1);
                break;
                // EB illegal
                // EC illegal
                // ED illegal
                case 0xee:
                // XOR NN
                tempValue=gbReadOpcode(PC.W++);
                AF.B.B1^=tempValue;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xef:
                // RST 28
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0028;
                break;
                case 0xf0:
                // LD A,(FF00+NN)
                AF.B.B1 = gbReadMemory(0xff00+gbReadOpcode(PC.W++));
                break;
                case 0xf1:
                // POP AF
                AF.B.B0=gbReadMemory(SP.W++);
                AF.B.B1=gbReadMemory(SP.W++);
                break;
                case 0xf2:
                // LD A,(FF00+C)
                AF.B.B1 = gbReadMemory(0xff00+BC.B.B0);
                break;
                case 0xf3:
                // DI
                //   IFF&=0xFE;
                IFF&=(~0x21);
                break;
                // F4 illegal
                case 0xf5:
                // PUSH AF
                gbWriteMemory(--SP.W,AF.B.B1);
                gbWriteMemory(--SP.W,AF.B.B0);
                break;
                case 0xf6:
                // OR NN
                tempValue=gbReadOpcode(PC.W++);
                AF.B.B1|=tempValue;
                AF.B.B0=ZeroTable[AF.B.B1];
                break;
                case 0xf7:
                // RST 30
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0030;
                break;
                case 0xf8:
                // LD HL,SP+NN
                offset = (s8)gbReadOpcode(PC.W++);
                if(offset >= 0) 
                {
                    tempRegister.W = SP.W + offset;
                    AF.B.B0 = (SP.W > tempRegister.W ? C_FLAG : 0) |
                    ((SP.W^offset^tempRegister.W)&0x1000? H_FLAG:0);
                    HL.W = tempRegister.W;
                }
                else 
                {
                    tempRegister.W = SP.W + offset;
                    AF.B.B0 = (SP.W < tempRegister.W ? C_FLAG : 0) |
                    ((SP.W^offset^tempRegister.W)&0x1000?H_FLAG:0);
                    HL.W = tempRegister.W;
                }
                break;
                case 0xf9:
                // LD SP,HL
                SP.W=HL.W;
                break;
                case 0xfa:
                // LD A,(NNNN)
                tempRegister.B.B0=gbReadOpcode(PC.W++);
                tempRegister.B.B1=gbReadOpcode(PC.W++);
                AF.B.B1=gbReadMemory(tempRegister.W);
                break;
                case 0xfb:
                // EI
                IFF|=0x20;
                break;
                // FC illegal
                // FD illegal
                case 0xfe:
                // CP NN
                tempValue=gbReadOpcode(PC.W++);
                tempRegister.W=AF.B.B1-tempValue;
                AF.B.B0= N_FLAG|(tempRegister.B.B1?C_FLAG:0)|ZeroTable[tempRegister.B.B0]|
                ((AF.B.B1^tempValue^tempRegister.B.B0)&0x10?H_FLAG:0);
                break;
                case 0xff:
                // RST 38
                gbWriteMemory(--SP.W,PC.B.B1);
                gbWriteMemory(--SP.W,PC.B.B0);
                PC.W=0x0038;
                break;
                default:
                LOG("Unknown opcode %02x at %04x", gbReadOpcode(PC.W-1),PC.W-1);
                emulating = false;
                return;
            }
        }
        if(!emulating)
            return;
        if(gbDmaTicks) 
        {
            clockTicks += gbDmaTicks;
            gbDmaTicks = 0;
        }
        if(gbSgbMode) 
        {
            if(gbSgbPacketTimeout) 
            {
                gbSgbPacketTimeout -= clockTicks;
                if(gbSgbPacketTimeout <= 0)
                    gbSgbResetPacketState();
            }
        }
        ticksToStop -= clockTicks;
        // DIV register emulation
        gbDivTicks -= clockTicks;
        while(gbDivTicks <= 0) 
        {
            register_DIV++;
            gbDivTicks += GBDIV_CLOCK_TICKS;
        }
        if(register_LCDC & 0x80) 
        {
            // LCD stuff
            gbLcdTicks -= clockTicks;
            if(gbLcdMode == 1) 
            {
                // during V-BLANK,we need to increment LY at the same rate!
                gbLcdLYIncrementTicks -= clockTicks;
                while(gbLcdLYIncrementTicks <= 0) 
                {
                    gbLcdLYIncrementTicks += GBLY_INCREMENT_CLOCK_TICKS;
                    if(register_LY < 153) 
                    {
                        register_LY++;
                        gbCompareLYToLYC();
                        if(register_LY >= 153)
                            gbLcdLYIncrementTicks = 6;
                    }
                    else 
                    {
                        register_LY = 0x00;
                        // reset the window line
                        gbWindowLine = -1;
                        gbLcdLYIncrementTicks = GBLY_INCREMENT_CLOCK_TICKS * 2;
                        gbCompareLYToLYC();
                    }
                }
            }
            // our counter is off, see what we need to do
            while(gbLcdTicks <= 0) 
            {
                int framesToSkip = systemFrameSkip;
                if(speedup)
                    framesToSkip = 9; // try 6 FPS during speedup
                switch(gbLcdMode) 
                {
                    case 0:
                    // H-Blank
                    register_LY++;
                    gbCompareLYToLYC();
                    // check if we reached the V-Blank period       
                    if(register_LY == 144) 
                    {
                        // Yes, V-Blank
                        // set the LY increment counter
                        gbLcdLYIncrementTicks = gbLcdTicks + GBLY_INCREMENT_CLOCK_TICKS;
                        gbLcdTicks += GBLCD_MODE_1_CLOCK_TICKS;
                        gbLcdMode = 1;
                        if(register_LCDC & 0x80) 
                        {
                            gbInterrupt |= 1; // V-Blank interrupt
                            gbInterruptWait = 6;
                            if(register_STAT & 0x10)
                                gbInterrupt |= 2;
                        }
                        gbFrameCount++;
                        systemFrame();
                        if((gbFrameCount % 10) == 0)
                            system10Frames(60);
                        if(gbFrameCount >= 60) 
                        {
                            u32 currentTime = systemGetClock();
                            if(currentTime != gbLastTime)
                                systemShowSpeed(100000/(currentTime - gbLastTime));
                            else
                                systemShowSpeed(0);
                            gbLastTime = currentTime;
                            gbFrameCount = 0;       
                        }
                        if(systemReadJoypads()) 
                        {
                            // read joystick
                            if(gbSgbMode && gbSgbMultiplayer) 
                            {
                                if(gbSgbFourPlayers) 
                                {
                                    gbJoymask[0] = systemReadJoypad(0);
                                    gbJoymask[1] = systemReadJoypad(1);
                                    gbJoymask[2] = systemReadJoypad(2);
                                    gbJoymask[3] = systemReadJoypad(3);
                                }
                                else 
                                {
                                    gbJoymask[0] = systemReadJoypad(0);
                                    gbJoymask[1] = systemReadJoypad(1);
                                }
                            }
                            else 
                            {
                                gbJoymask[0] = systemReadJoypad(-1);
                            }
                        }
                        int newmask = gbJoymask[0] & 255;
                        if(gbRom[0x147] == 0x22) 
                        {
                            systemUpdateMotionSensor();
                        }
                        if(newmask) 
                        {
                            gbInterrupt |= 16;
                        }
                        newmask = (gbJoymask[0] >> 10);
                        speedup = (newmask & 1) ? true : false;
                        gbCapture = (newmask & 2) ? true : false;
                        if(gbCapture && !gbCapturePrevious) 
                        {
                            gbCaptureNumber++;
                            systemScreenCapture(gbCaptureNumber);
                        }
                        gbCapturePrevious = gbCapture;
                        if(gbFrameSkipCount >= framesToSkip) 
                        {
                            systemDrawScreen();
                            gbFrameSkipCount = 0;
                        }
                        else
                            gbFrameSkipCount++;
                    }
                    else 
                    {
                        // go the the OAM being accessed mode
                        gbLcdTicks += GBLCD_MODE_2_CLOCK_TICKS;
                        gbLcdMode = 2;
                        // only one LCD interrupt per line. may need to generalize...
                        if(!(register_STAT & 0x40) ||
                        (register_LY != register_LYC)) 
                        {
                            if((register_STAT & 0x28) == 0x20)
                                gbInterrupt |= 2;
                        }
                    }
                    break;
                    case 1:
                    // V-Blank
                    // next mode is OAM being accessed mode
                    gbLcdTicks += GBLCD_MODE_2_CLOCK_TICKS;
                    gbLcdMode = 2;
                    if(!(register_STAT & 0x40) ||
                    (register_LY != register_LYC)) 
                    {
                        if((register_STAT & 0x28) == 0x20)
                            gbInterrupt |= 2;
                    }
                    break;
                    case 2:
                    // OAM being accessed mode
                    // next mode is OAM and VRAM in use
                    gbLcdTicks += GBLCD_MODE_3_CLOCK_TICKS;
                    gbLcdMode = 3;
                    break;
                    case 3:
                    // OAM and VRAM in use
                    // next mode is H-Blank
                    if(register_LY < 144) 
                    {
                        if(!gbSgbMask) 
                        {
                            if(gbFrameSkipCount >= framesToSkip) 
                            {
                                gbRenderLine();
                                gbDrawSprites();
                                switch(systemColorDepth) 
                                {
                                    case 16:
                                    {
                                        u16 * dest = (u16 *)pix +
                                        (gbBorderLineSkip+2) * (register_LY + gbBorderRowSkip+1)
                                        + gbBorderColumnSkip;
                                        for(int x = 0; x < 160; ) 
                                        {
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                            *dest++ = systemColorMap16[gbLineMix[x++]];
                                        }
                                        if(gbBorderOn)
                                            dest += gbBorderColumnSkip;
                                        *dest++ = 0; // for filters that read one pixel more
                                    }
                                    break;
                                    case 24:
                                    {
                                        u8 *dest = (u8 *)pix +
                                        3*(gbBorderLineSkip * (register_LY + gbBorderRowSkip) +
                                        gbBorderColumnSkip);
                                        for(int x = 0; x < 160;) 
                                        {
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                            *((u32 *)dest) = systemColorMap32[gbLineMix[x++]];
                                            dest+= 3;
                                        }
                                    }
                                    break;
                                    case 32:
                                    {
                                        u32 * dest = (u32 *)pix +
                                        (gbBorderLineSkip+1) * (register_LY + gbBorderRowSkip+1)
                                        + gbBorderColumnSkip;
                                        for(int x = 0; x < 160;) 
                                        {
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                            *dest++ = systemColorMap32[gbLineMix[x++]];
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    gbLcdTicks += GBLCD_MODE_0_CLOCK_TICKS;
                    gbLcdMode = 0;
                    // only one LCD interrupt per line. may need to generalize...
                    if(!(register_STAT & 0x40) ||
                    (register_LY != register_LYC)) 
                    {
                        if(register_STAT & 0x08)
                            gbInterrupt |= 2;
                    }
                    if(gbHdmaOn) 
                    {
                        gbDoHdma();
                    }
                    break;
                }
                // mark the correct lcd mode on STAT register
                register_STAT = (register_STAT & 0xfc) | gbLcdMode;
            }
        }
        // timer emulation
        if(gbTimerOn) 
        {
            gbTimerTicks -= clockTicks;
            while(gbTimerTicks <= 0) 
            {
                register_TIMA++;
                if(register_TIMA == 0) 
                {
                    // timer overflow!
                    // reload timer modulo
                    register_TIMA = register_TMA;
                    // flag interrupt
                    gbInterrupt |= 4;
                }
                gbTimerTicks += gbTimerClockTicks;
            }
        }
        soundTicks -= clockTicks;
        while(soundTicks < 0) 
        {
            soundTicks += SOUND_CLOCK_TICKS;
            gbSoundTick();
        }
        register_IF = gbInterrupt;
        if(IFF & 0x20) 
        {
            IFF &= 0xdf;
            IFF |= 0x01;
            gbInterruptWait = 0;
        }
        else if(gbInterrupt) 
        {
            if(gbInterruptWait == 0) 
            {
                //        gbInterruptWait = 0;
                if(IFF & 0x01) 
                {
                    if((gbInterrupt & 1) && (register_IE & 1)) 
                    {
                        gbVblank_interrupt();
                        continue;
                    }
                    if((gbInterrupt & 2) && (register_IE & 2)) 
                    {
                        gbLcd_interrupt();
                        continue;
                    }
                    if((gbInterrupt & 4) && (register_IE & 4)) 
                    {
                        gbTimer_interrupt();
                        continue;
                    }
                    if((gbInterrupt & 8) && (register_IE & 8)) 
                    {
                        gbSerial_interrupt();
                        continue;
                    }
                    if((gbInterrupt & 16) && (register_IE & 16)) 
                    {
                        gbJoypad_interrupt();
                        continue;
                    }
                }
            }
            else 
            {
                gbInterruptWait -= clockTicks;
                if(gbInterruptWait < 0)
                    gbInterruptWait = 0;
            }
        }
        if(ticksToStop <= 0) 
        {
            if(!(register_LCDC & 0x80)) 
            {
                if(systemReadJoypads()) 
                {
                    // read joystick
                    if(gbSgbMode && gbSgbMultiplayer) 
                    {
                        if(gbSgbFourPlayers) 
                        {
                            gbJoymask[0] = systemReadJoypad(0);
                            gbJoymask[1] = systemReadJoypad(1);
                            gbJoymask[2] = systemReadJoypad(2);
                            gbJoymask[3] = systemReadJoypad(3);
                        }
                        else 
                        {
                            gbJoymask[0] = systemReadJoypad(0);
                            gbJoymask[1] = systemReadJoypad(1);
                        }
                    }
                    else 
                    {
                        gbJoymask[0] = systemReadJoypad(-1);
                    }
                }
            }
            return;
        }
    }
}
void CGbEmu::systemFrame()
{
}
void CGbEmu::system10Frames(int _iRate)
{
}
void CGbEmu::systemShowSpeed(int _iSpeed)
{
}
int CGbEmu::systemReadJoypads()
{
    return TRUE;
}
int CGbEmu::systemReadJoypad(int i)
{
    if(this->call_back_read_joypad)
        return this->call_back_read_joypad(param_read_joypad,i);
    return 0;
}
void CGbEmu::systemUpdateMotionSensor()
{
}
void CGbEmu::systemScreenCapture(int _iNum)
{
}
void CGbEmu::systemDrawScreen()
{
    //TODO: draw screen here
    systemRenderedFrames++;
}
void CGbEmu::gbRenderLine()
{
    u8 * bank0;
    u8 * bank1;
    if(gbCgbMode) 
    {
        bank0 = &gbVram[0x0000];
        bank1 = &gbVram[0x2000];
    }
    else 
    {
        bank0 = &gbMemory[0x8000];
        bank1 = NULL;
    }
    int tile_map = 0x1800;
    if((register_LCDC & 8) != 0)
        tile_map = 0x1c00;
    int tile_pattern = 0x0800;
    if((register_LCDC & 16) != 0)
        tile_pattern = 0x0000;
    int x = 0;
    int y = register_LY;
    if(y >= 144)
        return;
    //  int yLine = (y + gbBorderRowSkip) * gbBorderLineSkip;
    int sx = register_SCX;
    int sy = register_SCY;
    sy+=y;
    sy &= 255;
    int tx = sx >> 3;
    int ty = sy >> 3;
    int bx = 1 << (7 - (sx & 7));
    int by = sy & 7;
    int tile_map_line_y = tile_map + ty * 32;
    int tile_map_address = tile_map_line_y + tx;
    u8 attrs = 0;
    if(bank1 != NULL)
        attrs = bank1[tile_map_address];
    u8 tile = bank0[tile_map_address];
    tile_map_address++;
    if((register_LCDC & 16) == 0) 
    {
        if(tile < 128) tile += 128;
        else tile -= 128;
    }
    int tile_pattern_address = tile_pattern + tile * 16 + by*2;
    if(register_LCDC & 0x80) 
    {
        if((register_LCDC & 0x01 || gbCgbMode) && (layerSettings & 0x0100)) 
        {
            while(x < 160) 
            {
                u8 tile_a = 0;
                u8 tile_b = 0;
                if(attrs & 0x40) 
                {
                    tile_pattern_address = tile_pattern + tile * 16 + (7-by)*2;
                }
                if(attrs & 0x08) 
                {
                    tile_a = bank1[tile_pattern_address++];
                    tile_b = bank1[tile_pattern_address];
                }
                else 
                {
                    tile_a = bank0[tile_pattern_address++];
                    tile_b = bank0[tile_pattern_address];
                }
                if(attrs & 0x20) 
                {
                    tile_a = gbInvertTab[tile_a];
                    tile_b = gbInvertTab[tile_b];
                }
                while(bx > 0) 
                {
                    u8 c = (tile_a & bx) ? 1 : 0;
                    c += ((tile_b & bx) ? 2 : 0);
                    gbLineBuffer[x] = c; // mark the gbLineBuffer color
                    if(attrs & 0x80)
                        gbLineBuffer[x] |= 0x300;
                    if(gbCgbMode) 
                    {
                        c = c + (attrs & 7)*4;
                    }
                    else 
                    {
                        c = gbBgp[c];         
                        if(gbSgbMode && !gbCgbMode) 
                        {
                            int dx = x >> 3;
                            int dy = y >> 3;
                            int palette = gbSgbATF[dy * 20 + dx];
                            if(c == 0)
                                palette = 0;
                            c = c + 4*palette;
                        }
                    }
                    gbLineMix[x] = gbColorOption ? gbColorFilter[gbPalette[c]] :
                    gbPalette[c];
                    x++;
                    if(x >= 160)
                        break;
                    bx >>= 1;
                }
                tx++;
                if(tx == 32)
                    tx = 0;
                bx = 128;
                if(bank1)
                    attrs = bank1[tile_map_line_y + tx];
                tile = bank0[tile_map_line_y + tx];
                if((register_LCDC & 16) == 0) 
                {
                    if(tile < 128) tile += 128;
                    else tile -= 128;
                }
                tile_pattern_address = tile_pattern + tile * 16 + by * 2;
            }
        }
        else 
        {
            for(int i = 0; i < 160; i++) 
            {
                gbLineMix[i] = gbPalette[0];
                gbLineBuffer[i] = 0;
            }
        }
        // do the window display
        if((register_LCDC & 0x20) && (layerSettings & 0x2000)) 
        {
            int wy = register_WY;
            if(y >= wy) 
            {
                int wx = register_WX;
                wx -= 7;
                if( wx <= 159 && gbWindowLine <= 143) 
                {
                    tile_map = 0x1800;
                    if((register_LCDC & 0x40) != 0)
                        tile_map = 0x1c00;
                    if(gbWindowLine == -1) 
                    {
                        gbWindowLine = 0;
                    }
                    tx = 0;
                    ty = gbWindowLine >> 3;
                    bx = 128;
                    by = gbWindowLine & 7;
                    if(wx < 0) 
                    {
                        bx >>= (-wx);
                        wx = 0;
                    }
                    tile_map_line_y = tile_map + ty * 32;
                    tile_map_address = tile_map_line_y + tx;
                    x = wx;
                    tile = bank0[tile_map_address];
                    u8 attrs = 0;
                    if(bank1)
                        attrs = bank1[tile_map_address];
                    tile_map_address++;
                    if((register_LCDC & 16) == 0) 
                    {
                        if(tile < 128) tile += 128;
                        else tile -= 128;
                    }
                    tile_pattern_address = tile_pattern + tile * 16 + by*2;
                    while(x < 160) 
                    {
                        u8 tile_a = 0;
                        u8 tile_b = 0;
                        if(attrs & 0x40) 
                        {
                            tile_pattern_address = tile_pattern + tile * 16 + (7-by)*2;
                        }
                        if(attrs & 0x08) 
                        {
                            tile_a = bank1[tile_pattern_address++];
                            tile_b = bank1[tile_pattern_address];
                        }
                        else 
                        {
                            tile_a = bank0[tile_pattern_address++];
                            tile_b = bank0[tile_pattern_address];
                        }
                        if(attrs & 0x20) 
                        {
                            tile_a = gbInvertTab[tile_a];
                            tile_b = gbInvertTab[tile_b];
                        }
                        while(bx > 0) 
                        {
                            u8 c = (tile_a & bx) != 0 ? 1 : 0;
                            c += ((tile_b & bx) != 0 ? 2 : 0);
                            if(attrs & 0x80)
                                gbLineBuffer[x] = 0x300 + c;
                            else
                                gbLineBuffer[x] = 0x100 + c;
                            if(gbCgbMode) 
                            {
                                c = c + (attrs & 7) * 4;
                            }
                            else 
                            {
                                c = gbBgp[c];         
                                if(gbSgbMode && ! gbCgbMode) 
                                {
                                    int dx = x >> 3;
                                    int dy = y >> 3;
                                    int palette = gbSgbATF[dy * 20 + dx];
                                    if(c == 0)
                                        palette = 0;
                                    c = c + 4*palette;            
                                }
                            }
                            gbLineMix[x] = gbColorOption ? gbColorFilter[gbPalette[c]] :
                            gbPalette[c];
                            x++;
                            if(x >= 160)
                                break;
                            bx >>= 1;
                        }
                        tx++;
                        if(tx == 32)
                            tx = 0;
                        bx = 128;
                        tile = bank0[tile_map_line_y + tx];
                        if(bank1)
                            attrs = bank1[tile_map_line_y + tx];
                        if((register_LCDC & 16) == 0) 
                        {
                            if(tile < 128) tile += 128;
                            else tile -= 128;
                        }
                        tile_pattern_address = tile_pattern + tile * 16 + by * 2;
                    }
                    gbWindowLine++;
                }
            }
        }
    }
    else 
    {
        for(int i = 0; i < 160; i++) 
        {
            gbLineMix[i] = gbPalette[0];
            gbLineBuffer[i] = 0;
        }
    }
}
void CGbEmu::gbDrawSpriteTile(int tile, int x,int y,int t, int flags,int size,int spriteNumber)
{
    u8 * bank0;
    u8 * bank1;
    if(gbCgbMode) 
    {
        if(register_VBK & 1) 
        {
            bank0 = &gbVram[0x0000];
            bank1 = &gbVram[0x2000];
        }
        else 
        {
            bank0 = &gbVram[0x0000];
            bank1 = &gbVram[0x2000];
        }
    }
    else 
    {
        bank0 = &gbMemory[0x8000];
        bank1 = NULL;
    }
    int init = 0x0000;
    //  int yLine = (y+gbBorderRowSkip) * gbBorderLineSkip;
    u8 *pal = gbObp0;
    int flipx = (flags & 0x20);
    int flipy = (flags & 0x40);
    if((flags & 0x10))
        pal = gbObp1;
    if(flipy) 
    {
        t = (size ? 15 : 7) - t;
    }
    int prio =  flags & 0x80;
    int address = init + tile * 16 + 2*t;
    int a = 0;
    int b = 0;
    if(gbCgbMode && flags & 0x08) 
    {
        a = bank1[address++];
        b = bank1[address++];
    }
    else 
    {
        a = bank0[address++];
        b = bank0[address++];
    }
    for(int xx = 0; xx < 8; xx++) 
    {
        u8 mask = 1 << (7-xx);
        u8 c = 0;
        if( (a & mask))
            c++;
        if( (b & mask))
            c+=2;
        if(c==0) continue;
        int xxx = xx+x;
        if(flipx)
            xxx = (7-xx+x);
        if(xxx < 0 || xxx > 159)
            continue;
        u16 color = gbLineBuffer[xxx];
        if(prio) 
        {
            if(color < 0x200 && ((color & 0xFF) != 0))
                continue;
        }
        if(color >= 0x300 && color != 0x300)
            continue;
        else if(color >= 0x200 && color < 0x300) 
        {
            int sprite = color & 0xff;
            int spriteX = gbMemory[0xfe00 + 4 * sprite + 1] - 8;
            if(spriteX == x) 
            {
                if(sprite < spriteNumber)
                    continue;
            }
            else 
            {
                if(gbCgbMode) 
                {
                    if(sprite < spriteNumber)
                        continue;
                }
                else 
                {
                    if(spriteX < x+8)
                        continue;
                }
            }
        }
        gbLineBuffer[xxx] = 0x200 + spriteNumber;
        // make sure that sprites will work even in CGB mode
        if(gbCgbMode) 
        {
            c = c + (flags & 0x07)*4 + 32;
        }
        else 
        {
            c = pal[c];
            if(gbSgbMode && !gbCgbMode) 
            {
                int dx = xxx >> 3;
                int dy = y >> 3;
                int palette = gbSgbATF[dy * 20 + dx];
                if(c == 0)
                    palette = 0;
                c = c + 4*palette;              
            }
            else 
            {
                c += 4;
            }
        }
        gbLineMix[xxx] = gbColorOption ? gbColorFilter[gbPalette[c]] :
        gbPalette[c];
    }
}
void CGbEmu::gbDrawSprites()
{
    int x = 0;
    int y = 0;
    int count = 0;
    int size = (register_LCDC & 4);
    if(!(register_LCDC & 0x80))
        return;
    if((register_LCDC & 2) && (layerSettings & 0x1000)) 
    {
        int yc = register_LY;
        int address = 0xfe00;
        for(int i = 0; i < 40; i++) 
        {
            y = gbMemory[address++];
            x = gbMemory[address++];
            int tile = gbMemory[address++];
            if(size)
                tile &= 254;
            int flags = gbMemory[address++];
            if(x > 0 && y > 0 && x < 168 && y < 160) 
            {
                // check if sprite intersects current line
                int t = yc -y + 16;
                if(size && t >=0 && t < 16) 
                {
                    gbDrawSpriteTile(tile,x-8,yc,t,flags,size,i);
                    count++;
                }
                else if(!size && t >= 0 && t < 8) 
                {
                    gbDrawSpriteTile(tile, x-8, yc, t, flags,size,i);
                    count++;
                }
            }
            // sprite limit reached!
            if(count >= 10)
                break;
        }
    }
}
void CGbEmu::gbSoundTick()
{
}


int CGbEmu::InitColorMap()
{
    for(int i = 0; i < 24;) 
    {
        systemGbPalette[i++] = (0x1f) | (0x1f << 5) | (0x1f << 10);
        systemGbPalette[i++] = (0x15) | (0x15 << 5) | (0x15 << 10);
        systemGbPalette[i++] = (0x0c) | (0x0c << 5) | (0x0c << 10);
        systemGbPalette[i++] = 0;
    }
    
    systemRedShift = 11;
    systemGreenShift = 6;
    systemBlueShift = 0;

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

int CGbEmu::gbWriteBMPFile(char *filename)
{
    ASSERT(filename);
    
    CFile file;
    
    file.Init();
    file.OpenFile(filename,"wb+");
    return this->gbWriteBMPFile(&file);
}

int CGbEmu::gbLoadRom(char *filename)
{
    ASSERT(filename);

    CFile file;

    file.Init();
    ASSERT( file.OpenFile(filename,"rb") );

    return this->gbLoadRom(&file);
}

int CGbEmu::gbWriteBMPFile(CFileBase *file)
{
    ASSERT(file);

    if(gbBorderOn)
        return utilWriteBMPFile(file, 256, 224, pix);
    return utilWriteBMPFile(file, 160, 144, pix);
}

int CGbEmu::gbReadBatteryFile(CFileBase *file)
{
    ASSERT(file);
    
    int res = 0;

    if(gbBattery) 
    {
        int type = gbRom[0x147];
        switch(type) 
        {
        case 0x03:
            res = gbReadSaveMBC1(file);
            break;
        case 0x06:
            res = gbReadSaveMBC2(file);
            break;
        case 0x0f:
        case 0x10:
        case 0x13:
            res = gbReadSaveMBC3(file);             
            break;
        case 0x1b:
        case 0x1e:
            res = gbReadSaveMBC5(file);
            break;
        case 0x22:
            res = gbReadSaveMBC7(file);
        case 0xff:
            res = gbReadSaveMBC1(file);
            break;
        }
    }
    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    
    return OK;
}

int CGbEmu::gbReadSaveMBC1(CFileBase *file)
{
    ASSERT(file && this->gbRam);
    
    int ret;

    file->Seek(0);
    ret = file->Read(this->gbRam,this->gbRamSize);
    
    return ret == this->gbRamSize;
}


int CGbEmu::gbReadSaveMBC2(CFileBase *file)
{
    ASSERT(file && this->gbMemory);
    
    int ret;

    file->Seek(0);
    ret = file->Read(&gbMemory[0xa000],256);
    
    return ret == 256;
}

int CGbEmu::gbReadSaveMBC3(CFileBase *file)
{
    ASSERT(file && this->gbRam);
    
    int ret;
    
    file->Seek(0);
    ret = file->Read(this->gbRam,this->gbRamSize);
    
    if(ret != this->gbRamSize)
    {
        file->Seek(0);
        ret = file->Read(&gbDataMBC3.mapperSeconds,44);

        if(ret != 44)
            return ERROR;
    }

    return OK;  
}

int CGbEmu::gbReadSaveMBC5(CFileBase *file)
{
    return gbReadSaveMBC1(file);
}

int CGbEmu::gbReadSaveMBC7(CFileBase *file)
{
    return gbReadSaveMBC2(file);
}

int CGbEmu::gbWriteBatteryFile(CFileBase *file, int extendedSave)
{
    ASSERT(file);

    if(gbBattery) 
    {
        int type = gbRom[0x147];

        switch(type) {
        case 0x03:
            gbWriteSaveMBC1(file);
            break;
        case 0x06:
            gbWriteSaveMBC2(file);
            break;
        case 0x0f:
        case 0x10:
        case 0x13:
            gbWriteSaveMBC3(file, extendedSave);
            break;
        case 0x1b:
        case 0x1e:
            gbWriteSaveMBC5(file);
            break;
        case 0x22:
            gbWriteSaveMBC7(file);
            break;
        case 0xff:
            gbWriteSaveMBC1(file);
            break;
        }
    }
    return OK;
}

int CGbEmu::gbWriteSaveMBC1(CFileBase *file)
{
    ASSERT(file && gbRam);
    int ret;

    file->SetSize(0);
    ret = file->Write(this->gbRam,this->gbRamSize);

    return ret == this->gbRamSize;
}

int CGbEmu::gbWriteSaveMBC2(CFileBase *file)
{
    ASSERT(file && gbMemory);

    file->SetSize(0);
    int ret = file->Write(&gbMemory[0xa000],256);

    return ret == 256;
}

int CGbEmu::gbWriteSaveMBC3(CFileBase *file, int extendedSave)
{
    ASSERT(file && gbRam);

    file->SetSize(0);
    file->Write(this->gbRam,this->gbRamSize);

    if(extendedSave)
    {
        file->Write(&gbDataMBC3.mapperSeconds,44);
    }

    return OK;
}
int CGbEmu::gbWriteSaveMBC5(CFileBase *file)
{
    return this->gbWriteSaveMBC1(file);
}
int CGbEmu::gbWriteSaveMBC7(CFileBase *file)
{
    return this->gbWriteSaveMBC2(file);
}

int CGbEmu::gbReadBatteryFile(char *filename)
{
    CFile file;

    file.Init();
    ASSERT( file.OpenFile(filename,"rb"));  
    return this->gbReadBatteryFile(&file);
}

int CGbEmu::gbWriteBatteryFile(char *filename)
{
    CFile file;
    file.Init();
    file.OpenFile(filename,"wb+");

    return this->gbWriteBatteryFile(&file,1);
}

int CGbEmu::utilWriteBMPHeader(CFileBase *file, int w, int h)
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

int CGbEmu::utilWriteBMPBits(CFileBase *file, int w, int h, u8 *pix)
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

int CGbEmu::gbGetScreenSize(int *w, int *h)
{
    ASSERT(w && h);

    if(this->gbBorderOn)
    {
        *w = 256;
        *h = 224;
    }
    else
    {
        *w = 160;
        *h = 144;
    }

    return OK;
}

int CGbEmu::gbWriteSaveGameStruct(CFileBase *file)
{
    ASSERT(file);

    file->Write(&PC.W,sizeof(u16));
    file->Write(&SP.W,sizeof(u16));
    file->Write(&AF.W,sizeof(u16));
    file->Write(&BC.W,sizeof(u16));
    file->Write(&DE.W,sizeof(u16));
    file->Write(&HL.W,sizeof(u16));
    file->Write(&IFF,sizeof(u8));
    file->Write(&GBLCD_MODE_0_CLOCK_TICKS,sizeof(int));
    file->Write(&GBLCD_MODE_1_CLOCK_TICKS,sizeof(int));
    file->Write(&GBLCD_MODE_2_CLOCK_TICKS,sizeof(int));
    file->Write(&GBLCD_MODE_3_CLOCK_TICKS,sizeof(int));
    file->Write(&GBDIV_CLOCK_TICKS,sizeof(int));
    file->Write(&GBLY_INCREMENT_CLOCK_TICKS,sizeof(int));
    file->Write(&GBTIMER_MODE_0_CLOCK_TICKS,sizeof(int));
    file->Write(&GBTIMER_MODE_1_CLOCK_TICKS,sizeof(int));
    file->Write(&GBTIMER_MODE_2_CLOCK_TICKS,sizeof(int));
    file->Write(&GBTIMER_MODE_3_CLOCK_TICKS,sizeof(int));
    file->Write(&GBSERIAL_CLOCK_TICKS,sizeof(int));
    file->Write(&GBSYNCHRONIZE_CLOCK_TICKS,sizeof(int));
    file->Write(&gbDivTicks,sizeof(int));
    file->Write(&gbLcdMode,sizeof(int));
    file->Write(&gbLcdTicks,sizeof(int));
    file->Write(&gbLcdLYIncrementTicks,sizeof(int));
    file->Write(&gbTimerTicks,sizeof(int));
    file->Write(&gbTimerClockTicks,sizeof(int));
    file->Write(&gbSerialTicks,sizeof(int));
    file->Write(&gbSerialBits,sizeof(int));
    file->Write(&gbInterrupt,sizeof(int));
    file->Write(&gbInterruptWait,sizeof(int));
    file->Write(&gbSynchronizeTicks,sizeof(int));
    file->Write(&gbTimerOn,sizeof(int));
    file->Write(&gbTimerMode,sizeof(int));
    file->Write(&gbSerialOn,sizeof(int));
    file->Write(&gbWindowLine,sizeof(int));
    file->Write(&gbCgbMode,sizeof(int));
    file->Write(&gbVramBank,sizeof(int));
    file->Write(&gbWramBank,sizeof(int));
    file->Write(&gbHdmaSource,sizeof(int));
    file->Write(&gbHdmaDestination,sizeof(int));
    file->Write(&gbHdmaBytes,sizeof(int));
    file->Write(&gbHdmaOn,sizeof(int));
    file->Write(&gbSpeed,sizeof(int));
    file->Write(&gbSgbMode,sizeof(int));
    file->Write(&register_DIV,sizeof(u8));
    file->Write(&register_TIMA,sizeof(u8));
    file->Write(&register_TMA,sizeof(u8));
    file->Write(&register_TAC,sizeof(u8));
    file->Write(&register_IF,sizeof(u8));
    file->Write(&register_LCDC,sizeof(u8));
    file->Write(&register_STAT,sizeof(u8));
    file->Write(&register_SCY,sizeof(u8));
    file->Write(&register_SCX,sizeof(u8));
    file->Write(&register_LY,sizeof(u8));
    file->Write(&register_LYC,sizeof(u8));
    file->Write(&register_DMA,sizeof(u8));
    file->Write(&register_WY,sizeof(u8));
    file->Write(&register_WX,sizeof(u8));
    file->Write(&register_VBK,sizeof(u8));
    file->Write(&register_HDMA1,sizeof(u8));
    file->Write(&register_HDMA2,sizeof(u8));
    file->Write(&register_HDMA3,sizeof(u8));
    file->Write(&register_HDMA4,sizeof(u8));
    file->Write(&register_HDMA5,sizeof(u8));
    file->Write(&register_SVBK,sizeof(u8));
    file->Write(&register_IE,sizeof(u8));
    file->Write(&gbBgp[0],sizeof(u8));
    file->Write(&gbBgp[1],sizeof(u8));
    file->Write(&gbBgp[2],sizeof(u8));
    file->Write(&gbBgp[3],sizeof(u8));
    file->Write(&gbObp0[0],sizeof(u8));
    file->Write(&gbObp0[1],sizeof(u8));
    file->Write(&gbObp0[2],sizeof(u8));
    file->Write(&gbObp0[3],sizeof(u8));
    file->Write(&gbObp1[0],sizeof(u8));
    file->Write(&gbObp1[1],sizeof(u8));
    file->Write(&gbObp1[2],sizeof(u8));
    file->Write(&gbObp1[3],sizeof(u8));

    return OK;
}

int CGbEmu::gbWriteSaveState(CFileBase *file)
{
    ASSERT(file);

    int t;
    t = 10; file->Write(&t,sizeof(int)); //save version
    file->Write(&gbRom[0x134],15);
    this->gbWriteSaveGameStruct(file);
    file->Write(&IFF,2);
    
    if(this->gbSgbMode)
    {
        this->gbSgbSaveGame(file);
    }
    file->Write(&gbDataMBC1, sizeof(gbDataMBC1));
    file->Write(&gbDataMBC2, sizeof(gbDataMBC2));
    file->Write(&gbDataMBC3, sizeof(gbDataMBC3));
    file->Write(&gbDataMBC5, sizeof(gbDataMBC5));
    file->Write(&gbDataHuC1, sizeof(gbDataHuC1));
    file->Write(&gbDataHuC3, sizeof(gbDataHuC3));

    file->Write(gbPalette,128*sizeof(u16));
    file->Write(&gbMemory[0x8000], 0x8000);

    if(gbRamSize && gbRam)
    {
        file->Write(gbRam,gbRamSize);
    }

    if(gbCgbMode)
    {
        file->Write(gbVram,0x4000);
        file->Write(gbWram,0x8000);
    }

    return OK;
}

int CGbEmu::gbSgbSaveGame(CFileBase *file)
{
    ASSERT(file);

    this->gbWriteSgbSaveStructV3(file);
    file->Write(gbSgbBorder,2048);
    file->Write(gbSgbBorderChar,32*256);
    file->Write(gbSgbPacket,16*7);
    file->Write(gbSgbSCPPalette,4*512*sizeof(u16));
    file->Write(gbSgbATF,20*18);
    file->Write(gbSgbATFList,45*20*18);

    return OK;
}

int CGbEmu::gbWriteSgbSaveStructV3(CFileBase *file)
{
    file->Write(&gbSgbMask,sizeof(int));
    file->Write(&gbSgbPacketState,sizeof(int));
    file->Write(&gbSgbBit,sizeof(int));
    file->Write(&gbSgbPacketNBits,sizeof(int));
    file->Write(&gbSgbPacketByte,sizeof(int));
    file->Write(&gbSgbPacketNumber,sizeof(int));
    file->Write(&gbSgbMultiplayer,sizeof(int));
    file->Write(&gbSgbNextController,sizeof(u8));
    file->Write(&gbSgbReadingController,sizeof(u8));
    file->Write(&gbSgbFourPlayers,sizeof(int));
    
    return OK;
}

int CGbEmu::gbReadSaveState(CFileBase *file)
{
    ASSERT(file);
    int t;
    u8 romname[20];

    file->Read(&t,4);
    ASSERT(t == 10);    //version
    
    file->Read(romname,15);

    if(memcmp(&gbRom[0x134],romname,15) != 0)
    {
        LOG("not the same game, load state fail\n");
        return ERROR;
    }

    this->gbReadSaveGameStruct(file);
    file->Read(&IFF,2);

    if(this->gbSgbMode)
        this->gbSgbReadGame(file);
    else
        gbSgbMask = 0;

    file->Read( &gbDataMBC1, sizeof(gbDataMBC1));
    file->Read( &gbDataMBC2, sizeof(gbDataMBC2));
    
    file->Read( &gbDataMBC3, sizeof(gbDataMBC3));
    file->Read( &gbDataMBC5, sizeof(gbDataMBC5));
    file->Read( &gbDataHuC1, sizeof(gbDataHuC1));
    file->Read( &gbDataHuC3, sizeof(gbDataHuC3));

    memset(pix, 0, 257*226*sizeof(u32));
    
    file->Read( gbPalette, 128 * sizeof(u16));
    file->Read( &gbMemory[0x8000], 0x8000);

    if(gbRamSize && gbRam) 
    {
        file->Read( gbRam, gbRamSize);
    }

    gbMemoryMap[0x00] = &gbRom[0x0000];
    gbMemoryMap[0x01] = &gbRom[0x1000];
    gbMemoryMap[0x02] = &gbRom[0x2000];
    gbMemoryMap[0x03] = &gbRom[0x3000];
    gbMemoryMap[0x04] = &gbRom[0x4000];
    gbMemoryMap[0x05] = &gbRom[0x5000];
    gbMemoryMap[0x06] = &gbRom[0x6000];
    gbMemoryMap[0x07] = &gbRom[0x7000];
    gbMemoryMap[0x08] = &gbMemory[0x8000];
    gbMemoryMap[0x09] = &gbMemory[0x9000];
    gbMemoryMap[0x0a] = &gbMemory[0xa000];
    gbMemoryMap[0x0b] = &gbMemory[0xb000];
    gbMemoryMap[0x0c] = &gbMemory[0xc000];
    gbMemoryMap[0x0d] = &gbMemory[0xd000];
    gbMemoryMap[0x0e] = &gbMemory[0xe000];
    gbMemoryMap[0x0f] = &gbMemory[0xf000];    

    int type = gbRom[0x147];
    switch(type) 
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
        // MBC 1
        memoryUpdateMapMBC1();
        break;
    case 0x05:
    case 0x06:
        // MBC2
        memoryUpdateMapMBC2();
        break;
    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
        // MBC 3
        memoryUpdateMapMBC3();
        break;
    case 0x19:
    case 0x1a:
    case 0x1b:
        // MBC5
        memoryUpdateMapMBC5();
        break;
    case 0x1c:
    case 0x1d:
    case 0x1e:
        // MBC 5 Rumble
        memoryUpdateMapMBC5();
        break;
    case 0x22:
        // MBC 7
        memoryUpdateMapMBC7();
        break;
    case 0xfe:
        // HuC3
        memoryUpdateMapHuC3();
        break;
    case 0xff:
        // HuC1
        memoryUpdateMapHuC1();
        break;
    }
    
    if(gbCgbMode) 
    {
        file->Read( gbVram, 0x4000);
        file->Read( gbWram, 0x8000);
        int value = register_SVBK;
        if(value == 0)
            value = 1;
        gbMemoryMap[0x08] = &gbVram[register_VBK * 0x2000];
        gbMemoryMap[0x09] = &gbVram[register_VBK * 0x2000 + 0x1000];
        gbMemoryMap[0x0d] = &gbWram[value * 0x1000];
    }

    if(gbBorderOn) 
        gbSgbRenderBorder();

    systemDrawScreen();

    systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

    return OK;
}

int CGbEmu::gbReadSaveGameStruct(CFileBase *file)
{
    ASSERT(file);

    file->Read(&PC.W,sizeof(u16));
    file->Read(&SP.W,sizeof(u16));
    file->Read(&AF.W,sizeof(u16));
    file->Read(&BC.W,sizeof(u16));
    file->Read(&DE.W,sizeof(u16));
    file->Read(&HL.W,sizeof(u16));
    file->Read(&IFF,sizeof(u8));
    file->Read(&GBLCD_MODE_0_CLOCK_TICKS,sizeof(int));
    file->Read(&GBLCD_MODE_1_CLOCK_TICKS,sizeof(int));
    file->Read(&GBLCD_MODE_2_CLOCK_TICKS,sizeof(int));
    file->Read(&GBLCD_MODE_3_CLOCK_TICKS,sizeof(int));
    file->Read(&GBDIV_CLOCK_TICKS,sizeof(int));
    file->Read(&GBLY_INCREMENT_CLOCK_TICKS,sizeof(int));
    file->Read(&GBTIMER_MODE_0_CLOCK_TICKS,sizeof(int));
    file->Read(&GBTIMER_MODE_1_CLOCK_TICKS,sizeof(int));
    file->Read(&GBTIMER_MODE_2_CLOCK_TICKS,sizeof(int));
    file->Read(&GBTIMER_MODE_3_CLOCK_TICKS,sizeof(int));
    file->Read(&GBSERIAL_CLOCK_TICKS,sizeof(int));
    file->Read(&GBSYNCHRONIZE_CLOCK_TICKS,sizeof(int));
    file->Read(&gbDivTicks,sizeof(int));
    file->Read(&gbLcdMode,sizeof(int));
    file->Read(&gbLcdTicks,sizeof(int));
    file->Read(&gbLcdLYIncrementTicks,sizeof(int));
    file->Read(&gbTimerTicks,sizeof(int));
    file->Read(&gbTimerClockTicks,sizeof(int));
    file->Read(&gbSerialTicks,sizeof(int));
    file->Read(&gbSerialBits,sizeof(int));
    file->Read(&gbInterrupt,sizeof(int));
    file->Read(&gbInterruptWait,sizeof(int));
    file->Read(&gbSynchronizeTicks,sizeof(int));
    file->Read(&gbTimerOn,sizeof(int));
    file->Read(&gbTimerMode,sizeof(int));
    file->Read(&gbSerialOn,sizeof(int));
    file->Read(&gbWindowLine,sizeof(int));
    file->Read(&gbCgbMode,sizeof(int));
    file->Read(&gbVramBank,sizeof(int));
    file->Read(&gbWramBank,sizeof(int));
    file->Read(&gbHdmaSource,sizeof(int));
    file->Read(&gbHdmaDestination,sizeof(int));
    file->Read(&gbHdmaBytes,sizeof(int));
    file->Read(&gbHdmaOn,sizeof(int));
    file->Read(&gbSpeed,sizeof(int));
    file->Read(&gbSgbMode,sizeof(int));
    file->Read(&register_DIV,sizeof(u8));
    file->Read(&register_TIMA,sizeof(u8));
    file->Read(&register_TMA,sizeof(u8));
    file->Read(&register_TAC,sizeof(u8));
    file->Read(&register_IF,sizeof(u8));
    file->Read(&register_LCDC,sizeof(u8));
    file->Read(&register_STAT,sizeof(u8));
    file->Read(&register_SCY,sizeof(u8));
    file->Read(&register_SCX,sizeof(u8));
    file->Read(&register_LY,sizeof(u8));
    file->Read(&register_LYC,sizeof(u8));
    file->Read(&register_DMA,sizeof(u8));
    file->Read(&register_WY,sizeof(u8));
    file->Read(&register_WX,sizeof(u8));
    file->Read(&register_VBK,sizeof(u8));
    file->Read(&register_HDMA1,sizeof(u8));
    file->Read(&register_HDMA2,sizeof(u8));
    file->Read(&register_HDMA3,sizeof(u8));
    file->Read(&register_HDMA4,sizeof(u8));
    file->Read(&register_HDMA5,sizeof(u8));
    file->Read(&register_SVBK,sizeof(u8));
    file->Read(&register_IE,sizeof(u8));
    file->Read(&gbBgp[0],sizeof(u8));
    file->Read(&gbBgp[1],sizeof(u8));
    file->Read(&gbBgp[2],sizeof(u8));
    file->Read(&gbBgp[3],sizeof(u8));
    file->Read(&gbObp0[0],sizeof(u8));
    file->Read(&gbObp0[1],sizeof(u8));
    file->Read(&gbObp0[2],sizeof(u8));
    file->Read(&gbObp0[3],sizeof(u8));
    file->Read(&gbObp1[0],sizeof(u8));
    file->Read(&gbObp1[1],sizeof(u8));
    file->Read(&gbObp1[2],sizeof(u8));
    file->Read(&gbObp1[3],sizeof(u8));
    
    return OK;
}

int CGbEmu::gbSgbReadGame(CFileBase *file)
{
    ASSERT(file);
    this->gbReadSgbSaveStructV3(file);
    
    file->Read(gbSgbBorder,2048);
    file->Read(gbSgbBorderChar,32*256);
    file->Read(gbSgbPacket,16*7);
    file->Read(gbSgbSCPPalette,4*512*sizeof(u16));
    file->Read(gbSgbATF,20*18);
    file->Read(gbSgbATFList,45*20*18);

    return OK;
}

int CGbEmu::gbReadSgbSaveStructV3(CFileBase *file)
{
    ASSERT(file);

    file->Read(&gbSgbMask,sizeof(int));
    file->Read(&gbSgbPacketState,sizeof(int));
    file->Read(&gbSgbBit,sizeof(int));
    file->Read(&gbSgbPacketNBits,sizeof(int));
    file->Read(&gbSgbPacketByte,sizeof(int));
    file->Read(&gbSgbPacketNumber,sizeof(int));
    file->Read(&gbSgbMultiplayer,sizeof(int));
    file->Read(&gbSgbNextController,sizeof(u8));
    file->Read(&gbSgbReadingController,sizeof(u8));
    file->Read(&gbSgbFourPlayers,sizeof(int));
    
    return OK;
}
