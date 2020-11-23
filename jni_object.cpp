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
	this->m_callback_contexts = NULL;
	this->m_callback_contexts_size = 0;
	return OK;
}
status_t CJniObject::Init()
{
	this->InitBasic();
	return OK;
}
status_t CJniObject::Destroy()
{
    DEL_ARRAY(m_callback_contexts);
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


CCallbackContext* CJniObject::GetCallbackContexts()
{
    return m_callback_contexts;
}

int CJniObject::GetCallbackContextsSize()
{
    return m_callback_contexts_size;
}

CCallbackContext* CJniObject::GetCallbackContextsElem(int _index)
{
    ASSERT(this->m_callback_contexts);
    ASSERT(_index >= 0 && _index < m_callback_contexts_size);
    return &m_callback_contexts[_index];
}

status_t CJniObject::AllocCallbackContexts(int _len)
{
    if(m_callback_contexts_size == _len)
        return OK;
    DEL_ARRAY(this->m_callback_contexts);
    if(_len > 0)
    {
        NEW_ARRAY(this->m_callback_contexts,CCallbackContext,_len);
        for(int i = 0; i < _len; i++)
        {
            this->m_callback_contexts[i].Init();
        }
    }
    this->m_callback_contexts_size = _len;
    return OK;
}

status_t CJniObject::SetCallbackContexts(CCallbackContext _callback_contexts[], int _len)
{
    ASSERT(_callback_contexts);
    this->AllocCallbackContexts(_len);
    for(int i = 0; i < _len; i++)
    {
        this->m_callback_contexts[i].Copy(&_callback_contexts[i]);
    }
    return OK;
}

status_t CJniObject::SetCallbackContexts(CCallbackContext *_callback_contexts[], int _len)
{
    ASSERT(_callback_contexts);
    this->AllocCallbackContexts(_len);
    for(int i = 0; i < _len; i++)
    {
        this->m_callback_contexts[i].Copy(_callback_contexts[i]);
    }
    return OK;
}

status_t CJniObject::SetCallbackContextsElem(int _index,CCallbackContext *_callback_contexts)
{
    ASSERT(this->m_callback_contexts);
    ASSERT(_index >= 0 && _index < m_callback_contexts_size);
    ASSERT(_callback_contexts);
    this->m_callback_contexts[_index].Copy(_callback_contexts);
    return OK;
}

status_t CJniObject::SetCallback(int index, JNIEnv* env, jobject cb_obj, const char *cb_name)
{
	ASSERT(env && cb_obj && cb_name);

	if(m_callback_contexts_size == 0)
	{
		this->AllocCallbackContexts(8); //default size
	}

	CCallbackContext *context = this->GetCallbackContextsElem(index);
	ASSERT(context);
	return context->SetCallback(env,cb_obj,cb_name);
}

CCallbackContext* CJniObject::GetCallback(int index)
{
	CCallbackContext *context = this->GetCallbackContextsElem(index);
	if(context == NULL)return NULL;
	if(!context->IsValid())return NULL;
	return context;
}
