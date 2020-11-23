#ifndef __MAIN_LOOP_CONTEXT_H
#define __MAIN_LOOP_CONTEXT_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include <jni.h>
#include "mem.h"
#include "memfile.h"
#include "taskmgr.h"
#include "taskrunner.h"

class CMainLoopContext
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CTaskMgr m_TaskMgr;
    CTaskRunner m_TaskRunner;
/*##End Members##*/
    JNIEnv *m_Env;
public:
    CMainLoopContext();
    virtual ~CMainLoopContext();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

/*##Begin Getter_H##*/
    CTaskMgr* GetTaskMgr();
    CTaskRunner* GetTaskRunner();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/

    status_t SetJniEnv(JNIEnv *env);
    JNIEnv* GetJniEnv();
};

#endif
