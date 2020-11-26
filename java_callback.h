#ifndef __JAVA_CALLBACK_H
#define __JAVA_CALLBACK_H

#include <jni.h>
#include "mem.h"
#include "memfile.h"


class CJavaCallback{
public:
	JNIEnv* m_env;
	jobject m_callback_obj;
	jclass m_callback_class;
	jmethodID m_put_method_id;
	jmethodID m_run_method_id;
	jmethodID m_clear_method_id;
public:
    CJavaCallback();
    virtual ~CJavaCallback();

    status_t InitBasic();
    status_t Init(JNIEnv *env,jobject cb_obj);
    status_t Destroy();

    status_t Copy(CJavaCallback *_p);
    int Comp(CJavaCallback *_p);
    status_t Print(CFileBase *_buf);

    status_t Clear();
    jobject Run();

    status_t Put(const char *key, int val);
    status_t Put(const char *key, int64_t val);
    status_t Put(const char *key, const char *val);

};

#endif
