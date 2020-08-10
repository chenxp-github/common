// PcmPlayer.h: interface for the CPcmPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCMPLAYER_H__27E61933_07FF_431B_9638_2A42991538B7__INCLUDED_)
#define AFX_PCMPLAYER_H__27E61933_07FF_431B_9638_2A42991538B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmsystem.h>

#include "common.h"
#include "mem.h"

#define PCM_FMT_S16_LE      1
#define PCM_FMT_S8          2

#define WAVE_BUF_SIZE           (16*1024)

class CPcmPlayer{
public:
    HWAVEOUT hwo;
    WAVEHDR *wave_hdrs;
    CMem *mem_bufs;
public:
    int WriteAllPcmData(void *buf,int size);
    int WritePcmData(void *buf,int size);
    int FindAvailBuf();
    int OpenWaveOut(int channels,int format,int sample_rate);
    int OpenWaveOut(WAVEFORMATEX *wfx);
    int GetWavFileParam(CFileBase *file,WAVEFORMATEX *wfx);
    CPcmPlayer();
    virtual ~CPcmPlayer();
    int Init();
    int Destroy();
    int InitBasic();
};

#endif // !defined(AFX_PCMPLAYER_H__27E61933_07FF_431B_9638_2A42991538B7__INCLUDED_)
