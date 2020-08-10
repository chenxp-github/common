#include "gps_rmc.h"
#include "syslog.h"
#include "mem_tool.h"
#include "gps_helper.h"

CGpsRmc::CGpsRmc()
{
    this->InitBasic();
}

CGpsRmc::~CGpsRmc()
{
    this->Destroy();
}

status_t CGpsRmc::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    this->m_utc_time.InitBasic();
    this->m_status = 0;
    this->m_latitude = 0;
    this->m_longitude = 0;
    this->m_latitude_hemisphere = 0;
    this->m_longitude_hemisphere = 0;
    this->m_speed_knots = 0;
    this->m_track_angle = 0;
    this->m_utc_date.InitBasic();
    this->m_magnetic_variation = 0;
    this->m_declination = 0;
    this->m_mode = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsRmc::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
    this->m_utc_time.Init();
    this->m_utc_date.Init();
/*##End Init##*/
    return OK;
}

status_t CGpsRmc::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
    this->m_utc_time.Destroy();
    this->m_utc_date.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsRmc::Copy(CGpsRmc *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    this->m_utc_time.Copy(&_p->m_utc_time);
    this->m_status = _p->m_status;
    this->m_latitude = _p->m_latitude;
    this->m_longitude = _p->m_longitude;
    this->m_latitude_hemisphere = _p->m_latitude_hemisphere;
    this->m_longitude_hemisphere = _p->m_longitude_hemisphere;
    this->m_speed_knots = _p->m_speed_knots;
    this->m_track_angle = _p->m_track_angle;
    this->m_utc_date.Copy(&_p->m_utc_date);
    this->m_magnetic_variation = _p->m_magnetic_variation;
    this->m_declination = _p->m_declination;
    this->m_mode = _p->m_mode;
/*##End Copy##*/
    return OK;
}

int CGpsRmc::Comp(CGpsRmc *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsRmc::Print(CFileBase *_buf)
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
    _buf->Log("status = %c",m_status);
    _buf->Log("latitude = %f",m_latitude);
    _buf->Log("longitude = %f",m_longitude);
    _buf->Log("latitude_hemisphere = %c",m_latitude_hemisphere);
    _buf->Log("longitude_hemisphere = %c",m_longitude_hemisphere);
    _buf->Log("speed_knots = %f",m_speed_knots);
    _buf->Log("track_angle = %f",m_track_angle);
    _buf->Log("utc_date = {");
    _buf->IncLogLevel(1);
    this->m_utc_date.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("magnetic_variation = %f",m_magnetic_variation);
    _buf->Log("declination = %c",m_declination);
    _buf->Log("mode = %c",m_mode);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetUtcTime@@*/
CGpsUtcTime* CGpsRmc::GetUtcTime()
{
    return &m_utc_time;
}
/*@@End  Function GetUtcTime@@*/

/*@@Begin Function GetStatus@@*/
char CGpsRmc::GetStatus()
{
    return m_status;
}
/*@@End  Function GetStatus@@*/

/*@@Begin Function GetLatitude@@*/
double CGpsRmc::GetLatitude()
{
    return m_latitude;
}
/*@@End  Function GetLatitude@@*/

/*@@Begin Function GetLongitude@@*/
double CGpsRmc::GetLongitude()
{
    return m_longitude;
}
/*@@End  Function GetLongitude@@*/

/*@@Begin Function GetLatitudeHemisphere@@*/
char CGpsRmc::GetLatitudeHemisphere()
{
    return m_latitude_hemisphere;
}
/*@@End  Function GetLatitudeHemisphere@@*/

/*@@Begin Function GetLongitudeHemisphere@@*/
char CGpsRmc::GetLongitudeHemisphere()
{
    return m_longitude_hemisphere;
}
/*@@End  Function GetLongitudeHemisphere@@*/

/*@@Begin Function GetSpeedKnots@@*/
double CGpsRmc::GetSpeedKnots()
{
    return m_speed_knots;
}
/*@@End  Function GetSpeedKnots@@*/

/*@@Begin Function GetTrackAngle@@*/
double CGpsRmc::GetTrackAngle()
{
    return m_track_angle;
}
/*@@End  Function GetTrackAngle@@*/

/*@@Begin Function GetUtcDate@@*/
CGpsUtcDate* CGpsRmc::GetUtcDate()
{
    return &m_utc_date;
}
/*@@End  Function GetUtcDate@@*/

/*@@Begin Function GetMagneticVariation@@*/
double CGpsRmc::GetMagneticVariation()
{
    return m_magnetic_variation;
}
/*@@End  Function GetMagneticVariation@@*/

/*@@Begin Function GetDeclination@@*/
char CGpsRmc::GetDeclination()
{
    return m_declination;
}
/*@@End  Function GetDeclination@@*/

/*@@Begin Function SetUtcTime@@*/
status_t CGpsRmc::SetUtcTime(CGpsUtcTime *_utc_time)
{
    ASSERT(_utc_time);
    return this->m_utc_time.Copy(_utc_time);
}
/*@@End  Function SetUtcTime@@*/

/*@@Begin Function SetStatus@@*/
status_t CGpsRmc::SetStatus(char _status)
{
    this->m_status = _status;
    return OK;
}
/*@@End  Function SetStatus@@*/

/*@@Begin Function SetLatitude@@*/
status_t CGpsRmc::SetLatitude(double _latitude)
{
    this->m_latitude = _latitude;
    return OK;
}
/*@@End  Function SetLatitude@@*/

/*@@Begin Function SetLongitude@@*/
status_t CGpsRmc::SetLongitude(double _longitude)
{
    this->m_longitude = _longitude;
    return OK;
}
/*@@End  Function SetLongitude@@*/

/*@@Begin Function SetLatitudeHemisphere@@*/
status_t CGpsRmc::SetLatitudeHemisphere(char _latitude_hemisphere)
{
    this->m_latitude_hemisphere = _latitude_hemisphere;
    return OK;
}
/*@@End  Function SetLatitudeHemisphere@@*/

/*@@Begin Function SetLongitudeHemisphere@@*/
status_t CGpsRmc::SetLongitudeHemisphere(char _longitude_hemisphere)
{
    this->m_longitude_hemisphere = _longitude_hemisphere;
    return OK;
}
/*@@End  Function SetLongitudeHemisphere@@*/

/*@@Begin Function SetSpeedKnots@@*/
status_t CGpsRmc::SetSpeedKnots(double _speed_knots)
{
    this->m_speed_knots = _speed_knots;
    return OK;
}
/*@@End  Function SetSpeedKnots@@*/

/*@@Begin Function SetTrackAngle@@*/
status_t CGpsRmc::SetTrackAngle(double _track_angle)
{
    this->m_track_angle = _track_angle;
    return OK;
}
/*@@End  Function SetTrackAngle@@*/

/*@@Begin Function SetUtcDate@@*/
status_t CGpsRmc::SetUtcDate(CGpsUtcDate *_utc_date)
{
    ASSERT(_utc_date);
    return this->m_utc_date.Copy(_utc_date);
}
/*@@End  Function SetUtcDate@@*/

/*@@Begin Function SetMagneticVariation@@*/
status_t CGpsRmc::SetMagneticVariation(double _magnetic_variation)
{
    this->m_magnetic_variation = _magnetic_variation;
    return OK;
}
/*@@End  Function SetMagneticVariation@@*/

/*@@Begin Function SetDeclination@@*/
status_t CGpsRmc::SetDeclination(char _declination)
{
    this->m_declination = _declination;
    return OK;
}
/*@@End  Function SetDeclination@@*/


/*@@Begin Function GetMode@@*/
char CGpsRmc::GetMode()
{
    return m_mode;
}
/*@@End  Function GetMode@@*/

/*@@Begin Function SetMode@@*/
status_t CGpsRmc::SetMode(char _mode)
{
    this->m_mode = _mode;
    return OK;
}
/*@@End  Function SetMode@@*/
/*@@ Insert Function Here @@*/

status_t CGpsRmc::ToGpsString(CFileBase *out)
{
    ASSERT(out);

    HeadToString(out);
    out->Putc(',');
   
    if(HasUtcTime())
        m_utc_time.ToGpsString(out);
    out->Putc(',');

    if(HasStatus())
        out->Printf("%c",m_status);
    out->Putc(',');

    if(HasLatitude())
        LatitudeToGpsString(m_latitude,out);
    out->Putc(',');
    
    if(HasLatitudeHemisphere())
        out->Printf("%c",m_latitude_hemisphere);
    out->Putc(',');

    if(HasLongitude())
        LongitudeToGpsString(m_longitude,out);
    out->Putc(',');

    if(HasLatitudeHemisphere())
        out->Printf("%c",m_longitude_hemisphere);
    out->Putc(',');

    if(HasSpeedKnots())
        out->Printf("%.3f",m_speed_knots);
    out->Putc(',');

    if(HasTrackAngle())
        out->Printf("%.2f",m_track_angle);
    out->Putc(',');

    if(HasUtcDate())
        m_utc_date.ToGpsString(out);
    out->Putc(',');

    if(HasMagneticVariation())
        out->Printf("%.3f",m_magnetic_variation);

    out->Putc(',');

    if(HasDeclination())
        out->Putc(m_declination);

    out->Putc(',');
    
    if(HasMode())
        out->Putc(m_mode);

    return OK;
}
