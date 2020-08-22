#include "gps_helper.h"
#include "syslog.h"
#include "mem_tool.h"
#include <math.h>

status_t FloatToFixedGpsString(double v, int len1, int len2, CFileBase *out)
{
    ASSERT(out);
    char buf[1024];
    sprintf(buf,"%f",v);
    
    int i,ppos = -1;
    int len = strlen(buf);

    for(i = 0; i < len; i++)
    {               
        if(buf[i] == '.')
        {
            ppos = i;
            break;
        }
    }

    int int_part_len = len;
    if(ppos >= 0)
        int_part_len = ppos;

    if(len1 > 0)
    {
        for(i = 0; i < len1-int_part_len; i++)
        {
            out->Putc('0');
        }
    }

    for(i = 0; i < int_part_len; i++)
    {
        out->Putc(buf[i]);
    }
    
    int x_part_len = len - int_part_len - 1;
    if(x_part_len > len2) x_part_len = len2;

    if(ppos >= 0)
        out->Putc('.');

    int s = ppos+1;
    for(i = s; i < s+x_part_len; i++)
    {
        out->Putc(buf[i]);
    }

    if(len2 > 0)
    {
        for(i = 0; i < len2-x_part_len; i++)
        {
            out->Putc('0');
        }
    }
    return OK;
}

status_t LatitudeToGpsString(double lat, CFileBase *out)
{
    int degs = (int)floor(lat);
    double s = lat - degs;
    double f = s *60;
    int f_part1 = (int)floor(f);
    int f_part2 = (int)((f - f_part1)*10000);
    out->Printf("%02d%02d.%04d",degs,f_part1,f_part2);
    return OK;
}

status_t LongitudeToGpsString(double lon, CFileBase *out)
{
    int degs = (int)floor(lon);
    double s = lon - degs;
    double f = s *60;
    int f_part1 = (int)floor(f);
    int f_part2 = (int)((f - f_part1)*10000);
    out->Printf("%03d%02d.%04d",degs,f_part1,f_part2);
    return OK;
}

uint8_t CalcuChecksum(CFileBase *file)
{
    ASSERT(file);
    file->Seek(0);
    
    uint8_t r = 0;

    while(!file->IsEnd())
    {
        char ch = file->Getc();
        r ^= ch;
    }

    return r;
}