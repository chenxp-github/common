#include "jnilib_filemanager.h"
#include "jnilib_filebase.h"
#include "mem_tool.h"
#include "syslog.h"
#include "dirmgr.h"
#include "fileutils.h"
#include "java_callback.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_FILEMANAGER

static jboolean filemanager_deletefile(JNIEnv* _env,jobject _this_obj, jstring _name)
{
    const char *name = _env->GetStringUTFChars(_name,NULL);
    ASSERT(name);

    jboolean _ret0 = CDirMgr::DeleteFile(name);

    _env->ReleaseStringUTFChars(_name,name);

    return _ret0;
}

static jstring filemanager_readlink(JNIEnv* _env,jobject _this_obj, jstring _link_name)
{
    const char *link_name = _env->GetStringUTFChars(_link_name,NULL);
    ASSERT(link_name);

    char buf[LBUF_SIZE];

    int n = readlink(link_name,buf,sizeof(buf));
    if(n <= 0) return NULL;
    buf[n] = 0; 

    jstring ret0 = _env->NewStringUTF(buf);

    _env->ReleaseStringUTFChars(_link_name,link_name);

    return ret0;
}

static jboolean filemanager_savefile(JNIEnv* _env,jobject _this_obj, jstring _path, jobject _file)
{
    const char *path = _env->GetStringUTFChars(_path,NULL);
    ASSERT(path);
    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    CMem mem_path(path);
    jboolean _ret0 = CDirMgr::SaveFileSuper(&mem_path,file);

    _env->ReleaseStringUTFChars(_path,path);

    return _ret0;
}

static jboolean filemanager_createdir(JNIEnv* _env,jobject _this_obj, jstring _path)
{
    const char *path = _env->GetStringUTFChars(_path,NULL);
    ASSERT(path);
    CMem mem_path(path);
    jboolean _ret0 = CDirMgr::CreateDirSuper(&mem_path);

    _env->ReleaseStringUTFChars(_path,path);

    return _ret0;
}

static jboolean filemanager_copysinglefile_v1(JNIEnv* _env,jobject _this_obj, jstring _from, jstring _to, jlong _start, jlong _size, jlong _to_start, jint _mode)
{
    const char *from = _env->GetStringUTFChars(_from,NULL);
    ASSERT(from);
    const char *to = _env->GetStringUTFChars(_to,NULL);
    ASSERT(to);
    
    CMem mem_from(from);
    CMem mem_to(to);

    fsize_t start = (fsize_t)_start;
    fsize_t size = (fsize_t)_size;
    fsize_t to_start = (fsize_t)_to_start;
    uint32_t mode = (uint32_t)_mode;

    jboolean _ret0 = (int)CopySingleFile_Interruptable(
        &mem_from,&mem_to,start,size,to_start,mode,
        NULL
    );

    _env->ReleaseStringUTFChars(_from,from);
    _env->ReleaseStringUTFChars(_to,to);

    return _ret0;
}

static jboolean filemanager_copysinglefile_v2(JNIEnv* _env,jobject _this_obj, jstring _from, jstring _to, jlong _start, jlong _size, jint _mode)
{
    const char *from = _env->GetStringUTFChars(_from,NULL);
    ASSERT(from);
    const char *to = _env->GetStringUTFChars(_to,NULL);
    ASSERT(to);

    CMem mem_from(from);
    CMem mem_to(to);

    fsize_t start = (fsize_t)_start;
    fsize_t size = (fsize_t)_size;
    uint32_t mode = (uint32_t)_mode;

    jboolean _ret0 = (int)CopySingleFile_Interruptable(
        &mem_from,&mem_to,start,size,mode,
        NULL
    );

    _env->ReleaseStringUTFChars(_from,from);
    _env->ReleaseStringUTFChars(_to,to);

    return _ret0;
}

static jboolean filemanager_copysinglefile_v3(JNIEnv* _env,jobject _this_obj, jstring _from, jstring _to, jint _mode)
{
    const char *from = _env->GetStringUTFChars(_from,NULL);
    ASSERT(from);
    const char *to = _env->GetStringUTFChars(_to,NULL);
    ASSERT(to);

    CMem mem_from(from);
    CMem mem_to(to);


    uint32_t mode = (uint32_t)_mode;

    jboolean _ret0 = (int)CopySingleFile_Interruptable(
        &mem_from,&mem_to,mode,NULL
    );

    _env->ReleaseStringUTFChars(_from,from);
    _env->ReleaseStringUTFChars(_to,to);

    return _ret0;
}

static jboolean filemanager_istextfile_v1(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = IsTextFile(file);


    return _ret0;
}

static jboolean filemanager_istextfile_v2(JNIEnv* _env,jobject _this_obj, jstring _fn)
{
    const char *fn = _env->GetStringUTFChars(_fn,NULL);
    ASSERT(fn);

    jboolean _ret0 = IsTextFile(fn);

    _env->ReleaseStringUTFChars(_fn,fn);

    return _ret0;
}

static jboolean filemanager_converteol_v1(JNIEnv* _env,jobject _this_obj, jobject _in, jobject _out, jint _mode)
{
    CFileBase *in = get_filebase(_env,_in);
    ASSERT(in);
    CFileBase *out = get_filebase(_env,_out);
    ASSERT(out);
    int mode = (int)_mode;

    jboolean _ret0 = ConvertEol(in,out,mode);


    return _ret0;
}

static jboolean filemanager_converteol_v2(JNIEnv* _env,jobject _this_obj, jstring _in, jstring _out, jint _mode)
{
    const char *in = _env->GetStringUTFChars(_in,NULL);
    ASSERT(in);
    const char *out = _env->GetStringUTFChars(_out,NULL);
    ASSERT(out);
    int mode = (int)_mode;

    jboolean _ret0 = ConvertEol(in,out,mode);

    _env->ReleaseStringUTFChars(_in,in);
    _env->ReleaseStringUTFChars(_out,out);

    return _ret0;
}

static jboolean filemanager_isfilesame_v1(JNIEnv* _env,jobject _this_obj, jobject _src1, jobject _src2)
{
    CFileBase *src1 = get_filebase(_env,_src1);
    ASSERT(src1);
    CFileBase *src2 = get_filebase(_env,_src2);
    ASSERT(src2);

    jboolean _ret0 = IsFileSame(src1,src2);


    return _ret0;
}

static jboolean filemanager_isfilesame_v2(JNIEnv* _env,jobject _this_obj, jstring _fn1, jstring _fn2)
{
    const char *fn1 = _env->GetStringUTFChars(_fn1,NULL);
    ASSERT(fn1);
    const char *fn2 = _env->GetStringUTFChars(_fn2,NULL);
    ASSERT(fn2);

    jboolean _ret0 = IsFileSame(fn1,fn2);

    _env->ReleaseStringUTFChars(_fn1,fn1);
    _env->ReleaseStringUTFChars(_fn2,fn2);

    return _ret0;
}

static jboolean filemanager_deletedir(JNIEnv* _env,jobject _this_obj, jstring _dir)
{
    const char *dir = _env->GetStringUTFChars(_dir,NULL);
    ASSERT(dir);

    CMem mem_dir(dir);
    jboolean _ret0 = CDirMgr::DeleteDir(&mem_dir);

    _env->ReleaseStringUTFChars(_dir,dir);

    return _ret0;
}

static jboolean filemanager_searchdir(JNIEnv* _env,jobject _this_obj, jstring _dir, jboolean _recursive, jobject _onsearchdir_callback_obj)
{
    const char *dir = _env->GetStringUTFChars(_dir,NULL);
    ASSERT(dir);
    if(dir[0] == 0)dir="/";
    bool recursive = (bool)_recursive;
    jobject onsearchdir_callback_obj = _onsearchdir_callback_obj;
    const char *onsearchdir_callback_name = "run";
    ASSERT(onsearchdir_callback_name);


    BEGIN_CLOSURE(on_search_dir)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(char*,file_name,1);
        CLOSURE_PARAM_PTR(char*,full_name,2);
        CLOSURE_PARAM_INT(is_dir,3);
        CLOSURE_PARAM_INT64(size,4);
        CLOSURE_PARAM_INT64(last_write_time,5);
        CLOSURE_PARAM_PTR(JNIEnv*,env,10);
        CLOSURE_PARAM_PTR(jobject,callback_obj,11);

        CJavaCallback cb;
        cb.Init(env,callback_obj);

        cb.Put("event",event);
        cb.Put("file_name",file_name);
        cb.Put("full_name",full_name);
        cb.Put("is_dir",is_dir);
        cb.Put("size",size);
        cb.Put("last_write_time",last_write_time);
        cb.Run();
        cb.Clear();
		return OK;
    }
    END_CLOSURE(on_search_dir);

    on_search_dir.SetParamPointer(10,_env);
    on_search_dir.SetParamPointer(11,_onsearchdir_callback_obj);

    CMem mem_dir(dir);
    jboolean _ret0 = CDirMgr::SearchDir_Interruptable(
		&mem_dir,recursive,&on_search_dir,
		NULL
	);

    _env->ReleaseStringUTFChars(_dir,dir);

    return _ret0;
}



static const JNINativeMethod filemanager_native_methods[]={
    {"DeleteFile","(Ljava/lang/String;)Z",(void*)filemanager_deletefile},
    {"ReadLink","(Ljava/lang/String;)Ljava/lang/String;",(void*)filemanager_readlink},
    {"SaveFile","(Ljava/lang/String;Lcom/jni/common/CFileBase;)Z",(void*)filemanager_savefile},
    {"CreateDir","(Ljava/lang/String;)Z",(void*)filemanager_createdir},
    {"CopySingleFile","(Ljava/lang/String;Ljava/lang/String;JJJI)Z",(void*)filemanager_copysinglefile_v1},
    {"CopySingleFile","(Ljava/lang/String;Ljava/lang/String;JJI)Z",(void*)filemanager_copysinglefile_v2},
    {"CopySingleFile","(Ljava/lang/String;Ljava/lang/String;I)Z",(void*)filemanager_copysinglefile_v3},
    {"IsTextFile","(Lcom/jni/common/CFileBase;)Z",(void*)filemanager_istextfile_v1},
    {"IsTextFile","(Ljava/lang/String;)Z",(void*)filemanager_istextfile_v2},
    {"ConvertEol","(Lcom/jni/common/CFileBase;Lcom/jni/common/CFileBase;I)Z",(void*)filemanager_converteol_v1},
    {"ConvertEol","(Ljava/lang/String;Ljava/lang/String;I)Z",(void*)filemanager_converteol_v2},
    {"IsFileSame","(Lcom/jni/common/CFileBase;Lcom/jni/common/CFileBase;)Z",(void*)filemanager_isfilesame_v1},
    {"IsFileSame","(Ljava/lang/String;Ljava/lang/String;)Z",(void*)filemanager_isfilesame_v2},
    {"DeleteDir","(Ljava/lang/String;)Z",(void*)filemanager_deletedir},
    {"SearchDir","(Ljava/lang/String;ZLjava/lang/Object;)Z",(void*)filemanager_searchdir},
};
status_t register_filemanager_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(filemanager_native_methods)/sizeof(filemanager_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, filemanager_native_methods , size) == JNI_OK);
    return OK;
}
