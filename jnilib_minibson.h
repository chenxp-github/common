#ifndef __JNILIB_MINIBSON_H
#define __JNILIB_MINIBSON_H

#include "jni_helper.h"
#include "minibson.h"

#define JAVA_CLASS_PATH_MINIBSON "com/jni/common/CMiniBson"

CMiniBson* get_minibson(JNIEnv* env,jobject obj);
jobject create_java_minibson(JNIEnv* env,CMiniBson *minibson, bool is_weak);
status_t register_minibson_native_methods(JNIEnv* env);

#endif
