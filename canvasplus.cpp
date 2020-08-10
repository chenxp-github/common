#include "canvasplus.h"

CCanvasPlus::CCanvasPlus()
{
    this->InitBasic();
}
CCanvasPlus::~CCanvasPlus()
{
    this->Destroy();
}
status_t CCanvasPlus::InitBasic()
{
    CCanvas::InitBasic();
    this->mGraphics = NULL;
    this->mScreenHeight = 0;
    this->mScreenWidth = 0;
    this->mSelfAlloc = false;
    return OK;
}
status_t CCanvasPlus::Init()
{
    this->InitBasic();
    CCanvas::Init();
    this->callback_draw = draw;
    this->param_draw[MAX_CALLBACK_PARAMS-1] = this;
    return OK;
}
status_t CCanvasPlus::Destroy()
{
    if(this->mSelfAlloc)
        delete this->mGraphics;
    CCanvas::Destroy();
    this->InitBasic();
    return OK;
}
status_t CCanvasPlus::Copy(CCanvasPlus *p)
{
    if(this == p)
        return OK;
    CCanvas::Copy(p);
    //add your code
    return OK;
}
status_t CCanvasPlus::Comp(CCanvasPlus *p)
{
    return 0;
}
status_t CCanvasPlus::Print()
{
    CCanvas::Print();
    //add your code
    return TRUE;
}

status_t CCanvasPlus::AttachGraphics(Graphics *g,int32_t sw, int32_t sh)
{
    ASSERT(g);
    this->mGraphics = g;
    this->mScreenWidth = sw;
    this->mScreenHeight = sh;
    this->mSelfAlloc = false;
    
    CRect r;
    r.SetWH(0,0,sw,sh);
    this->SetCanvasRect(&r);

    return OK;
}

status_t CCanvasPlus::get_screen_size(void **p)
{
    p[1] = (void*)this->mScreenWidth;
    p[2] = (void*)this->mScreenHeight;
    return OK;
}

status_t CCanvasPlus::draw_h_line(void **p)
{
    int x1 = (int_ptr_t)p[1];
    int x2 = (int_ptr_t)p[2];
    int y = (int_ptr_t)p[3];

    p[2] = (void*)y;
    p[3] = (void*)x2;
    p[4] = (void*)y;

    return draw_line(p);
}

status_t CCanvasPlus::draw_v_line(void **p)
{
    int x = (int_ptr_t)p[1];
    int y1 = (int_ptr_t)p[2];
    int y2 = (int_ptr_t)p[3];
    p[3] = (void*)x;
    p[4] = (void*)y2;
    return draw_line(p);
}

status_t CCanvasPlus::draw_line(void **p)
{
    int x1 = (int_ptr_t)p[1];
    int y1 = (int_ptr_t)p[2];
    int x2 = (int_ptr_t)p[3];
    int y2 = (int_ptr_t)p[4];

    Pen *pen = (Pen*)this->param_draw[10];
    ASSERT(pen);
    this->mGraphics->DrawLine(pen,x1,y1,x2,y2);
    return OK;
}

status_t CCanvasPlus::draw_text(void **p)
{
    int x = (int_ptr_t)p[1];
    int y = (int_ptr_t)p[2];
    const wchar_t *str = (const wchar_t*)p[3];
    CRect *clip_rect = (CRect*)p[4];
    Font *font = (Font*)p[10];
    Brush *brush = (Brush*)p[11];
    ASSERT(str && clip_rect && font && brush);
    
    Rect r;

    r.X = clip_rect->left;
    r.Y = clip_rect->top;
    r.Width = clip_rect->GetWidth();
    r.Height = clip_rect->GetHeight();

    this->mGraphics->SetClip(r);
    this->mGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
    this->mGraphics->DrawString(str,-1,font,PointF((float)x,(float)y),brush);
    this->mGraphics->ResetClip();
    return OK;
}

status_t CCanvasPlus::fill_rect(void **p)
{
    CRect *pr = (CRect *)p[1];
    ASSERT(pr);
    Brush *brush = (Brush*) this->param_draw[10];
    this->mGraphics->FillRectangle (brush,pr->left,pr->top,pr->GetWidth(),pr->GetHeight());
    return OK;
}

status_t CCanvasPlus::bitblt(void **p)
{
    CRect *pr = (CRect *)p[1];
    int32_t src_x = (int_ptr_t)p[2];
    int32_t src_y = (int_ptr_t)p[3];

    Image *img = (Image*)p[10];
    ASSERT(img);    

    Rect destRect;

    destRect.X = pr->left;
    destRect.Y = pr->top;
    destRect.Width = pr->GetWidth();
    destRect.Height = pr->GetHeight();
    this->mGraphics->DrawImage(img,destRect,src_x,src_y,destRect.Width,destRect.Height,UnitPixel);  
    return OK;
}

status_t CCanvasPlus::draw(void **p)
{
    int32_t oper = (int_ptr_t)p[0];

    CCanvasPlus *self = (CCanvasPlus *)p[MAX_CALLBACK_PARAMS-1];
    ASSERT(self);   

    switch(oper)
    {
        case OP_GET_SCREEN_SIZE: self->get_screen_size(p);break;
        case OP_DRAW_H_LINE: self->draw_h_line(p);break;
        case OP_DRAW_V_LINE: self->draw_v_line(p);break;
        case OP_DRAW_LINE: self->draw_line(p);break;
        case OP_DRAW_TEXT: self->draw_text(p);break;
        case OP_BIT_BLT: self->bitblt(p);break;
        case OP_FILL_RECT:self->fill_rect(p);break;
        default: ASSERT(0); break;
    }

    return OK;
}

status_t CCanvasPlus::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pen *pen)
{
    ASSERT(pen);
    this->param_draw[10] = pen;
    return CCanvas::DrawLine(x1,y1,x2,y2);
}

status_t CCanvasPlus::FillRect(CRect *pr, Brush *brush)
{
    ASSERT(pr && brush);

    this->param_draw[10] = brush;
    return CCanvas::FillRect(pr);
}

status_t CCanvasPlus::DrawRect(CRect *pr, Pen *pen)
{
    ASSERT(pr && pen);

    this->param_draw[10] = pen;
    return CCanvas::DrawRect(pr);
}

status_t CCanvasPlus::DrawImage(Image *img, int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y)
{
    ASSERT(img);

    this->param_draw[10] = img;
    return CCanvas::BitBlt(x,y,w,h,src_x,src_y);
}

status_t CCanvasPlus::AttachBitmap(Bitmap *bmp)
{
    ASSERT(bmp);
    
    if(this->mSelfAlloc)
        delete this->mGraphics;

    this->mGraphics = Graphics::FromImage(bmp);
    this->mScreenWidth = bmp->GetWidth();
    this->mScreenHeight = bmp->GetHeight();
    this->mSelfAlloc = true;

    CRect r;
    r.SetWH(0,0,this->mScreenWidth,this->mScreenHeight);
    this->SetCanvasRect(&r);
    return OK;
}

status_t CCanvasPlus::DrawImage(Image *img, int32_t x, int32_t y)
{
    ASSERT(img);
    return this->DrawImage(img,x,y,img->GetWidth(),img->GetHeight(),0,0);
}

status_t CCanvasPlus::DrawText(int32_t x, int32_t y, const wchar_t *text,Font *font,Brush *brush)
{
    ASSERT(text && font && brush);
    this->param_draw[10] = font;
    this->param_draw[11] = brush;
    return CCanvas::DrawText(x,y,(const char*)text);
}

status_t CCanvasPlus::MeasureString(const wchar_t *text, Font *font,CRect *size)
{
    ASSERT(text && size && font);
    size->Empty();

    RectF r;

    this->mGraphics->MeasureString(text,-1,font,PointF(0.0,0.0),&r);

    size->left = (int32_t)r.X;
    size->top = (int32_t)r.Y;
    size->right = (int32_t)r.GetRight();
    size->bottom = (int32_t)r.GetBottom();

    return OK;
}

status_t CCanvasPlus::DrawScaledImage(Image *img, CRect *dst_rect)
{
    ASSERT(dst_rect && img);
    CRect r;
    r.Copy(dst_rect);
    this->ToFinalRectWithoutClip(&r);
    this->mGraphics->DrawImage(img,CRectToRect(&r));
    return OK;
}

Rect CCanvasPlus::CRectToRect(CRect *r)
{
    Rect t;
    t.X = r->left;
    t.Y = r->top;
    t.Width = r->GetWidth();
    t.Height = r->GetHeight();

    return t;
}

status_t CCanvasPlus::DrawRotatedImage(Image *img, float angle, int32_t center_x, int32_t center_y)
{
    ASSERT(img);
    this->ToFinalPoint(&center_x,&center_y);
    int32_t hw = img->GetWidth() / 2;
    int32_t hh = img->GetHeight() / 2;
    this->mGraphics->TranslateTransform((float)center_x + hw,(float)center_y+hh);
    this->mGraphics->RotateTransform(angle);
    this->mGraphics->DrawImage(img,-hw, -hh,img->GetWidth(),img->GetHeight());
    this->mGraphics->ResetTransform();
    return OK;
}

status_t CCanvasPlus::MeasureString(const wchar_t *text, CRect *layoutRect, Font *font, CRect *size)
{
    ASSERT(text && size && font && layoutRect);
    size->Empty();

    RectF r,lr;

    lr.X = (float)layoutRect->left;
    lr.Y = (float)layoutRect->top;
    lr.Width = (float)layoutRect->GetWidth();
    lr.Height = (float)layoutRect->GetHeight();
    
    this->mGraphics->MeasureString(text,-1,font,lr,&r);

    size->left = (int32_t)r.X;
    size->top = (int32_t)r.Y;
    size->right = (int32_t)r.GetRight();
    size->bottom = (int32_t)r.GetBottom();

    return OK;
}

status_t CCanvasPlus::DrawText(CRect *layoutRect, const wchar_t *text, Font *font, Brush *brush)
{
    RectF lr;

    lr.X = (float)layoutRect->left;
    lr.Y = (float)layoutRect->top;
    lr.Width = (float)layoutRect->GetWidth();
    lr.Height = (float)layoutRect->GetHeight();

    this->mGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
    this->mGraphics->DrawString(text,-1,font,lr,StringFormat::GenericDefault(),brush);

    return OK;
}
