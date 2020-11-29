#include "jnilib_serversidepeer.h"
#include "jnilib_minibson.h"
#include "mem_tool.h"
#include "syslog.h"
#include "jni_helper.h"
#include "main_loop_context_manager.h"
#include "java_callback.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_SERVERSIDEPEER

JNI_GET_OBJ_FUNC(CServerSidePeer,serversidepeer)
JNI_GC_FUNC(CServerSidePeer,serversidepeer)
JNI_CREATE_JAVA_OBJ_FUNC(CServerSidePeer,serversidepeer)

static jint serversidepeer__new(JNIEnv* _env,jobject _this_obj)
{
    CMainLoopContext *context = GetMainLoopContext(_env);
    ASSERT(context);

    CServerSidePeer *_this = NULL;
    NEW(_this,CServerSidePeer);
    _this->Init(context->GetTaskMgr(),context->GetPeerGlobals());

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jint serversidepeer_getaliveclientnumber(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    jint _ret0 = _this->GetAliveClientNumber();


    return _ret0;
}

static jboolean serversidepeer_clearsendingqueue(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->ClearSendingQueue();


    return _ret0;
}

static jint serversidepeer_getsendingqueuelength(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    jint _ret0 = _this->GetSendingQueueLength();


    return _ret0;
}

static jboolean serversidepeer_resumefetchmessagetask(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->ResumeFetchMessageTask();


    return _ret0;
}

static jboolean serversidepeer_suspendfetchmessagetask(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->SuspendFetchMessageTask();


    return _ret0;
}

static jboolean serversidepeer_startfetchmessagetask(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->StartFetchMessageTask();


    return _ret0;
}

static jboolean serversidepeer_setcanfetchmessage(JNIEnv* _env,jobject _this_obj, jboolean _can)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);

    bool can = (bool)_can;

    jboolean _ret0 = _this->SetCanFetchMessage(can);


    return _ret0;
}

static jboolean serversidepeer_connect(JNIEnv* _env,jobject _this_obj, jstring _peer_name)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);

    const char *peer_name = _env->GetStringUTFChars(_peer_name,NULL);
    ASSERT(peer_name);

    jboolean _ret0 = _this->Connect(peer_name);

    _env->ReleaseStringUTFChars(_peer_name,peer_name);

    return _ret0;
}

static jboolean serversidepeer_start(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Start();


    return _ret0;
}

static jstring serversidepeer_getname(JNIEnv* _env,jobject _this_obj)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);


    CMem* _ret0 = _this->GetName();
    ASSERT(_ret0 && _ret0->CStr());
    jstring ret0 = _env->NewStringUTF(_ret0->CStr());


    return ret0;
}

static jboolean serversidepeer_setname(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jboolean _ret0 = _this->SetName(name);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean serversidepeer_sendmessage_v1(JNIEnv* _env,jobject _this_obj, jstring _to, jint _func, jint _cbid, jint _flags, jstring _body)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);

    const char *to = _env->GetStringUTFChars(_to,NULL);
    ASSERT(to);
    int func = (int)_func;
    int cbid = (int)_cbid;
    uint32_t flags = (uint32_t)_flags;
    const char *body = _env->GetStringUTFChars(_body,NULL);
    
    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->SetTo(to);
    msg->SetFunc(func);
    msg->SetCallbackId(cbid);
    msg->SetFlags(flags);
    int body_type = msg->GetBodyType();

    ASSERT(body_type == CPeerMessage::JSON || body_type == CPeerMessage::STRING);

    if(body)
    {
        msg->SetBodyString(body);
    }
    
    msg->SetBodyType(body_type);

    jboolean _ret0 = _this->SendMessage(msg);

    _env->ReleaseStringUTFChars(_to,to);
    _env->ReleaseStringUTFChars(_body,body);

    return _ret0;
}

static jboolean serversidepeer_sendmessage_v2(JNIEnv* _env,jobject _this_obj, jstring _to, jint _func, jint _cbid, jint _flags, jobject _bson)
{
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);

    const char *to = _env->GetStringUTFChars(_to,NULL);
    ASSERT(to);
    int func = (int)_func;
    int cbid = (int)_cbid;
    uint32_t flags = (uint32_t)_flags;
    CMiniBson *bson = get_minibson(_env,_bson);
    ASSERT(bson);

    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->SetTo(to);
    msg->SetFunc(func);
    msg->SetCallbackId(cbid);
    msg->SetFlags(flags);
    int body_type = msg->GetBodyType();

    ASSERT(body_type == CPeerMessage::BSON);

    if(bson)
    {
        msg->TransferBody(bson->GetRawData());   
    }
    
    msg->SetBodyType(body_type);
    
    jboolean _ret0 = _this->SendMessage(msg);    

    _env->ReleaseStringUTFChars(_to,to);

    return _ret0;
}

static jboolean serversidepeer_setonmessage(JNIEnv* _env,jobject _this_obj, jobject _onmessage_callback_obj)
{    
    CServerSidePeer *_this = get_serversidepeer(_env,_this_obj);
    ASSERT(_this);

    BEGIN_CLOSURE_FUNC(on_message)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(jobject,_onmessage_callback_obj,11);
        CLOSURE_PARAM_PTR(JNIEnv*,_env,12);

        CJavaCallback cb;
        cb.Init(_env,_onmessage_callback_obj);
        cb.Put("event",event);

        if(event == PEER_EVENT_GOT_MESSAGE)
        {
            CLOSURE_PARAM_PTR(CPeerMessage*,msg,1);
            ASSERT(msg);
            ASSERT(msg->GetFrom());

            cb.Put("method",msg->GetFunc());
            cb.Put("from",msg->GetFrom()->CStr());
            cb.Put("callback_id",msg->GetCallbackId());
            cb.Put("flags",(int)msg->GetFlags());
            cb.Put("body_type",(int)msg->GetBodyType());
            if(msg->GetBodyType() == CPeerMessage::STRING)
            {
                CMem *val = msg->GetBody();
                ASSERT(val);
                cb.Put("body",val->CStr());
            }
            else
            {
                CMiniBson tmp_bson;
                tmp_bson.Init();
                tmp_bson.LoadRawBuf(msg->GetBody());                
                cb.Put("body",create_java_minibson(_env,&tmp_bson,true));				
            }
        }

        cb.Run();
        cb.Clear();
        return OK;
    }
    END_CLOSURE_FUNC(on_message);
    
    _this->Callback()->SetFunc(on_message);
    _this->Callback()->SetParamPointer(11,_onmessage_callback_obj);    
    _this->Callback()->SetParamPointer(12,_env);
    return OK;
}


static const JNINativeMethod serversidepeer_native_methods[]={
    {"_gc","()I",(void*)serversidepeer__gc},
    {"_new","()Z",(void*)serversidepeer__new},
    {"getAliveClientNumber","()I",(void*)serversidepeer_getaliveclientnumber},
    {"clearSendingQueue","()Z",(void*)serversidepeer_clearsendingqueue},
    {"getSendingQueueLength","()I",(void*)serversidepeer_getsendingqueuelength},
    {"resumeFetchMessageTask","()Z",(void*)serversidepeer_resumefetchmessagetask},
    {"suspendFetchMessageTask","()Z",(void*)serversidepeer_suspendfetchmessagetask},
    {"startFetchMessageTask","()Z",(void*)serversidepeer_startfetchmessagetask},
    {"setCanFetchMessage","(Z)Z",(void*)serversidepeer_setcanfetchmessage},
    {"connect","(Ljava/lang/String;)Z",(void*)serversidepeer_connect},
    {"start","()Z",(void*)serversidepeer_start},
    {"getName","()Ljava/lang/String;",(void*)serversidepeer_getname},
    {"setName","(Ljava/lang/String;)Z",(void*)serversidepeer_setname},
    {"sendMessage","(Ljava/lang/String;IIILjava/lang/String;)Z",(void*)serversidepeer_sendmessage_v1},
    {"sendMessage","(Ljava/lang/String;IIILcom/jni/common/CMiniBson;)Z",(void*)serversidepeer_sendmessage_v2},    
    {"setOnMessage","(Ljava/lang/Object;)Z",(void*)serversidepeer_setonmessage},
};
status_t register_serversidepeer_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(serversidepeer_native_methods)/sizeof(serversidepeer_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, serversidepeer_native_methods , size) == JNI_OK);
    return OK;
}
