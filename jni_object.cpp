#include "jni_object.h"
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
	this->m_native_ptr.InitBasic();
	this->m_is_weak = false;
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

CJniObject *CJniObject::GetJniObject(JNIEnv* env,jobject obj)
{
	ASSERT(env && obj);
	jclass class_obj=env->GetObjectClass(obj);
	jfieldID id=env->GetFieldID(class_obj,"__obj","J");
	jlong _o = env->GetLongField(obj,id);
	return (CJniObject*)_o;
}

void *CJniObject::GetNativePointer()
{
	if(m_native_ptr.IsValid())
		return m_native_ptr.GetRawPtr();
	return NULL;
}

void *CJniObject::GetNativePointer(JNIEnv* env,jobject obj)
{
	ASSERT(env && obj);
	CJniObject *o = GetJniObject(env,obj);
	ASSERT(o);
	return o->GetNativePointer();
}

status_t CJniObject::SetNativePointer(const void *ptr,WeakRefContext_t *context)
{
	ASSERT(ptr && context);
	return m_native_ptr.WeakRef(ptr,context);
}

bool CJniObject::IsWeak()
{
	return this->m_is_weak;
}

status_t CJniObject::SetIsWeak(bool isWeak)
{
	this->m_is_weak = isWeak;
	return OK;
}

status_t CJniObject::ClearJniObject(JNIEnv *env, jobject obj)
{
	ASSERT(env && obj);
	jclass class_obj=env->GetObjectClass(obj);
	ASSERT(class_obj);
	jfieldID id=env->GetFieldID(class_obj,"__obj","J");
	ASSERT(id);
	env->SetLongField(obj,id,0);
	return OK;
}
