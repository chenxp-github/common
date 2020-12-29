#include <string.h>
#include <jni.h>
#include "cruntime.h"
#include "syslog.h"

extern jint main_init(JNIEnv* env,jobject thiz);
extern jint main_destroy(JNIEnv* env,jobject thiz);
extern jint main_mainloop(JNIEnv* env,jobject thiz);

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
jint Java_com_jni_common_NativeBridge_initNative(JNIEnv* env,jobject thiz )
{
	return main_init(env,thiz);
}

jint Java_com_jni_common_NativeBridge_destroyNative(JNIEnv* env,jobject thiz )
{
	return main_destroy(env,thiz);
}

jint Java_com_jni_common_NativeBridge_mainLoop(JNIEnv* env,jobject thiz )
{
	return main_mainloop(env,thiz);
}

#ifdef __cplusplus
} //extern "C"
#endif


