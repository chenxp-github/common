#include "crt_gba.h"
#include "syslog.h"

void *crt_memset(void *buf, int_ptr_t n, int_ptr_t size)
{
    return memset(buf,n,size);
}

void *crt_memcpy(void *dest, const void *src, int_ptr_t n)
{
    return memcpy(dest, src, n);
}

int crt_memcmp(const void *buf1, const void *buf2, int_ptr_t count)
{
    return memcmp(buf1,buf2,count);
}
int64_t crt_atoi64(const char *str)
{
    ASSERT(0);
    return 0;
}

status_t crt_fsize_to_str(fsize_t size, char *str)
{
    sprintf(str,"%d",size);
    return OK;
}

fsize_t crt_str_to_fsize(const char *str)
{
    ASSERT(0);
    return 0;
}
int crt_is_file_handle(FILE_HANDLE fd)
{
    return fd >= 0;
}
FILE_HANDLE crt_fopen(const char *fn, const char *mode)
{
    ASSERT(0);
    return 0;
}
void crt_fclose(FILE_HANDLE fd)
{
}

fsize_t crt_fseek(FILE_HANDLE fd, fsize_t offset, int where)
{
    ASSERT(0);
    return 0;
}

fsize_t crt_ftell(FILE_HANDLE fd)
{
    ASSERT(0);
    return 0;
}

int_ptr_t crt_fread(FILE_HANDLE fd,void *buffer, int_ptr_t size)
{
    ASSERT(0);
    return 0;
}
int_ptr_t crt_fwrite(FILE_HANDLE fd,const void*buffer,int_ptr_t size)
{
    ASSERT(0);
    return 0;
}

int32_t crt_vsprintf(char *string, const char *format, crt_va_list param)
{
    return vsprintf(string, format, param);
}

int_ptr_t crt_strlen(const char *s)
{
    return strlen(s);
}

char *crt_strcpy(char *dst,const char *src)
{
    return strcpy(dst,src);
}

int crt_strcmp(const char *s1,const char * s2)
{
    return strcmp(s1,s2);
}

int crt_stricmp(const char *s1,const char * s2)
{
    return stricmp(s1,s2);
}

char *crt_strcat(char *dest,const char *src)
{
    return strcat(dest,src);
}

char *crt_strchr(const char *src,int32_t ch)
{
    return strchr(src,ch);
}

void crt_strcpy_w(wchar_t *dst, const wchar_t *src)
{
    while(*src) *dst++ = *src++;
    *dst++ = 0;
}

wchar_t *crt_strcat_w(wchar_t *dest,const wchar_t *src)
{
    wchar_t *cp = dest;
    while (*cp) cp++;
    while (((*cp++) = (*src++)));
    return dest;
}

int_ptr_t crt_strlen_w(const wchar_t *str)
{
    int_ptr_t s = 0;
    while(*str++)s++; 
    return s;
}

wchar_t crt_tolwr_w(wchar_t uc)
{
    if(uc >= L'A' && uc <= L'Z')
    {
        uc += L'a' - L'A';
    }
    return uc;
}

wchar_t crt_toupr_w(wchar_t uc)
{
    if(uc >= L'a' && uc <= L'z')
    {
        uc -= L'a' - L'A';
    }
    return uc;
}

status_t crt_strcmp_w(const wchar_t *s1, const wchar_t *s2)
{
    status_t ret = 0;
    while (!(ret = *s1 - *s2) && *s2) ++s1, ++s2;
    return ret;
}

status_t crt_stricmp_w(const wchar_t *s1, const wchar_t *s2)
{
    wchar_t f, l;
    do 
    {
        f = crt_tolwr_w(*s1);
        l = crt_tolwr_w(*s2);
        s1++;
        s2++;
    } while ((f) && (f == l));  
    return f - l;
}

int32_t crt_tolower(int32_t c)
{
    return tolower(c);
}

int32_t crt_toupper(int32_t c)
{
    return toupper(c);
}

int crt_wvsprintf(wchar_t *buf, const wchar_t *format, crt_va_list param)
{
    ASSERT(0);
    return 0;
}

static uint32_t old_timer = 0;
uint32_t crt_get_sys_timer()
{
    if(old_timer == 0) old_timer = get_sys_timer();
    return get_sys_timer() - old_timer;
}

status_t crt_need_chdir_before_open()
{
    return 0;
}

status_t crt_open_dir(const char *dir, void **p)
{
    ASSERT(0);
    return 0;
}

status_t crt_close_dir(void **p)
{
    ASSERT(0);
    return 0;
}

status_t crt_read_dir(void **p)
{
    ASSERT(0);
    return 0;
}

char crt_get_path_splitor()
{
    return '/';
}

status_t crt_get_cur_dir(char *path, int32_t max)
{
    ASSERT(0);
    return 0;
}

status_t crt_mkdir(const char *dir)
{
    ASSERT(0);
    return 0;
}

status_t crt_rmdir(const char *dir)
{
    ASSERT(0);
    return 0;
}

status_t crt_unlink(const char *filename)
{
    ASSERT(0);
    return 0;
}

status_t crt_chdir(const char *dir)
{
    ASSERT(0);
    return 0;
}

status_t crt_chdir_w(const wchar_t *dir)
{
    ASSERT(0);
    return 0;
}

status_t crt_open_dir_w(const wchar_t *dir, void **p)
{
    ASSERT(0);
    return 0;
}

status_t crt_read_dir_w(void **p)
{
    ASSERT(0);
    return 0;
}

wchar_t crt_get_path_splitor_w()
{
    return L'/';
}

status_t crt_get_cur_dir_w(wchar_t *path, int32_t max)
{
    ASSERT(0);
    return 0;
}

status_t crt_mkdir_w(const wchar_t *dir)
{
    ASSERT(0);
    return 0;
}

status_t crt_rmdir_w(const wchar_t *dir)
{
    ASSERT(0);
    return 0;
}

status_t crt_putchar(int32_t ch)
{
    return putchar(ch);
}

void crt_msleep(int32_t ms)
{
    msleep(ms);
}

wchar_t crt_gbk_char_to_unicode(const char *gbk)
{
    return gb_char_to_unicode(gbk);
}

void crt_unicode_char_to_gbk(wchar_t uc,char *gb)
{
    unicode_to_gb_char(uc,gb);
}

