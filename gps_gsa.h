#ifndef __GPS_GSA_H
#define __GPS_GSA_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsGsa
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    char m_fix_mode;
    int m_fix_value;
    int m_satellite_prns[12];
    int m_has_satellite_prns[12];
    double m_dilution_of_precision;
    double m_horizontal_dilution_of_precision;
    double m_vertical_dilution_of_precision;
/*##End Members##*/

public:
    CGpsGsa();
    virtual ~CGpsGsa();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsGsa *_p);
    int Comp(CGpsGsa *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    char GetFixMode();
    int GetFixValue();
    int* GetSatellitePrns();
    int GetSatellitePrnsSize();
    int GetSatellitePrnsElem(int _index);
    int* GetHasSatellitePrns();
    int GetHasSatellitePrnsSize();
    int GetHasSatellitePrnsElem(int _index);
    double GetDilutionOfPrecision();
    double GetHorizontalDilutionOfPrecision();
    double GetVerticalDilutionOfPrecision();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetFixMode(char _fix_mode);
    status_t SetFixValue(int _fix_value);
    status_t SetSatellitePrns(int _satellite_prns[], int _len);
    status_t SetSatellitePrns(int *_satellite_prns[], int _len);
    status_t SetSatellitePrnsElem(int _index,int _satellite_prns);
    status_t SetHasSatellitePrns(int _has_satellite_prns[], int _len);
    status_t SetHasSatellitePrns(int *_has_satellite_prns[], int _len);
    status_t SetHasSatellitePrnsElem(int _index,int _has_satellite_prns);
    status_t SetDilutionOfPrecision(double _dilution_of_precision);
    status_t SetHorizontalDilutionOfPrecision(double _horizontal_dilution_of_precision);
    status_t SetVerticalDilutionOfPrecision(double _vertical_dilution_of_precision);
/*##End Setter_H##*/
    status_t ToGpsString(CFileBase *out);

    GPSDATA_FLAG1_FUNC(HasFixMode,0x00000001);
    GPSDATA_FLAG1_FUNC(HasFixValue,0x00000002);
    GPSDATA_FLAG1_FUNC(HasSatellitePrns,0x00000004);
    GPSDATA_FLAG1_FUNC(HasDilutionOfPrecision,0x00000008);
    GPSDATA_FLAG1_FUNC(HasHorizontalDilutionOfPrecision,0x00000010);
    GPSDATA_FLAG1_FUNC(HasVerticalDilutionOfPrecision,0x00000020);

    status_t ClearAllSatallitePrnFlags();
};

#endif
