#ifndef __GPS_GSV_H
#define __GPS_GSV_H

/*##Begin Inlcudes##*/
#include "gps_data.h"
#include "gps_satellite_info.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CGpsGsv
/*##Begin Bases##*/
:public CGpsData
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_number_of_sentences;
    int m_sentence;
    int m_number_of_satellites;
    CGpsSatelliteInfo *m_satellite_infos;
    int m_satellite_infos_size;
/*##End Members##*/

public:
    CGpsGsv();
    virtual ~CGpsGsv();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsGsv *_p);
    int Comp(CGpsGsv *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetNumberOfSentences();
    int GetSentence();
    int GetNumberOfSatellites();
    CGpsSatelliteInfo* GetSatelliteInfos();
    int GetSatelliteInfosSize();
    CGpsSatelliteInfo* GetSatelliteInfosElem(int _index);
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetNumberOfSentences(int _number_of_sentences);
    status_t SetSentence(int _sentence);
    status_t SetNumberOfSatellites(int _number_of_satellites);
    status_t AllocSatelliteInfos(int _len);
    status_t SetSatelliteInfos(CGpsSatelliteInfo _satellite_infos[], int _len);
    status_t SetSatelliteInfos(CGpsSatelliteInfo *_satellite_infos[], int _len);
    status_t SetSatelliteInfosElem(int _index,CGpsSatelliteInfo *_satellite_infos);
/*##End Setter_H##*/

    status_t ToGpsString(CFileBase *out);

    GPSDATA_FLAG1_FUNC(HasNumberOfSentences,0x00000001);
    GPSDATA_FLAG1_FUNC(HasSentence,0x00000002);
    GPSDATA_FLAG1_FUNC(HasNumberOfSatellites,0x00000004);

};

#endif
