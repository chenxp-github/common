#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include "common.h"

typedef WORD pixel_type;

class CGraphics{
public:
    static char *i_buf_asc,*i_buf_hzk12;
    pixel_type *vram;
    int screen_width,screen_height;
    int xpitch,ypitch; //pixel
    int clip_x1, clip_y1, clip_x2, clip_y2;
public:
    int Rotate270();
    int Rotate180();
    int Rotate90();
    int SetPitch(int xp,int yp);
    int Copy(CGraphics *g);
    int DrawHz12(int x,int y,char *str,pixel_type color);
    int FillRect_Clip(int x,int y,int width,int height,pixel_type color);
    int DrawRect_Clip(int left,int top,int width,int height,pixel_type color);
    int VLine_Clip(int x,int y,int h,pixel_type color);
    int HLine_Clip(int x,int y,int width,pixel_type color);
    int DrawMonoImage_Clip(int x,int y,int width,int height,void *buf,pixel_type fc);
    int GetClippingRect(int *dx1,int *dy1,int *dx2,int *dy2);
    int SetClipping(int x1,int y1,int x2,int y2);
    int Clear();
    int SetVRam(void *buf);
    int SetSize(int width,int height);
    int ClearScreen(pixel_type color);
    void SetPixel(int x,int y,pixel_type color);
    void VLine(int x,int y,int h,pixel_type color);
    void HLine(int x,int y,int w,pixel_type color);
    void FillRect(int x,int y,int w,int h,pixel_type color);
    void DrawRect(int left,int top,int width,int height,pixel_type color);
    void GetLine(int x,int y,int w,void *buf);
    void PutLine(int x,int y,int w,void *buf);
    int GetImage(int x,int y,int w,int h,void *buf);
    int PutImage(int x,int y,int w,int h,void *buf);
    void ScrollUp(int cy,pixel_type bc);
    int ImageSize(int w,int h);
    int DrawMonoImage(  int rcl,int rct,int rcr,int rcb,int x,int y,int width,int height,char *buf,pixel_type fore_color);
    pixel_type * MapVRam(int x,int y);
    CGraphics();
    virtual ~CGraphics();
    int Init();
    int Destroy();
    int InitBasic();
};

#endif
