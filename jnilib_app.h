#ifndef __JNILIB_APP_H
#define __JNILIB_APP_H

#include "jni_helper.h"

#define JAVA_CLASS_PATH_APP "com/jni/common/CApp"

status_t register_app_native_methods(JNIEnv* env);

#endif
