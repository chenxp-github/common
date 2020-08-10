#ifndef __GPS_ZDA_H
#define __GPS_ZDA_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
#include "gps_utc_time.h"
#include "gps_utc_date.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsZda
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CGpsUtcTime m_utc_time;
    CGpsUtcDate m_utc_date;
    int m_local_zone_hours;
    int m_local_zone_minutes;
/*##End Members##*/
    status_t ToGpsString(CFileBase *out);
public:
    CGpsZda();
    virtual ~CGpsZda();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsZda *_p);
    int Comp(CGpsZda *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CGpsUtcTime* GetUtcTime();
    CGpsUtcDate* GetUtcDate();
    int GetLocalZoneHours();
    int GetLocalZoneMinutes();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetUtcTime(CGpsUtcTime *_utc_time);
    status_t SetUtcDate(CGpsUtcDate *_utc_date);
    status_t SetLocalZoneHours(int _local_zone_hours);
    status_t SetLocalZoneMinutes(int _local_zone_minutes);
/*##End Setter_H##*/

    GPSDATA_FLAG1_FUNC(HasUtcTime,0x00000001);
    GPSDATA_FLAG1_FUNC(HasUtcDate,0x00000002);
    GPSDATA_FLAG1_FUNC(HasLocalZoneHours,0x00000004);
    GPSDATA_FLAG1_FUNC(HasLocalZoneMinutes,0x00000008);
};

#endif
