#include "jnilib_mem.h"
#include "jnilib_filebase.h"
#include "mem_tool.h"
#include "syslog.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_MEM

JNI_GET_OBJ_FUNC(CMem,mem)
JNI_GC_FUNC(CMem,mem)
JNI_CREATE_JAVA_OBJ_FUNC(CMem,mem)

static jint mem__new(JNIEnv* _env,jobject _this_obj)
{

    CMem *_this = NULL;
    NEW(_this,CMem);
    _this->Init();

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jboolean mem_ismalloc(JNIEnv* _env,jobject _this_obj)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->IsMalloc();


    return _ret0;
}

static jboolean mem_transfer(JNIEnv* _env,jobject _this_obj, jobject _from)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);

    CMem *from = get_mem(_env,_from);
    ASSERT(from);

    jboolean _ret0 = _this->Transfer(from);


    return _ret0;
}

static jboolean mem_realloc(JNIEnv* _env,jobject _this_obj, jlong _newsize)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);

    int_ptr_t newsize = (int_ptr_t)_newsize;

    jboolean _ret0 = _this->Realloc(newsize);


    return _ret0;
}

static jboolean mem_strequ(JNIEnv* _env,jobject _this_obj, jobject _str, jboolean _case_sensitive)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);

    CMem *str = get_mem(_env,_str);
    ASSERT(str);
    bool case_sensitive = (bool)_case_sensitive;

    jboolean _ret0 = _this->StrEqu(str,case_sensitive);


    return _ret0;
}

static jbyte mem_c(JNIEnv* _env,jobject _this_obj, jint _index)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);

    int index = (int)_index;

    jbyte _ret0 = _this->C(index);


    return _ret0;
}

static jboolean mem_malloc(JNIEnv* _env,jobject _this_obj, jlong _asize)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);

    int_ptr_t asize = (int_ptr_t)_asize;

    jboolean _ret0 = _this->Malloc(asize);


    return _ret0;
}

static jboolean mem_zero(JNIEnv* _env,jobject _this_obj)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Zero();


    return _ret0;
}

static jboolean mem_copy(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->Copy(file);


    return _ret0;
}

static jstring mem_cstr(JNIEnv* _env,jobject _this_obj)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);


    const char* _ret0 = _this->CStr();
    ASSERT(_ret0);
    jstring ret0 = _env->NewStringUTF(_ret0);


    return ret0;
}

static jboolean mem_init(JNIEnv* _env,jobject _this_obj)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Init();


    return _ret0;
}

static jboolean mem_destroy(JNIEnv* _env,jobject _this_obj)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Destroy();


    return _ret0;
}

static jlong mem_getrawbuf(JNIEnv* _env,jobject _this_obj)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = (jlong)_this->GetRawBuf();


    return _ret0;
}

static jboolean mem_setrawbuf(JNIEnv* _env,jobject _this_obj, jlong _buf, jlong _size, jboolean _is_const)
{
    CMem *_this = get_mem(_env,_this_obj);
    ASSERT(_this);

    int_ptr_t buf = (int_ptr_t)_buf;
    int_ptr_t size = (int_ptr_t)_size;
    bool is_const = (bool)_is_const;

    jboolean _ret0 = _this->SetRawBuf((void*)buf,size,is_const);


    return _ret0;
}


static const JNINativeMethod mem_native_methods[]={
    {"_gc","()I",(void*)mem__gc},
    {"_new","()Z",(void*)mem__new},
    {"isMalloc","()Z",(void*)mem_ismalloc},
    {"transfer","(Lcom/jni/common/CMem;)Z",(void*)mem_transfer},
    {"realloc","(J)Z",(void*)mem_realloc},
    {"strEqu","(Lcom/jni/common/CMem;Z)Z",(void*)mem_strequ},
    {"c","(I)B",(void*)mem_c},
    {"malloc","(J)Z",(void*)mem_malloc},
    {"zero","()Z",(void*)mem_zero},
    {"copy","(Lcom/jni/common/CFileBase;)Z",(void*)mem_copy},
    {"cStr","()Ljava/lang/String;",(void*)mem_cstr},
    {"init","()Z",(void*)mem_init},
    {"getRawBuf","()J",(void*)mem_getrawbuf},
    {"setRawBuf","(JJZ)Z",(void*)mem_setrawbuf},
};
status_t register_mem_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(mem_native_methods)/sizeof(mem_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, mem_native_methods , size) == JNI_OK);
    return OK;
}
