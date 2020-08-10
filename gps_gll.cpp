#include "gps_gll.h"
#include "syslog.h"
#include "mem_tool.h"
#include "gps_helper.h"

CGpsGll::CGpsGll()
{
    this->InitBasic();
}

CGpsGll::~CGpsGll()
{
    this->Destroy();
}

status_t CGpsGll::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    this->m_latitude = 0;
    this->m_longitude = 0;
    this->m_utc_time.InitBasic();
    this->m_status = 0;
    this->m_mode = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsGll::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
    this->m_utc_time.Init();
/*##End Init##*/
    return OK;
}

status_t CGpsGll::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
    this->m_utc_time.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsGll::Copy(CGpsGll *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    this->m_latitude = _p->m_latitude;
    this->m_longitude = _p->m_longitude;
    this->m_utc_time.Copy(&_p->m_utc_time);
    this->m_status = _p->m_status;
    this->m_mode = _p->m_mode;
/*##End Copy##*/
    return OK;
}

int CGpsGll::Comp(CGpsGll *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsGll::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CGpsData::Print(_buf);
    _buf->Log("latitude = %f",m_latitude);
    _buf->Log("longitude = %f",m_longitude);
    _buf->Log("utc_time = {");
    _buf->IncLogLevel(1);
    this->m_utc_time.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("status = %c",m_status);
    _buf->Log("mode = %c",m_mode);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetLatitude@@*/
double CGpsGll::GetLatitude()
{
    return m_latitude;
}
/*@@End  Function GetLatitude@@*/

/*@@Begin Function GetLongitude@@*/
double CGpsGll::GetLongitude()
{
    return m_longitude;
}
/*@@End  Function GetLongitude@@*/

/*@@Begin Function GetUtcTime@@*/
CGpsUtcTime* CGpsGll::GetUtcTime()
{
    return &m_utc_time;
}
/*@@End  Function GetUtcTime@@*/

/*@@Begin Function GetStatus@@*/
char CGpsGll::GetStatus()
{
    return m_status;
}
/*@@End  Function GetStatus@@*/

/*@@Begin Function SetLatitude@@*/
status_t CGpsGll::SetLatitude(double _latitude)
{
    this->m_latitude = _latitude;
    return OK;
}
/*@@End  Function SetLatitude@@*/

/*@@Begin Function SetLongitude@@*/
status_t CGpsGll::SetLongitude(double _longitude)
{
    this->m_longitude = _longitude;
    return OK;
}
/*@@End  Function SetLongitude@@*/

/*@@Begin Function SetUtcTime@@*/
status_t CGpsGll::SetUtcTime(CGpsUtcTime *_utc_time)
{
    ASSERT(_utc_time);
    return this->m_utc_time.Copy(_utc_time);
}
/*@@End  Function SetUtcTime@@*/

/*@@Begin Function SetStatus@@*/
status_t CGpsGll::SetStatus(char _status)
{
    this->m_status = _status;
    return OK;
}
/*@@End  Function SetStatus@@*/


/*@@Begin Function GetMode@@*/
char CGpsGll::GetMode()
{
    return m_mode;
}
/*@@End  Function GetMode@@*/

/*@@Begin Function SetMode@@*/
status_t CGpsGll::SetMode(char _mode)
{
    this->m_mode = _mode;
    return OK;
}
/*@@End  Function SetMode@@*/
/*@@ Insert Function Here @@*/
status_t CGpsGll::ToGpsString(CFileBase *out)
{
    ASSERT(out);
    
    HeadToString(out);
    out->Putc(',');
    
    if(HasLatitude())
        LatitudeToGpsString(m_latitude,out);

    out->Putc(',');
    out->Puts("N,");

    if(HasLongitude())
        LongitudeToGpsString(m_longitude,out);
    out->Putc(',');
    out->Puts("E,");

    if(HasUtcTime())
        m_utc_time.ToGpsString(out);
    out->Putc(',');

    if(HasStatus())
        out->Putc(m_status);
    out->Putc(',');

    if(HasMode())
        out->Putc(m_mode);
    return OK;
}

