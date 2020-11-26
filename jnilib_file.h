#ifndef __JNILIB_FILE_H
#define __JNILIB_FILE_H

#include "jni_helper.h"
#include "file.h"

#define JAVA_CLASS_PATH_FILE "com/jni/common/CFile"

CFile* get_file(JNIEnv* env,jobject obj);
jobject create_java_file(JNIEnv* env,CFile *file, bool is_weak);
status_t register_file_native_methods(JNIEnv* env);

#endif
