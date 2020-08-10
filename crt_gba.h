#ifndef __CRT_GBA_H
#define __CRT_GBA_H

#include "config.h"
#include "types.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "ctype.h"
#include "types.h"
#include "syscall.h"

#define TRUE        1
#define FALSE       0
#define OK          1
#define ERROR       0

#define SEEK_END    0
#define SEEK_SET    1

#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif

#define LOCAL_ENCODING ENCODING_GBK

#define LBUF_SIZE           2048
#define FILEBASE_LBUF_SIZE  (32*1024)

#define MAX_FSIZE           (0x7fffffff)
typedef int32_t         fsize_t;
typedef uint32_t            ufsize_t;
typedef int             FILE_HANDLE;


#define crt_va_list     va_list
#define crt_va_end      va_end
#define crt_va_list     va_list
#define crt_va_start    va_start

#ifdef __cplusplus
extern "C" {
#endif

void *crt_memset(void *buf, int32_t n, int32_t size);
void *crt_memcpy(void *dest, const void *src, int32_t n);
int32_t crt_memcmp(const void *buf1, const void *buf2, int32_t count);
int64_t crt_atoi64(const char *str);
fsize_t crt_str_to_fsize(const char *str);
status_t crt_fsize_to_str(fsize_t size, char *str);
int crt_is_file_handle(FILE_HANDLE fd);
FILE_HANDLE crt_fopen(const char *fn, const char *mode);
void crt_fclose(FILE_HANDLE fd);
fsize_t crt_fseek(FILE_HANDLE fd, fsize_t offset, int where);
fsize_t crt_ftell(FILE_HANDLE fd);
int_ptr_t crt_fread(FILE_HANDLE fd,void *buffer, int_ptr_t size);
int_ptr_t crt_fwrite(FILE_HANDLE fd,const void*buffer,int_ptr_t size);
int32_t crt_vsprintf(char *string, const char *format, crt_va_list param);
int32_t crt_strlen(const char *s);
char *crt_strcpy(char *dst,const char *src);
int32_t crt_strcmp(const char *s1,const char * s2);
int32_t crt_stricmp(const char *s1,const char * s2);
char *crt_strcat(char *dest,const char *src);
char *crt_strchr(const char *src,int32_t ch);
void crt_strcpy_w(wchar_t *dst, const wchar_t *src);
wchar_t *crt_strcat_w(wchar_t *dest,const wchar_t *src);
int32_t crt_strlen_w(const wchar_t *str);
wchar_t crt_tolwr_w(wchar_t uc);
wchar_t crt_toupr_w(wchar_t uc);
status_t crt_strcmp_w(const wchar_t *s1, const wchar_t *s2);
status_t crt_stricmp_w(const wchar_t *s1, const wchar_t *s2);
int32_t crt_tolower(int32_t c);
int32_t crt_toupper(int32_t c);
int32_t crt_wvsprintf(wchar_t *buf, const wchar_t *format, crt_va_list param);
uint32_t crt_get_sys_timer();
status_t crt_need_chdir_before_open();
status_t crt_open_dir(const char *dir, void **p);
status_t crt_close_dir(void **p);
status_t crt_read_dir(void **p);
char crt_get_path_splitor();
int32_t crt_get_cur_dir(char *path, int32_t max);
status_t crt_mkdir(const char *dir);
status_t crt_rmdir(const char *dir);
status_t crt_unlink(const char *filename);
status_t crt_chdir(const char *dir);
status_t crt_chdir_w(const wchar_t *dir);
status_t crt_open_dir_w(const wchar_t *dir, void **p);
status_t crt_read_dir_w(void **p);
wchar_t crt_get_path_splitor_w();
status_t crt_get_cur_dir_w(wchar_t *path, int32_t max);
status_t crt_mkdir_w(const wchar_t *dir);
status_t crt_rmdir_w(const wchar_t *dir);
status_t crt_putchar(int32_t ch);
void crt_msleep(int32_t ms);
wchar_t crt_gbk_char_to_unicode(const char *gbk);
void crt_unicode_char_to_gbk(wchar_t uc,char *gb);
int32_t crt_get_unique_id();

#ifdef __cplusplus
} //extern "C"
#endif

#endif

