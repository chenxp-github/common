// FFMPEG_Player.h: interface for the CFFMPEG_Player class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FFMPEG_PLAYER_H__B5CE5B2B_3B22_4AE8_A4FF_5C0DB7413365__INCLUDED_)
#define AFX_FFMPEG_PLAYER_H__B5CE5B2B_3B22_4AE8_A4FF_5C0DB7413365__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common.h"
#include "mem.h"

extern "C" 
{
#include "ffmpeg_log_u.h"
#include "ffmpeg_pixdesc_u.h"
#include "ffmpeg_avformat_f.h"
#include "ffmpeg_avdevice.h"
#include "ffmpeg_swscale_s.h"
#include "ffmpeg_audioconvert_c.h"
#include "ffmpeg_colorspace_c.h"
#include "ffmpeg_avfft_c.h"
#include "ffmpeg_avfilter_l.h"
#include "ffmpeg_avfiltergraph_l.h"
#include "ffmpeg_graphparser_l.h"
}


class CFFMPEG_Player{
public:
    AVFormatContext *pFormatCtx;
    AVCodecContext *pVideoCodecCtx;
    AVCodecContext *pAudioCodecCtx;
    AVCodec *pVideoCodec;
    AVCodec *pAudioCodec;
    AVFrame *pFrame;
    int video_stream;
    int audio_stream;
public:
    int FrameToBmp(CMem *mem_bmp,int bmp_w,int bmp_h);
    int GetPacketPlayTime(AVPacket *packet);
    AVStream * GetAudioStream();
    AVStream * GetVideoStream();
    int AudioDecodeFrame(CMem *mem,AVPacket *packet);
    int IsAudioPacket(AVPacket *packet);
    int FrameToBmp(CMem *mem_bmp);
    int VideoDecodeFrame(int *frame_finished,AVPacket *packet);
    int IsVideoPacket(AVPacket *packet);
    int ReadFrame(AVPacket *packet);
    PixelFormat GetVideoPixFmt();
    int GetVideoHeight();
    int GetVideoWidth();
    int OpenInputStream(const char *url);
    static int Init_FFMPEG();
    CFFMPEG_Player();
    virtual ~CFFMPEG_Player();
    int Init();
    int Destroy();
    int InitBasic();
};
#endif // !defined(AFX_FFMPEG_PLAYER_H__B5CE5B2B_3B22_4AE8_A4FF_5C0DB7413365__INCLUDED_)
