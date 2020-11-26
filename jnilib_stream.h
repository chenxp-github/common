#ifndef __JNILIB_STREAM_H
#define __JNILIB_STREAM_H

#include "jni_helper.h"
#include "stream.h"

#define JAVA_CLASS_PATH_STREAM "com/jni/common/CStream"

CStream* get_stream(JNIEnv* env,jobject obj);
jobject create_java_stream(JNIEnv* env,CStream *stream, bool is_weak);
status_t register_stream_native_methods(JNIEnv* env);

#endif
