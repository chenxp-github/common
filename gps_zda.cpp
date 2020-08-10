#include "gps_zda.h"
#include "syslog.h"
#include "mem_tool.h"
#include "gps_helper.h"

CGpsZda::CGpsZda()
{
    this->InitBasic();
}

CGpsZda::~CGpsZda()
{
    this->Destroy();
}

status_t CGpsZda::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    this->m_utc_time.InitBasic();
    this->m_utc_date.InitBasic();
    this->m_local_zone_hours = 0;
    this->m_local_zone_minutes = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsZda::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
    this->m_utc_time.Init();
    this->m_utc_date.Init();
/*##End Init##*/
    return OK;
}

status_t CGpsZda::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
    this->m_utc_time.Destroy();
    this->m_utc_date.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsZda::Copy(CGpsZda *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    this->m_utc_time.Copy(&_p->m_utc_time);
    this->m_utc_date.Copy(&_p->m_utc_date);
    this->m_local_zone_hours = _p->m_local_zone_hours;
    this->m_local_zone_minutes = _p->m_local_zone_minutes;
/*##End Copy##*/
    return OK;
}

int CGpsZda::Comp(CGpsZda *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsZda::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CGpsData::Print(_buf);
    _buf->Log("utc_time = {");
    _buf->IncLogLevel(1);
    this->m_utc_time.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("utc_date = {");
    _buf->IncLogLevel(1);
    this->m_utc_date.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("local_zone_hours = %d",m_local_zone_hours);
    _buf->Log("local_zone_minutes = %d",m_local_zone_minutes);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetUtcTime@@*/
CGpsUtcTime* CGpsZda::GetUtcTime()
{
    return &m_utc_time;
}
/*@@End  Function GetUtcTime@@*/

/*@@Begin Function GetUtcDate@@*/
CGpsUtcDate* CGpsZda::GetUtcDate()
{
    return &m_utc_date;
}
/*@@End  Function GetUtcDate@@*/

/*@@Begin Function GetLocalZoneHours@@*/
int CGpsZda::GetLocalZoneHours()
{
    return m_local_zone_hours;
}
/*@@End  Function GetLocalZoneHours@@*/

/*@@Begin Function GetLocalZoneMinutes@@*/
int CGpsZda::GetLocalZoneMinutes()
{
    return m_local_zone_minutes;
}
/*@@End  Function GetLocalZoneMinutes@@*/

/*@@Begin Function SetUtcTime@@*/
status_t CGpsZda::SetUtcTime(CGpsUtcTime *_utc_time)
{
    ASSERT(_utc_time);
    return this->m_utc_time.Copy(_utc_time);
}
/*@@End  Function SetUtcTime@@*/

/*@@Begin Function SetUtcDate@@*/
status_t CGpsZda::SetUtcDate(CGpsUtcDate *_utc_date)
{
    ASSERT(_utc_date);
    return this->m_utc_date.Copy(_utc_date);
}
/*@@End  Function SetUtcDate@@*/

/*@@Begin Function SetLocalZoneHours@@*/
status_t CGpsZda::SetLocalZoneHours(int _local_zone_hours)
{
    this->m_local_zone_hours = _local_zone_hours;
    return OK;
}
/*@@End  Function SetLocalZoneHours@@*/

/*@@Begin Function SetLocalZoneMinutes@@*/
status_t CGpsZda::SetLocalZoneMinutes(int _local_zone_minutes)
{
    this->m_local_zone_minutes = _local_zone_minutes;
    return OK;
}
/*@@End  Function SetLocalZoneMinutes@@*/

/*@@ Insert Function Here @@*/
status_t CGpsZda::ToGpsString(CFileBase *out)
{
    ASSERT(out);
    
    HeadToString(out);
    out->Putc(',');

    m_utc_time.ToGpsString(out);
    out->Putc(',');

    out->Printf("%02d,",m_utc_date.GetMonth());
    out->Printf("%02d,",m_utc_date.GetDay());
    out->Printf("%02d,",m_utc_date.GetYear());
    
    out->Puts("00,00");
    return OK;
}
