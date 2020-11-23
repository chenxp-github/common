#ifndef __CALLBACK_CONTEXT_H
#define __CALLBACK_CONTEXT_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include <jni.h>
#include "mem.h"
#include "memfile.h"

class CCallbackContext
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    JNIEnv *m_env;
    jobject m_obj;
    CMem m_name;
/*##End Members##*/

public:
    CCallbackContext();
    virtual ~CCallbackContext();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CCallbackContext *_p);
    int Comp(CCallbackContext *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    JNIEnv* GetEnv();
    jobject GetObj();
    CMem* GetName();
    const char* GetNameStr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetEnv(JNIEnv *_env);
    status_t SetObj(jobject _obj);
    status_t SetName(CMem *_name);
    status_t SetName(const char *_name);
/*##End Setter_H##*/

    status_t ClearCallback();
    status_t SetCallback(JNIEnv *env, jobject obj, const char *name);
    bool IsValid();
};

#endif
