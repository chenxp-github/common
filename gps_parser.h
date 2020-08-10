#ifndef __GPS_PARSER_H
#define __GPS_PARSER_H

#include "closure.h"
#include "gps_data.h"
#include "gps_utc_time.h"
#include "gps_utc_date.h"
#include "gps_rmc.h"
#include "gps_vtg.h"
#include "gps_gga.h"
#include "gps_gsa.h"
#include "gps_gll.h"
#include "gps_zda.h"
#include "gps_gsv.h"

class CGpsParser{
public:
    WEAK_REF_DEFINE();
public:
    bool m_HitStar;
    bool m_CurIsEmpty;
public:		
    CGpsParser();
    ~CGpsParser();

    status_t Init();
    status_t InitBasic();
    status_t Destroy();

    status_t NextItem(CFileBase *line, CFileBase *item);    
    bool IsCurEmpty();
    bool IsCurValid();

    static status_t ParseHead(CMem *line,CGpsData *data);
    static status_t ParseUtcTime(CMem *str, CGpsUtcTime *tm);
    static status_t ParseUtcDate(CMem *str, CGpsUtcDate *date);
    static status_t ParseLatitude(CMem *str, double *deg,double *sec);
    static status_t ParseLogitude(CMem *str, double *deg,double *sec);
    static status_t ParseRmc(CMem *line, CGpsRmc *rmc);
    static status_t ParseVtg(CMem *line, CGpsVtg *vtg);
    static status_t ParseGga(CMem *line, CGpsGga *gga);
    static status_t ParseGsa(CMem *line, CGpsGsa *gsa);
    static status_t ParseGll(CMem *line, CGpsGll *gll);
    static status_t ParseZda(CMem *line, CGpsZda *zda);
    static status_t ParseGsv(CMem *line, CGpsGsv *gsv);

    static status_t ParseSingleLine(CMem *line, CClosure *closure);
    static status_t ParseWholeFile(CFileBase *file, CClosure *closure);    
};



#endif
