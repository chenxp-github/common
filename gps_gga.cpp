#include "gps_gga.h"
#include "syslog.h"
#include "mem_tool.h"
#include "gps_helper.h"

CGpsGga::CGpsGga()
{
    this->InitBasic();
}

CGpsGga::~CGpsGga()
{
    this->Destroy();
}

status_t CGpsGga::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    this->m_utc_time.InitBasic();
    this->m_latitude = 0;
    this->m_longitude = 0;
    this->m_altitude = 0;
    this->m_quality = 0;
    this->m_number_of_satellites = 0;
    this->m_horizontal_dilution = 0;
    this->m_height_of_geoid = 0;
    this->m_differential_gps_data_period = 0;
    this->m_the_differential_reference_base_station_number = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsGga::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
    this->m_utc_time.Init();
/*##End Init##*/
    return OK;
}

status_t CGpsGga::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
    this->m_utc_time.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsGga::Copy(CGpsGga *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    this->m_utc_time.Copy(&_p->m_utc_time);
    this->m_latitude = _p->m_latitude;
    this->m_longitude = _p->m_longitude;
    this->m_altitude = _p->m_altitude;
    this->m_quality = _p->m_quality;
    this->m_number_of_satellites = _p->m_number_of_satellites;
    this->m_horizontal_dilution = _p->m_horizontal_dilution;
    this->m_height_of_geoid = _p->m_height_of_geoid;
    this->m_differential_gps_data_period = _p->m_differential_gps_data_period;
    this->m_the_differential_reference_base_station_number = _p->m_the_differential_reference_base_station_number;
/*##End Copy##*/
    return OK;
}

int CGpsGga::Comp(CGpsGga *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsGga::Print(CFileBase *_buf)
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
    _buf->Log("latitude = %f",m_latitude);
    _buf->Log("longitude = %f",m_longitude);
    _buf->Log("altitude = %f",m_altitude);
    _buf->Log("quality = %d",m_quality);
    _buf->Log("number_of_satellites = %d",m_number_of_satellites);
    _buf->Log("horizontal_dilution = %f",m_horizontal_dilution);
    _buf->Log("height_of_geoid = %f",m_height_of_geoid);
    _buf->Log("differential_gps_data_period = %d",m_differential_gps_data_period);
    _buf->Log("the_differential_reference_base_station_number = %d",m_the_differential_reference_base_station_number);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetUtcTime@@*/
CGpsUtcTime* CGpsGga::GetUtcTime()
{
    return &m_utc_time;
}
/*@@End  Function GetUtcTime@@*/

/*@@Begin Function GetLatitude@@*/
double CGpsGga::GetLatitude()
{
    return m_latitude;
}
/*@@End  Function GetLatitude@@*/

/*@@Begin Function GetLongitude@@*/
double CGpsGga::GetLongitude()
{
    return m_longitude;
}
/*@@End  Function GetLongitude@@*/

/*@@Begin Function GetAltitude@@*/
double CGpsGga::GetAltitude()
{
    return m_altitude;
}
/*@@End  Function GetAltitude@@*/

/*@@Begin Function GetQuality@@*/
uint8_t CGpsGga::GetQuality()
{
    return m_quality;
}
/*@@End  Function GetQuality@@*/

/*@@Begin Function GetNumberOfSatellites@@*/
int CGpsGga::GetNumberOfSatellites()
{
    return m_number_of_satellites;
}
/*@@End  Function GetNumberOfSatellites@@*/

/*@@Begin Function GetHorizontalDilution@@*/
double CGpsGga::GetHorizontalDilution()
{
    return m_horizontal_dilution;
}
/*@@End  Function GetHorizontalDilution@@*/

/*@@Begin Function GetHeightOfGeoid@@*/
double CGpsGga::GetHeightOfGeoid()
{
    return m_height_of_geoid;
}
/*@@End  Function GetHeightOfGeoid@@*/

/*@@Begin Function SetUtcTime@@*/
status_t CGpsGga::SetUtcTime(CGpsUtcTime *_utc_time)
{
    ASSERT(_utc_time);
    return this->m_utc_time.Copy(_utc_time);
}
/*@@End  Function SetUtcTime@@*/

/*@@Begin Function SetLatitude@@*/
status_t CGpsGga::SetLatitude(double _latitude)
{
    this->m_latitude = _latitude;
    return OK;
}
/*@@End  Function SetLatitude@@*/

/*@@Begin Function SetLongitude@@*/
status_t CGpsGga::SetLongitude(double _longitude)
{
    this->m_longitude = _longitude;
    return OK;
}
/*@@End  Function SetLongitude@@*/

/*@@Begin Function SetAltitude@@*/
status_t CGpsGga::SetAltitude(double _altitude)
{
    this->m_altitude = _altitude;
    return OK;
}
/*@@End  Function SetAltitude@@*/

/*@@Begin Function SetQuality@@*/
status_t CGpsGga::SetQuality(uint8_t _quality)
{
    this->m_quality = _quality;
    return OK;
}
/*@@End  Function SetQuality@@*/

/*@@Begin Function SetNumberOfSatellites@@*/
status_t CGpsGga::SetNumberOfSatellites(int _number_of_satellites)
{
    this->m_number_of_satellites = _number_of_satellites;
    return OK;
}
/*@@End  Function SetNumberOfSatellites@@*/

/*@@Begin Function SetHorizontalDilution@@*/
status_t CGpsGga::SetHorizontalDilution(double _horizontal_dilution)
{
    this->m_horizontal_dilution = _horizontal_dilution;
    return OK;
}
/*@@End  Function SetHorizontalDilution@@*/

/*@@Begin Function SetHeightOfGeoid@@*/
status_t CGpsGga::SetHeightOfGeoid(double _height_of_geoid)
{
    this->m_height_of_geoid = _height_of_geoid;
    return OK;
}
/*@@End  Function SetHeightOfGeoid@@*/


/*@@Begin Function GetDifferentialGpsDataPeriod@@*/
int CGpsGga::GetDifferentialGpsDataPeriod()
{
    return m_differential_gps_data_period;
}
/*@@End  Function GetDifferentialGpsDataPeriod@@*/

/*@@Begin Function GetTheDifferentialReferenceBaseStationNumber@@*/
int CGpsGga::GetTheDifferentialReferenceBaseStationNumber()
{
    return m_the_differential_reference_base_station_number;
}
/*@@End  Function GetTheDifferentialReferenceBaseStationNumber@@*/

/*@@Begin Function SetDifferentialGpsDataPeriod@@*/
status_t CGpsGga::SetDifferentialGpsDataPeriod(int _differential_gps_data_period)
{
    this->m_differential_gps_data_period = _differential_gps_data_period;
    return OK;
}
/*@@End  Function SetDifferentialGpsDataPeriod@@*/

/*@@Begin Function SetTheDifferentialReferenceBaseStationNumber@@*/
status_t CGpsGga::SetTheDifferentialReferenceBaseStationNumber(int _the_differential_reference_base_station_number)
{
    this->m_the_differential_reference_base_station_number = _the_differential_reference_base_station_number;
    return OK;
}
/*@@End  Function SetTheDifferentialReferenceBaseStationNumber@@*/
/*@@ Insert Function Here @@*/

status_t CGpsGga::ToGpsString(CFileBase *out)
{
    ASSERT(out);
    
    HeadToString(out);
    out->Putc(',');

    if(HasUtcTime())
        m_utc_time.ToGpsString(out);
    out->Putc(',');

    if(HasLatitude())
        LatitudeToGpsString(m_latitude,out);
    out->Putc(',');
    out->Puts("N,");

    if(HasLongitude())
        LongitudeToGpsString(m_longitude,out);
    out->Putc(',');
    out->Puts("E,");

    if(HasQuality())
        out->Printf("%d",m_quality);
    out->Puts(",");

    if(HasNumberOfSatellites())
        out->Printf("%d",m_number_of_satellites);
    out->Puts(",");

    if(HasHorizontalDilution())
        FloatToFixedGpsString(m_horizontal_dilution,0,2,out);
    out->Putc(',');

    if(HasAltitude())
        FloatToFixedGpsString(m_altitude,0,1,out);
    out->Puts(",M,");    

    if(HasHorizontalDilution())
        FloatToFixedGpsString(m_height_of_geoid,0,1,out);
    out->Puts(",M,");

    if(HasDifferentialGpsDataPeriod())
        out->Printf("%d",m_differential_gps_data_period);
    out->Puts(",");

    if(HasTheDifferentialReferenceBaseStationNumber())
        out->Printf("%04d",m_the_differential_reference_base_station_number);
    return OK;
}
