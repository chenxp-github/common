#include "gps_vtg.h"
#include "syslog.h"
#include "mem_tool.h"
#include "gps_helper.h"

CGpsVtg::CGpsVtg()
{
    this->InitBasic();
}

CGpsVtg::~CGpsVtg()
{
    this->Destroy();
}

status_t CGpsVtg::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    this->m_true_track_angle = 0;
    this->m_magnetic_track_angle = 0;
    this->m_speed_knots = 0;
    this->m_speed_kilometers_per_hour = 0;
    this->m_mode = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsVtg::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
/*##End Init##*/
    return OK;
}

status_t CGpsVtg::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsVtg::Copy(CGpsVtg *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    this->m_true_track_angle = _p->m_true_track_angle;
    this->m_magnetic_track_angle = _p->m_magnetic_track_angle;
    this->m_speed_knots = _p->m_speed_knots;
    this->m_speed_kilometers_per_hour = _p->m_speed_kilometers_per_hour;
    this->m_mode = _p->m_mode;
/*##End Copy##*/
    return OK;
}

int CGpsVtg::Comp(CGpsVtg *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsVtg::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CGpsData::Print(_buf);
    _buf->Log("true_track_angle = %f",m_true_track_angle);
    _buf->Log("magnetic_track_angle = %f",m_magnetic_track_angle);
    _buf->Log("speed_knots = %f",m_speed_knots);
    _buf->Log("speed_kilometers_per_hour = %f",m_speed_kilometers_per_hour);
    _buf->Log("mode = %c",m_mode);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetTrueTrackAngle@@*/
double CGpsVtg::GetTrueTrackAngle()
{
    return m_true_track_angle;
}
/*@@End  Function GetTrueTrackAngle@@*/

/*@@Begin Function GetMagneticTrackAngle@@*/
double CGpsVtg::GetMagneticTrackAngle()
{
    return m_magnetic_track_angle;
}
/*@@End  Function GetMagneticTrackAngle@@*/

/*@@Begin Function GetSpeedKnots@@*/
double CGpsVtg::GetSpeedKnots()
{
    return m_speed_knots;
}
/*@@End  Function GetSpeedKnots@@*/

/*@@Begin Function GetSpeedKilometersPerHour@@*/
double CGpsVtg::GetSpeedKilometersPerHour()
{
    return m_speed_kilometers_per_hour;
}
/*@@End  Function GetSpeedKilometersPerHour@@*/

/*@@Begin Function SetTrueTrackAngle@@*/
status_t CGpsVtg::SetTrueTrackAngle(double _true_track_angle)
{
    this->m_true_track_angle = _true_track_angle;
    return OK;
}
/*@@End  Function SetTrueTrackAngle@@*/

/*@@Begin Function SetMagneticTrackAngle@@*/
status_t CGpsVtg::SetMagneticTrackAngle(double _magnetic_track_angle)
{
    this->m_magnetic_track_angle = _magnetic_track_angle;
    return OK;
}
/*@@End  Function SetMagneticTrackAngle@@*/

/*@@Begin Function SetSpeedKnots@@*/
status_t CGpsVtg::SetSpeedKnots(double _speed_knots)
{
    this->m_speed_knots = _speed_knots;
    return OK;
}
/*@@End  Function SetSpeedKnots@@*/

/*@@Begin Function SetSpeedKilometersPerHour@@*/
status_t CGpsVtg::SetSpeedKilometersPerHour(double _speed_kilometers_per_hour)
{
    this->m_speed_kilometers_per_hour = _speed_kilometers_per_hour;
    return OK;
}
/*@@End  Function SetSpeedKilometersPerHour@@*/


/*@@Begin Function GetMode@@*/
char CGpsVtg::GetMode()
{
    return m_mode;
}
/*@@End  Function GetMode@@*/

/*@@Begin Function SetMode@@*/
status_t CGpsVtg::SetMode(char _mode)
{
    this->m_mode = _mode;
    return OK;
}
/*@@End  Function SetMode@@*/
/*@@ Insert Function Here @@*/
status_t CGpsVtg::ToGpsString(CFileBase *out)
{
    ASSERT(out);
        
    HeadToString(out);
    out->Putc(',');
    
    if(HasTrueTrackAngle())
        FloatToFixedGpsString(m_true_track_angle,0,2,out);
    out->Puts(",T,");

    if(HasMagneticTrackAngle())
    {
        FloatToFixedGpsString(m_magnetic_track_angle,0,2,out);
    }

    out->Puts(",M,");

    if(HasSpeedKnots())
        FloatToFixedGpsString(m_speed_knots,0,3,out);
    out->Puts(",N,");

    if(HasSpeedKilometersPerHour())
        FloatToFixedGpsString(m_speed_kilometers_per_hour,0,3,out);
    out->Puts(",K,");

    if(HasMode())
        out->Printf("%c",m_mode);

    return OK;
}
