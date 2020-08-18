#include "asn_member.h"
#include "syslog.h"
#include "mem_tool.h"
#include "asn_descriptor.h"

CAsnMember::CAsnMember()
{
    this->InitBasic();
}

CAsnMember::~CAsnMember()
{
    this->Destroy();
}

status_t CAsnMember::InitBasic()
{
    this->m_mem_ptr = NULL;
    return OK;
}

status_t CAsnMember::Init()
{
    this->InitBasic();
    return OK;
}

status_t CAsnMember::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CAsnMember::Copy(CAsnMember *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

    this->m_mem_ptr = _p->m_mem_ptr;
    return OK;
}

int CAsnMember::Comp(CAsnMember *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CAsnMember::Print(CFileBase *_buf)
{
    ASSERT(_buf);
    _buf->Log("mem_ptr = 0x%p",m_mem_ptr);
    return OK;
}

asn_TYPE_member_t* CAsnMember::Get()
{
    return m_mem_ptr;
}

status_t CAsnMember::Set(asn_TYPE_member_t *_mem_ptr)
{
    this->m_mem_ptr = _mem_ptr;
    return OK;
}

status_t CAsnMember::GetDescriptor(CAsnDescriptor *descriptor)
{
    ASSERT(descriptor);
    descriptor->Set(m_mem_ptr->type);
    return OK;
}

const char *CAsnMember::GetName()
{
    return m_mem_ptr->name;
}

status_t CAsnMember::GetVarName(CMem *out)
{
    ASSERT(out);

    const char *name = this->GetName();
    ASSERT(name);

    while(*name)
    {
        char ch = *name;

        if((ch >= '0' && ch <= '9') ||
           (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z'))
        {
            out->Putc(ch);
        }
        else
        {
            out->Putc('_');
        }
        name++;
    }
    return OK;
}

bool CAsnMember::IsOptional()
{
    return m_mem_ptr->optional != 0;
}

int CAsnMember::GetOffset()
{
    return m_mem_ptr->memb_offset;
}

bool CAsnMember::IsPointer()
{
    return m_mem_ptr->flags == ATF_POINTER;
}
