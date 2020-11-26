#ifndef __JNILIB_FILEBASE_H
#define __JNILIB_FILEBASE_H

#include "jni_helper.h"
#include "filebase.h"

#define JAVA_CLASS_PATH_FILEBASE "com/jni/common/CFileBase"

CFileBase* get_filebase(JNIEnv* env,jobject obj);
jobject create_java_filebase(JNIEnv* env,CFileBase *filebase, bool is_weak);
status_t register_filebase_native_methods(JNIEnv* env);

#endif
