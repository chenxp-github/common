#include "jnilib_mmapfile.h"
#include "jnilib_stream.h"
#include "mem_tool.h"
#include "syslog.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_MMAPFILE

JNI_GET_OBJ_FUNC(CMMapFile,mmapfile)
JNI_GC_FUNC(CMMapFile,mmapfile)
JNI_CREATE_JAVA_OBJ_FUNC(CMMapFile,mmapfile)

static jint mmapfile__new(JNIEnv* _env,jobject _this_obj)
{

    CMMapFile *_this = NULL;
    NEW(_this,CMMapFile);
    _this->Init();

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jboolean mmapfile_unlink(JNIEnv* _env,jobject _this_obj)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Unlink();


    return _ret0;
}

static jboolean mmapfile_sync(JNIEnv* _env,jobject _this_obj)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Sync();


    return _ret0;
}

static jlong mmapfile_getsize(JNIEnv* _env,jobject _this_obj)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetSize();


    return _ret0;
}

static jboolean mmapfile_openreadwrite(JNIEnv* _env,jobject _this_obj, jstring _filename)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);

    const char *filename = _env->GetStringUTFChars(_filename,NULL);
    ASSERT(filename);

    jboolean _ret0 = _this->OpenReadWrite(filename);

    _env->ReleaseStringUTFChars(_filename,filename);

    return _ret0;
}

static jboolean mmapfile_opencreate(JNIEnv* _env,jobject _this_obj, jstring _filename, jint _size)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);

    const char *filename = _env->GetStringUTFChars(_filename,NULL);
    ASSERT(filename);
    int size = (int)_size;

    jboolean _ret0 = _this->OpenCreate(filename,size);

    _env->ReleaseStringUTFChars(_filename,filename);

    return _ret0;
}

static jboolean mmapfile_openreadonly(JNIEnv* _env,jobject _this_obj, jstring _filename)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);

    const char *filename = _env->GetStringUTFChars(_filename,NULL);
    ASSERT(filename);

    jboolean _ret0 = _this->OpenReadOnly(filename);

    _env->ReleaseStringUTFChars(_filename,filename);

    return _ret0;
}

static jboolean mmapfile_close(JNIEnv* _env,jobject _this_obj)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Close();


    return _ret0;
}

static jboolean mmapfile_setfilename(JNIEnv* _env,jobject _this_obj, jstring _filename)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);

    const char *filename = _env->GetStringUTFChars(_filename,NULL);
    ASSERT(filename);

    jboolean _ret0 = _this->SetFileName(filename);

    _env->ReleaseStringUTFChars(_filename,filename);

    return _ret0;
}

static jobject mmapfile_stream(JNIEnv* _env,jobject _this_obj)
{
    CMMapFile *_this = get_mmapfile(_env,_this_obj);
    ASSERT(_this);

    if(_this->GetData() == NULL)
        return NULL;
    if(_this->GetSize() == 0)
        return NULL;

    CStream *stream;
    NEW(stream,CStream);
    stream->Init();
    stream->SetRawBuf(_this->GetData(),_this->GetSize(),false);
    jobject ret0 = create_java_stream(_env,stream,false);

    return ret0;
}


static const JNINativeMethod mmapfile_native_methods[]={
    {"_gc","()I",(void*)mmapfile__gc},
    {"_new","()Z",(void*)mmapfile__new},
    {"unlink","()Z",(void*)mmapfile_unlink},
    {"sync","()Z",(void*)mmapfile_sync},
    {"getSize","()J",(void*)mmapfile_getsize},
    {"openReadWrite","(Ljava/lang/String;)Z",(void*)mmapfile_openreadwrite},
    {"openCreate","(Ljava/lang/String;I)Z",(void*)mmapfile_opencreate},
    {"openReadOnly","(Ljava/lang/String;)Z",(void*)mmapfile_openreadonly},
    {"close","()Z",(void*)mmapfile_close},
    {"setFileName","(Ljava/lang/String;)Z",(void*)mmapfile_setfilename},
    {"stream","()Lcom/jni/common/CStream;",(void*)mmapfile_stream},
};
status_t register_mmapfile_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(mmapfile_native_methods)/sizeof(mmapfile_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, mmapfile_native_methods , size) == JNI_OK);
    return OK;
}
