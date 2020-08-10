#ifndef __ASN_DESCRIPTOR_H
#define __ASN_DESCRIPTOR_H

#include "mem.h"
#include "memfile.h"
#include "constr_TYPE.h"

class CAsnMember;
class CAsnDescriptor{
public:
    asn_TYPE_descriptor_t *m_def;

public:
    CAsnDescriptor();
    virtual ~CAsnDescriptor();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CAsnDescriptor *_p);
    int Comp(CAsnDescriptor *_p);
    status_t Print(CFileBase *_buf);

    asn_TYPE_descriptor_t* Get();
    status_t Set(asn_TYPE_descriptor_t *_def);

    const char *GetName();
    int GetMembersCount();
    status_t GetMember(int index,CAsnMember *member);
    bool IsAsnChoice();
    int GetAsnChoicePresent(const void *choice_ptr);
    int GetAsnChoiceMemberIndex(const void *choice_ptr);
    status_t GetAsnEnumeratedValue(long enum_value,CMem *out);
    bool IsAsnSequence();
    bool IsAsnInteger();
    bool IsAsnOctetString();
    bool IsAsnBitString();
    bool IsAsnEnumerated();
    bool IsAsnSequenceOf();
    bool IsAsnIA5String();
};

#endif
