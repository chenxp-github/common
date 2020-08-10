#include "cniobject.h"
#include "mem_tool.h"
#include "syslog.h"

CCniObject::CCniObject()
{
    this->InitBasic();
}
CCniObject::~CCniObject()
{
    this->Destroy();
}
status_t CCniObject::InitBasic()
{
    this->local_obj = NULL;
    this->is_weak_ref = false;
    return OK;
}
status_t CCniObject::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CCniObject::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}

CCniObject *CCniObject::GetCniObj(int_ptr_t obj)
{
    return (CCniObject*)obj;
}

void *CCniObject::GetLocalObj()
{
    return this->local_obj;
}

void *CCniObject::GetLocalObj(int_ptr_t obj)
{
    CCniObject *o = GetCniObj(obj);
    ASSERT(o);
    return o->GetLocalObj();
}

status_t CCniObject::SetLocalObj(void *obj)
{
    this->local_obj = obj;
    return OK;
}

bool CCniObject::IsWeakRef()
{
    return this->is_weak_ref;
}

status_t CCniObject::SetWeakRef(bool isWeak)
{
    this->is_weak_ref = isWeak;
    return OK;
}

