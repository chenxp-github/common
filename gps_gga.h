#ifndef __GPS_GGA_H
#define __GPS_GGA_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
#include "gps_utc_time.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsGga
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CGpsUtcTime m_utc_time;
    double m_latitude;
    double m_longitude;
    double m_altitude;
    uint8_t m_quality;
    int m_number_of_satellites;
    double m_horizontal_dilution;
    double m_height_of_geoid;
    int m_differential_gps_data_period;
    int m_the_differential_reference_base_station_number;
/*##End Members##*/

public:
    CGpsGga();
    virtual ~CGpsGga();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsGga *_p);
    int Comp(CGpsGga *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CGpsUtcTime* GetUtcTime();
    double GetLatitude();
    double GetLongitude();
    double GetAltitude();
    uint8_t GetQuality();
    int GetNumberOfSatellites();
    double GetHorizontalDilution();
    double GetHeightOfGeoid();
    int GetDifferentialGpsDataPeriod();
    int GetTheDifferentialReferenceBaseStationNumber();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetUtcTime(CGpsUtcTime *_utc_time);
    status_t SetLatitude(double _latitude);
    status_t SetLongitude(double _longitude);
    status_t SetAltitude(double _altitude);
    status_t SetQuality(uint8_t _quality);
    status_t SetNumberOfSatellites(int _number_of_satellites);
    status_t SetHorizontalDilution(double _horizontal_dilution);
    status_t SetHeightOfGeoid(double _height_of_geoid);
    status_t SetDifferentialGpsDataPeriod(int _differential_gps_data_period);
    status_t SetTheDifferentialReferenceBaseStationNumber(int _the_differential_reference_base_station_number);
/*##End Setter_H##*/
    status_t ToGpsString(CFileBase *out);

    GPSDATA_FLAG1_FUNC(HasUtcTime,0x00000001);
    GPSDATA_FLAG1_FUNC(HasLatitude,0x00000002);
    GPSDATA_FLAG1_FUNC(HasLongitude,0x00000004);
    GPSDATA_FLAG1_FUNC(HasAltitude,0x00000008);
    GPSDATA_FLAG1_FUNC(HasQuality,0x00000010);
    GPSDATA_FLAG1_FUNC(HasNumberOfSatellites,0x00000020);
    GPSDATA_FLAG1_FUNC(HasHorizontalDilution,0x00000040);
    GPSDATA_FLAG1_FUNC(HasHeightOfGeoid,0x00000080);
    GPSDATA_FLAG1_FUNC(HasDifferentialGpsDataPeriod,0x00000100);
    GPSDATA_FLAG1_FUNC(HasTheDifferentialReferenceBaseStationNumber,0x00000200);
};

#endif
