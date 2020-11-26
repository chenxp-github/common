#include "jnilib_file.h"
#include "mem_tool.h"
#include "syslog.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_FILE

JNI_GET_OBJ_FUNC(CFile,file)
JNI_GC_FUNC(CFile,file)
JNI_CREATE_JAVA_OBJ_FUNC(CFile,file)

static jint file__new(JNIEnv* _env,jobject _this_obj)
{

    CFile *_this = NULL;
    NEW(_this,CFile);
    _this->Init();

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jboolean file_isopened(JNIEnv* _env,jobject _this_obj)
{
    CFile *_this = get_file(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->IsOpened();


    return _ret0;
}

static jboolean file_openfile(JNIEnv* _env,jobject _this_obj, jstring _fn, jstring _mode)
{
    CFile *_this = get_file(_env,_this_obj);
    ASSERT(_this);

    const char *fn = _env->GetStringUTFChars(_fn,NULL);
    ASSERT(fn);
    const char *mode = _env->GetStringUTFChars(_mode,NULL);
    ASSERT(mode);

    jboolean _ret0 = _this->OpenFile(fn,mode);

    _env->ReleaseStringUTFChars(_fn,fn);
    _env->ReleaseStringUTFChars(_mode,mode);

    return _ret0;
}

static jboolean file_closefile(JNIEnv* _env,jobject _this_obj)
{
    CFile *_this = get_file(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->CloseFile();


    return _ret0;
}

static jlong file_getsizereal(JNIEnv* _env,jobject _this_obj)
{
    CFile *_this = get_file(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetSizeReal();


    return _ret0;
}

static jboolean file_hasbuffer(JNIEnv* _env,jobject _this_obj)
{
    CFile *_this = get_file(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->HasBuffer();


    return _ret0;
}

static jboolean file_setbufsize(JNIEnv* _env,jobject _this_obj, jlong _size)
{
    CFile *_this = get_file(_env,_this_obj);
    ASSERT(_this);

    int_ptr_t size = (int_ptr_t)_size;

    jboolean _ret0 = _this->SetBufSize(size);


    return _ret0;
}


static const JNINativeMethod file_native_methods[]={
    {"_gc","()I",(void*)file__gc},
    {"_new","()Z",(void*)file__new},
    {"isOpened","()Z",(void*)file_isopened},
    {"openFile","(Ljava/lang/String;Ljava/lang/String;)Z",(void*)file_openfile},
    {"closeFile","()Z",(void*)file_closefile},
    {"getSizeReal","()J",(void*)file_getsizereal},
    {"hasBuffer","()Z",(void*)file_hasbuffer},
    {"setBufSize","(J)Z",(void*)file_setbufsize},
};
status_t register_file_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(file_native_methods)/sizeof(file_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, file_native_methods , size) == JNI_OK);
    return OK;
}
