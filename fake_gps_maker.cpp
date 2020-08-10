#include "fake_gps_maker.h"
#include "xlog.h"
#include "mem_tool.h"
#include "gps_parser.h"
#include "gps_helper.h"

#define MS_TO_KNOTS 1.9438446603753
#define MS_TO_KMH 3.6

#define ARRAY_SIZE(a) (int)(sizeof(a)/sizeof(a[0]))

struct simple_satallite_info{
    int prn;
    int elevation;
    int azimuth;
    int snr;
};

CFakeGpsMaker::CFakeGpsMaker()
{
    this->InitBasic();
}

CFakeGpsMaker::~CFakeGpsMaker()
{
    this->Destroy();
}

status_t CFakeGpsMaker::InitBasic()
{
    return OK;
}

status_t CFakeGpsMaker::Init()
{
    this->InitBasic();
    return OK;
}

status_t CFakeGpsMaker::Destroy()
{
    this->InitBasic();
    return OK;
}

static status_t add_gps_string(CFileBase *str, CFileBase *out)
{
    ASSERT(str && out);
    out->Puts("$");
    out->Puts(str);
    out->Printf("*%02X\r\n",CalcuChecksum(str));
    return OK;
}

status_t CFakeGpsMaker::BuildGpsText(v2x_hal::CCommonGpsAppData *gps, CFileBase *out)
{
    ASSERT(gps && out);

    CGpsUtcDate start_utc_data;
    start_utc_data.Init();
    start_utc_data.SetYear(gps->GetDate()->GetYear());
    start_utc_data.SetMonth(gps->GetDate()->GetMonth());
    start_utc_data.SetDay(gps->GetDate()->GetDay());

    CGpsUtcTime start_utc_time;
    start_utc_time.Init();
    start_utc_time.SetHour(gps->GetTime()->GetHour());
    start_utc_time.SetMinute(gps->GetTime()->GetMin());
    start_utc_time.SetSecond(gps->GetTime()->GetSec());
    start_utc_time.SetMillisecond(gps->GetTime()->GetSecFrac());

    LOCAL_MEM(mem);
    ///////////////////////////////////
    CGpsZda zda;
    zda.Init();
    zda.SetSignalSource("GN");
    zda.SetStatementIdentifier("ZDA");
    zda.SetUtcDate(&start_utc_data);
    zda.SetUtcTime(&start_utc_time);

    mem.SetSize(0);
    zda.ToGpsString(&mem);
    add_gps_string(&mem, out);
    ///////////////////////////////////
    CGpsRmc rmc;
    rmc.Init();
    rmc.SetSignalSource("GN");
    rmc.SetStatementIdentifier("RMC");
    rmc.SetStatus('A');
    rmc.SetLatitude(gps->GetPosition()->GetLatitude());
    rmc.SetLongitude(gps->GetPosition()->GetLongitude());
    rmc.SetLatitudeHemisphere('N');
    rmc.SetLongitudeHemisphere('E');
    rmc.SetTrackAngle(gps->GetDirection());
    rmc.SetSpeedKnots(gps->GetSpeed() * MS_TO_KNOTS);
    rmc.SetUtcDate(&start_utc_data);
    rmc.SetUtcTime(&start_utc_time);    
    rmc.SetMode('D');
    rmc.SetHasDeclination(false);
    rmc.SetHasMagneticVariation(false);

    mem.SetSize(0);
    rmc.ToGpsString(&mem);
    add_gps_string(&mem, out);
    ////////////////////////////////////////////////
    CGpsVtg vtg;
    vtg.Init();
    vtg.SetSignalSource("GN");
    vtg.SetStatementIdentifier("VTG");
    vtg.SetTrueTrackAngle(gps->GetDirection());
    vtg.SetHasMagneticTrackAngle(false);
    vtg.SetSpeedKilometersPerHour(gps->GetSpeed() * MS_TO_KMH);
    vtg.SetSpeedKnots(gps->GetSpeed() * MS_TO_KNOTS);
    mem.SetSize(0);
    vtg.ToGpsString(&mem);
    add_gps_string(&mem, out);
    ////////////////////////////////////////////////////
    CGpsGga gga;
    gga.Init();
    gga.SetSignalSource("GN");
    gga.SetStatementIdentifier("GGA");
    gga.SetUtcTime(&start_utc_time);
    gga.SetLatitude(gps->GetPosition()->GetLatitude());
    gga.SetLongitude(gps->GetPosition()->GetLongitude());
    gga.SetAltitude(gps->GetPosition()->GetAltitude());
    gga.SetQuality(2);
    gga.SetNumberOfSatellites(gps->GetSatNumber());
    gga.SetHorizontalDilution(0.62);
    gga.SetHasDifferentialGpsDataPeriod(false);

    mem.SetSize(0);
    gga.ToGpsString(&mem);
    add_gps_string(&mem, out);
    ////////////////////////////////////////////////////
    CGpsGsa gsa;
    gsa.SetSignalSource("GN");
    gsa.SetStatementIdentifier("GSA");

    gsa.SetFixMode('A');
    gsa.SetFixValue(3);
    gsa.SetDilutionOfPrecision(1.12);
    gsa.SetHorizontalDilutionOfPrecision(0.61);
    gsa.SetVerticalDilutionOfPrecision(0.94);

    int prns[]={11,1,18,30,28,8,7,22,42,17,3,50,85,75,74,84,76,86,69,};
    int gga_items = ARRAY_SIZE(prns)/12;
    if(ARRAY_SIZE(prns)%12)gga_items++;

    for(int i = 0; i < gga_items; i++)
    {
        gsa.ClearAllSatallitePrnFlags();
        for(int k = 0; k < 12; k++)
        {
            if(i*12+k >= ARRAY_SIZE(prns))
                break;
            gsa.SetHasSatellitePrnsElem(k,true);
            gsa.SetSatellitePrnsElem(k,prns[i*12+k]);
        }

        mem.SetSize(0);
        gsa.ToGpsString(&mem);
        add_gps_string(&mem, out);
    }

    /////////////////////////////////////////////////////////
    struct simple_satallite_info stat_infos[] = {
        {1,80,144,48},
        {3,17,149,32},
        {7,26,203,45},
        {8,36, 70,45},
        {10,3,29,30},
        {11,77,45,46},
        {17,21,272,43},
        {18,61,58,48},
        {22,29,129,45},
        {27,5,80,35},
        {28,49,312,41},
        {30,41,240,46},
        {40,7,255,30},
        {41,27,232,43},
        {42,37,149,35},
        {50,37,149,35},
    };

    int gsv_items = ARRAY_SIZE(stat_infos)/4;
    if(ARRAY_SIZE(stat_infos)%4) gsv_items ++;

    CGpsGsv gsv;
    gsv.Init();
    gsv.SetSignalSource("GP");
    gsv.SetStatementIdentifier("GSV");

    for(int i = 0; i < gsv_items; i++)
    {
        gsv.SetNumberOfSentences(gsv_items);
        gsv.SetSentence(i+1);
        gsv.SetNumberOfSatellites(ARRAY_SIZE(stat_infos));

        int num = ARRAY_SIZE(stat_infos) - i*4;
        if(num > 4)num = 4;

        gsv.AllocSatelliteInfos(num);

        for(int j = 0; j < num; j++)
        {           
            int k = i*4 + j;

            CGpsSatelliteInfo *info = gsv.GetSatelliteInfosElem(j);
            ASSERT(info);

            info->SetPrnNumber(prns[k]);
            info->SetElevation(stat_infos[k].elevation);
            info->SetAzimuth(stat_infos[k].azimuth);
            info->SetSnr(stat_infos[k].snr);            
        }

        mem.SetSize(0);
        gsv.ToGpsString(&mem);
        add_gps_string(&mem, out);
    }
    ///////////////////////////////////////////////
    CGpsGll gll;
    gll.Init();

    gll.SetSignalSource("GN");
    gll.SetStatementIdentifier("GLL");

    gll.SetLatitude(gps->GetPosition()->GetLatitude());
    gll.SetLongitude(gps->GetPosition()->GetLongitude());
    gll.SetUtcTime(&start_utc_time);
    gll.SetStatus('A');
    gll.SetMode('D');

    mem.SetSize(0);
    gll.ToGpsString(&mem);
    add_gps_string(&mem, out);
    /////////////////////////////////////////////////

    return OK;
}
