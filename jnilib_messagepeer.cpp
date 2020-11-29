#include "jnilib_messagepeer.h"
#include "jnilib_mem.h"
#include "jnilib_minibson.h"
#include "mem_tool.h"
#include "syslog.h"
#include "main_loop_context.h"
#include "java_callback.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_MESSAGEPEER

JNI_GET_OBJ_FUNC(CMessagePeer,messagepeer)
JNI_GC_FUNC(CMessagePeer,messagepeer)
JNI_CREATE_JAVA_OBJ_FUNC(CMessagePeer,messagepeer)

static jint messagepeer__new(JNIEnv* _env,jobject _this_obj)
{
    CMainLoopContext *context = GetMainLoopContext(_env);
    ASSERT(context);

    CMessagePeer *_this = NULL;
    NEW(_this,CMessagePeer);
    _this->Init(context->GetTaskMgr());

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jboolean messagepeer_setmaxretries(JNIEnv* _env,jobject _this_obj, jint _max_retries)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    int max_retries = (int)_max_retries;

    jboolean _ret0 = _this->SetMaxRetries(max_retries);


    return _ret0;
}

static jboolean messagepeer_clearsendingqueue(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->ClearSendingQueue();


    return _ret0;
}

static jint messagepeer_getsendingqueuelength(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);


    jint _ret0 = _this->GetSendingQueueLength();


    return _ret0;
}

static jboolean messagepeer_isconnected(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->IsConnected();


    return _ret0;
}

static jboolean messagepeer_setcanfetchmessage(JNIEnv* _env,jobject _this_obj, jboolean _can_fetch)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    bool can_fetch = (bool)_can_fetch;

    jboolean _ret0 = _this->SetCanFetchMessage(can_fetch);


    return _ret0;
}

static jobjectArray messagepeer_getallconnectedpeers(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    CMemStk *all = _this->GetAllConnectedPeers();
    ASSERT(all);    
    jobjectArray ret0 = BuildJavaStringArray(_env,all);

    return ret0;
}

static jboolean messagepeer_connect(JNIEnv* _env,jobject _this_obj, jstring _peer_name)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    const char *peer_name = _env->GetStringUTFChars(_peer_name,NULL);
    ASSERT(peer_name);

    jboolean _ret0 = _this->Connect(peer_name);

    _env->ReleaseStringUTFChars(_peer_name,peer_name);

    return _ret0;
}

static jboolean messagepeer_start(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Start();


    return _ret0;
}

static jboolean messagepeer_setname_v1(JNIEnv* _env,jobject _this_obj, jobject _name)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);

    jboolean _ret0 = _this->SetName(name);


    return _ret0;
}

static jboolean messagepeer_setname_v2(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jboolean _ret0 = _this->SetName(name);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean messagepeer_setserver_v1(JNIEnv* _env,jobject _this_obj, jobject _server)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    CMem *server = get_mem(_env,_server);
    ASSERT(server);

    jboolean _ret0 = _this->SetServer(server);


    return _ret0;
}

static jboolean messagepeer_setserver_v2(JNIEnv* _env,jobject _this_obj, jstring _server)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    const char *server = _env->GetStringUTFChars(_server,NULL);
    ASSERT(server);

    jboolean _ret0 = _this->SetServer(server);

    _env->ReleaseStringUTFChars(_server,server);

    return _ret0;
}

static jboolean messagepeer_setport(JNIEnv* _env,jobject _this_obj, jint _port)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);

    int port = (int)_port;

    jboolean _ret0 = _this->SetPort(port);


    return _ret0;
}

static jstring messagepeer_getname(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);


    CMem* _ret0 = _this->GetName();
    ASSERT(_ret0);
    ASSERT(_ret0->CStr());
    jstring ret0 = _env->NewStringUTF(_ret0->CStr());

    return ret0;
}

static jstring messagepeer_getserver(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);


    CMem* _ret0 = _this->GetServer();
    ASSERT(_ret0 && _ret0->CStr());
    jstring ret0 = _env->NewStringUTF(_ret0->CStr());


    return ret0;
}

static jint messagepeer_getport(JNIEnv* _env,jobject _this_obj)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
    ASSERT(_this);


    jint _ret0 = _this->GetPort();


    return _ret0;
}

static jboolean messagepeer_sendmessage_v1(JNIEnv* _env,jobject _this_obj, jstring _to, jint _func, jint _cbid, jint _flags, jstring _body)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
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

static jboolean messagepeer_sendmessage_v2(JNIEnv* _env,jobject _this_obj, jstring _to, jint _func, jint _cbid, jint _flags, jobject _bson)
{
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
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

static jboolean messagepeer_setonmessage(JNIEnv* _env,jobject _this_obj, jobject _onmessage_callback_obj)
{    
    CMessagePeer *_this = get_messagepeer(_env,_this_obj);
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


static const JNINativeMethod messagepeer_native_methods[]={
    {"_gc","()I",(void*)messagepeer__gc},
    {"_new","()Z",(void*)messagepeer__new},
    {"setMaxRetries","(I)Z",(void*)messagepeer_setmaxretries},
    {"clearSendingQueue","()Z",(void*)messagepeer_clearsendingqueue},
    {"getSendingQueueLength","()I",(void*)messagepeer_getsendingqueuelength},
    {"isConnected","()Z",(void*)messagepeer_isconnected},
    {"setCanFetchMessage","(Z)Z",(void*)messagepeer_setcanfetchmessage},
    {"getAllConnectedPeers","()[Ljava/lang/String;",(void*)messagepeer_getallconnectedpeers},
    {"connect","(Ljava/lang/String;)Z",(void*)messagepeer_connect},
    {"start","()Z",(void*)messagepeer_start},
    {"setName","(Lcom/jni/common/CMem;)Z",(void*)messagepeer_setname_v1},
    {"setName","(Ljava/lang/String;)Z",(void*)messagepeer_setname_v2},
    {"setServer","(Lcom/jni/common/CMem;)Z",(void*)messagepeer_setserver_v1},
    {"setServer","(Ljava/lang/String;)Z",(void*)messagepeer_setserver_v2},
    {"setPort","(I)Z",(void*)messagepeer_setport},
    {"getName","()Ljava/lang/String;",(void*)messagepeer_getname},
    {"getServer","()Ljava/lang/String;",(void*)messagepeer_getserver},
    {"getPort","()I",(void*)messagepeer_getport},
    {"sendMessage","(Ljava/lang/String;IIILjava/lang/String;)Z",(void*)messagepeer_sendmessage_v1},
    {"sendMessage","(Ljava/lang/String;IIILcom/jni/common/CMiniBson;)Z",(void*)messagepeer_sendmessage_v2},    
    {"setOnMessage","(Ljava/lang/Object;)Z",(void*)messagepeer_setonmessage},
};
status_t register_messagepeer_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(messagepeer_native_methods)/sizeof(messagepeer_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, messagepeer_native_methods , size) == JNI_OK);
    return OK;
}
