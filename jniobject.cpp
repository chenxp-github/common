#include "jniobject.h"
#include "mem_tool.h"
#include "syslog.h"

CJniObject::CJniObject()
{
    this->InitBasic();
}
CJniObject::~CJniObject()
{
    this->Destroy();
}
status_t CJniObject::InitBasic()
{
    this->local_obj = NULL;
    this->is_weak_ref = false;
    return OK;
}
status_t CJniObject::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CJniObject::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}

CJniObject *CJniObject::GetJniObj(JNIEnv* env,jobject obj )
{
    ASSERT(env && obj);
    jclass class_obj=env->GetObjectClass(obj);
    jfieldID id=env->GetFieldID(class_obj,"__obj","I");
    jint _o = env->GetIntField(obj,id);
    return (CJniObject*)_o;
}

void *CJniObject::GetLocalObj()
{
    return this->local_obj;
}

void *CJniObject::GetLocalObj(JNIEnv* env,jobject obj)
{
    ASSERT(env && obj);
    CJniObject *o = GetJniObj(env,obj);
    ASSERT(o);
    return o->GetLocalObj();
}

status_t CJniObject::SetLocalObj(void *obj)
{
    this->local_obj = obj;
}

bool CJniObject::IsWeakRef()
{
    return this->is_weak_ref;
}

status_t CJniObject::SetWeakRef(bool isWeak)
{
    this->is_weak_ref = isWeak;
    return OK;
}

status_t CJniObject::ClearJniObj(JNIEnv *env, jobject obj)
{
    ASSERT(env && obj);
    jclass class_obj=env->GetObjectClass(obj);
    ASSERT(class_obj);
    jfieldID id=env->GetFieldID(class_obj,"__obj","I");
    ASSERT(id);
    env->SetIntField(obj,id,0);
    return OK;
}
