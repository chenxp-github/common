#include "gps_satellite_info.h"
#include "syslog.h"
#include "mem_tool.h"

CGpsSatelliteInfo::CGpsSatelliteInfo()
{
    this->InitBasic();
}

CGpsSatelliteInfo::~CGpsSatelliteInfo()
{
    this->Destroy();
}

status_t CGpsSatelliteInfo::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    this->m_prn_number = 0;
    this->m_elevation = 0;
    this->m_azimuth = 0;
    this->m_snr = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsSatelliteInfo::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
/*##End Init##*/
    return OK;
}

status_t CGpsSatelliteInfo::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsSatelliteInfo::Copy(CGpsSatelliteInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    this->m_prn_number = _p->m_prn_number;
    this->m_elevation = _p->m_elevation;
    this->m_azimuth = _p->m_azimuth;
    this->m_snr = _p->m_snr;
/*##End Copy##*/
    return OK;
}

int CGpsSatelliteInfo::Comp(CGpsSatelliteInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsSatelliteInfo::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CGpsData::Print(_buf);
    _buf->Log("prn_number = %d",m_prn_number);
    _buf->Log("elevation = %d",m_elevation);
    _buf->Log("azimuth = %d",m_azimuth);
    _buf->Log("snr = %d",m_snr);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetPrnNumber@@*/
int CGpsSatelliteInfo::GetPrnNumber()
{
    return m_prn_number;
}
/*@@End  Function GetPrnNumber@@*/

/*@@Begin Function GetElevation@@*/
int CGpsSatelliteInfo::GetElevation()
{
    return m_elevation;
}
/*@@End  Function GetElevation@@*/

/*@@Begin Function GetAzimuth@@*/
int CGpsSatelliteInfo::GetAzimuth()
{
    return m_azimuth;
}
/*@@End  Function GetAzimuth@@*/

/*@@Begin Function GetSnr@@*/
int CGpsSatelliteInfo::GetSnr()
{
    return m_snr;
}
/*@@End  Function GetSnr@@*/

/*@@Begin Function SetPrnNumber@@*/
status_t CGpsSatelliteInfo::SetPrnNumber(int _prn_number)
{
    this->m_prn_number = _prn_number;
    return OK;
}
/*@@End  Function SetPrnNumber@@*/

/*@@Begin Function SetElevation@@*/
status_t CGpsSatelliteInfo::SetElevation(int _elevation)
{
    this->m_elevation = _elevation;
    return OK;
}
/*@@End  Function SetElevation@@*/

/*@@Begin Function SetAzimuth@@*/
status_t CGpsSatelliteInfo::SetAzimuth(int _azimuth)
{
    this->m_azimuth = _azimuth;
    return OK;
}
/*@@End  Function SetAzimuth@@*/

/*@@Begin Function SetSnr@@*/
status_t CGpsSatelliteInfo::SetSnr(int _snr)
{
    this->m_snr = _snr;
    return OK;
}
/*@@End  Function SetSnr@@*/

/*@@ Insert Function Here @@*/
