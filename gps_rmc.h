#ifndef __GPS_RMC_H
#define __GPS_RMC_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
#include "gps_utc_time.h"
#include "gps_utc_date.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsRmc
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CGpsUtcTime m_utc_time;
    char m_status;
    double m_latitude;
    double m_longitude;
    char m_latitude_hemisphere;
    char m_longitude_hemisphere;
    double m_speed_knots;
    double m_track_angle;
    CGpsUtcDate m_utc_date;
    double m_magnetic_variation;
    char m_declination;
    char m_mode;
/*##End Members##*/

public:
    CGpsRmc();
    virtual ~CGpsRmc();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsRmc *_p);
    int Comp(CGpsRmc *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CGpsUtcTime* GetUtcTime();
    char GetStatus();
    double GetLatitude();
    double GetLongitude();
    char GetLatitudeHemisphere();
    char GetLongitudeHemisphere();
    double GetSpeedKnots();
    double GetTrackAngle();
    CGpsUtcDate* GetUtcDate();
    double GetMagneticVariation();
    char GetDeclination();
    char GetMode();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetUtcTime(CGpsUtcTime *_utc_time);
    status_t SetStatus(char _status);
    status_t SetLatitude(double _latitude);
    status_t SetLongitude(double _longitude);
    status_t SetLatitudeHemisphere(char _latitude_hemisphere);
    status_t SetLongitudeHemisphere(char _longitude_hemisphere);
    status_t SetSpeedKnots(double _speed_knots);
    status_t SetTrackAngle(double _track_angle);
    status_t SetUtcDate(CGpsUtcDate *_utc_date);
    status_t SetMagneticVariation(double _magnetic_variation);
    status_t SetDeclination(char _declination);
    status_t SetMode(char _mode);
/*##End Setter_H##*/

    status_t ToGpsString(CFileBase *out);

    GPSDATA_FLAG1_FUNC(HasUtcTime,0x00000001);
    GPSDATA_FLAG1_FUNC(HasStatus,0x00000002);
    GPSDATA_FLAG1_FUNC(HasLatitude,0x00000004);
    GPSDATA_FLAG1_FUNC(HasLongitude,0x00000008);
    GPSDATA_FLAG1_FUNC(HasLatitudeHemisphere,0x00000010);
    GPSDATA_FLAG1_FUNC(HasLongitudeHemisphere,0x00000020);
    GPSDATA_FLAG1_FUNC(HasSpeedKnots,0x00000040);
    GPSDATA_FLAG1_FUNC(HasTrackAngle,0x00000080);
    GPSDATA_FLAG1_FUNC(HasUtcDate,0x00000100);
    GPSDATA_FLAG1_FUNC(HasMagneticVariation,0x00000200);
    GPSDATA_FLAG1_FUNC(HasDeclination,0x00000400);
    GPSDATA_FLAG1_FUNC(HasMode,0x00000800);

};

#endif
