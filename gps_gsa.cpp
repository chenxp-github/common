#include "gps_gsa.h"
#include "syslog.h"
#include "mem_tool.h"
#include "gps_helper.h"

CGpsGsa::CGpsGsa()
{
    this->InitBasic();
}

CGpsGsa::~CGpsGsa()
{
    this->Destroy();
}

status_t CGpsGsa::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    int i = 0;

    this->m_fix_mode = 0;
    this->m_fix_value = 0;
    for(i = 0; i < 12; i++)
    {
        this->m_satellite_prns[i] = 0;
    }
    for(i = 0; i < 12; i++)
    {
        this->m_has_satellite_prns[i] = 0;
    }
    this->m_dilution_of_precision = 0;
    this->m_horizontal_dilution_of_precision = 0;
    this->m_vertical_dilution_of_precision = 0;
/*##End InitBasic##*/

    for(i = 0; i < 12; i++)
    {
        this->m_has_satellite_prns[i] = 1;
    }
    return OK;
}

status_t CGpsGsa::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
/*##End Init##*/
    return OK;
}

status_t CGpsGsa::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsGsa::Copy(CGpsGsa *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    int i = 0;

    this->m_fix_mode = _p->m_fix_mode;
    this->m_fix_value = _p->m_fix_value;
    for(i = 0; i < 12; i++)
    {
        this->m_satellite_prns[i] = _p->m_satellite_prns[i];
    }
    for(i = 0; i < 12; i++)
    {
        this->m_has_satellite_prns[i] = _p->m_has_satellite_prns[i];
    }
    this->m_dilution_of_precision = _p->m_dilution_of_precision;
    this->m_horizontal_dilution_of_precision = _p->m_horizontal_dilution_of_precision;
    this->m_vertical_dilution_of_precision = _p->m_vertical_dilution_of_precision;
/*##End Copy##*/
    return OK;
}

int CGpsGsa::Comp(CGpsGsa *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsGsa::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CGpsData::Print(_buf);
    int i = 0;

    _buf->Log("fix_mode = %c",m_fix_mode);
    _buf->Log("fix_value = %d",m_fix_value);
    _buf->Log("satellite_prns = [");
    _buf->IncLogLevel(1);
    for(i = 0; i < 12; i++)
    {
        _buf->Log("%d,",m_satellite_prns[i]);
    }
    _buf->IncLogLevel(-1);
    _buf->Log("]");
    _buf->Log("has_satellite_prns = [");
    _buf->IncLogLevel(1);
    for(i = 0; i < 12; i++)
    {
        _buf->Log("%d,",m_has_satellite_prns[i]);
    }
    _buf->IncLogLevel(-1);
    _buf->Log("]");
    _buf->Log("dilution_of_precision = %f",m_dilution_of_precision);
    _buf->Log("horizontal_dilution_of_precision = %f",m_horizontal_dilution_of_precision);
    _buf->Log("vertical_dilution_of_precision = %f",m_vertical_dilution_of_precision);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetFixMode@@*/
char CGpsGsa::GetFixMode()
{
    return m_fix_mode;
}
/*@@End  Function GetFixMode@@*/

/*@@Begin Function GetFixValue@@*/
int CGpsGsa::GetFixValue()
{
    return m_fix_value;
}
/*@@End  Function GetFixValue@@*/

/*@@Begin Function GetSatellitePrns@@*/
int* CGpsGsa::GetSatellitePrns()
{
    return m_satellite_prns;
}
/*@@End  Function GetSatellitePrns@@*/

/*@@Begin Function GetSatellitePrnsLen@@*/
int CGpsGsa::GetSatellitePrnsSize()
{
    return 12;
}
/*@@End  Function GetSatellitePrnsLen@@*/

/*@@Begin Function GetSatellitePrnsElem@@*/
int CGpsGsa::GetSatellitePrnsElem(int _index)
{
    ASSERT(_index >= 0 && _index < 12);
    return m_satellite_prns[_index];
}
/*@@End  Function GetSatellitePrnsElem@@*/

/*@@Begin Function GetDilutionOfPrecision@@*/
double CGpsGsa::GetDilutionOfPrecision()
{
    return m_dilution_of_precision;
}
/*@@End  Function GetDilutionOfPrecision@@*/

/*@@Begin Function GetHorizontalDilutionOfPrecision@@*/
double CGpsGsa::GetHorizontalDilutionOfPrecision()
{
    return m_horizontal_dilution_of_precision;
}
/*@@End  Function GetHorizontalDilutionOfPrecision@@*/

/*@@Begin Function GetVerticalDilutionOfPrecision@@*/
double CGpsGsa::GetVerticalDilutionOfPrecision()
{
    return m_vertical_dilution_of_precision;
}
/*@@End  Function GetVerticalDilutionOfPrecision@@*/

/*@@Begin Function SetFixMode@@*/
status_t CGpsGsa::SetFixMode(char _fix_mode)
{
    this->m_fix_mode = _fix_mode;
    return OK;
}
/*@@End  Function SetFixMode@@*/

/*@@Begin Function SetFixValue@@*/
status_t CGpsGsa::SetFixValue(int _fix_value)
{
    this->m_fix_value = _fix_value;
    return OK;
}
/*@@End  Function SetFixValue@@*/

/*@@Begin Function SetSatellitePrns@@*/
status_t CGpsGsa::SetSatellitePrns(int _satellite_prns[], int _len)
{
    ASSERT(_satellite_prns);
    ASSERT(_len <= 12);
    memcpy(this->m_satellite_prns,_satellite_prns,sizeof(int)*_len);
    return OK;
}
/*@@End  Function SetSatellitePrns@@*/

/*@@Begin Function SetSatellitePrns_V2@@*/
status_t CGpsGsa::SetSatellitePrns(int *_satellite_prns[], int _len)
{
    ASSERT(_satellite_prns);
    ASSERT(_len <= 12);
    memcpy(this->m_satellite_prns,_satellite_prns,sizeof(int)*_len);
    return OK;
}
/*@@End  Function SetSatellitePrns_V2@@*/

/*@@Begin Function SetSatellitePrnsElem@@*/
status_t CGpsGsa::SetSatellitePrnsElem(int _index,int _satellite_prns)
{
    ASSERT(_index >= 0 && _index < 12);
    this->m_satellite_prns[_index] = _satellite_prns;
    return OK;
}
/*@@End  Function SetSatellitePrnsElem@@*/

/*@@Begin Function SetDilutionOfPrecision@@*/
status_t CGpsGsa::SetDilutionOfPrecision(double _dilution_of_precision)
{
    this->m_dilution_of_precision = _dilution_of_precision;
    return OK;
}
/*@@End  Function SetDilutionOfPrecision@@*/

/*@@Begin Function SetHorizontalDilutionOfPrecision@@*/
status_t CGpsGsa::SetHorizontalDilutionOfPrecision(double _horizontal_dilution_of_precision)
{
    this->m_horizontal_dilution_of_precision = _horizontal_dilution_of_precision;
    return OK;
}
/*@@End  Function SetHorizontalDilutionOfPrecision@@*/

/*@@Begin Function SetVerticalDilutionOfPrecision@@*/
status_t CGpsGsa::SetVerticalDilutionOfPrecision(double _vertical_dilution_of_precision)
{
    this->m_vertical_dilution_of_precision = _vertical_dilution_of_precision;
    return OK;
}
/*@@End  Function SetVerticalDilutionOfPrecision@@*/


/*@@Begin Function GetHasSatellitePrns@@*/
int* CGpsGsa::GetHasSatellitePrns()
{
    return m_has_satellite_prns;
}
/*@@End  Function GetHasSatellitePrns@@*/

/*@@Begin Function GetHasSatellitePrnsLen@@*/
int CGpsGsa::GetHasSatellitePrnsSize()
{
    return 12;
}
/*@@End  Function GetHasSatellitePrnsLen@@*/

/*@@Begin Function GetHasSatellitePrnsElem@@*/
int CGpsGsa::GetHasSatellitePrnsElem(int _index)
{
    ASSERT(_index >= 0 && _index < 12);
    return m_has_satellite_prns[_index];
}
/*@@End  Function GetHasSatellitePrnsElem@@*/

/*@@Begin Function SetHasSatellitePrns@@*/
status_t CGpsGsa::SetHasSatellitePrns(int _has_satellite_prns[], int _len)
{
    ASSERT(_has_satellite_prns);
    ASSERT(_len <= 12);
    memcpy(this->m_has_satellite_prns,_has_satellite_prns,sizeof(int)*_len);
    return OK;
}
/*@@End  Function SetHasSatellitePrns@@*/

/*@@Begin Function SetHasSatellitePrns_V2@@*/
status_t CGpsGsa::SetHasSatellitePrns(int *_has_satellite_prns[], int _len)
{
    ASSERT(_has_satellite_prns);
    ASSERT(_len <= 12);
    memcpy(this->m_has_satellite_prns,_has_satellite_prns,sizeof(int)*_len);
    return OK;
}
/*@@End  Function SetHasSatellitePrns_V2@@*/

/*@@Begin Function SetHasSatellitePrnsElem@@*/
status_t CGpsGsa::SetHasSatellitePrnsElem(int _index,int _has_satellite_prns)
{
    ASSERT(_index >= 0 && _index < 12);
    this->m_has_satellite_prns[_index] = _has_satellite_prns;
    return OK;
}
/*@@End  Function SetHasSatellitePrnsElem@@*/
/*@@ Insert Function Here @@*/
status_t CGpsGsa::ToGpsString(CFileBase *out)
{
    ASSERT(out);
    
    HeadToString(out);
    out->Putc(',');
    
    if(HasFixMode())
        out->Putc(m_fix_mode);
    out->Putc(',');

    if(HasFixValue())
        out->Printf("%d",m_fix_value);
    out->Putc(',');

    for(int i = 0; i < 12; i++)
    {
        int v = this->GetSatellitePrnsElem(i);
        int has = this->GetHasSatellitePrnsElem(i);
        if(has)
        {
            out->Printf("%02d",v);
        }            
        out->Putc(',');
    }

    if(HasDilutionOfPrecision())
    {
        FloatToFixedGpsString(m_dilution_of_precision,0,2,out);
    }
    out->Putc(',');
    
    if(HasHorizontalDilutionOfPrecision())
    {
        FloatToFixedGpsString(m_horizontal_dilution_of_precision,0,2,out);
    }
    out->Putc(',');

    if(HasVerticalDilutionOfPrecision())
    {
        FloatToFixedGpsString(m_vertical_dilution_of_precision,0,2,out);
    }

    return OK;
}

status_t CGpsGsa::ClearAllSatallitePrnFlags()
{
    memset(m_has_satellite_prns,0,sizeof(m_has_satellite_prns));
    return OK;
}
