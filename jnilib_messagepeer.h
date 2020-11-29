#ifndef __JNILIB_MESSAGEPEER_H
#define __JNILIB_MESSAGEPEER_H

#include "jni_helper.h"
#include "messagepeer.h"

#define JAVA_CLASS_PATH_MESSAGEPEER "com/jni/common/CMessagePeer"

CMessagePeer* get_messagepeer(JNIEnv* env,jobject obj);
jobject create_java_messagepeer(JNIEnv* env,CMessagePeer *messagepeer, bool is_weak);
status_t register_messagepeer_native_methods(JNIEnv* env);

#endif
