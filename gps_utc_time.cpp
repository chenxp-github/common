#include "gps_utc_time.h"
#include "syslog.h"
#include "mem_tool.h"

CGpsUtcTime::CGpsUtcTime()
{
    this->InitBasic();
}

CGpsUtcTime::~CGpsUtcTime()
{
    this->Destroy();
}

status_t CGpsUtcTime::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_hour = 0;
    this->m_minute = 0;
    this->m_second = 0;
    this->m_millisecond = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsUtcTime::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    
/*##End Init##*/
    return OK;
}

status_t CGpsUtcTime::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsUtcTime::Copy(CGpsUtcTime *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_hour = _p->m_hour;
    this->m_minute = _p->m_minute;
    this->m_second = _p->m_second;
    this->m_millisecond = _p->m_millisecond;
/*##End Copy##*/
    return OK;
}

int CGpsUtcTime::Comp(CGpsUtcTime *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsUtcTime::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("hour = %d",m_hour);
    _buf->Log("minute = %d",m_minute);
    _buf->Log("second = %d",m_second);
    _buf->Log("millisecond = %d",m_millisecond);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetHour@@*/
int CGpsUtcTime::GetHour()
{
    return m_hour;
}
/*@@End  Function GetHour@@*/

/*@@Begin Function GetMinute@@*/
int CGpsUtcTime::GetMinute()
{
    return m_minute;
}
/*@@End  Function GetMinute@@*/

/*@@Begin Function GetSecond@@*/
int CGpsUtcTime::GetSecond()
{
    return m_second;
}
/*@@End  Function GetSecond@@*/

/*@@Begin Function GetMillisecond@@*/
int CGpsUtcTime::GetMillisecond()
{
    return m_millisecond;
}
/*@@End  Function GetMillisecond@@*/

/*@@Begin Function SetHour@@*/
status_t CGpsUtcTime::SetHour(int _hour)
{
    this->m_hour = _hour;
    return OK;
}
/*@@End  Function SetHour@@*/

/*@@Begin Function SetMinute@@*/
status_t CGpsUtcTime::SetMinute(int _minute)
{
    this->m_minute = _minute;
    return OK;
}
/*@@End  Function SetMinute@@*/

/*@@Begin Function SetSecond@@*/
status_t CGpsUtcTime::SetSecond(int _second)
{
    this->m_second = _second;
    return OK;
}
/*@@End  Function SetSecond@@*/

/*@@Begin Function SetMillisecond@@*/
status_t CGpsUtcTime::SetMillisecond(int _millisecond)
{
    this->m_millisecond = _millisecond;
    return OK;
}
/*@@End  Function SetMillisecond@@*/

/*@@ Insert Function Here @@*/

status_t CGpsUtcTime::ToGpsString(CFileBase *out)
{
    ASSERT(out);
    out->Printf("%02d%02d%02d.%02d",
        m_hour,m_minute,m_second,
        m_millisecond/10
    );
    return OK;
}

status_t CGpsUtcTime::ForwardSeconds(double sec)
{
    double d = this->ToDoubleSeconds() + sec;
    return this->FromDoubleSeconds(d);
}

double CGpsUtcTime::ToDoubleSeconds()
{
    double d = 0;
    d += m_hour*60*60;
    d += m_minute *60;
    d += m_second;
    d += m_millisecond / 1000.0;
    return d;
}

status_t CGpsUtcTime::FromDoubleSeconds(double d_sec)
{
    m_hour = (int)(d_sec/3600.0);
    m_hour %= 24;

    d_sec -= (m_hour * 3600);
    m_minute = (int)(d_sec/60);
    m_minute %= 60;

    d_sec -= (m_minute*60);
    m_second = (int)d_sec;

    d_sec -= m_second;
    m_millisecond = d_sec*1000;

    return OK;
}
