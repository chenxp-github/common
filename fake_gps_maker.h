#ifndef __FAKE_GPS_MAKER_H
#define __FAKE_GPS_MAKER_H

#include "mem.h"
#include "memfile.h"
#include "v2x_common_gps_app_data.h"

class CFakeGpsMaker{

public:
    CFakeGpsMaker();
    virtual ~CFakeGpsMaker();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    static status_t BuildGpsText(v2x_hal::CCommonGpsAppData *gps, CFileBase *out);
};

#endif
