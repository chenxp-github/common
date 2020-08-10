#include "gps_gsv.h"
#include "syslog.h"
#include "mem_tool.h"
#include "gps_helper.h"

CGpsGsv::CGpsGsv()
{
    this->InitBasic();
}

CGpsGsv::~CGpsGsv()
{
    this->Destroy();
}

status_t CGpsGsv::InitBasic()
{
/*##Begin InitBasic##*/
    CGpsData::InitBasic();
    this->m_number_of_sentences = 0;
    this->m_sentence = 0;
    this->m_number_of_satellites = 0;
    this->m_satellite_infos = NULL;
    this->m_satellite_infos_size = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CGpsGsv::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CGpsData::Init();
/*##End Init##*/
    return OK;
}

status_t CGpsGsv::Destroy()
{
/*##Begin Destroy##*/
    CGpsData::Destroy();
    DEL_ARRAY(m_satellite_infos);
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsGsv::Copy(CGpsGsv *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CGpsData::Copy(_p);
    int i = 0;

    this->m_number_of_sentences = _p->m_number_of_sentences;
    this->m_sentence = _p->m_sentence;
    this->m_number_of_satellites = _p->m_number_of_satellites;
    this->AllocSatelliteInfos(_p->m_satellite_infos_size);
    for(i = 0; i < m_satellite_infos_size; i++)
    {
        this->m_satellite_infos[i].Copy(&_p->m_satellite_infos[i]);
    }
/*##End Copy##*/
    return OK;
}

int CGpsGsv::Comp(CGpsGsv *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsGsv::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CGpsData::Print(_buf);
    int i = 0;

    _buf->Log("number_of_sentences = %d",m_number_of_sentences);
    _buf->Log("sentence = %d",m_sentence);
    _buf->Log("number_of_satellites = %d",m_number_of_satellites);
    _buf->Log("satellite_infos = [");
    _buf->IncLogLevel(1);
    for(i = 0; i < m_satellite_infos_size; i++)
    {
        _buf->Log("[%d]={",i);
        _buf->IncLogLevel(1);
        this->m_satellite_infos[i].Print(_buf);
        _buf->IncLogLevel(-1);
        _buf->Log("},");
    }
    _buf->IncLogLevel(-1);
    _buf->Log("]");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetNumberOfSentences@@*/
int CGpsGsv::GetNumberOfSentences()
{
    return m_number_of_sentences;
}
/*@@End  Function GetNumberOfSentences@@*/

/*@@Begin Function GetSentence@@*/
int CGpsGsv::GetSentence()
{
    return m_sentence;
}
/*@@End  Function GetSentence@@*/

/*@@Begin Function GetNumberOfSatellites@@*/
int CGpsGsv::GetNumberOfSatellites()
{
    return m_number_of_satellites;
}
/*@@End  Function GetNumberOfSatellites@@*/

/*@@Begin Function GetSatelliteInfos@@*/
CGpsSatelliteInfo* CGpsGsv::GetSatelliteInfos()
{
    return m_satellite_infos;
}
/*@@End  Function GetSatelliteInfos@@*/

/*@@Begin Function GetSatelliteInfosLen@@*/
int CGpsGsv::GetSatelliteInfosSize()
{
    return m_satellite_infos_size;
}
/*@@End  Function GetSatelliteInfosLen@@*/

/*@@Begin Function GetSatelliteInfosElem@@*/
CGpsSatelliteInfo* CGpsGsv::GetSatelliteInfosElem(int _index)
{
    ASSERT(this->m_satellite_infos);
    ASSERT(_index >= 0 && _index < m_satellite_infos_size);
    return &m_satellite_infos[_index];
}
/*@@End  Function GetSatelliteInfosElem@@*/

/*@@Begin Function SetNumberOfSentences@@*/
status_t CGpsGsv::SetNumberOfSentences(int _number_of_sentences)
{
    this->m_number_of_sentences = _number_of_sentences;
    return OK;
}
/*@@End  Function SetNumberOfSentences@@*/

/*@@Begin Function SetSentence@@*/
status_t CGpsGsv::SetSentence(int _sentence)
{
    this->m_sentence = _sentence;
    return OK;
}
/*@@End  Function SetSentence@@*/

/*@@Begin Function SetNumberOfSatellites@@*/
status_t CGpsGsv::SetNumberOfSatellites(int _number_of_satellites)
{
    this->m_number_of_satellites = _number_of_satellites;
    return OK;
}
/*@@End  Function SetNumberOfSatellites@@*/

/*@@Begin Function AllocSatelliteInfos@@*/
status_t CGpsGsv::AllocSatelliteInfos(int _len)
{
    if(m_satellite_infos_size == _len)
        return OK;
    DEL_ARRAY(this->m_satellite_infos);
    if(_len > 0)
    {
        NEW_ARRAY(this->m_satellite_infos,CGpsSatelliteInfo,_len);
        for(int i = 0; i < _len; i++)
        {
            this->m_satellite_infos[i].Init();
        }
    }
    this->m_satellite_infos_size = _len;
    return OK;
}
/*@@End  Function AllocSatelliteInfos@@*/

/*@@Begin Function SetSatelliteInfos@@*/
status_t CGpsGsv::SetSatelliteInfos(CGpsSatelliteInfo _satellite_infos[], int _len)
{
    ASSERT(_satellite_infos);
    this->AllocSatelliteInfos(_len);
    for(int i = 0; i < _len; i++)
    {
        this->m_satellite_infos[i].Copy(&_satellite_infos[i]);
    }
    return OK;
}
/*@@End  Function SetSatelliteInfos@@*/

/*@@Begin Function SetSatelliteInfos_V2@@*/
status_t CGpsGsv::SetSatelliteInfos(CGpsSatelliteInfo *_satellite_infos[], int _len)
{
    ASSERT(_satellite_infos);
    this->AllocSatelliteInfos(_len);
    for(int i = 0; i < _len; i++)
    {
        this->m_satellite_infos[i].Copy(_satellite_infos[i]);
    }
    return OK;
}
/*@@End  Function SetSatelliteInfos_V2@@*/

/*@@Begin Function SetSatelliteInfosElem@@*/
status_t CGpsGsv::SetSatelliteInfosElem(int _index,CGpsSatelliteInfo *_satellite_infos)
{
    ASSERT(this->m_satellite_infos);
    ASSERT(_index >= 0 && _index < m_satellite_infos_size);
    ASSERT(_satellite_infos);
    this->m_satellite_infos[_index].Copy(_satellite_infos);
    return OK;
}
/*@@End  Function SetSatelliteInfosElem@@*/

/*@@ Insert Function Here @@*/
status_t CGpsGsv::ToGpsString(CFileBase *out)
{
    ASSERT(out);
    
    HeadToString(out);
    out->Putc(',');

    if(HasNumberOfSentences())
        out->Printf("%d",m_number_of_sentences);
    out->Puts(",");

    if(HasSentence())
        out->Printf("%d",m_sentence);
    out->Puts(",");

    out->Printf("%02d,",m_number_of_satellites);

    int n = m_number_of_satellites - (m_sentence-1)*4;
    if(n > 4) n = 4;

    ASSERT(n == m_satellite_infos_size);
    
    for(int i = 0; i < n; i++)
    {
        CGpsSatelliteInfo *info = this->GetSatelliteInfosElem(i);

        if(info->HasPrnNumber())
        {
            out->Printf("%02d",info->GetPrnNumber());
        }
        out->Puts(",");

        if(info->HasElevation())
        {
            out->Printf("%02d",info->GetElevation());
        }
        out->Puts(",");

        if(info->HasAzimuth())
        {
            out->Printf("%03d",info->GetAzimuth());
        }
        out->Puts(",");

        if(info->HasSnr())
        {
            out->Printf("%02d",info->GetSnr());
        }

        if(i<n-1)
        {
            out->Puts(",");        
        }
    }
    
    return OK;   
}
