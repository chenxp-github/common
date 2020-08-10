#ifndef __GPS_GLL_H
#define __GPS_GLL_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
#include "gps_utc_time.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsGll
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    double m_latitude;
    double m_longitude;
    CGpsUtcTime m_utc_time;
    char m_status;
    char m_mode;
/*##End Members##*/

public:
    CGpsGll();
    virtual ~CGpsGll();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsGll *_p);
    int Comp(CGpsGll *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    double GetLatitude();
    double GetLongitude();
    CGpsUtcTime* GetUtcTime();
    char GetStatus();
    char GetMode();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetLatitude(double _latitude);
    status_t SetLongitude(double _longitude);
    status_t SetUtcTime(CGpsUtcTime *_utc_time);
    status_t SetStatus(char _status);
    status_t SetMode(char _mode);
/*##End Setter_H##*/
    status_t ToGpsString(CFileBase *out);

    GPSDATA_FLAG1_FUNC(HasLatitude,0x00000001);
    GPSDATA_FLAG1_FUNC(HasLongitude,0x00000002);
    GPSDATA_FLAG1_FUNC(HasUtcTime,0x00000004);
    GPSDATA_FLAG1_FUNC(HasStatus,0x00000008);
    GPSDATA_FLAG1_FUNC(HasMode,0x00000010);
};

#endif
