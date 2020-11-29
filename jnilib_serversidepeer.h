#ifndef __JNILIB_SERVERSIDEPEER_H
#define __JNILIB_SERVERSIDEPEER_H

#include "jni_helper.h"
#include "serversidepeer.h"

#define JAVA_CLASS_PATH_SERVERSIDEPEER "com/jni/common/CServerSidePeer"

CServerSidePeer* get_serversidepeer(JNIEnv* env,jobject obj);
jobject create_java_serversidepeer(JNIEnv* env,CServerSidePeer *serversidepeer, bool is_weak);
status_t register_serversidepeer_native_methods(JNIEnv* env);

#endif
