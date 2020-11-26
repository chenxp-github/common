#include "jnilib_minibson.h"
#include "jnilib_filebase.h"
#include "jnilib_mem.h"
#include "jnilib_stream.h"
#include "mem_tool.h"
#include "syslog.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_MINIBSON

JNI_GET_OBJ_FUNC(CMiniBson,minibson)
JNI_GC_FUNC(CMiniBson,minibson)
JNI_CREATE_JAVA_OBJ_FUNC(CMiniBson,minibson)

static jint minibson__new(JNIEnv* _env,jobject _this_obj)
{

    CMiniBson *_this = NULL;
    NEW(_this,CMiniBson);
    _this->Init();

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jboolean minibson_loadrawbuf_v1(JNIEnv* _env,jobject _this_obj, jbyteArray _data)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    int8_t *data = (int8_t*)_env->GetByteArrayElements(_data,0);
    ASSERT(data);
    int data_len = _env->GetArrayLength(_data);

    jboolean _ret0 = _this->LoadRawBuf(data,data_len);

    _env->ReleaseByteArrayElements(_data,data,0);

    return _ret0;
}

static jboolean minibson_loadrawbuf_v2(JNIEnv* _env,jobject _this_obj, jobject _buf)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *buf = get_mem(_env,_buf);
    ASSERT(buf);

    jboolean _ret0 = _this->LoadRawBuf(buf);


    return _ret0;
}

static jboolean minibson_copy(JNIEnv* _env,jobject _this_obj, jobject _p)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMiniBson *p = get_minibson(_env,_p);
    ASSERT(p);

    jboolean _ret0 = _this->Copy(p);


    return _ret0;
}

static jboolean minibson_enddocument_v1(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->EndDocument();


    return _ret0;
}

static jboolean minibson_startdocument_v1(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->StartDocument();


    return _ret0;
}

static jboolean minibson_loadbson_v1(JNIEnv* _env,jobject _this_obj, jobject _data)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CFileBase *data = get_filebase(_env,_data);
    ASSERT(data);

    jboolean _ret0 = _this->LoadBson(data);


    return _ret0;
}

static jboolean minibson_loadbson_v2(JNIEnv* _env,jobject _this_obj, jstring _fn)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *fn = _env->GetStringUTFChars(_fn,NULL);
    ASSERT(fn);

    jboolean _ret0 = _this->LoadBson(fn);

    _env->ReleaseStringUTFChars(_fn,fn);

    return _ret0;
}

static jboolean minibson_loadbson_v3(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->LoadBson();


    return _ret0;
}

static jboolean minibson_isend(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->IsEnd();


    return _ret0;
}

static jboolean minibson_getdocument(JNIEnv* _env,jobject _this_obj, jstring _name, jobject _doc)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    CMiniBson *doc = get_minibson(_env,_doc);
    ASSERT(doc);

    jboolean _ret0 = _this->GetDocument(name,doc);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jlong minibson_startdocument_v2(JNIEnv* _env,jobject _this_obj, jobject _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);

    jlong _ret0 = 0;
    _this->StartDocument(name,&_ret0);

    return _ret0;
}

static jlong minibson_startdocument_v3(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jlong _ret0 = 0;
    _this->StartDocument(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_enddocument_v2(JNIEnv* _env,jobject _this_obj, jlong _offset)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    fsize_t offset = (fsize_t)_offset;

    jboolean _ret0 = _this->EndDocument(offset);


    return _ret0;
}

static jboolean minibson_resetpointer(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->ResetPointer();


    return _ret0;
}

static jboolean minibson_getbinary(JNIEnv* _env,jobject _this_obj, jstring _name, jobject _bin)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    CMem *bin = get_mem(_env,_bin);
    ASSERT(bin);

    jboolean _ret0 = _this->GetBinary(name,bin);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_getstring_v1(JNIEnv* _env,jobject _this_obj, jstring _name, jobject _str)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    CMem *str = get_mem(_env,_str);
    ASSERT(str);

    jboolean _ret0 = _this->GetString(name,str);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jstring minibson_getstring_v2(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    CMem mem;
    mem.Init();
    _this->GetString(name,&mem);
    ASSERT(mem.StrLen() > 0);

    jstring ret0 = _env->NewStringUTF(mem.CStr());

    _env->ReleaseStringUTFChars(_name,name);

    return ret0;
}

static jboolean minibson_getboolean(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    bool _ret0 = false;
    _this->GetBoolean(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_skipstring(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->SkipString();


    return _ret0;
}

static jdouble minibson_getdouble(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jdouble _ret0 = 0;
    _this->GetDouble(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jlong minibson_getint64(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jlong _ret0 = 0;
    _this->GetInt64(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jstring minibson_tojson(JNIEnv* _env,jobject _this_obj, jboolean _bracket)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    bool bracket = (bool)_bracket;

    CMemFile json;
    json.Init();
    _this->ToJson(&json,bracket);
    if(json.GetSize() < 0)return NULL;

    CMem mem;
    mem.Init();
    mem.Copy(&json);

    jstring ret0 = _env->NewStringUTF(mem.CStr());

    return ret0;
}

static jint minibson_getint32(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jint _ret0 = 0;
    _this->GetInt32(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jint minibson_getuint32(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    uint32_t _ret0 = 0;
    _this->GetUInt32(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jshort minibson_getint16(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jshort _ret0 = 0;
    _this->GetInt16(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jshort minibson_getuint16(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    uint16_t _ret0 = 0;
    _this->GetUInt16(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jbyte minibson_getint8(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jbyte _ret0 = 0;
    _this->GetInt8(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jbyte minibson_getuint8(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    uint8_t _ret0 = 0;
    _this->GetUInt8(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putdouble_v1(JNIEnv* _env,jobject _this_obj, jstring _name, jdouble _d)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    double d = (double)_d;

    jboolean _ret0 = _this->PutDouble(name,d);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putdouble_v2(JNIEnv* _env,jobject _this_obj, jobject _name, jdouble _d)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    double d = (double)_d;

    jboolean _ret0 = _this->PutDouble(name,d);


    return _ret0;
}

static jboolean minibson_putboolean_v1(JNIEnv* _env,jobject _this_obj, jobject _name, jboolean _b)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    bool b = (bool)_b;

    jboolean _ret0 = _this->PutBoolean(name,b);


    return _ret0;
}

static jboolean minibson_putboolean_v2(JNIEnv* _env,jobject _this_obj, jstring _name, jboolean _b)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    bool b = (bool)_b;

    jboolean _ret0 = _this->PutBoolean(name,b);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putdocument_v1(JNIEnv* _env,jobject _this_obj, jstring _name, jobject _bson)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    CMiniBson *bson = get_minibson(_env,_bson);
    ASSERT(bson);

    jboolean _ret0 = _this->PutDocument(name,bson);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jint minibson_getdocumentsize(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jint _ret0 = _this->GetDocumentSize();


    return _ret0;
}

static jboolean minibson_putdocument_v2(JNIEnv* _env,jobject _this_obj, jobject _name, jobject _bson)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    CMiniBson *bson = get_minibson(_env,_bson);
    ASSERT(bson);

    jboolean _ret0 = _this->PutDocument(name,bson);


    return _ret0;
}

static jboolean minibson_writeint64(JNIEnv* _env,jobject _this_obj, jlong _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    int64_t i = (int64_t)_i;

    jboolean _ret0 = _this->WriteInt64(i);


    return _ret0;
}

static jboolean minibson_putint64_v1(JNIEnv* _env,jobject _this_obj, jobject _name, jlong _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    int64_t i = (int64_t)_i;

    jboolean _ret0 = _this->PutInt64(name,i);


    return _ret0;
}

static jboolean minibson_putint64_v2(JNIEnv* _env,jobject _this_obj, jstring _name, jlong _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    int64_t i = (int64_t)_i;

    jboolean _ret0 = _this->PutInt64(name,i);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putbinary_v1(JNIEnv* _env,jobject _this_obj, jstring _name, jobject _bin)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    CFileBase *bin = get_filebase(_env,_bin);
    ASSERT(bin);

    jboolean _ret0 = _this->PutBinary(name,bin);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putbinary_v2(JNIEnv* _env,jobject _this_obj, jobject _name, jobject _bin)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    CFileBase *bin = get_filebase(_env,_bin);
    ASSERT(bin);

    jboolean _ret0 = _this->PutBinary(name,bin);


    return _ret0;
}

static jboolean minibson_putbinary_v3(JNIEnv* _env,jobject _this_obj, jstring _name, jbyteArray _bin)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    int8_t *bin = (int8_t*)_env->GetByteArrayElements(_bin,0);
    ASSERT(bin);
    int bin_len = _env->GetArrayLength(_bin);

    jboolean _ret0 = _this->PutBinary(name,bin,bin_len);

    _env->ReleaseStringUTFChars(_name,name);
    _env->ReleaseByteArrayElements(_bin,bin,0);

    return _ret0;
}

static jboolean minibson_putbinary_v4(JNIEnv* _env,jobject _this_obj, jobject _name, jbyteArray _bin)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    int8_t *bin = (int8_t*)_env->GetByteArrayElements(_bin,0);
    ASSERT(bin);
    int bin_len = _env->GetArrayLength(_bin);

    jboolean _ret0 = _this->PutBinary(name,bin,bin_len);

    _env->ReleaseByteArrayElements(_bin,bin,0);

    return _ret0;
}

static jboolean minibson_putstring_v1(JNIEnv* _env,jobject _this_obj, jstring _name, jstring _str)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    jboolean _ret0 = _this->PutString(name,str);

    _env->ReleaseStringUTFChars(_name,name);
    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jboolean minibson_putstring_v2(JNIEnv* _env,jobject _this_obj, jstring _name, jobject _str)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    CMem *str = get_mem(_env,_str);
    ASSERT(str);

    jboolean _ret0 = _this->PutString(name,str);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putstring_v3(JNIEnv* _env,jobject _this_obj, jobject _name, jobject _str)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    CMem *str = get_mem(_env,_str);
    ASSERT(str);

    jboolean _ret0 = _this->PutString(name,str);


    return _ret0;
}

static jboolean minibson_putint32_v1(JNIEnv* _env,jobject _this_obj, jstring _name, jint _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    int32_t i = (int32_t)_i;

    jboolean _ret0 = _this->PutInt32(name,i);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putuint32_v1(JNIEnv* _env,jobject _this_obj, jstring _name, jint _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    uint32_t i = (uint32_t)_i;

    jboolean _ret0 = _this->PutUInt32(name,i);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putint16(JNIEnv* _env,jobject _this_obj, jstring _name, jshort _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    int16_t i = (int16_t)_i;

    jboolean _ret0 = _this->PutInt16(name,i);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putuint16(JNIEnv* _env,jobject _this_obj, jstring _name, jshort _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    uint16_t i = (uint16_t)_i;

    jboolean _ret0 = _this->PutUInt16(name,i);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putint8(JNIEnv* _env,jobject _this_obj, jstring _name, jbyte _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    int8_t i = (int8_t)_i;

    jboolean _ret0 = _this->PutInt8(name,i);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putuint8(JNIEnv* _env,jobject _this_obj, jstring _name, jbyte _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    uint8_t i = (uint8_t)_i;

    jboolean _ret0 = _this->PutUInt8(name,i);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_putint32_v2(JNIEnv* _env,jobject _this_obj, jobject _name, jint _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    int32_t i = (int32_t)_i;

    jboolean _ret0 = _this->PutInt32(name,i);


    return _ret0;
}

static jboolean minibson_putuint32_v2(JNIEnv* _env,jobject _this_obj, jobject _name, jint _i)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);
    uint32_t i = (uint32_t)_i;

    jboolean _ret0 = _this->PutUInt32(name,i);


    return _ret0;
}

static jboolean minibson_setrawbuf(JNIEnv* _env,jobject _this_obj, jobject _buf)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *buf = get_mem(_env,_buf);
    ASSERT(buf);

    jboolean _ret0 = _this->SetRawBuf(buf);


    return _ret0;
}

static jobject minibson_getrawdata(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    CMem* _ret0 = _this->GetRawData();
    ASSERT(_ret0);
    jobject ret0 = create_java_mem(_env,_ret0,true);


    return ret0;
}

static jlong minibson_getpointerpos(JNIEnv* _env,jobject _this_obj)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetPointerPos();


    return _ret0;
}

static jboolean minibson_setpointerpos(JNIEnv* _env,jobject _this_obj, jlong _pos)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    fsize_t pos = (fsize_t)_pos;

    jboolean _ret0 = _this->SetPointerPos(pos);


    return _ret0;
}

static jlong minibson_startarray_v1(JNIEnv* _env,jobject _this_obj, jobject _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CMem *name = get_mem(_env,_name);
    ASSERT(name);

    jlong _ret0 = 0;
    _this->StartArray(name,&_ret0);

    return _ret0;
}

static jlong minibson_startarray_v2(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jlong _ret0 = 0;
    _this->StartArray(name,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_endarray(JNIEnv* _env,jobject _this_obj, jlong _offset, jint _array_len)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    fsize_t offset = (fsize_t)_offset;
    int32_t array_len = (int32_t)_array_len;

    jboolean _ret0 = _this->EndArray(offset,array_len);


    return _ret0;
}

static jint minibson_getarray(JNIEnv* _env,jobject _this_obj, jstring _name, jobject _doc)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);
    CMiniBson *doc = get_minibson(_env,_doc);
    ASSERT(doc);

    jint _ret0 = 0;
    _this->GetArray(name,doc,&_ret0);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jboolean minibson_peeknext(JNIEnv* _env,jobject _this_obj, jobject _out)
{
    CMiniBson *_this = get_minibson(_env,_this_obj);
    ASSERT(_this);

    CStream *out = get_stream(_env,_out);
    ASSERT(out);

    int type = 0;
    LOCAL_MEM(name);

    jboolean _ret0 = _this->PeekNext(&type,&name);

    out->PutInt32(type);
    out->PutString(&name);
    return _ret0;
}

static const JNINativeMethod minibson_native_methods[]={
    {"_gc","()I",(void*)minibson__gc},
    {"_new","()Z",(void*)minibson__new},
    {"loadRawBuf","([B)Z",(void*)minibson_loadrawbuf_v1},
    {"loadRawBuf","(Lcom/jni/common/CMem;)Z",(void*)minibson_loadrawbuf_v2},
    {"copy","(Lcom/jni/common/CMiniBson;)Z",(void*)minibson_copy},
    {"endDocument","()Z",(void*)minibson_enddocument_v1},
    {"startDocument","()Z",(void*)minibson_startdocument_v1},
    {"loadBson","(Lcom/jni/common/CFileBase;)Z",(void*)minibson_loadbson_v1},
    {"loadBson","(Ljava/lang/String;)Z",(void*)minibson_loadbson_v2},
    {"loadBson","()Z",(void*)minibson_loadbson_v3},
    {"isEnd","()Z",(void*)minibson_isend},
    {"getDocument","(Ljava/lang/String;Lcom/jni/common/CMiniBson;)Z",(void*)minibson_getdocument},
    {"startDocument","(Lcom/jni/common/CMem;)J",(void*)minibson_startdocument_v2},
    {"startDocument","(Ljava/lang/String;)J",(void*)minibson_startdocument_v3},
    {"endDocument","(J)Z",(void*)minibson_enddocument_v2},
    {"resetPointer","()Z",(void*)minibson_resetpointer},
    {"getBinary","(Ljava/lang/String;Lcom/jni/common/CMem;)Z",(void*)minibson_getbinary},
    {"getString","(Ljava/lang/String;Lcom/jni/common/CMem;)Z",(void*)minibson_getstring_v1},
    {"getString","(Ljava/lang/String;)Ljava/lang/String;",(void*)minibson_getstring_v2},
    {"getBoolean","(Ljava/lang/String;)Z",(void*)minibson_getboolean},
    {"skipString","()Z",(void*)minibson_skipstring},
    {"getDouble","(Ljava/lang/String;)D",(void*)minibson_getdouble},
    {"getInt64","(Ljava/lang/String;)J",(void*)minibson_getint64},
    {"toJson","(Z)Ljava/lang/String;",(void*)minibson_tojson},
    {"getInt32","(Ljava/lang/String;)I",(void*)minibson_getint32},
    {"getUInt32","(Ljava/lang/String;)I",(void*)minibson_getuint32},
    {"getInt16","(Ljava/lang/String;)S",(void*)minibson_getint16},
    {"getUInt16","(Ljava/lang/String;)S",(void*)minibson_getuint16},
    {"getInt8","(Ljava/lang/String;)B",(void*)minibson_getint8},
    {"getUInt8","(Ljava/lang/String;)B",(void*)minibson_getuint8},
    {"putDouble","(Ljava/lang/String;D)Z",(void*)minibson_putdouble_v1},
    {"putDouble","(Lcom/jni/common/CMem;D)Z",(void*)minibson_putdouble_v2},
    {"putBoolean","(Lcom/jni/common/CMem;Z)Z",(void*)minibson_putboolean_v1},
    {"putBoolean","(Ljava/lang/String;Z)Z",(void*)minibson_putboolean_v2},
    {"putDocument","(Ljava/lang/String;Lcom/jni/common/CMiniBson;)Z",(void*)minibson_putdocument_v1},
    {"getDocumentSize","()I",(void*)minibson_getdocumentsize},
    {"putDocument","(Lcom/jni/common/CMem;Lcom/jni/common/CMiniBson;)Z",(void*)minibson_putdocument_v2},
    {"writeInt64","(J)Z",(void*)minibson_writeint64},
    {"putInt64","(Lcom/jni/common/CMem;J)Z",(void*)minibson_putint64_v1},
    {"putInt64","(Ljava/lang/String;J)Z",(void*)minibson_putint64_v2},
    {"putBinary","(Ljava/lang/String;Lcom/jni/common/CFileBase;)Z",(void*)minibson_putbinary_v1},
    {"putBinary","(Lcom/jni/common/CMem;Lcom/jni/common/CFileBase;)Z",(void*)minibson_putbinary_v2},
    {"putBinary","(Ljava/lang/String;[B)Z",(void*)minibson_putbinary_v3},
    {"putBinary","(Lcom/jni/common/CMem;[B)Z",(void*)minibson_putbinary_v4},
    {"putString","(Ljava/lang/String;Ljava/lang/String;)Z",(void*)minibson_putstring_v1},
    {"putString","(Ljava/lang/String;Lcom/jni/common/CMem;)Z",(void*)minibson_putstring_v2},
    {"putString","(Lcom/jni/common/CMem;Lcom/jni/common/CMem;)Z",(void*)minibson_putstring_v3},
    {"putInt32","(Ljava/lang/String;I)Z",(void*)minibson_putint32_v1},
    {"putUInt32","(Ljava/lang/String;I)Z",(void*)minibson_putuint32_v1},
    {"putInt16","(Ljava/lang/String;S)Z",(void*)minibson_putint16},
    {"putUInt16","(Ljava/lang/String;S)Z",(void*)minibson_putuint16},
    {"putInt8","(Ljava/lang/String;B)Z",(void*)minibson_putint8},
    {"putUInt8","(Ljava/lang/String;B)Z",(void*)minibson_putuint8},
    {"putInt32","(Lcom/jni/common/CMem;I)Z",(void*)minibson_putint32_v2},
    {"putUInt32","(Lcom/jni/common/CMem;I)Z",(void*)minibson_putuint32_v2},
    {"setRawBuf","(Lcom/jni/common/CMem;)Z",(void*)minibson_setrawbuf},
    {"getRawData","()Lcom/jni/common/CMem;",(void*)minibson_getrawdata},
    {"getPointerPos","()J",(void*)minibson_getpointerpos},
    {"setPointerPos","(J)Z",(void*)minibson_setpointerpos},
    {"startArray","(Lcom/jni/common/CMem;)J",(void*)minibson_startarray_v1},
    {"startArray","(Ljava/lang/String;)J",(void*)minibson_startarray_v2},
    {"endArray","(JI)Z",(void*)minibson_endarray},
    {"getArray","(Ljava/lang/String;Lcom/jni/common/CMiniBson;)I",(void*)minibson_getarray},
    {"peekNext","(Lcom/jni/common/CStream;)Z",(void*)minibson_peeknext},
};
status_t register_minibson_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(minibson_native_methods)/sizeof(minibson_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, minibson_native_methods , size) == JNI_OK);
    return OK;
}
