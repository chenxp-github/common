#include "jnilib_stream.h"
#include "jnilib_mem.h"
#include "mem_tool.h"
#include "syslog.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_STREAM

JNI_GET_OBJ_FUNC(CStream,stream)
JNI_GC_FUNC(CStream,stream)
JNI_CREATE_JAVA_OBJ_FUNC(CStream,stream)

static jint stream__new(JNIEnv* _env,jobject _this_obj)
{

    CStream *_this = NULL;
    NEW(_this,CStream);
    _this->Init();

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jboolean stream_copy(JNIEnv* _env,jobject _this_obj, jobject __p)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    CStream *_p = get_stream(_env,__p);
    ASSERT(_p);

    jboolean _ret0 = _this->Copy(_p);


    return _ret0;
}

static jboolean stream_putstring_v1(JNIEnv* _env,jobject _this_obj, jstring _str)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    jboolean _ret0 = _this->PutString(str);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jstring stream_getstring(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    CMem mem;
    mem.Init();
    _this->GetString(&mem);
    ASSERT(mem.StrLen() > 0);
    jstring ret0 = _env->NewStringUTF(mem.CStr());

    return ret0;
}

static jboolean stream_putstring_v2(JNIEnv* _env,jobject _this_obj, jobject _str)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    CMem *str = get_mem(_env,_str);
    ASSERT(str);

    jboolean _ret0 = _this->PutString(str);


    return _ret0;
}

static jboolean stream_getbinary_v1(JNIEnv* _env,jobject _this_obj, jobject _out)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    CMem *out = get_mem(_env,_out);
    ASSERT(out);

    jboolean _ret0 = _this->GetBinary(out);


    return _ret0;
}

static jbyteArray stream_getbinary_v2(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    CMem bin;
    bin.Init();
    _this->GetBinary(&bin);
    if(bin.GetSize() <= 0)
    	return NULL;

    int _ret0_len = bin.GetSize();
    jbyteArray ret0 = _env->NewByteArray(_ret0_len);
    _env->SetByteArrayRegion(ret0,0,_ret0_len,(const jbyte*)bin.GetRawBuf());



    return ret0;
}

static jboolean stream_putbinary(JNIEnv* _env,jobject _this_obj, jobject _bin)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    CMem *bin = get_mem(_env,_bin);
    ASSERT(bin);

    jboolean _ret0 = _this->PutBinary(bin);


    return _ret0;
}

static jboolean stream_getzeroendstring_v1(JNIEnv* _env,jobject _this_obj, jobject _str)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    CMem *str = get_mem(_env,_str);
    ASSERT(str);

    jboolean _ret0 = _this->GetZeroEndString(str);


    return _ret0;
}

static jstring stream_getzeroendstring_v2(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    CMem str;
    str.Init();
    _this->GetZeroEndString(&str);
    if(str.StrLen() == 0)return NULL;
    jstring ret0 = _env->NewStringUTF(str.CStr());

    return ret0;
}

static jboolean stream_putzeroendstring(JNIEnv* _env,jobject _this_obj, jstring _str)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    jboolean _ret0 = _this->PutZeroEndString(str);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jboolean stream_putdouble(JNIEnv* _env,jobject _this_obj, jdouble _d)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    double d = (double)_d;

    jboolean _ret0 = _this->PutDouble(d);


    return _ret0;
}

static jdouble stream_getdouble(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jdouble _ret0 = _this->GetDouble();


    return _ret0;
}

static jboolean stream_putbool(JNIEnv* _env,jobject _this_obj, jboolean _b)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    bool b = (bool)_b;

    jboolean _ret0 = _this->PutBool(b);


    return _ret0;
}

static jboolean stream_getbool(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->GetBool();


    return _ret0;
}

static jboolean stream_putint8(JNIEnv* _env,jobject _this_obj, jbyte _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    int8_t i = (int8_t)_i;

    jboolean _ret0 = _this->PutInt8(i);


    return _ret0;
}

static jbyte stream_getint8(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jbyte _ret0 = _this->GetInt8();


    return _ret0;
}

static jboolean stream_putint16(JNIEnv* _env,jobject _this_obj, jshort _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    int16_t i = (int16_t)_i;

    jboolean _ret0 = _this->PutInt16(i);


    return _ret0;
}

static jshort stream_getint16(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jshort _ret0 = _this->GetInt16();


    return _ret0;
}

static jboolean stream_putint32(JNIEnv* _env,jobject _this_obj, jint _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    int32_t i = (int32_t)_i;

    jboolean _ret0 = _this->PutInt32(i);


    return _ret0;
}

static jint stream_getint32(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jint _ret0 = _this->GetInt32();


    return _ret0;
}

static jboolean stream_putint64(JNIEnv* _env,jobject _this_obj, jlong _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    int64_t i = (int64_t)_i;

    jboolean _ret0 = _this->PutInt64(i);


    return _ret0;
}

static jlong stream_getint64(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetInt64();


    return _ret0;
}

static jboolean stream_putuint8(JNIEnv* _env,jobject _this_obj, jbyte _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    uint8_t i = (uint8_t)_i;

    jboolean _ret0 = _this->PutUInt8(i);


    return _ret0;
}

static jbyte stream_getuint8(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jbyte _ret0 = _this->GetUInt8();


    return _ret0;
}

static jboolean stream_putuint16(JNIEnv* _env,jobject _this_obj, jshort _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    uint16_t i = (uint16_t)_i;

    jboolean _ret0 = _this->PutUInt16(i);


    return _ret0;
}

static jshort stream_getuint16(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jshort _ret0 = _this->GetUInt16();


    return _ret0;
}

static jboolean stream_putuint32(JNIEnv* _env,jobject _this_obj, jint _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    uint32_t i = (uint32_t)_i;

    jboolean _ret0 = _this->PutUInt32(i);


    return _ret0;
}

static jint stream_getuint32(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jint _ret0 = _this->GetUInt32();


    return _ret0;
}

static jboolean stream_putuint64(JNIEnv* _env,jobject _this_obj, jlong _i)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);

    uint64_t i = (uint64_t)_i;

    jboolean _ret0 = _this->PutUInt64(i);


    return _ret0;
}

static jlong stream_getuint64(JNIEnv* _env,jobject _this_obj)
{
    CStream *_this = get_stream(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetUInt64();


    return _ret0;
}


static const JNINativeMethod stream_native_methods[]={
    {"_gc","()I",(void*)stream__gc},
    {"_new","()Z",(void*)stream__new},
    {"copy","(Lcom/jni/common/CStream;)Z",(void*)stream_copy},
    {"putString","(Ljava/lang/String;)Z",(void*)stream_putstring_v1},
    {"getString","()Ljava/lang/String;",(void*)stream_getstring},
    {"putString","(Lcom/jni/common/CMem;)Z",(void*)stream_putstring_v2},
    {"getBinary","(Lcom/jni/common/CMem;)Z",(void*)stream_getbinary_v1},
    {"getBinary","()[B",(void*)stream_getbinary_v2},
    {"putBinary","(Lcom/jni/common/CMem;)Z",(void*)stream_putbinary},
    {"getZeroEndString","(Lcom/jni/common/CMem;)Z",(void*)stream_getzeroendstring_v1},
    {"getZeroEndString","()Ljava/lang/String;",(void*)stream_getzeroendstring_v2},
    {"putZeroEndString","(Ljava/lang/String;)Z",(void*)stream_putzeroendstring},
    {"putDouble","(D)Z",(void*)stream_putdouble},
    {"getDouble","()D",(void*)stream_getdouble},
    {"putBool","(Z)Z",(void*)stream_putbool},
    {"getBool","()Z",(void*)stream_getbool},
    {"putInt8","(B)Z",(void*)stream_putint8},
    {"getInt8","()B",(void*)stream_getint8},
    {"putInt16","(S)Z",(void*)stream_putint16},
    {"getInt16","()S",(void*)stream_getint16},
    {"putInt32","(I)Z",(void*)stream_putint32},
    {"getInt32","()I",(void*)stream_getint32},
    {"putInt64","(J)Z",(void*)stream_putint64},
    {"getInt64","()J",(void*)stream_getint64},
    {"putUInt8","(B)Z",(void*)stream_putuint8},
    {"getUInt8","()B",(void*)stream_getuint8},
    {"putUInt16","(S)Z",(void*)stream_putuint16},
    {"getUInt16","()S",(void*)stream_getuint16},
    {"putUInt32","(I)Z",(void*)stream_putuint32},
    {"getUInt32","()I",(void*)stream_getuint32},
    {"putUInt64","(J)Z",(void*)stream_putuint64},
    {"getUInt64","()J",(void*)stream_getuint64},
};
status_t register_stream_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(stream_native_methods)/sizeof(stream_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, stream_native_methods , size) == JNI_OK);
    return OK;
}
