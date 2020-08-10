#ifndef __CLIB_H
#define __CLIB_H

#include <stdio.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

void *my_memcpy(void * dst, const void * src, int len);
time_t my_time( time_t *inTT );

#ifdef __cplusplus
} //extern "C"
#endif

#endif
