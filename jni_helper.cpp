#include "jni_helper.h"
#include "syslog.h"

status_t GetStringArrayElements(JNIEnv *env, jobjectArray str_arr, CMemStk *out)
{
	ASSERT(env && str_arr && out);
	out->Clear();
    int arr_len = env->GetArrayLength(str_arr);
    if(arr_len <= 0) return ERROR;
    for(int i = 0; i < arr_len; i++)
    {
    	jstring str = (jstring)env->GetObjectArrayElement(str_arr,i);
    	const char* c_str = env->GetStringUTFChars(str,NULL);
    	out->Push(c_str);
    	env->ReleaseStringUTFChars(str,c_str);
    }
	return OK;
}

jobjectArray BuildJavaStringArray(JNIEnv *env, CMemStk *arr)
{
	ASSERT(env && arr);

	if(arr->GetLen() <= 0) return NULL;

	jclass objClass = env->FindClass("java/lang/String");
	ASSERT(objClass);

	jobjectArray java_arr = env->NewObjectArray(arr->GetLen(),objClass,0);
	ASSERT(java_arr);

	for(int i = 0; i < arr->GetLen(); i++)
	{
		CMem *p = arr->GetElem(i);
		jstring jstr = env->NewStringUTF(p->CStr());
		env->SetObjectArrayElement(java_arr, i,jstr);
		env->DeleteLocalRef(jstr);
	}
	env->DeleteLocalRef(java_arr);
	env->DeleteLocalRef(objClass);
	return java_arr;
}

jobjectArray BuildJavaStringArray(JNIEnv *env, CMem **arr, int arr_len)
{
	ASSERT(env && arr);
	if(arr_len <= 0) return NULL;

	jclass objClass = env->FindClass("java/lang/String");
	ASSERT(objClass);

	jobjectArray java_arr = env->NewObjectArray(arr_len,objClass,0);

	for(int i = 0; i < arr_len; i++)
	{
		CMem *p = arr[i];
		ASSERT(p);
		jstring jstr = env->NewStringUTF(p->CStr());
		env->SetObjectArrayElement(java_arr, i,jstr);
		env->DeleteLocalRef(jstr);
	}
	env->DeleteLocalRef(java_arr);
	env->DeleteLocalRef(objClass);
	return java_arr;
}

jobjectArray BuildJavaStringArray(JNIEnv *env, CMem *arr, int arr_len)
{
	ASSERT(env && arr);
	if(arr_len <= 0) return NULL;

	jclass objClass = env->FindClass("java/lang/String");
	ASSERT(objClass);
	jobjectArray java_arr = env->NewObjectArray(arr_len,objClass,0);

	for(int i = 0; i < arr_len; i++)
	{
		CMem *p = &arr[i];
		jstring jstr = env->NewStringUTF(p->CStr());
		env->SetObjectArrayElement(java_arr, i,jstr);
		env->DeleteLocalRef(jstr);
	}
	env->DeleteLocalRef(java_arr);
    env->DeleteLocalRef(objClass);
	return java_arr;
}


