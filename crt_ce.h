#ifndef __CRT_CE_H
#define __CRT_CE_H

#define USE_SOCKET_MODULE   0

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>
#include <commctrl.h>
#include <mmsystem.h>

#define TRUE        1
#define FALSE       0
#define OK          1
#define ERROR       0

#define WM_NCCREATE         0x0081
#define Shell_NotifyIconW   Shell_NotifyIcon

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;

#if __GNUC__
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;
#else
typedef __int64             int64_t;
typedef unsigned __int64    uint64_t;
#endif

typedef int32_t             status_t;
typedef int32_t             int_ptr_t;

#define LBUF_SIZE           2048
#define FILEBASE_LBUF_SIZE  (32*1024)

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
FILE *crt_fopen(const char *fn, const char *mode);
void crt_fclose(FILE *fp);
int32_t crt_fseek(FILE *fp, int32_t offset, int32_t where);
int32_t crt_ftell(FILE *fp);
int32_t crt_fread( void *buffer, int32_t size, int32_t count, FILE *stream );
size_t crt_fwrite(const void*buffer,int32_t size,int32_t count,FILE*stream);
int32_t crt_vsprintf(char *string, const char *format, crt_va_list param);
int32_t crt_strlen(const char *s);
char *crt_strcpy(char *dst,const char *src);
int32_t crt_strcmp(const char *s1,const char * s2);
char *crt_strcat(char *dest,const char *src);
void crt_strcpy_w(wchar_t *dst, const wchar_t *src);
wchar_t *crt_strcat_w(wchar_t *dest,const wchar_t *src);
int32_t crt_strlen_w(const wchar_t *str);
int32_t crt_tolower(int32_t c);
int32_t crt_toupper(int32_t c);
int32_t crt_wvsprintf(wchar_t *buf, const wchar_t *format, crt_va_list param);
FILE *crt_fopen_w(const wchar_t *fn, const wchar_t *mode);
uint32_t crt_get_sys_timer();
status_t crt_open_dir(const char *dir, void **p);
status_t crt_close_dir(void **p);
status_t crt_read_dir(void **p);
char crt_get_path_splitor();
status_t crt_get_cur_dir(char *path, int32_t max);
status_t crt_mkdir(const char *dir);
status_t crt_rmdir(const char *dir);
status_t crt_unlink(const char *filename);
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
char *crt_strchr(const char *src,int32_t ch);
status_t crt_chdir(const char *dir);
status_t crt_chdir_w(const wchar_t *dir);
status_t crt_need_chdir_before_open();
status_t crt_strcmp_w(const wchar_t *s1, const wchar_t *s2);
wchar_t crt_tolwr_w(wchar_t uc);
wchar_t crt_toupr_w(wchar_t uc);
status_t crt_strcmp_w(const wchar_t *s1, const wchar_t *s2);
status_t crt_stricmp_w(const wchar_t *s1, const wchar_t *s2);
int32_t crt_strlen_w(const wchar_t *str);
///////////////////////////////////////////////////////////////////
#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////
#include <winsock.h>

int32_t crt_socket( int32_t af, int32_t type, int32_t protocol );
int32_t crt_closesocket(int32_t s);
int32_t crt_send(int32_t s, const char *buf, int32_t len, int32_t flag);
int32_t crt_recv(int32_t s, char *buf, int32_t len, int32_t flag);
void crt_socket_start();
void crt_socket_end();
int32_t crt_select(int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
status_t crt_set_blocking_mode(int32_t s,status_t enable);
int32_t crt_gethostname(char *name, int32_t namelen);
struct hostent *crt_gethostbyname(const char * name);
char *crt_inet_ntoa (struct in_addr in);
uint16_t crt_htons( uint16_t hostshort);
int32_t crt_bind ( int32_t s,const struct sockaddr * name, int32_t namelen);
int32_t crt_listen(int32_t s, int32_t backlog);
int32_t crt_accept( int32_t s, struct sockaddr *addr,int32_t *addrlen);
int32_t crt_connect( int32_t s, const struct sockaddr* name,int32_t namelen);
status_t crt_is_connect_complete(int32_t s);
status_t crt_get_all_ip(char *out);
///////////////////////////////////////////////////////////////////
#endif //#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////

#ifdef __cplusplus
} //extern "C"
#endif

#endif

