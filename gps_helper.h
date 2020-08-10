#ifndef __GPS_HELPER_H
#define __GPS_HELPER_H

#include "mem.h"
#include "memfile.h"

status_t FloatToFixedGpsString(double v, int len1, int len2, CFileBase *out);
//status_t FloatToTrimedString(double v, CFileBase *out);
status_t LatitudeToGpsString(double lat, CFileBase *out);
status_t LongitudeToGpsString(double lon, CFileBase *out);
status_t AltitudeToGpsString(double alt, CFileBase *out);
uint8_t CalcuChecksum(CFileBase *file);

#endif
