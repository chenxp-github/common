#include "gps_utc_date.h"
#include "syslog.h"
#include "mem_tool.h"

CGpsUtcDate::CGpsUtcDate()
{
    this->InitBasic();
}

CGpsUtcDate::~CGpsUtcDate()
{
    this->Destroy();
}

status_t CGpsUtcDate::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_year = 0;
    this->m_month = 0;
    this->m_day = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsUtcDate::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    
/*##End Init##*/
    return OK;
}

status_t CGpsUtcDate::Destroy()
{
    WEAK_REF_DESTROY();
/*##Begin Destroy##*/
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsUtcDate::Copy(CGpsUtcDate *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_year = _p->m_year;
    this->m_month = _p->m_month;
    this->m_day = _p->m_day;
/*##End Copy##*/
    return OK;
}

int CGpsUtcDate::Comp(CGpsUtcDate *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsUtcDate::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("year = %d",m_year);
    _buf->Log("month = %d",m_month);
    _buf->Log("day = %d",m_day);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetYear@@*/
int CGpsUtcDate::GetYear()
{
    return m_year;
}
/*@@End  Function GetYear@@*/

/*@@Begin Function GetMonth@@*/
int CGpsUtcDate::GetMonth()
{
    return m_month;
}
/*@@End  Function GetMonth@@*/

/*@@Begin Function GetDay@@*/
int CGpsUtcDate::GetDay()
{
    return m_day;
}
/*@@End  Function GetDay@@*/

/*@@Begin Function SetYear@@*/
status_t CGpsUtcDate::SetYear(int _year)
{
    this->m_year = _year;
    return OK;
}
/*@@End  Function SetYear@@*/

/*@@Begin Function SetMonth@@*/
status_t CGpsUtcDate::SetMonth(int _month)
{
    this->m_month = _month;
    return OK;
}
/*@@End  Function SetMonth@@*/

/*@@Begin Function SetDay@@*/
status_t CGpsUtcDate::SetDay(int _day)
{
    this->m_day = _day;
    return OK;
}
/*@@End  Function SetDay@@*/

/*@@ Insert Function Here @@*/
status_t CGpsUtcDate::ToGpsString(CFileBase *out)
{
    ASSERT(out);
    out->Printf("%02d%02d%02d",m_day,m_month,m_year-2000);
    return OK;
}
