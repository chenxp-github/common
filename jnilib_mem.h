#ifndef __JNILIB_MEM_H
#define __JNILIB_MEM_H

#include "jni_helper.h"
#include "mem.h"

#define JAVA_CLASS_PATH_MEM "com/jni/common/CMem"

CMem* get_mem(JNIEnv* env,jobject obj);
jobject create_java_mem(JNIEnv* env,CMem *mem, bool is_weak);
status_t register_mem_native_methods(JNIEnv* env);

#endif
