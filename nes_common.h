#ifndef __NES_COMMON_H
#define __NES_COMMON_H

#include "common.h"

#define NES_ZERO_ARRAY(name,size) \
do{\
int _i;\
for(_i=0;_i<size;_i++)\
name[_i]=0;\
}while(0)\

#ifdef _WIN32_
typedef signed __int64      SQWORD;
typedef unsigned __int64    QWORD;
#endif

#endif
