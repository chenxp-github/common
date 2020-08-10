#ifndef __GB_COMMON_H
#define __GB_COMMON_H

#include "common.h"
#include "memfile.h"
#include "mem.h"

#define HAVE_GB_EMU         1

typedef DWORD   u32;
typedef WORD    u16;
typedef BYTE    u8;

typedef char    s8;
typedef short   s16;
typedef int     s32;

#define GB_WORDS_BIGENDIAN      0
#define GBSGB_NONE              0
#define GBSGB_RESET             1
#define GBSGB_PACKET_TRANSMIT   2

#define USE_TICKS_AS  380
#define SOUND_MAGIC   0x60000000
#define SOUND_MAGIC_2 0x30000000
#define NOISE_MAGIC 5

#define SYSTEM_SAVE_UPDATED 30
#define SYSTEM_SAVE_NOT_UPDATED 0

#define C_FLAG 0x10
#define H_FLAG 0x20
#define N_FLAG 0x40
#define Z_FLAG 0x80

#endif
