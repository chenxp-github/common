#ifndef __CNIOBJECT_H
#define __CNIOBJECT_H

#include "common.h"
#include "mem.h"
#include "dll.h"

class CCniObject{
public:
    bool is_weak_ref;
    void *local_obj;
public:
    status_t SetWeakRef(bool isWeak);
    bool IsWeakRef();
    status_t SetLocalObj(void *obj);
    CCniObject();
    virtual ~CCniObject();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
    void *GetLocalObj();
    static CCniObject *GetCniObj(int_ptr_t obj);
    static void *GetLocalObj(int_ptr_t obj);
};

#endif
