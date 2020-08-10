#ifndef __CANVAS_H
#define __CANVAS_H

#include "common.h"
#include "rect.h"

#define OP_ERROR                        0
#define OP_DRAW_PIXEL                   1
#define OP_GET_SCREEN_SIZE              2
#define OP_DRAW_H_LINE                  3
#define OP_DRAW_V_LINE                  4
#define OP_DRAW_LINE                    5
#define OP_DRAW_TEXT                    6
#define OP_BIT_BLT                      7
#define OP_FILL_RECT                    8
#define OP_TRANSPARENT_BLT              9

class CCanvas{
public:
    CCanvas *mParent;
    int32_t mViewX,mViewY;
    CRect *mRect;
    void *param_draw[MAX_CALLBACK_PARAMS];
    status_t (*callback_draw)(void **p);
public:
    status_t ToFinalRectWithoutClip(CRect *r);
    status_t GetViewRect(CRect *pr);
    void S2L(int32_t *x, int32_t *y);
    status_t TransparentBlt(int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y);
    status_t SetCanvasRect(int32_t left, int32_t top, int32_t right, int32_t bottom);
    status_t BitBlt_Common(int32_t op,int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y);
    CCanvas * GetFinalCanvas();
    status_t BitBlt(int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y);
    status_t CallDrawFunc(int32_t operation);
    status_t DrawText(int32_t x,int32_t y,const char *str);
    status_t DrawLine(int32_t x1,int32_t y1, int32_t x2,int32_t y2);
    status_t FillRect(CRect *r);
    status_t DrawRect(CRect *r);
    status_t DrawVLine(int32_t x,int32_t y1, int32_t y2);
    status_t DrawHLine(int32_t x1,int32_t x2,int32_t y);
    status_t DrawPixel(int32_t x,int32_t y);
    status_t ToFinalPoint(int32_t *x,int32_t *y);
    status_t SetParentCanvas(CCanvas *canvas);
    status_t SetCanvasRect(CRect *r);
    status_t ToFinalRect(CRect *r);
    status_t L2S(CRect *r);
    void L2S(int32_t *x, int32_t *y);
    CCanvas();
    virtual ~CCanvas();
    status_t Init();
    status_t Destroy();
    status_t Copy(CCanvas *p);
    status_t Comp(CCanvas *p);
    status_t Print();
    status_t InitBasic();
};

#endif
