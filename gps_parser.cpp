#include "gps_parser.h"
#include "syslog.h"
#include "mem_tool.h"

CGpsParser::CGpsParser()
{
    this->InitBasic();
}

CGpsParser::~CGpsParser()
{
    this->Destroy();
}

status_t CGpsParser::InitBasic()
{
    WEAK_REF_CLEAR();
    m_HitStar = false;
    m_CurIsEmpty = false;
    return OK;
}

status_t CGpsParser::Init()
{
    this->InitBasic();
    
    return OK;
}

status_t CGpsParser::Destroy()
{
    WEAK_REF_DESTROY();
    this->InitBasic();
    return OK;
}

status_t CGpsParser::NextItem(CFileBase *line, CFileBase *item)
{
    ASSERT(line && item);
    item->SetSize(0);
    
    if(line->IsEnd())
    {
        m_CurIsEmpty = true;
        return ERROR;
    }

    m_CurIsEmpty = false;

    if(m_HitStar)
    {
        m_HitStar = false;
        item->Putc('*');
        return OK;
    }

    while(!line->IsEnd())
    {
        char ch = line->Getc();
        if(ch == ',')
        {
            break;
        }

        if(ch == '*')
        {
            m_HitStar = true;
            break;
        }
        item->Putc(ch);
    }

    m_CurIsEmpty = item->StrLen() == 0;
    return OK;
}

bool CGpsParser::IsCurEmpty()
{
    return m_CurIsEmpty;
}

bool CGpsParser::IsCurValid()
{
    return !m_CurIsEmpty;
}

status_t CGpsParser::ParseHead(CMem *line,CGpsData *data)
{
    ASSERT(line && data);

    line->Seek(0);
    char ch = line->Getc();
    ASSERT(ch == '$');
    
    char ss[4];
    
    ss[0] = line->Getc();
    ss[1] = line->Getc();
    ss[2] = 0;
    data->SetSignalSource(ss);
    
    LOCAL_MEM(mem);
    
    while(!line->IsEnd())
    {
        ch = line->Getc();
        if(ch == ',' || ch == ' ')
        {
            break;
        }
        
        mem.Putc(ch);
    }
    
    data->SetStatementIdentifier(&mem);
    return OK;
}

status_t CGpsParser::ParseRmc(CMem *line, CGpsRmc *rmc)
{
    ASSERT(line && rmc);
    
    rmc->ClearAllFlags();

    LOCAL_MEM(mem);
    line->Seek(0);

    CGpsParser parser;
    parser.Init();
    parser.NextItem(line,&mem); //$GNRMC

    ParseHead(&mem,rmc);

    parser.NextItem(line,&mem);
    ParseUtcTime(&mem,rmc->GetUtcTime());
    rmc->SetHasUtcTime(parser.IsCurValid());

    parser.NextItem(line,&mem);
    rmc->SetStatus(mem.C(0));
    rmc->SetHasStatus(parser.IsCurValid());

    double deg = 0,sec = 0;
    parser.NextItem(line,&mem);
    ParseLatitude(&mem,&deg,&sec); 
    rmc->SetLatitude(deg+sec/60.0);
    rmc->SetHasLatitude(parser.IsCurValid());
   
    parser.NextItem(line,&mem);
    rmc->SetLatitudeHemisphere(mem.C(0));
    rmc->SetHasLatitudeHemisphere(parser.IsCurValid());

    parser.NextItem(line,&mem);
    ParseLogitude(&mem,&deg,&sec); 
    rmc->SetLongitude(deg+sec/60.0);
    rmc->SetHasLongitude(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
    rmc->SetLongitudeHemisphere(mem.C(0));
    rmc->SetHasLongitudeHemisphere(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
    rmc->SetSpeedKnots(atof(mem.CStr()));
    rmc->SetHasSpeedKnots(parser.IsCurValid());

    parser.NextItem(line,&mem);
    rmc->SetTrackAngle(atof(mem.CStr()));
    rmc->SetHasTrackAngle(parser.IsCurValid());

    parser.NextItem(line,&mem);
    ParseUtcDate(&mem,rmc->GetUtcDate());
    rmc->SetHasUtcDate(parser.IsCurValid());

    parser.NextItem(line,&mem);
    rmc->SetMagneticVariation(atof(mem.CStr()));
    rmc->SetHasMagneticVariation(parser.IsCurValid());

    parser.NextItem(line,&mem);
    rmc->SetDeclination(mem.C(0));
    rmc->SetHasDeclination(parser.IsCurValid());

    parser.NextItem(line,&mem);
    rmc->SetMode(mem.C(0));
    rmc->SetHasMode(parser.IsCurValid());

    return OK;
}

status_t CGpsParser::ParseVtg(CMem *line, CGpsVtg *vtg)
{
    ASSERT(line && vtg);
    
    LOCAL_MEM(mem);
    line->Seek(0);
    
    CGpsParser parser;
    parser.Init();
    parser.NextItem(line,&mem); //$GNVTG
    
    ParseHead(&mem,vtg);
    
    parser.NextItem(line,&mem);
    vtg->SetTrueTrackAngle(atof(mem.CStr()));
    vtg->SetHasTrueTrackAngle(parser.IsCurValid());

    parser.NextItem(line,&mem);
    //ASSERT(mem.StrCmp("T") == 0);

    parser.NextItem(line,&mem);
    vtg->SetMagneticTrackAngle(atof(mem.CStr()));
    vtg->SetHasMagneticTrackAngle(parser.IsCurValid());

    parser.NextItem(line,&mem);
    //ASSERT(mem.StrCmp("M") == 0);

    parser.NextItem(line,&mem);
    vtg->SetSpeedKnots(atof(mem.CStr()));
    vtg->SetHasSpeedKnots(parser.IsCurValid());

    parser.NextItem(line,&mem);
    //ASSERT(mem.StrCmp("N") == 0);

    parser.NextItem(line,&mem);
    vtg->SetSpeedKilometersPerHour(atof(mem.CStr()));
    vtg->SetHasSpeedKilometersPerHour(parser.IsCurValid());

    parser.NextItem(line,&mem);
    //ASSERT(mem.StrCmp("K") == 0);

    parser.NextItem(line,&mem);
    vtg->SetMode(mem.C(0));
    vtg->SetHasMode(parser.IsCurValid());

    return OK;
}

status_t CGpsParser::ParseUtcTime(CMem *str, CGpsUtcTime *tm)
{
    ASSERT(str && tm);
    if(str->StrLen() < 9)
    {
        return ERROR;
    }
   
    char buf[4];
    const char *p = str->CStr();

    buf[0] = p[0];
    buf[1] = p[1];
    buf[2] = 0;
    tm->SetHour(atoi(buf));

    buf[0] = p[2];
    buf[1] = p[3];
    buf[2] = 0;
    tm->SetMinute(atoi(buf));

    buf[0] = p[4];
    buf[1] = p[5];
    buf[2] = 0;
    tm->SetSecond(atoi(buf));

    ASSERT(p[6] == '.');

    buf[0] = p[7];
    buf[1] = p[8];
    buf[2] = 0;
    tm->SetMillisecond(atoi(buf)*10);

    return OK;
}

status_t CGpsParser::ParseUtcDate(CMem *str, CGpsUtcDate *date)
{
    ASSERT(str && date);
    if(str->StrLen() < 6)
    {
        return ERROR;
    }
    
    char buf[4];
    const char *p = str->CStr();
    
    buf[0] = p[0];
    buf[1] = p[1];
    buf[2] = 0;
    date->SetMonth(atoi(buf));
    
    buf[0] = p[2];
    buf[1] = p[3];
    buf[2] = 0;
    date->SetDay(atoi(buf));
    
    buf[0] = p[4];
    buf[1] = p[5];
    buf[2] = 0;
    date->SetYear(atoi(buf)+2000);
           
    return OK;
}

status_t CGpsParser::ParseLatitude(CMem *str, double *deg,double *sec)
{
    ASSERT(str && deg && sec);
    
    if(str->StrLen() < 6)
    {
        *deg = 0;
        *sec = 0;
        return ERROR;
    }
    
    char buf[4];
    const char *p = str->CStr();

    buf[0] = p[0];
    buf[1] = p[1];
    buf[2] = 0;

    *deg = atof(buf);
    *sec = atof(p+2);

    return OK;
}

status_t CGpsParser::ParseLogitude(CMem *str, double *deg,double *sec)
{
    ASSERT(str && deg && sec);
    
    if(str->StrLen() < 6)
    {
        *deg = 0;
        *sec = 0;
        return ERROR;
    }
    
    char buf[4];
    const char *p = str->CStr();
    
    buf[0] = p[0];
    buf[1] = p[1];
    buf[2] = p[2];
    buf[3] = 0;
    
    *deg = atof(buf);
    *sec = atof(p+3);
    
    return OK;
}

status_t CGpsParser::ParseGga(CMem *line, CGpsGga *gga)
{
    ASSERT(line && gga);
    
    LOCAL_MEM(mem);
    line->Seek(0);
    
    CGpsParser parser;
    parser.Init();
    parser.NextItem(line,&mem); //$GNGGA
    
    ParseHead(&mem,gga);

    parser.NextItem(line,&mem);
    ParseUtcTime(&mem,gga->GetUtcTime());
    gga->SetHasUtcTime(parser.IsCurValid());

    double deg = 0,sec = 0;
    parser.NextItem(line,&mem);        
    ParseLatitude(&mem,&deg,&sec); 
    gga->SetLatitude(deg+sec/60.0);
    gga->SetHasLatitude(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
    
    parser.NextItem(line,&mem);
    ParseLogitude(&mem,&deg,&sec); 
    gga->SetLongitude(deg+sec/60.0);
    gga->SetHasLongitude(parser.IsCurValid());
    
    parser.NextItem(line,&mem);

    parser.NextItem(line,&mem);
    gga->SetQuality(atoi(mem.CStr()));
    gga->SetHasQuality(parser.IsCurValid());

    parser.NextItem(line,&mem);
    gga->SetNumberOfSatellites(atoi(mem.CStr()));
    gga->SetHasNumberOfSatellites(parser.IsCurValid());

    parser.NextItem(line,&mem);
    gga->SetHorizontalDilution(atof(mem.CStr()));
    gga->SetHasHorizontalDilution(parser.IsCurValid());

    parser.NextItem(line,&mem);
    gga->SetAltitude(atof(mem.CStr()));
    gga->SetHasAltitude(parser.IsCurValid());

    parser.NextItem(line,&mem);
    //ASSERT(mem.StrCmp("M") == 0);

    parser.NextItem(line,&mem);
    gga->SetHeightOfGeoid(atof(mem.CStr()));
    gga->SetHasHeightOfGeoid(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
    //ASSERT(mem.StrCmp("M") == 0);

    parser.NextItem(line,&mem);
    gga->SetDifferentialGpsDataPeriod(atoi(mem.CStr()));
    gga->SetHasDifferentialGpsDataPeriod(parser.IsCurValid());

    parser.NextItem(line,&mem);
    gga->SetTheDifferentialReferenceBaseStationNumber(atoi(mem.CStr()));
    gga->SetHasTheDifferentialReferenceBaseStationNumber(parser.IsCurValid());

    return OK;
}

status_t CGpsParser::ParseGsa(CMem *line, CGpsGsa *gsa)
{
    ASSERT(line && gsa);
    
    LOCAL_MEM(mem);
    line->Seek(0);
    
    CGpsParser parser;
    parser.Init();
    parser.NextItem(line,&mem); //$GNGSA

    ParseHead(&mem,gsa);
    
    parser.NextItem(line,&mem);
    gsa->SetFixMode(mem.C(0));
    gsa->SetHasFixMode(parser.IsCurValid());

    parser.NextItem(line,&mem);
    gsa->SetFixValue(atoi(mem.CStr()));
    gsa->SetHasFixValue(parser.IsCurValid());
    
    for(int i = 0; i < 12; i++)
    {
        parser.NextItem(line,&mem);
        gsa->SetSatellitePrnsElem(i,atoi(mem.CStr()));
    }
    gsa->SetHasSatellitePrns(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
    gsa->SetDilutionOfPrecision(atof(mem.CStr()));
    gsa->SetHasDilutionOfPrecision(parser.IsCurValid());
  
    parser.NextItem(line,&mem);
    gsa->SetHorizontalDilutionOfPrecision(atof(mem.CStr()));
    gsa->SetHasHorizontalDilutionOfPrecision(parser.IsCurValid());
  
    parser.NextItem(line,&mem);
    gsa->SetVerticalDilutionOfPrecision(atof(mem.CStr()));
    gsa->SetHasVerticalDilutionOfPrecision(parser.IsCurValid());
  
    return OK;
}


status_t CGpsParser::ParseGll(CMem *line, CGpsGll *gll)
{
    ASSERT(line && gll);
    
    LOCAL_MEM(mem);
    line->Seek(0);
    
    CGpsParser parser;
    parser.Init();
    parser.NextItem(line,&mem); //$GNGLL
    
    ParseHead(&mem,gll);
    
    double deg = 0,sec = 0;
    parser.NextItem(line,&mem);        
    ParseLatitude(&mem,&deg,&sec); 
    gll->SetLatitude(deg+sec/60.0);
    gll->SetHasLatitude(parser.IsCurValid());
    
    parser.NextItem(line,&mem);

    parser.NextItem(line,&mem);
    ParseLogitude(&mem,&deg,&sec); 
    gll->SetLongitude(deg+sec/60.0);
    gll->SetHasLongitude(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
     
    parser.NextItem(line,&mem);
    ParseUtcTime(&mem,gll->GetUtcTime());
    gll->SetHasUtcTime(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
    gll->SetStatus(mem.C(0)); 
    gll->SetHasStatus(parser.IsCurValid());
    
    parser.NextItem(line,&mem);
    gll->SetMode(mem.C(0)); 
    gll->SetHasMode(parser.IsCurValid());
    
    return OK;
}

status_t CGpsParser::ParseZda(CMem *line, CGpsZda *zda)
{
    ASSERT(line && zda);
       
    LOCAL_MEM(mem);
    line->Seek(0);
    
    CGpsParser parser;
    parser.Init();
    parser.NextItem(line,&mem); //$GNGLL
    
    ParseHead(&mem,zda);

    parser.NextItem(line,&mem);
    ParseUtcTime(&mem,zda->GetUtcTime());
    zda->SetHasUtcTime(parser.IsCurValid());
  
    CGpsUtcDate *date = zda->GetUtcDate();

    parser.NextItem(line,&mem);   
    date->SetMonth(atoi(mem.CStr()));
    zda->SetHasUtcDate(parser.IsCurValid());

    parser.NextItem(line,&mem);   
    date->SetDay(atoi(mem.CStr()));

    parser.NextItem(line,&mem);   
    date->SetYear(atoi(mem.CStr()));

    parser.NextItem(line,&mem);   
    zda->SetLocalZoneHours(atoi(mem.CStr()));
    zda->SetHasLocalZoneHours(parser.IsCurValid());

    parser.NextItem(line,&mem);   
    zda->SetLocalZoneMinutes(atoi(mem.CStr()));
    zda->SetHasLocalZoneMinutes(parser.IsCurValid());

    return OK;
}

status_t CGpsParser::ParseGsv(CMem *line, CGpsGsv *gsv)
{
    ASSERT(line && gsv);
    
    LOCAL_MEM(mem);
    line->Seek(0);
    
    CGpsParser parser;
    parser.Init();
    parser.NextItem(line,&mem); //$GNGSV
    
    ParseHead(&mem,gsv);

    parser.NextItem(line,&mem);   
    gsv->SetNumberOfSentences(atoi(mem.CStr()));

    parser.NextItem(line,&mem);   
    gsv->SetSentence(atoi(mem.CStr()));

    parser.NextItem(line,&mem);   
    gsv->SetNumberOfSatellites(atoi(mem.CStr()));

    int n = gsv->GetNumberOfSatellites() - (gsv->GetSentence()-1)*4;
    if(n > 4) n = 4;
    
    gsv->AllocSatelliteInfos(n);
    
    for(int i = 0; i < n; i++)
    {
        CGpsSatelliteInfo *info = gsv->GetSatelliteInfosElem(i);
        parser.NextItem(line,&mem); 
        info->SetPrnNumber(atoi(mem.CStr()));
        info->SetHasPrnNumber(parser.IsCurValid());

        parser.NextItem(line,&mem); 
        info->SetElevation(atoi(mem.CStr()));
        info->SetHasElevation(parser.IsCurValid());

        parser.NextItem(line,&mem); 
        info->SetAzimuth(atoi(mem.CStr()));
        info->SetHasAzimuth(parser.IsCurValid());
        
        parser.NextItem(line,&mem); 
        info->SetSnr(atoi(mem.CStr()));
        info->SetHasSnr(parser.IsCurValid());
    }

    return OK;
}

status_t CGpsParser::ParseSingleLine(CMem *line, CClosure *closure)
{
    ASSERT(line && closure);
    if(line->C(0) != '$')
        return ERROR;

    closure->SetParamPointer(2,line);
    CGpsData head;
    head.Init();
    CGpsParser::ParseHead(line,&head);
    
    if(head.Is("RMC"))
    {
        CGpsRmc rmc;
        rmc.Init();
        CGpsParser::ParseRmc(line,&rmc);
        closure->SetParamPointer(1,&rmc);
        closure->Run();
    }
    else if(head.Is("VTG"))
    {
        CGpsVtg vtg;
        vtg.Init();
        CGpsParser::ParseVtg(line,&vtg);
        closure->SetParamPointer(1,&vtg);
        closure->Run();
    }
    else if(head.Is("GGA"))
    {            
        CGpsGga gga;
        gga.Init();
        CGpsParser::ParseGga(line,&gga);

        closure->SetParamPointer(1,&gga);
        closure->Run();
    }
    else if(head.Is("GSA"))
    {
        CGpsGsa gsa;
        gsa.Init();
        CGpsParser::ParseGsa(line,&gsa);

        closure->SetParamPointer(1,&gsa);
        closure->Run();            
    }
    else if(head.Is("GLL"))
    {
        CGpsGll gll;
        gll.Init();
        CGpsParser::ParseGll(line,&gll);

        closure->SetParamPointer(1,&gll);
        closure->Run();
    }
    else if(head.Is("ZDA"))
    {
        CGpsZda zda;
        zda.Init();
        CGpsParser::ParseZda(line,&zda);

        closure->SetParamPointer(1,&zda);
        closure->Run();
    }
    else if(head.Is("GSV"))
    {
        CGpsGsv gsv;
        gsv.Init();
        CGpsParser::ParseGsv(line,&gsv);

        closure->SetParamPointer(1,&gsv);
        closure->Run();
    }
    else
    {
        //XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
        //    "unknown gps data: %s",line.CStr()
        //);  
    }
    return OK;
}

status_t CGpsParser::ParseWholeFile(CFileBase *file, CClosure *closure)
{
    ASSERT(file && closure);
    LOCAL_MEM(line);
    while(file->ReadLine(&line))
    {
        line.Trim();
        if(line.C(0) == 0)
            continue;
        ParseSingleLine(&line,closure);
    }
    return OK;
}
