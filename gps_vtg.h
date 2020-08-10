#ifndef __GPS_VTG_H
#define __GPS_VTG_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsVtg
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    double m_true_track_angle;
    double m_magnetic_track_angle;
    double m_speed_knots;
    double m_speed_kilometers_per_hour;
    char m_mode;
/*##End Members##*/

public:
    CGpsVtg();
    virtual ~CGpsVtg();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsVtg *_p);
    int Comp(CGpsVtg *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    double GetTrueTrackAngle();
    double GetMagneticTrackAngle();
    double GetSpeedKnots();
    double GetSpeedKilometersPerHour();
    char GetMode();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetTrueTrackAngle(double _true_track_angle);
    status_t SetMagneticTrackAngle(double _magnetic_track_angle);
    status_t SetSpeedKnots(double _speed_knots);
    status_t SetSpeedKilometersPerHour(double _speed_kilometers_per_hour);
    status_t SetMode(char _mode);
/*##End Setter_H##*/

    status_t ToGpsString(CFileBase *out);

    GPSDATA_FLAG1_FUNC(HasTrueTrackAngle,0x00000001);
    GPSDATA_FLAG1_FUNC(HasMagneticTrackAngle,0x00000002);
    GPSDATA_FLAG1_FUNC(HasSpeedKnots,0x00000004);
    GPSDATA_FLAG1_FUNC(HasSpeedKilometersPerHour,0x00000008);
    GPSDATA_FLAG1_FUNC(HasMode,0x00000010);
};

#endif
