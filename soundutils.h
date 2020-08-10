#ifndef __SOUNDUTILS_H
#define __SOUNDUTILS_H

#include "common.h"
#include "filebase.h"

#if !HAVE_WINDOWS_H

typedef struct tWAVEFORMATEX {
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;
    uint32_t nDataOffset;
} WAVEFORMATEX;

#endif

class CSoundUtils{
public:
    static int32_t GetWaveDataOffset();
    static status_t ParseWavFile(CFileBase *file, WAVEFORMATEX *wfx);    
    static double GetWavDuration(CFileBase *file);
    static bool IsWavFormat(CFileBase *file);
};

#endif

