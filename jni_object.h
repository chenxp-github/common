#ifndef __JNI_OBJECT_H
#define __JNI_OBJECT_H

#include <jni.h>
#include "cruntime.h"
#include "mem.h"
#include "weak_ref.h"
#include "raw_weak_pointer.h"

class CJniObject{
public:
	bool m_is_weak;
	CRawWeakPointer m_native_ptr;
public:

	CJniObject();
	virtual ~CJniObject();
	status_t Init();
	status_t Destroy();
	status_t InitBasic();

	void *GetNativePointer();
	status_t SetIsWeak(bool isWeak);
	bool IsWeak();
	status_t SetNativePointer(const void *ptr,WeakRefContext_t *context);

	static status_t ClearJniObject(JNIEnv *env, jobject obj);
	static CJniObject *GetJniObject(JNIEnv* env,jobject obj);
	static void *GetNativePointer(JNIEnv* env,jobject obj);
};

#endif
