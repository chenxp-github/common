#ifndef __GPS_SATELLITE_INFO_H
#define __GPS_SATELLITE_INFO_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsSatelliteInfo
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_prn_number;
    int m_elevation;
    int m_azimuth;
    int m_snr;
/*##End Members##*/
public:
    CGpsSatelliteInfo();
    virtual ~CGpsSatelliteInfo();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsSatelliteInfo *_p);
    int Comp(CGpsSatelliteInfo *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetPrnNumber();
    int GetElevation();
    int GetAzimuth();
    int GetSnr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetPrnNumber(int _prn_number);
    status_t SetElevation(int _elevation);
    status_t SetAzimuth(int _azimuth);
    status_t SetSnr(int _snr);
/*##End Setter_H##*/
    GPSDATA_FLAG1_FUNC(HasPrnNumber,0x00000001);
    GPSDATA_FLAG1_FUNC(HasElevation,0x00000002);
    GPSDATA_FLAG1_FUNC(HasAzimuth,0x00000004);
    GPSDATA_FLAG1_FUNC(HasSnr,0x00000008);

    GPSDATA_FLAG1_FUNC(HasAll,0x00000010);
};

#endif
