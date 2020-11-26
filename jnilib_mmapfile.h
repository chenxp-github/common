#ifndef __JNILIB_MMAPFILE_H
#define __JNILIB_MMAPFILE_H

#include "jni_helper.h"
#include "mmapfile.h"

#define JAVA_CLASS_PATH_MMAPFILE "com/jni/common/CMMapFile"

CMMapFile* get_mmapfile(JNIEnv* env,jobject obj);
jobject create_java_mmapfile(JNIEnv* env,CMMapFile *mmapfile, bool is_weak);
status_t register_mmapfile_native_methods(JNIEnv* env);

#endif
