#ifndef __GPS_DATA_H
#define __GPS_DATA_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

#define GPSDATA_FLAG1_FUNC(func,bit) FLAG_FUNC(m_flags1,func,bit)
#define GPSDATA_FLAG2_FUNC(func,bit) FLAG_FUNC(m_flags2,func,bit)

class CGpsData
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    CMem m_signal_source;
    CMem m_statement_identifier;
    uint32_t m_flags1;
    uint32_t m_flags2;
/*##End Members##*/
public:
	bool Is(const char *id);
    CGpsData();
    virtual ~CGpsData();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CGpsData *_p);
    int Comp(CGpsData *_p);
    virtual status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetSignalSource();
    const char* GetSignalSourceStr();
    CMem* GetStatementIdentifier();
    const char* GetStatementIdentifierStr();
    uint32_t GetFlags1();
    uint32_t GetFlags2();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetSignalSource(CMem *_signal_source);
    status_t SetSignalSource(const char *_signal_source);
    status_t SetStatementIdentifier(CMem *_statement_identifier);
    status_t SetStatementIdentifier(const char *_statement_identifier);
    status_t SetFlags1(uint32_t _flags1);
    status_t SetFlags2(uint32_t _flags2);
/*##End Setter_H##*/

    status_t HeadToString(CFileBase *out);
    virtual status_t ToGpsString(CFileBase *out);

    status_t ClearAllFlags();
    status_t SetAllFlags();

};

#endif
