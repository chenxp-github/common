#ifndef __JNILIB_FILEMANAGER_H
#define __JNILIB_FILEMANAGER_H

#include "jni_helper.h"

#define JAVA_CLASS_PATH_FILEMANAGER "com/jni/common/CFileManager"

status_t register_filemanager_native_methods(JNIEnv* env);

#endif
