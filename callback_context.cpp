#include "callback_context.h"
#include "syslog.h"
#include "mem_tool.h"

CCallbackContext::CCallbackContext()
{
    this->InitBasic();
}

CCallbackContext::~CCallbackContext()
{
    this->Destroy();
}

status_t CCallbackContext::InitBasic()
{
/*##Begin InitBasic##*/
    this->m_env = NULL;
    this->m_obj = 0;
    this->m_name.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CCallbackContext::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_name.Init();
/*##End Init##*/
    return OK;
}

status_t CCallbackContext::Destroy()
{
	this->ClearCallback();
/*##Begin Destroy##*/
    this->m_name.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CCallbackContext::Copy(CCallbackContext *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_env = _p->m_env;
    this->m_obj = _p->m_obj;
    this->m_name.Copy(&_p->m_name);
/*##End Copy##*/
    return OK;
}

int CCallbackContext::Comp(CCallbackContext *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CCallbackContext::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("env = 0x%p",m_env);
    _buf->Log("obj = %d",m_obj);

    _buf->Log("name = %s",
        m_name.StrLen()>0?m_name.CStr():"<null>"
    );
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetEnv@@*/
JNIEnv* CCallbackContext::GetEnv()
{
    return m_env;
}
/*@@End  Function GetEnv@@*/

/*@@Begin Function GetObj@@*/
jobject CCallbackContext::GetObj()
{
    if(!this->IsValid())
        return NULL;
    return m_obj;
}
/*@@End  Function GetObj@@*/

/*@@Begin Function GetName@@*/
CMem* CCallbackContext::GetName()
{
    return &m_name;
}
/*@@End  Function GetName@@*/

/*@@Begin Function GetNameStr@@*/
const char* CCallbackContext::GetNameStr()
{
    return m_name.CStr();
}
/*@@End  Function GetNameStr@@*/

/*@@Begin Function SetEnv@@*/
status_t CCallbackContext::SetEnv(JNIEnv *_env)
{
    this->m_env = _env;
    return OK;
}
/*@@End  Function SetEnv@@*/

/*@@Begin Function SetObj@@*/
status_t CCallbackContext::SetObj(jobject _obj)
{
    this->m_obj = _obj;
    return OK;
}
/*@@End  Function SetObj@@*/

/*@@Begin Function SetName@@*/
status_t CCallbackContext::SetName(CMem *_name)
{
    ASSERT(_name);
    return this->m_name.Copy(_name);
}
/*@@End  Function SetName@@*/

/*@@Begin Function SetName-CStr@@*/
status_t CCallbackContext::SetName(const char *_name)
{
    CMem tmp(_name);
    return this->SetName(&tmp);
}
/*@@End  Function SetName-CStr@@*/

/*@@ Insert Function Here @@*/
status_t CCallbackContext::ClearCallback()
{
	if(m_env && m_obj)
	{
		m_env->DeleteWeakGlobalRef(m_obj);
		m_obj = NULL;
		m_env = NULL;
	}
	return OK;
}

status_t CCallbackContext::SetCallback(JNIEnv *env, jobject obj, const char *name)
{
	ASSERT(env && obj && name);
	this->ClearCallback();
	jobject gobj = env->NewWeakGlobalRef(obj);
	ASSERT(gobj);
	m_env = env;
	m_obj = gobj;
	this->SetName(name);
	return OK;
}

bool CCallbackContext::IsValid()
{
	if(m_env == NULL)return false;
	if(m_obj == NULL)return false;
	if(m_env->IsSameObject(m_obj,NULL))
	{
		return false;
	}
	return true;
}

