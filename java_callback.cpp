#include "java_callback.h"
#include "syslog.h"
#include "mem_tool.h"

CJavaCallback::CJavaCallback()
{
    this->InitBasic();
}

CJavaCallback::~CJavaCallback()
{
    this->Destroy();
}

status_t CJavaCallback::InitBasic()
{
	m_callback_obj = NULL;
	m_callback_class = NULL;
	m_put_method_id = NULL;
	m_run_method_id = NULL;
	m_clear_method_id = NULL;
	m_callback_class = NULL;
	m_env = NULL;
    return OK;
}

status_t CJavaCallback::Init(JNIEnv *env, jobject cb_obj)
{
    this->InitBasic();
    ASSERT(env && cb_obj);

    m_env = env;
    m_callback_obj = cb_obj;

    jclass _class = env->GetObjectClass(m_callback_obj);
	ASSERT(_class);
	m_callback_class = (jclass)env->NewGlobalRef(_class);
	ASSERT(m_callback_class);
	m_put_method_id = env->GetMethodID(m_callback_class,"put", "(Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;");
	ASSERT(m_put_method_id);
	m_run_method_id = env->GetMethodID(m_callback_class,"run", "()Ljava/lang/Object;");
	ASSERT(m_run_method_id);
	m_clear_method_id = env->GetMethodID(m_callback_class,"clear", "()V");
	ASSERT(m_clear_method_id);
    return OK;
}

status_t CJavaCallback::Destroy()
{
	if(m_env && m_callback_class)
	{
		m_env->DeleteGlobalRef(m_callback_class);
		m_callback_class = NULL;
	}

    this->InitBasic();
    return OK;
}

status_t CJavaCallback::Copy(CJavaCallback *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

    return OK;
}

int CJavaCallback::Comp(CJavaCallback *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CJavaCallback::Print(CFileBase *_buf)
{
    ASSERT(_buf);
    return OK;
}

status_t CJavaCallback::Clear()
{
	ASSERT(m_env && m_clear_method_id && m_callback_obj);
	m_env->CallVoidMethod(m_callback_obj,m_clear_method_id);
	return OK;
}

jobject CJavaCallback::Run()
{
	ASSERT(m_env && m_run_method_id && m_callback_obj);
	return m_env->CallObjectMethod(m_callback_obj,m_run_method_id);
}

status_t CJavaCallback::Put(const char *key, int val)
{
	ASSERT(key && m_env && m_put_method_id && m_callback_obj);

    jclass cls_integer = m_env->FindClass("java/lang/Integer");
    ASSERT(cls_integer);
    jmethodID ctor_integer = m_env->GetMethodID(cls_integer,"<init>","(I)V");
    ASSERT(ctor_integer);

    jstring jkey = m_env->NewStringUTF(key);
    jobject jobj = m_env->NewObject(cls_integer,ctor_integer,val);
    m_env->CallObjectMethod(m_callback_obj,m_put_method_id,jkey,jobj);
    m_env->DeleteLocalRef(jkey);
    m_env->DeleteLocalRef(jobj);
    m_env->DeleteLocalRef(cls_integer);
    return OK;
}

status_t CJavaCallback::Put(const char *key, int64_t val)
{
	ASSERT(key && m_env && m_put_method_id && m_callback_obj);

    jclass cls_long = m_env->FindClass("java/lang/Long");
    ASSERT(cls_long);
    jmethodID ctor_long = m_env->GetMethodID(cls_long,"<init>","(J)V");
    ASSERT(ctor_long);
    jstring jkey = m_env->NewStringUTF(key);
    jobject jobj = m_env->NewObject(cls_long,ctor_long,val);
    m_env->CallObjectMethod(m_callback_obj,m_put_method_id,jkey,jobj);
    m_env->DeleteLocalRef(jkey);
    m_env->DeleteLocalRef(jobj);
    m_env->DeleteLocalRef(cls_long);
    return OK;
}

status_t CJavaCallback::Put(const char *key, const char *val)
{
	ASSERT(key && val && m_env && m_put_method_id && m_callback_obj);

	jstring jkey = m_env->NewStringUTF(key);
	jstring jval = m_env->NewStringUTF(val);
	m_env->CallObjectMethod(m_callback_obj,m_put_method_id,jkey,jval);
	m_env->DeleteLocalRef(jkey);
	m_env->DeleteLocalRef(jval);
	return OK;
}

status_t CJavaCallback::Put(const char *key, jobject val)
{
    ASSERT(key && val);
    jstring jkey = m_env->NewStringUTF(key);
    m_env->CallObjectMethod(m_callback_obj,m_put_method_id,jkey,val);
    m_env->DeleteLocalRef(jkey);
    return OK;
}

