#include "main_loop_context.h"
#include "syslog.h"
#include "mem_tool.h"

CMainLoopContext::CMainLoopContext()
{
    this->InitBasic();
}

CMainLoopContext::~CMainLoopContext()
{
    this->Destroy();
}

status_t CMainLoopContext::InitBasic()
{
/*##Begin InitBasic##*/
    this->m_TaskMgr.InitBasic();
    this->m_TaskRunner.InitBasic();
    this->m_PeerGlobals.InitBasic();
/*##End InitBasic##*/
    m_Env = NULL;
    return OK;
}

status_t CMainLoopContext::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_TaskMgr.Init();
    this->m_TaskRunner.Init();    
/*##End Init##*/
    this->m_PeerGlobals.Init(&m_TaskMgr);
    return OK;
}

status_t CMainLoopContext::Destroy()
{
/*##Begin Destroy##*/
    this->m_TaskMgr.Destroy();
    this->m_TaskRunner.Destroy();
    this->m_PeerGlobals.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

/*@@Begin Function GetTaskMgr@@*/
CTaskMgr* CMainLoopContext::GetTaskMgr()
{
    return &m_TaskMgr;
}
/*@@End  Function GetTaskMgr@@*/

/*@@Begin Function GetTaskRunner@@*/
CTaskRunner* CMainLoopContext::GetTaskRunner()
{
    return &m_TaskRunner;
}
/*@@End  Function GetTaskRunner@@*/


/*@@Begin Function GetPeerGlobals@@*/
CPeerGlobals* CMainLoopContext::GetPeerGlobals()
{
    return &m_PeerGlobals;
}
/*@@End  Function GetPeerGlobals@@*/
/*@@ Insert Function Here @@*/
status_t CMainLoopContext::SetJniEnv(JNIEnv *env)
{
	m_Env = env;
	return OK;
}

JNIEnv* CMainLoopContext::GetJniEnv()
{
	return m_Env;
}

status_t CMainLoopContext::Schedule()
{
    m_TaskMgr.Schedule();
    m_TaskRunner.Schedule();
    return OK;
}