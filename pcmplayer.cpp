// PcmPlayer.cpp: implementation of the CPcmPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PcmPlayer.h"

#pragma comment(lib,"winmm.lib")

#define WAVE_PCM_16             16
#define WAVE_PCM_1               1

/// wfx header offsets
#define OFFSET_FILE_LEFT         4
#define OFFSET_HEAD_MARK         8
#define OFFSET_WAVE_PCM1        16
#define OFFSET_WAVE_PCM2        20
#define OFFSET_CHANNELS         22
#define OFFSET_SAMPLESPERSEC    24
#define OFFSET_AVGBYTESPERSEC   28
#define OFFSET_BLOCKALIGN       32
#define OFFSET_BITSPERSAMPLE    34
#define OFFSET_DATA_MARK        36
#define OFFSET_DATA_SIZE        40
#define OFFSET_WAVEDATA         44
#define HEADER_SIZE             OFFSET_WAVEDATA
#define EOF_EXTRA_INFO          60

#define MAX_WAV_HEADERS         8
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPcmPlayer::CPcmPlayer()
{
    this->InitBasic();
}
CPcmPlayer::~CPcmPlayer()
{
    this->Destroy();
}
int CPcmPlayer::InitBasic()
{
    this->hwo = 0;
    this->wave_hdrs = NULL;
    this->mem_bufs = NULL;
    return OK;
}
int CPcmPlayer::Init()
{
    this->InitBasic();
    
    int i;

    NEW_ARRAY(this->wave_hdrs,WAVEHDR,MAX_WAV_HEADERS);
    NEW_ARRAY(this->mem_bufs,CMem,MAX_WAV_HEADERS);

    memset(this->wave_hdrs,0,sizeof(WAVEHDR)*MAX_WAV_HEADERS);

    for(i = 0; i < MAX_WAV_HEADERS; i++)
    {
        mem_bufs[i].Init();
        mem_bufs[i].Malloc(WAVE_BUF_SIZE);
    }

    return OK;
}
int CPcmPlayer::Destroy()
{
    int i;

    if(this->hwo)
    {
        waveOutReset(hwo);
        for(i = 0; i < MAX_WAV_HEADERS; i++)
        {
            waveOutUnprepareHeader(hwo,&this->wave_hdrs[i],sizeof(WAVEHDR));            
        }
        waveOutClose(hwo);
        this->hwo = NULL;
    }
    
    DEL_ARRAY(this->mem_bufs);
    DEL_ARRAY(this->wave_hdrs);

    this->InitBasic();
    return OK;
}

int CPcmPlayer::GetWavFileParam(CFileBase *file, WAVEFORMATEX *wfx)
{
    ASSERT(file && wfx);
    
    char buf[100];
    int wave_size = 0;
    const char *key;

    key = "RIFF";
    file->Seek(0);
    file->Read(buf,strlen(key)); 
    buf[strlen(key)] = 0;
    ASSERT(strcmp(buf,key) == 0);

    key="WAVEfmt";
    file->Seek(OFFSET_HEAD_MARK);
    file->Read(buf,strlen(key));
    buf[strlen(key)] = 0;
    ASSERT(strcmp(buf,key) == 0);
    
    DWORD dw = 0;
    file->Seek(OFFSET_WAVE_PCM1);
    file->Read(&dw,4);
    ASSERT(dw == WAVE_PCM_16);

    WORD w = 0;
    file->Seek(OFFSET_WAVE_PCM2);
    file->Read(&w,2);
    ASSERT(w == WAVE_PCM_1);

    key = "data";
    file->Seek(OFFSET_DATA_MARK);
    file->Read(buf,strlen(key));
    buf[strlen(key)] = 0;

    if(strcmp(buf,key) == 0)
    {
        file->Seek(OFFSET_DATA_SIZE);
        file->Read(&wave_size,4);
    }
    else
    {
        file->Seek(OFFSET_FILE_LEFT);
        file->Read(&wave_size,4);
        wave_size -=  ( HEADER_SIZE - EOF_EXTRA_INFO );
    }

    wfx->cbSize = 0;
    wfx->wFormatTag = WAVE_FORMAT_PCM;

    file->Seek(OFFSET_SAMPLESPERSEC);
    file->Read(&(wfx->nSamplesPerSec),sizeof(wfx->nSamplesPerSec));

    file->Seek(OFFSET_BITSPERSAMPLE);
    file->Read(&(wfx->wBitsPerSample),sizeof(wfx->wBitsPerSample));

    file->Seek(OFFSET_CHANNELS);
    file->Read(&(wfx->nChannels),sizeof(wfx->nChannels));

    file->Seek(OFFSET_BLOCKALIGN);
    file->Read(&(wfx->nBlockAlign),sizeof(wfx->nBlockAlign));

    file->Seek(OFFSET_AVGBYTESPERSEC);
    file->Read(&(wfx->nAvgBytesPerSec),sizeof(wfx->nAvgBytesPerSec));

    return OK;
}

int CPcmPlayer::OpenWaveOut(WAVEFORMATEX *wfx)
{
    ASSERT(this->hwo == NULL && wfx);

    DWORD res = waveOutOpen(&hwo,WAVE_MAPPER, wfx, (DWORD)NULL, (DWORD)this, CALLBACK_NULL);
    ASSERT (res == MMSYSERR_NOERROR );

    int i;
    for(i = 0; i < MAX_WAV_HEADERS; i++)
    {       
        this->wave_hdrs[i].dwFlags = WHDR_DONE;
    }

    return OK;
}

int CPcmPlayer::OpenWaveOut(int channels, int format, int sample_rate)
{
    WAVEFORMATEX wfx;

    wfx.cbSize = 0;
    wfx.wBitsPerSample = 16;
    wfx.nChannels = channels;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nSamplesPerSec = sample_rate;
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample/8);

    if(format == PCM_FMT_S16_LE)
        wfx.wBitsPerSample = 16;
    if(format == PCM_FMT_S8)
        wfx.wBitsPerSample = 8;

    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    return this->OpenWaveOut(&wfx);
}

int CPcmPlayer::FindAvailBuf()
{
    int i;
    for(i = 0; i < MAX_WAV_HEADERS; i++)
    {
        if(this->wave_hdrs[i].dwFlags & WHDR_DONE)
        {
            return i;
        }
    }
    return -1;
}

int CPcmPlayer::WritePcmData(void *buf, int size)
{
    ASSERT(size <= WAVE_BUF_SIZE);
    
    int i = this->FindAvailBuf();
    if(i < 0) return 0;

    memcpy(this->mem_bufs[i].p,buf,size);

    waveOutUnprepareHeader(hwo,&this->wave_hdrs[i],sizeof(WAVEHDR));
    this->wave_hdrs[i].dwFlags = 0;
    this->wave_hdrs[i].lpData = (char*)this->mem_bufs[i].p;
    this->wave_hdrs[i].dwBufferLength = size;

    waveOutPrepareHeader(hwo,&this->wave_hdrs[i],sizeof(WAVEHDR));
    waveOutWrite(hwo,&wave_hdrs[i],sizeof(WAVEHDR));

    return size;
}

int CPcmPlayer::WriteAllPcmData(void *buf, int size)
{
    
    int left_size = size;
    char *p = (char*)buf;
    int block_size,ws;

    while(left_size > 0)
    {
        if(left_size > WAVE_BUF_SIZE)
            block_size = WAVE_BUF_SIZE;
        else
            block_size = left_size;
        ws = this->WritePcmData(p,block_size);
        if(ws <= 0)
        {
            Sleep(1);
        }
        else
        {
            p += ws;
            left_size -= ws;
        }
    }

    return OK;
}
