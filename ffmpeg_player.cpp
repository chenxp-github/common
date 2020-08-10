// FFMPEG_Player.cpp: implementation of the CFFMPEG_Player class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FFMPEG_Player.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFFMPEG_Player::CFFMPEG_Player()
{
    this->InitBasic();
}
CFFMPEG_Player::~CFFMPEG_Player()
{
    this->Destroy();
}
int CFFMPEG_Player::InitBasic()
{
    this->pFormatCtx = NULL;
    this->pVideoCodecCtx = NULL;
    this->pAudioCodecCtx = NULL;

    this->pVideoCodec = NULL;
    this->pAudioCodec = NULL;

    this->pFrame = NULL;

    this->video_stream = -1;
    this->audio_stream = -1;

    return OK;
}
int CFFMPEG_Player::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
int CFFMPEG_Player::Destroy()
{
    if(this->pVideoCodecCtx)
    {
        avcodec_close(pVideoCodecCtx);
        this->pVideoCodecCtx = NULL;
    }

    if(this->pAudioCodecCtx)
    {
        avcodec_close(pAudioCodecCtx);
        this->pAudioCodecCtx = NULL;
    }

    if(this->pFrame)
    {
        av_free(this->pFrame);
        this->pFrame = NULL;
    }

    if(this->pFormatCtx)
    {
        av_close_input_file(this->pFormatCtx);
        this->pFormatCtx = NULL;
    }

    this->InitBasic();
    
    return OK;
}

int CFFMPEG_Player::Init_FFMPEG()
{
    av_log_set_level(5);
    avcodec_register_all();
    avdevice_register_all();
    avfilter_register_all();
    av_register_all();
    
    return OK;
}

int CFFMPEG_Player::OpenInputStream(const char *url)
{
    ASSERT(url && this->pFormatCtx == NULL);

    int i;
    AVStream *pstream;

    av_open_input_file(&pFormatCtx, url, NULL, 0, NULL);
    ASSERT(pFormatCtx);

    ASSERT(av_find_stream_info(pFormatCtx) >= 0);
    
    this->video_stream = -1;
    this->audio_stream = -1;

    for(i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
            this->video_stream = i;
            break;
        }
    }

    for(i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
        {
            this->audio_stream = i;
            break;
        }
    }

    if(this->video_stream >= 0)
    {
        pstream = this->GetVideoStream();
        ASSERT(pstream);
        this->pVideoCodecCtx = pstream->codec;
        ASSERT(this->pVideoCodecCtx);
        pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
        ASSERT(pVideoCodec);
        ASSERT( avcodec_open(pVideoCodecCtx, pVideoCodec) >= 0);
    }

    if(this->audio_stream >= 0)
    {
        pstream = this->GetAudioStream();
        ASSERT(pstream);
        this->pAudioCodecCtx = pstream->codec;
        ASSERT(this->pAudioCodecCtx);
        pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
        ASSERT(pAudioCodec);
        ASSERT( avcodec_open(pAudioCodecCtx, pAudioCodec) >= 0);
    }

    pFrame=avcodec_alloc_frame();
    ASSERT(pFrame);

    return OK;
}

int CFFMPEG_Player::GetVideoWidth()
{
    ASSERT(this->pVideoCodecCtx);
    return this->pVideoCodecCtx->width;
}

int CFFMPEG_Player::GetVideoHeight()
{
    ASSERT(this->pVideoCodecCtx);
    return this->pVideoCodecCtx->height;
}

PixelFormat CFFMPEG_Player::GetVideoPixFmt()
{
    if(this->pVideoCodecCtx)
        return this->pVideoCodecCtx->pix_fmt;
    return PIX_FMT_NONE;
}

int CFFMPEG_Player::ReadFrame(AVPacket *packet)
{
    ASSERT(packet && pFormatCtx);   
    return (av_read_frame(pFormatCtx, packet) >= 0);
}

int CFFMPEG_Player::IsVideoPacket(AVPacket *packet)
{
    ASSERT(packet);
    return (packet->stream_index == this->video_stream);
}

int CFFMPEG_Player::VideoDecodeFrame(int *frame_finished, AVPacket *packet)
{
    ASSERT(frame_finished && packet);
    return avcodec_decode_video2(pVideoCodecCtx, pFrame, frame_finished, packet);   
}

int CFFMPEG_Player::FrameToBmp(CMem *mem_bmp)
{
    ASSERT(mem_bmp);
    return this->FrameToBmp(mem_bmp,this->GetVideoWidth(),this->GetVideoHeight());
}

int CFFMPEG_Player::IsAudioPacket(AVPacket *packet)
{
    ASSERT(packet);
    return (packet->stream_index == this->audio_stream);
}

int CFFMPEG_Player::AudioDecodeFrame(CMem *mem,AVPacket *packet)
{
    ASSERT(mem && packet);

    mem->SetSize(0);

    char *pbuf = mem->p;
    int audio_size;
    int ret;

    uint8_t *old_data = packet->data;
    int old_size = packet->size;
    
    while(packet->size > 0)
    {
        audio_size = mem->GetMaxSize() - mem->GetSize();
        if(audio_size <= 0)
            break;

        ASSERT(audio_size >= AVCODEC_MAX_AUDIO_FRAME_SIZE);

        ret = avcodec_decode_audio3(pAudioCodecCtx,(int16_t*)pbuf,&audio_size,packet);

        if(ret < 0) break;
        if(audio_size < 0)  continue;
    
        mem->SetSize(mem->GetSize() + audio_size);

        pbuf += audio_size;
        packet->data += audio_size;
        packet->size -= audio_size;
    }
    
    packet->size = old_size;
    packet->data = old_data;

    return OK;
}

AVStream * CFFMPEG_Player::GetVideoStream()
{
    ASSERT(this->pFormatCtx);
    ASSERT(this->video_stream >= 0);
    return this->pFormatCtx->streams[this->video_stream];
}

AVStream * CFFMPEG_Player::GetAudioStream()
{
    ASSERT(this->pFormatCtx);
    ASSERT(this->audio_stream >= 0);
    return this->pFormatCtx->streams[this->audio_stream];
}

int CFFMPEG_Player::GetPacketPlayTime(AVPacket *packet)
{
    AVStream *pstream = this->GetVideoStream();
    
    ASSERT(pstream);
    ASSERT(packet->dts !=  AV_NOPTS_VALUE);
    return (int)(packet->dts * av_q2d(pstream->time_base) * 1000);
}

int CFFMPEG_Player::FrameToBmp(CMem *mem_bmp, int bmp_w, int bmp_h)
{
    ASSERT(mem_bmp);
    
    int video_width = this->GetVideoWidth();
    int video_height = this->GetVideoHeight();
    PixelFormat video_pix_fmt = this->GetVideoPixFmt();

    int bmp_size = 54 + bmp_w* bmp_h * 3;
    ASSERT(mem_bmp->GetMaxSize() >= bmp_size);

    int t,bits = 24;

    mem_bmp->SetSize(0);
    mem_bmp->Putc('B');
    mem_bmp->Putc('M');
    t = 54 + bmp_w*bmp_h*(bits >> 3);mem_bmp->Write(&t,4);  
    t = 0; mem_bmp->Write(&t,4);
    t = 0x36; mem_bmp->Write(&t,4);
    t = 0x28; mem_bmp->Write(&t,4);
    t = bmp_w; mem_bmp->Write(&t,4);
    t = bmp_h; mem_bmp->Write(&t,4);
    t = 1; mem_bmp->Write(&t,2);
    t = bits;mem_bmp->Write(&t,2);
    t = 0; mem_bmp->Write(&t,4);
    t = (bits >> 3)*bmp_w*bmp_h; mem_bmp->Write(&t,4);
    //BMP has 54 bytes header
    mem_bmp->FillBlock(54 - mem_bmp->GetOffset(),0);

    uint8_t *rgb_data = (uint8_t*)mem_bmp->p + 54;
    uint8_t *rgb_src[3];
    int rgb_stride[3];
    
    rgb_src[0]= rgb_data + bmp_w*(bmp_h-1)*3;
    rgb_src[1]= 0;
    rgb_src[2]= 0;

    rgb_stride[0]=-3 * bmp_w;
    rgb_stride[1]=0;
    rgb_stride[2]=0;

    SwsContext *img_convert_ctx = sws_getContext(video_width, video_height,
                        video_pix_fmt,bmp_w, bmp_h, 
                        PIX_FMT_BGR24, SWS_BICUBIC,NULL, NULL, NULL);

    ASSERT(img_convert_ctx);
    ASSERT(pFrame);

    sws_scale(img_convert_ctx,pFrame->data,pFrame->linesize,0,bmp_h,rgb_src,rgb_stride);
    sws_freeContext(img_convert_ctx);

    mem_bmp->SetSize(bmp_size);

    return OK;
}
