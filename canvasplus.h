#ifndef __CANVASPLUS_H
#define __CANVASPLUS_H

#include "canvas.h"
#include "imageplus.h"

class CCanvasPlus:public CCanvas{
public:
    Graphics *mGraphics;
    int32_t mScreenWidth, mScreenHeight;
    bool mSelfAlloc;
public:
    status_t DrawText(CRect *layoutRect,const wchar_t *text,Font *font,Brush *brush);
    status_t MeasureString(const wchar_t *text, CRect *layoutRect,Font *font,CRect *size);
    status_t DrawRotatedImage(Image *img, float angle, int32_t center_x, int32_t center_y);
    static Rect CRectToRect(CRect *r);
    status_t DrawScaledImage(Image *img,CRect *dst_rect);
    status_t MeasureString(const wchar_t *text, Font *font, CRect *size);
    status_t DrawText(int32_t x, int32_t y, const wchar_t *text,Font *font,Brush *brush);
    status_t DrawImage(Image *img, int32_t x, int32_t y);
    status_t AttachBitmap(Bitmap *bmp);
    status_t DrawImage(Image *img, int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y);
    status_t DrawRect(CRect *pr, Pen *pen);
    status_t FillRect(CRect *pr, Brush *brush);
    status_t DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pen *pen);
    status_t AttachGraphics(Graphics *g, int32_t sw, int32_t sh);
    CCanvasPlus();
    virtual ~CCanvasPlus();
    status_t Init();
    status_t Destroy();
    status_t Copy(CCanvasPlus *p);
    status_t Comp(CCanvasPlus *p);
    status_t Print();
    status_t InitBasic();
    static status_t draw(void **p);
    status_t fill_rect(void **p);
    status_t bitblt(void **p);
    status_t draw_text(void **p);
    status_t draw_line(void **p);
    status_t draw_v_line(void **p);
    status_t draw_h_line(void **p);
    status_t get_screen_size(void **p);
};

#endif
