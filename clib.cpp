#include "stdafx.h"
#include "clib.h"

void *my_memcpy(void * dst, void * src, int len)
{
    int * plDst = (int *) dst;
    int * plSrc = (int *) src; 

    if (!((int)src & 0xFFFFFFFC) && !((int)dst & 0xFFFFFFFC))
    {
        while (len >= 4)
        {
            *plDst++ = *plSrc++;
            len -= 4;
        }
    } 

    char * pcDst = (char *) plDst;
    char * pcSrc = (char *) plSrc;

    while (len--)
    {
        *pcDst++ = *pcSrc++;
    } 

    return (dst);
}

time_t my_time( time_t *inTT ) 
{
    SYSTEMTIME sysTimeStruct;
    FILETIME fTime;
    ULARGE_INTEGER int64time;
    time_t locTT = 0;
    
    if ( inTT == NULL ) 
    {
        inTT = &locTT;
    }
    
    GetSystemTime( &sysTimeStruct );

    if ( SystemTimeToFileTime( &sysTimeStruct, &fTime ) ) 
    {
        memcpy( &int64time, &fTime, sizeof( FILETIME ) );

        /* Subtract the value for 1970-01-01 00:00 (UTC) */
        int64time.QuadPart -= 0x19db1ded53e8000;

        /* Convert to seconds. */
        int64time.QuadPart /= 10000000;
        
        *inTT = (long)int64time.QuadPart;
    }
    
    return *inTT;
}
