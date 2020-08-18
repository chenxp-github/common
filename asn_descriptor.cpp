#include "asn_descriptor.h"
#include "syslog.h"
#include "mem_tool.h"
#include "asn_member.h"
#include "constr_CHOICE.h"
#include "constr_SEQUENCE.h"
#include "NativeInteger.h"
#include "OCTET_STRING.h"
#include "IA5String.h"
#include "BIT_STRING.h"
#include "NativeEnumerated.h"
#include "constr_SEQUENCE_OF.h"


CAsnDescriptor::CAsnDescriptor()
{
    this->InitBasic();
}

CAsnDescriptor::~CAsnDescriptor()
{
    this->Destroy();
}

status_t CAsnDescriptor::InitBasic()
{
    this->m_def = NULL;
    return OK;
}

status_t CAsnDescriptor::Init()
{
    this->InitBasic();
    return OK;
}

status_t CAsnDescriptor::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CAsnDescriptor::Copy(CAsnDescriptor *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

    this->m_def = _p->m_def;
    return OK;
}

int CAsnDescriptor::Comp(CAsnDescriptor *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CAsnDescriptor::Print(CFileBase *_buf)
{
    ASSERT(_buf);
    _buf->Log("def = 0x%p",m_def);
    return OK;
}

asn_TYPE_descriptor_t* CAsnDescriptor::Get()
{
    return m_def;
}

status_t CAsnDescriptor::Set(asn_TYPE_descriptor_t *_def)
{
    this->m_def = _def;
    return OK;
}

const char *CAsnDescriptor::GetName()
{
    return m_def->name;
}

int CAsnDescriptor::GetMembersCount()
{
    return m_def->elements_count;
}

status_t CAsnDescriptor::GetMember(int index,CAsnMember *member)
{
    ASSERT(member);
    ASSERT(index >= 0 && index <= GetMembersCount());
    member->Set(&m_def->elements[index]);
    return OK;
}

bool CAsnDescriptor::IsAsnChoice()
{
    return m_def->op == &asn_OP_CHOICE;
}

int CAsnDescriptor::GetAsnChoicePresent(const void *choice_ptr)
{
    ASSERT(this->IsAsnChoice());
    ASSERT(choice_ptr);

    asn_CHOICE_specifics_t *spec = (asn_CHOICE_specifics_t*)m_def->specifics;
    ASSERT(spec);

    ASSERT(sizeof(int) == spec->pres_size);

    int p = 0xffffffff;
    memcpy(&p,(char*)choice_ptr + spec->pres_offset,spec->pres_size);
    return p;
}

int CAsnDescriptor::GetAsnChoiceMemberIndex(int present)
{
    RASSERT(present > 0,-1);

    asn_CHOICE_specifics_t *spec = (asn_CHOICE_specifics_t*)m_def->specifics;
    ASSERT(spec);

    for(uint32_t i = 0; i < spec->tag2el_count; i++)
    {
        if(spec->tag2el[i].el_no+1 == (uint32_t)present)
            return spec->tag2el[i].el_no;
    }
    return -1;
}

int CAsnDescriptor::GetAsnChoiceMemberIndex(const void *choice_ptr)
{
    RASSERT(choice_ptr,-1);
    RASSERT(this->IsAsnChoice(),-1);
    int present = this->GetAsnChoicePresent(choice_ptr);
    return this->GetAsnChoiceMemberIndex(present);
}

bool CAsnDescriptor::IsAsnSequence()
{
    return m_def->op == &asn_OP_SEQUENCE;
}

bool CAsnDescriptor::IsAsnInteger()
{
    return m_def->op == &asn_OP_NativeInteger;
}

bool CAsnDescriptor::IsAsnOctetString()
{
    return m_def->op == &asn_OP_OCTET_STRING;
}
bool CAsnDescriptor::IsAsnEnumerated()
{
    return m_def->op == &asn_OP_NativeEnumerated;
}
bool CAsnDescriptor::IsAsnBitString()
{
    return m_def->op == &asn_OP_BIT_STRING;
}
bool CAsnDescriptor::IsAsnSequenceOf()
{
    return m_def->op == &asn_OP_SEQUENCE_OF;
}
status_t CAsnDescriptor::GetAsnEnumeratedValue(long enum_value,CMem *out)
{
    ASSERT(out);
    ASSERT(this->IsAsnEnumerated());
    asn_INTEGER_specifics_t *spec = (asn_INTEGER_specifics_t*)m_def->specifics;
    ASSERT(spec);

    const asn_INTEGER_enum_map_t *enum_map = spec->value2enum;
    ASSERT(enum_map);

    for(int i = 0; i < spec->map_count;i++)
    {
        if(enum_map[i].nat_value == enum_value)
        {
            out->SetSize(0);
            out->Puts(enum_map[i].enum_name);
            return OK;
        }
    }

    return ERROR;
}

bool CAsnDescriptor::IsAsnIA5String()
{
     return m_def->op == &asn_OP_IA5String;
}

asn_CHOICE_specifics_t *CAsnDescriptor::GetChoiceSpecifics()
{
    ASSERT(this->IsAsnChoice());
    return (asn_CHOICE_specifics_t*)m_def->specifics;
}

asn_SEQUENCE_specifics_t *CAsnDescriptor::GetSequenceSpecifics()
{
    ASSERT(this->IsAsnSequence());
    return (asn_SEQUENCE_specifics_t*)m_def->specifics;
}

asn_SET_OF_specifics_t *CAsnDescriptor::GetSetOfSpecifics()
{
    ASSERT(this->IsAsnSequenceOf());
    return (asn_SET_OF_specifics_t*)m_def->specifics;
}