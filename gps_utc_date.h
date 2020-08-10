#ifndef __GPS_UTC_DATE_H
#define __GPS_UTC_DATE_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsUtcDate
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    int m_year;
    int m_month;
    int m_day;
/*##End Members##*/

public:
    CGpsUtcDate();
    virtual ~CGpsUtcDate();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsUtcDate *_p);
    int Comp(CGpsUtcDate *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetYear();
    int GetMonth();
    int GetDay();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetYear(int _year);
    status_t SetMonth(int _month);
    status_t SetDay(int _day);
/*##End Setter_H##*/

    status_t ToGpsString(CFileBase *out);
};

#endif
