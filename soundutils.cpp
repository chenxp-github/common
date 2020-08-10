#include "soundutils.h"
#include "syslog.h"

#define WAVE_FORMAT_PCM         1

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

status_t CSoundUtils::ParseWavFile(CFileBase *file, WAVEFORMATEX *wfx)
{
    ASSERT(file && wfx);
    
    char buf[100];
    int wave_size = 0;
    const char *key;

    ASSERT(IsWavFormat(file));
    
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

int32_t CSoundUtils::GetWaveDataOffset()
{
    return OFFSET_WAVEDATA;
}

double CSoundUtils::GetWavDuration(CFileBase *file)
{
    ASSERT(file);
    WAVEFORMATEX wfx;
    fsize_t old_off = file->GetOffset();
    if(!CSoundUtils::ParseWavFile(file,&wfx))
        return 0.0;
    double t = (double)(wfx.nSamplesPerSec*wfx.nChannels*(wfx.wBitsPerSample/8.0));
    ASSERT(t > 0);
    file->Seek(old_off);
    return ((double)file->GetSize())/t;
}

bool CSoundUtils::IsWavFormat(CFileBase *file)
{
    ASSERT(file);
    
    char buf[100];
    const char *key;
    
    key = "RIFF";
    file->Seek(0);
    file->Read(buf,strlen(key)); 
    buf[strlen(key)] = 0;
    if(strcmp(buf,key) != 0)
        return false;
    
    key="WAVEfmt";
    file->Seek(OFFSET_HEAD_MARK);
    file->Read(buf,strlen(key));
    buf[strlen(key)] = 0;
    if(strcmp(buf,key) != 0)
        return false;

    return true;
}
