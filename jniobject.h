#ifndef __JNIOBJECT_H
#define __JNIOBJECT_H

#include <jni.h>
#include "common.h"

class CJniObject{
public:
    bool is_weak_ref;
    void *local_obj;
public:
    static status_t ClearJniObj(JNIEnv *env, jobject obj);
    status_t SetWeakRef(bool isWeak);
    bool IsWeakRef();
    status_t SetLocalObj(void *obj);
    CJniObject();
    virtual ~CJniObject();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
    void *GetLocalObj();
    static CJniObject *GetJniObj(JNIEnv* env,jobject obj);
    static void *GetLocalObj(JNIEnv* env,jobject obj);
};

#endif
