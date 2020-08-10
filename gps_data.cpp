#include "gps_data.h"
#include "syslog.h"
#include "mem_tool.h"

CGpsData::CGpsData()
{
    this->InitBasic();
}

CGpsData::~CGpsData()
{
    this->Destroy();
}

status_t CGpsData::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_signal_source.InitBasic();
    this->m_statement_identifier.InitBasic();
    this->m_flags1 = 0xffffffff;
    this->m_flags2 = 0xffffffff;
/*##End InitBasic##*/
    this->SetAllFlags();
    return OK;
}

status_t CGpsData::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    
    this->m_signal_source.Init();
    this->m_statement_identifier.Init();
/*##End Init##*/
    return OK;
}

status_t CGpsData::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    this->m_signal_source.Destroy();
    this->m_statement_identifier.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CGpsData::Copy(CGpsData *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_signal_source.Copy(&_p->m_signal_source);
    this->m_statement_identifier.Copy(&_p->m_statement_identifier);
    this->m_flags1 = _p->m_flags1;
    this->m_flags2 = _p->m_flags2;
/*##End Copy##*/
    return OK;
}

int CGpsData::Comp(CGpsData *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CGpsData::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);

    _buf->Log("signal_source = %s",
        m_signal_source.StrLen()>0?m_signal_source.CStr():"<null>"
    );

    _buf->Log("statement_identifier = %s",
        m_statement_identifier.StrLen()>0?m_statement_identifier.CStr():"<null>"
    );
    _buf->Log("flags1 = %u",m_flags1);
    _buf->Log("flags2 = %u",m_flags2);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetSignalSource@@*/
CMem* CGpsData::GetSignalSource()
{
    return &m_signal_source;
}
/*@@End  Function GetSignalSource@@*/

/*@@Begin Function GetSignalSourceStr@@*/
const char* CGpsData::GetSignalSourceStr()
{
    return m_signal_source.CStr();
}
/*@@End  Function GetSignalSourceStr@@*/

/*@@Begin Function GetStatementIdentifier@@*/
CMem* CGpsData::GetStatementIdentifier()
{
    return &m_statement_identifier;
}
/*@@End  Function GetStatementIdentifier@@*/

/*@@Begin Function GetStatementIdentifierStr@@*/
const char* CGpsData::GetStatementIdentifierStr()
{
    return m_statement_identifier.CStr();
}
/*@@End  Function GetStatementIdentifierStr@@*/

/*@@Begin Function SetSignalSource@@*/
status_t CGpsData::SetSignalSource(CMem *_signal_source)
{
    ASSERT(_signal_source);
    return this->m_signal_source.Copy(_signal_source);
}
/*@@End  Function SetSignalSource@@*/

/*@@Begin Function SetSignalSource-CStr@@*/
status_t CGpsData::SetSignalSource(const char *_signal_source)
{
    CMem tmp(_signal_source);
    return this->SetSignalSource(&tmp);
}
/*@@End  Function SetSignalSource-CStr@@*/

/*@@Begin Function SetStatementIdentifier@@*/
status_t CGpsData::SetStatementIdentifier(CMem *_statement_identifier)
{
    ASSERT(_statement_identifier);
    return this->m_statement_identifier.Copy(_statement_identifier);
}
/*@@End  Function SetStatementIdentifier@@*/

/*@@Begin Function SetStatementIdentifier-CStr@@*/
status_t CGpsData::SetStatementIdentifier(const char *_statement_identifier)
{
    CMem tmp(_statement_identifier);
    return this->SetStatementIdentifier(&tmp);
}
/*@@End  Function SetStatementIdentifier-CStr@@*/


/*@@Begin Function GetFlags1@@*/
uint32_t CGpsData::GetFlags1()
{
    return m_flags1;
}
/*@@End  Function GetFlags1@@*/

/*@@Begin Function GetFlags2@@*/
uint32_t CGpsData::GetFlags2()
{
    return m_flags2;
}
/*@@End  Function GetFlags2@@*/

/*@@Begin Function SetFlags1@@*/
status_t CGpsData::SetFlags1(uint32_t _flags1)
{
    this->m_flags1 = _flags1;
    return OK;
}
/*@@End  Function SetFlags1@@*/

/*@@Begin Function SetFlags2@@*/
status_t CGpsData::SetFlags2(uint32_t _flags2)
{
    this->m_flags2 = _flags2;
    return OK;
}
/*@@End  Function SetFlags2@@*/
/*@@ Insert Function Here @@*/


bool CGpsData::Is(const char *id)
{
    ASSERT(id);
    return m_statement_identifier.StrCmp(id) == 0;
}

status_t CGpsData::HeadToString(CFileBase *out)
{
    ASSERT(out);
    out->Puts(GetSignalSource());
    out->Puts(GetStatementIdentifier());
    return OK;
}

status_t CGpsData::ToGpsString(CFileBase *out)
{
    ASSERT(0);
    return OK;
}

status_t CGpsData::SetAllFlags()
{
    this->m_flags1 = 0xffffffff;
    this->m_flags2 = 0xffffffff;
    return OK;
}

status_t CGpsData::ClearAllFlags()
{
    m_flags2 = 0;
    m_flags1 = 0;
    return OK;
}
