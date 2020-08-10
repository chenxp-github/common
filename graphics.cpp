#include "stdafx.h"
#include "graphics.h"
#include "syslog.h"

char *CGraphics::i_buf_asc = NULL;
char *CGraphics::i_buf_hzk12 = NULL;

#define HZ12_WIDTH  13
#define HZ12_HEIGHT 12
#define ASC12_WIDTH 6

CGraphics::CGraphics()
{
    this->InitBasic();
}
CGraphics::~CGraphics()
{
    this->Destroy();
}
int CGraphics::InitBasic()
{
    this->vram = NULL;
    this->screen_height = 0;
    this->screen_width = 0;
    this->clip_x1 = 0;
    this->clip_x2 = 0;
    this->clip_y1 = 0;
    this->clip_y2 = 0;
    this->xpitch = 0;
    this->ypitch = 0;

    return OK;
}
int CGraphics::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
int CGraphics::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}

pixel_type * CGraphics::MapVRam(int x, int y)
{
    ASSERT(this->vram);
    ASSERT(this->screen_width > 0 && this->screen_height > 0);

    return  vram + x*xpitch + y*ypitch;
}

void CGraphics::SetPixel(int x,int y,pixel_type color)
{
    pixel_type *p = MapVRam(x,y);
    *p = color; 
}

void CGraphics::VLine(int x,int y,int h,pixel_type color)
{
    int i;
    pixel_type *vp = MapVRam(x,y);
    for(i = 0; i < h; i++)
    {
        *vp = color;
        vp += ypitch;
    }  
}

void CGraphics::HLine(int x,int y,int w,pixel_type color)
{
    int i;
    pixel_type *vp = MapVRam(x,y);

    for(i = 0; i < w; i++)
    {
        *vp = color;
        vp += xpitch;
    }
}   

void CGraphics::FillRect(int x,int y,int w,int h,pixel_type color)
{
   int i;

   for( i = 0 ;i < h ; i++)
   {
       HLine(x,y+i,w,color);
   }   
}   

void CGraphics::DrawRect(int left,int top,int width,int height,pixel_type color)
{
    HLine(left,top,width,color);
    HLine(left,top+height-1,width,color);
    VLine(left,top,height,color);
    VLine(left+width-1,top,height,color);
}
void CGraphics::GetLine(int x,int y,int w,void *buf)
{
    int i;
    pixel_type *p = (pixel_type*)buf;
    pixel_type *vp = MapVRam(x,y);  

    for(i=0; i<w; i++)
    {
        p[i] = *vp;
        vp += xpitch;
    }
}
void CGraphics::PutLine(int x,int y,int w,void *buf)
{
    int i;
    pixel_type *p = (pixel_type*)buf;
    pixel_type *vp = MapVRam(x,y);  

    for(i = 0; i < w; i++)
    {
        *vp = p[i];         
        vp += xpitch;
    }
}
int CGraphics::GetImage(int x,int y,int w,int h,void *buf)
{
    int j;
    pixel_type *p = (pixel_type*)buf;
    
    for(j = 0; j < h; j++)
    {
        GetLine(x,j+y,w,p+j*w);
    }
    return OK;
}
int CGraphics::PutImage(int x,int y,int w,int h,void *buf)
{
    int j;
    pixel_type *p = (pixel_type*)buf;
    
    for(j = 0; j < h; j++)
    {
        PutLine(x,j+y,w,p+j*w);
    }
    
    return OK;
}

void CGraphics::ScrollUp(int cy,pixel_type bc)
{
    int i;

    for(i = 0; i < this->screen_height - cy; i++)
    {
        GetLine(0,i+cy,this->screen_width,MapVRam(0,i));
    }

    FillRect(0,this->screen_height-cy,this->screen_width,cy,bc);
}

int CGraphics::ImageSize(int w,int h)
{
    return w * h * sizeof(pixel_type);
}

int CGraphics::DrawMonoImage(  int rcl,int rct,int rcr,int rcb,
                      int x,int y,int width,int height,char *buf,
                      pixel_type fore_color)
{
    int i,j,pointer;
    int tx,ty,tr,tb;
    pixel_type *p;
    
    tx = (rcl > x) ? rcl : x;
    ty = (rct > y) ? rct : y;
    tr = (rcr < (x+width-1)) ? rcr : (x+width-1);
    tb = (rcb < (y+height-1))? rcb : (y+height-1);

    if(tx > tr || ty > tb) return FALSE;
    
    for(j = ty; j <= tb; j++)
    {
        p = MapVRam(tx,j);
        for(i = 0; i <= tr-tx; i++)     
        {
            pointer = (j - y) * width + (i-x+tx);
            if((buf[pointer>>3]>>(7 - (pointer&7)))&0x01)
                *(p+i*xpitch) = fore_color;
        }
    }
    return OK;
}

int CGraphics::ClearScreen(pixel_type color)
{
    FillRect(0,0,this->screen_width,this->screen_height,color);
    return OK;
}

int CGraphics::SetSize(int width, int height)
{
    this->screen_width = width;
    this->screen_height = height;
    this->SetClipping(0,0,width,height);
    return OK;
}

int CGraphics::SetVRam(void *buf)
{
    this->vram = (pixel_type*)buf;
    return OK;
}

int CGraphics::Clear()
{
    return this->InitBasic();
}

int CGraphics::SetClipping(int x1, int y1, int x2, int y2)
{
    clip_x1 = x1;
    clip_y1 = y1;
    clip_x2 = x2;
    clip_y2 = y2;

    if(clip_x1 < 0) clip_x1 = 0;
    if(clip_y1 < 0) clip_y1 = 0;
    if(clip_x2 < 0) clip_x2 = 0;
    if(clip_y2 < 0) clip_y2 = 0;
    
    if(clip_x2 >= this->screen_width)
        clip_x2 = this->screen_width - 1;   
    if(clip_y2 >= this->screen_height)
        clip_y2 = this->screen_height - 1;  

    return OK;
}

int CGraphics::GetClippingRect(int *dx1, int *dy1, int *dx2, int *dy2)
{
    *dx1 = ((*dx1) > clip_x1) ? (*dx1) : clip_x1;
    *dy1 = ((*dy1) > clip_y1) ? (*dy1) : clip_y1;
    *dx2 = ((*dx2) < clip_x2) ? (*dx2) : clip_x2;
    *dy2 = ((*dy2) < clip_y2) ? (*dy2) : clip_y2;

    if ( *dx1 > *dx2 || *dy1 > *dy2 )
    {
        return FALSE;
    }

    return TRUE;
}

int CGraphics::DrawMonoImage_Clip(int x, int y, int width, int height, void *buf, pixel_type fc)
{
    this->DrawMonoImage(clip_x1,clip_y1,clip_x2, clip_y2,
                        x,y,width,height,(char*) buf, fc);
    return OK;
}

int CGraphics::HLine_Clip(int x, int y, int width, pixel_type color)
{
    int x2 = x+width-1;
    int y2 = y;
    
    if( this->GetClippingRect(&x,&y,&x2,&y2) )
        this->HLine(x,y,x2-x+1,color);
    
    return OK;
}

int CGraphics::VLine_Clip(int x, int y, int h, pixel_type color)
{
    int x2 = x;
    int y2 = y + h - 1;
    
    if(GetClippingRect(&x,&y,&x2,&y2))
        VLine(x,y,y2-y+1,color);

    return OK;
}

int CGraphics::DrawRect_Clip(int left, int top, int width, int height, pixel_type color)
{
    HLine_Clip(left,top,width,color);
    HLine_Clip(left,top+height-1,width,color);
    VLine_Clip(left,top,height,color);
    VLine_Clip(left+width-1,top,height,color);

    return OK;
}

int CGraphics::FillRect_Clip(int x, int y, int width, int height, pixel_type color)
{
    int x1=x, y1=y, x2=x+width-1, y2=y+height-1;
    
    if( GetClippingRect(&x1,&y1,&x2,&y2) )
        this->FillRect(x1,y1,x2-x1+1,y2-y1+1,color);

    return OK;
}

int CGraphics::DrawHz12(int x, int y, char *str,pixel_type color)
{
    int qm,wm,offset;
    unsigned char *p;
    int startx;

    startx = x;
    p= (BYTE *) str;    
    
    ASSERT(this->i_buf_asc);

    while(*p)
    {
        if(*p >= 160 && *(p+1) >= 160)
        {
            qm = *p++ - 161;
            wm = *p++ - 161;
            offset = (wm + qm * 94) * 24L;
            ASSERT(this->i_buf_hzk12);
            this->DrawMonoImage_Clip(x, y, 16,12, this->i_buf_hzk12 + offset , color);
            x += HZ12_WIDTH;
        }
        else if(*p == '\n')
        {
            p ++;
            y += HZ12_HEIGHT;
            x = startx;
        }
        else
        {
            this->DrawMonoImage_Clip( x,y,8,12,this->i_buf_asc + (*p) * 12, color);
            x += ASC12_WIDTH;
            p ++;
        }
    }
    return OK;
}

int CGraphics::Copy(CGraphics *g)
{
    ASSERT(g);

    this->vram = g->vram;
    this->clip_x1 = g->clip_x1;
    this->clip_x2 = g->clip_x2;
    this->clip_y1 = g->clip_y1;
    this->clip_y2 = g->clip_y2;
    this->screen_height = g->screen_height;
    this->screen_width = g->screen_width;
    this->xpitch = g->xpitch;
    this->ypitch = g->ypitch;

    return OK;
}

int CGraphics::SetPitch(int xp, int yp)
{
    this->xpitch = xp;
    this->ypitch = yp;
    return OK;
}

int CGraphics::Rotate90()
{   
    this->vram += ((this->screen_height - 1) * this->ypitch);
    this->SetSize(screen_height,screen_width);
    this->SetPitch(-ypitch,xpitch);

    return OK;
}

int CGraphics::Rotate180()
{
    this->vram += ((screen_height - 1) * ypitch + (screen_width-1)*xpitch);
    this->SetPitch(-xpitch,-ypitch);

    return OK;
}

int CGraphics::Rotate270()
{
    this->vram += ((screen_width-1)*xpitch);
    this->SetPitch(ypitch,-xpitch);
    this->SetSize(screen_height,screen_width);

    return OK;
}
