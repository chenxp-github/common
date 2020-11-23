#ifndef __JNI_OBJECT_H
#define __JNI_OBJECT_H

#include <jni.h>
#include "cruntime.h"
#include "mem.h"
#include "weak_ref.h"
#include "raw_weak_pointer.h"
#include "callback_context.h"

class CJniObject{
public:
	bool m_is_weak;
	CRawWeakPointer m_native_ptr;

    CCallbackContext *m_callback_contexts;
    int m_callback_contexts_size;
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

	CCallbackContext* GetCallbackContexts();
	int GetCallbackContextsSize();
	CCallbackContext* GetCallbackContextsElem(int _index);
	status_t AllocCallbackContexts(int _len);
	status_t SetCallbackContexts(CCallbackContext _callback_contexts[], int _len);
	status_t SetCallbackContexts(CCallbackContext *_callback_contexts[], int _len);
	status_t SetCallbackContextsElem(int _index,CCallbackContext *_callback_contexts);

	static status_t ClearJniObject(JNIEnv *env, jobject obj);
	static CJniObject *GetJniObject(JNIEnv* env,jobject obj);
	static void *GetNativePointer(JNIEnv* env,jobject obj);

	status_t SetCallback(int index, JNIEnv* env, jobject cb_obj, const char *cb_name);
	CCallbackContext* GetCallback(int index);
};

#endif
