#ifndef __JNI_HELPER_H
#define __JNI_HELPER_H

#include <jni.h>
#include "jni_object.h"
#include "memstk.h"

#define JNI_GET_OBJ_FUNC(_class,_name)\
_class *get_##_name(JNIEnv* env,jobject obj )\
{\
	return (_class*)CJniObject::GetNativePointer(env,obj);\
}\

#define JNI_GC_FUNC(_class,_name)\
static jint _name##__gc(JNIEnv* env,jobject obj)\
{\
	CJniObject *jni_obj = CJniObject::GetJniObject(env,obj);\
	ASSERT(jni_obj);\
	if(!jni_obj->IsWeak())\
	{\
		_class *obj = (_class*)jni_obj->GetNativePointer();\
		DEL(obj);\
	}\
	DEL(jni_obj);\
	CJniObject::ClearJniObject(env,obj);\
	return OK;\
}\

#define JNI_CREATE_JAVA_OBJ_FUNC(_class,_name)\
jobject create_java_##_name(JNIEnv* env,_class *ptr, bool is_weak)\
{\
	ASSERT(ptr);\
	jclass class_##_name = env->FindClass(THIS_JAVA_CLASS_PATH);\
	ASSERT(class_##_name);\
	jmethodID m_id = env->GetMethodID(class_##_name, "__dummy", "()V");\
	ASSERT(m_id);\
	jobject obj = env->NewObject(class_##_name,m_id);\
	ASSERT(obj);\
	jfieldID id_obj=env->GetFieldID(class_##_name,"__obj","J");\
	CJniObject *jni_obj;\
	NEW(jni_obj,CJniObject);\
	jni_obj->Init();\
	WEAK_REF_CONTEXT_CREATE(ptr->__weak_ref_context);\
	jni_obj->SetNativePointer(ptr,ptr->__weak_ref_context);\
	jni_obj->SetIsWeak(is_weak);\
	env->SetLongField(obj,id_obj,(jlong)jni_obj);\
	env->DeleteLocalRef(class_##_name);\
	env->DeleteLocalRef(obj);\
	return obj;\
}\

#define JNI_WRAP_NATIVE_OBJECT(env,java_obj, ptr, is_weak)\
do{\
	jclass class_name = env->FindClass(THIS_JAVA_CLASS_PATH);\
	ASSERT(class_name);\
	CJniObject *jni_obj;\
	NEW(jni_obj,CJniObject);\
	jni_obj->Init();\
	WEAK_REF_CONTEXT_CREATE(ptr->__weak_ref_context);\
	jni_obj->SetNativePointer(ptr,ptr->__weak_ref_context);\
	jni_obj->SetIsWeak(is_weak);\
	jfieldID id_obj=(env)->GetFieldID(class_name,"__obj","J");\
	env->SetLongField(java_obj,id_obj,(jlong)jni_obj);\
	env->DeleteLocalRef(class_name);\
}while(0)\

#define BuildJavaObjectArray(env,arr,arr_len,cls_path,create_func,is_weak,ret) do\
{\
	ASSERT(env && (arr) && cls_path);\
	jclass objClass = env->FindClass(cls_path);\
	ASSERT(objClass);\
	if(arr_len > 0)\
	{\
		jobjectArray java_arr = env->NewObjectArray(arr_len,objClass,0);\
		for(int i = 0; i < arr_len; i++)\
		{\
			jobject jobj = create_func(env, &(arr)[i], is_weak);\
			env->SetObjectArrayElement(java_arr,i,jobj);\
		}\
		ret = java_arr;\
		env->DeleteLocalRef(java_arr);\
	}\
	env->DeleteLocalRef(objClass);\
}while(0)\

#define GetNativeObjectArray(env, jarrObj,ctype, arr, arr_len,cls_path,get_func) do\
{\
	ASSERT(env && jarrObj);\
	jclass objClass = env->FindClass(cls_path);\
	ASSERT(objClass);\
	arr_len = env->GetArrayLength(jarrObj);\
	MALLOC(arr,ctype*,arr_len);\
	for(int i = 0; i < arr_len; i++)\
	{\
		jobject jobj = env->GetObjectArrayElement(jarrObj,i);\
		ctype *ptr = get_func(env,jobj);\
		arr[i] = ptr;\
		env->DeleteLocalRef(jobj);\
	}\
	env->DeleteLocalRef(objClass);\
}while(0)\

#define ReleaseNativeObjectArray(arr) do{\
	FREE(arr);\
}while(0)\

status_t GetStringArrayElements(JNIEnv *env, jobjectArray str_arr, CMemStk *out);
jobjectArray BuildJavaStringArray(JNIEnv *env, CMemStk *arr);
jobjectArray BuildJavaStringArray(JNIEnv *env, CMem **arr, int arr_len);
jobjectArray BuildJavaStringArray(JNIEnv *env, CMem *arr, int arr_len);

class CMainLoopContext;
CMainLoopContext *GetMainLoopContext(JNIEnv* env);

#endif

