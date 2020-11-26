#include "jnilib_memfile.h"
#include "mem_tool.h"
#include "syslog.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_MEMFILE

JNI_GET_OBJ_FUNC(CMemFile,memfile)
JNI_GC_FUNC(CMemFile,memfile)
JNI_CREATE_JAVA_OBJ_FUNC(CMemFile,memfile)

static jint memfile__new_v1(JNIEnv* _env,jobject _this_obj)
{

    CMemFile *_this = NULL;
    NEW(_this,CMemFile);
    _this->Init();

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jint memfile__new_v2(JNIEnv* _env,jobject _this_obj, jlong _page_size, jlong _max_pages)
{
    int_ptr_t page_size = (int_ptr_t)_page_size;
    int_ptr_t max_pages = (int_ptr_t)_max_pages;

    CMemFile *_this = NULL;
    NEW(_this,CMemFile);
    _this->Init(page_size,max_pages);

    JNI_WRAP_NATIVE_OBJECT(_env,_this_obj, _this,false);
    return OK;
}

static jboolean memfile_transfer(JNIEnv* _env,jobject _this_obj, jobject _from)
{
    CMemFile *_this = get_memfile(_env,_this_obj);
    ASSERT(_this);

    CMemFile *from = get_memfile(_env,_from);
    ASSERT(from);

    jboolean _ret0 = _this->Transfer(from);


    return _ret0;
}


static const JNINativeMethod memfile_native_methods[]={
    {"_gc","()I",(void*)memfile__gc},
    {"_new","()Z",(void*)memfile__new_v1},
    {"_new","(JJ)Z",(void*)memfile__new_v2},
    {"transfer","(Lcom/jni/common/CMemFile;)Z",(void*)memfile_transfer},
};
status_t register_memfile_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(memfile_native_methods)/sizeof(memfile_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, memfile_native_methods , size) == JNI_OK);
    return OK;
}
