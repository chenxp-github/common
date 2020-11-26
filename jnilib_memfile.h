#ifndef __JNILIB_MEMFILE_H
#define __JNILIB_MEMFILE_H

#include "jni_helper.h"
#include "memfile.h"

#define JAVA_CLASS_PATH_MEMFILE "com/jni/common/CMemFile"

CMemFile* get_memfile(JNIEnv* env,jobject obj);
jobject create_java_memfile(JNIEnv* env,CMemFile *memfile, bool is_weak);
status_t register_memfile_native_methods(JNIEnv* env);

#endif
