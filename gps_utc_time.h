#ifndef __GPS_UTC_TIME_H
#define __GPS_UTC_TIME_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsUtcTime
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    int m_hour;
    int m_minute;
    int m_second;
    int m_millisecond;
/*##End Members##*/

public:
    CGpsUtcTime();
    virtual ~CGpsUtcTime();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsUtcTime *_p);
    int Comp(CGpsUtcTime *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetHour();
    int GetMinute();
    int GetSecond();
    int GetMillisecond();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetHour(int _hour);
    status_t SetMinute(int _minute);
    status_t SetSecond(int _second);
    status_t SetMillisecond(int _millisecond);
/*##End Setter_H##*/
    status_t ToGpsString(CFileBase *out);
    status_t ForwardSeconds(double sec);

    double ToDoubleSeconds();
    status_t FromDoubleSeconds(double d_sec);
};

#endif
