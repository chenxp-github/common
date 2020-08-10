#include "canvas.h"
#include "mem_tool.h"
#include "syslog.h"

CCanvas::CCanvas()
{
    this->InitBasic();
}
CCanvas::~CCanvas()
{
    this->Destroy();
}
status_t CCanvas::InitBasic()
{
    this->mRect = NULL;
    this->mParent = NULL;
    this->mViewX = 0;
    this->mViewY = 0;

    this->callback_draw = NULL;
    for(int32_t i = 0; i < MAX_CALLBACK_PARAMS; i++)
    {
        this->param_draw[i] = NULL;
    }

    return OK;
}
status_t CCanvas::Init()
{
    this->InitBasic();
    NEW(this->mRect,CRect);
    this->mRect->Init();
    return OK;
}
status_t CCanvas::Destroy()
{
    DEL(this->mRect);
    this->InitBasic();
    return OK;
}
status_t CCanvas::Copy(CCanvas *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
status_t CCanvas::Comp(CCanvas *p)
{
    return 0;
}
status_t CCanvas::Print()
{
    //add your code
    return TRUE;
}

void CCanvas::L2S(int32_t *x, int32_t *y)
{
    *x = *x - this->mViewX + this->mRect->left;
    *y = *y - this->mViewY + this->mRect->top;
}

status_t CCanvas::L2S(CRect *r)
{
    ASSERT(r);  
    this->L2S(&(r->left),&(r->top));
    this->L2S(&(r->right),&(r->bottom));
    return OK;
}

void CCanvas::S2L(int32_t *x, int32_t *y)
{
    *x = *x + this->mViewX - this->mRect->left;
    *y = *y + this->mViewY - this->mRect->top;
}

status_t CCanvas::ToFinalPoint(int32_t *x,int32_t *y)
{
    ASSERT(x && y);

    CCanvas *pc = this;
    while(pc)
    {
        pc->L2S(x,y);
        pc = pc->mParent;
    }
    return OK;
}

status_t CCanvas::ToFinalRect(CRect *r)
{
    ASSERT(r);
    CRect rt,rc,rs;

    rt.Copy(r);
    CCanvas *pc = this;
    while(pc)
    {
        pc->L2S(&rt);
        rc.Intersect(&rt,pc->mRect);
        rt.Copy(&rc);
        pc = pc->mParent;
    }
    this->CallDrawFunc(OP_GET_SCREEN_SIZE);
    rs.SetWH(0,0,(int32_t)this->param_draw[1],(int32_t)this->param_draw[2]);
    rt.Intersect(&rc,&rs);
    r->Copy(&rt);
    return OK;
}

status_t CCanvas::SetCanvasRect(CRect *r)
{
    ASSERT(r);
    return this->mRect->Copy(r);
}

status_t CCanvas::DrawPixel(int32_t x, int32_t y)
{
    CRect rt;

    rt.Set(x,y,x,y);
    this->ToFinalRect(&rt);
    if(rt.IsEmpty())return ERROR;
    this->param_draw[1] = (void*)rt.left;
    this->param_draw[2] = (void*)rt.top;
    this->CallDrawFunc(OP_DRAW_PIXEL);
    return OK;
}

status_t CCanvas::DrawHLine(int32_t x1, int32_t x2, int32_t y)
{
    CRect rt;

    rt.Set(x1,y,x2,y);
    this->ToFinalRect(&rt);

    if(rt.IsEmpty())
        return ERROR;
    
    this->param_draw[1] = (void*)rt.left;
    this->param_draw[2] = (void*)rt.right;
    this->param_draw[3] = (void*)rt.top;
    this->CallDrawFunc(OP_DRAW_H_LINE);
    return OK;
}

status_t CCanvas::DrawVLine(int32_t x, int32_t y1, int32_t y2)
{
    CRect rt;

    rt.Set(x,y1,x,y2);
    this->ToFinalRect(&rt);
    if(rt.IsEmpty())
        return ERROR;
        
    this->param_draw[1] = (void*)rt.left;
    this->param_draw[2] = (void*)rt.top;
    this->param_draw[3] = (void*)rt.bottom;
    this->CallDrawFunc(OP_DRAW_V_LINE);
    return OK;
}

status_t CCanvas::DrawRect(CRect *r)
{
    ASSERT(r);
    this->DrawHLine(r->left,r->right,r->top);
    this->DrawHLine(r->left,r->right,r->bottom);
    this->DrawVLine(r->left,r->top,r->bottom);
    this->DrawVLine(r->right,r->top,r->bottom);
    return OK;
}

status_t CCanvas::FillRect(CRect *r)
{
    ASSERT(r);

    CRect rt;
    rt.Copy(r);
    
    this->ToFinalRect(&rt);
    if(rt.IsEmpty())
        return ERROR;
    
    this->param_draw[1] = &rt;
    this->CallDrawFunc(OP_FILL_RECT);
    return OK;  
}

status_t CCanvas::SetParentCanvas(CCanvas *canvas)
{
    ASSERT(canvas);

    this->mParent = canvas;
    this->callback_draw = canvas->callback_draw;

    for(int32_t i = 0; i < MAX_CALLBACK_PARAMS; i++)
    {
        this->param_draw[i] = canvas->param_draw[i];
    }

    return OK;
}

#define ADD_NEW_POINT(x,y)  \
do{\
if(rt.PtInRect((x),(y)))\
{\
    minx = (x) > minx?(x):minx;\
    miny = (y) > miny?(y):miny;\
    maxx = (x) < maxx?(x):maxx;\
    maxy = (y) < maxy?(y):maxy;\
    k++;\
}}while(0)


status_t CCanvas::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    CRect rt,rc;
    
    rt.Set(x1,y1,x2,y2);
    rt.Normalize();
    this->ToFinalRect(&rt);
    
    this->ToFinalPoint(&x1,&y1);
    this->ToFinalPoint(&x2,&y2);

    if(rt.IsEmpty())
        return ERROR;

    status_t x,y,k = 0;
    int32_t minx=x1,miny=y1,maxx=x2,maxy=y2;
    if(x2 - x1 != 0)
    {
        x = rt.left;
        y = (y2-y1)*(x - x1) / (x2-x1) + y1;
        ADD_NEW_POINT(x,y);
        x = rt.right;
        y = (y2-y1)*(x - x1) / (x2-x1) + y1;
        ADD_NEW_POINT(x,y);
    }
    
    if(y2 - y1 != 0)
    {
        y = rt.top;
        x = (x2-x1)*(y-y1) / (y2-y1) + x1;
        ADD_NEW_POINT(x,y);
        y = rt.bottom;
        x = (x2-x1)*(y-y1) / (y2-y1) + x1;
        ADD_NEW_POINT(x,y);
    }

    if(k == 0) return ERROR;

    this->param_draw[1] = (void*)minx;
    this->param_draw[2] = (void*)miny;
    this->param_draw[3] = (void*)maxx;
    this->param_draw[4] = (void*)maxy;
    this->CallDrawFunc(OP_DRAW_LINE);
    return OK;
}

status_t CCanvas::DrawText(int32_t x, int32_t y, const char *str)
{
    CRect rt;

    rt.SetWH(-1000000,-1000000,2000000,2000000); // very big rect

    this->ToFinalRect(&rt);
    this->ToFinalPoint(&x,&y);

    if(rt.IsEmpty())
        return ERROR;
    
    this->param_draw[1] = (void*)x;
    this->param_draw[2] = (void*)y;
    this->param_draw[3] = (void*)str;
    this->param_draw[4] = (void*)&rt;
    
    this->CallDrawFunc(OP_DRAW_TEXT);
    return OK;
}

status_t CCanvas::CallDrawFunc(int32_t operation)
{
    ASSERT(this->callback_draw);
    this->param_draw[0] = (void*)operation;
    return this->callback_draw(this->param_draw);
}
status_t CCanvas::BitBlt_Common(int32_t op, int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y)
{
    CRect rc,rt;
    rt.SetWH(x,y,w,h);
    this->ToFinalRect(&rt);

    if(rt.IsEmpty())
        return ERROR;

    this->param_draw[1] = &rt;
    this->ToFinalPoint(&x,&y);

    if(x < rt.left)
        this->param_draw[2] = (void*)(src_x + (rt.left-x));
    else
        this->param_draw[2] = (void*)src_x;

    if(y < rt.top)
        this->param_draw[3] = (void*)(src_y + (rt.top-y));
    else 
        this->param_draw[3] = (void*) src_y;
    return this->CallDrawFunc(op);
}

status_t CCanvas::BitBlt(int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y)
{
    return this->BitBlt_Common(OP_BIT_BLT,x,y,w,h,src_x,src_y);
}

CCanvas * CCanvas::GetFinalCanvas()
{
    CCanvas *pc = this;

    while(pc)
    {
        if(pc->mParent == NULL)
            return pc;
        pc = pc->mParent;
    }
    
    return NULL;
}

status_t CCanvas::SetCanvasRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    CRect r;
    r.Set(left,top,right,bottom);
    return this->SetCanvasRect(&r);
}

status_t CCanvas::TransparentBlt(int32_t x, int32_t y, int32_t w, int32_t h, int32_t src_x, int32_t src_y)
{
    return this->BitBlt_Common(OP_TRANSPARENT_BLT,x,y,w,h,src_x,src_y);
}

status_t CCanvas::GetViewRect(CRect *pr)
{
    ASSERT(pr);
    return pr->SetWH(mViewX,mViewY,mRect->GetWidth(), mRect->GetHeight());
}

status_t CCanvas::ToFinalRectWithoutClip(CRect *r)
{
    ASSERT(r);
    CCanvas *pc = this;
    while(pc)
    {
        pc->L2S(r);
        pc = pc->mParent;
    }

    return OK;
}
