#ifndef __ASN_MEMBER_H
#define __ASN_MEMBER_H

#include "mem.h"
#include "memfile.h"
#include "constr_TYPE.h"

class CAsnDescriptor;
class CAsnMember{
public:
    asn_TYPE_member_t *m_mem_ptr;
public:
    CAsnMember();
    virtual ~CAsnMember();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CAsnMember *_p);
    int Comp(CAsnMember *_p);
    status_t Print(CFileBase *_buf);

    asn_TYPE_member_t* Get();
    status_t Set(asn_TYPE_member_t *_mem_ptr);
    status_t GetDescriptor(CAsnDescriptor *descriptor);
    const char *GetName();
    status_t GetVarName(CMem *out);
    bool IsOptional();
    bool IsPointer();
    int GetOffset();
};

#endif
