#include "jnilib_app.h"
#include "mem_tool.h"
#include "syslog.h"
#include "jni_helper.h"
#include "main_loop_context.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_APP



static jboolean app_startmessagecenter(JNIEnv* _env,jobject _this_obj, jint _port, jboolean _trust)
{
	CMainLoopContext *context = GetMainLoopContext(_env);
	ASSERT(context);
	CPeerGlobals *peer_globals = context->GetPeerGlobals();
	ASSERT(peer_globals);

    int port = (int)_port;
    bool trust = (bool)_trust;

    peer_globals->StartAccept(port);
    peer_globals->SetTrustMode(trust);

    return OK;
}


static const JNINativeMethod app_native_methods[]={
    {"StartMessageCenter","(IZ)Z",(void*)app_startmessagecenter},
};
status_t register_app_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(app_native_methods)/sizeof(app_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, app_native_methods , size) == JNI_OK);
    return OK;
}
