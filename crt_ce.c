#include "crt_ce.h"

#pragma comment(lib, "mmtimer.lib")
#pragma comment(lib, "Ceshell.lib")
#pragma comment(lib, "Commdlg.lib")


void *crt_memset(void *buf, int32_t n, int32_t size)
{
    return memset(buf,n,size);
}

void *crt_memcpy(void *dest, const void *src, int32_t n)
{
    return memcpy(dest, src, n);
}

int32_t crt_memcmp(const void *buf1, const void *buf2, int32_t count)
{
    return memcmp(buf1,buf2,count);
}

FILE *crt_fopen(const char *fn, const char *mode)
{
    return fopen(fn,mode);
}

void crt_fclose(FILE *fp)
{
    fclose(fp);
}

int32_t crt_fseek(FILE *fp, int32_t offset, int32_t where)
{
    return fseek(fp,offset,where);
}

int32_t crt_ftell(FILE *fp)
{
    return ftell(fp);
}

int32_t crt_fread( void *buffer, int32_t size, int32_t count, FILE *stream )
{
    return fread(buffer, size, count, stream);
}

size_t crt_fwrite(const void*buffer,int32_t size,int32_t count,FILE*stream)
{
    return fwrite(buffer, size,count, stream);
}

int32_t crt_vsprintf(char *string, const char *format, crt_va_list param)
{
    return vsprintf(string, format, param);
}

int32_t crt_strlen(const char *s)
{
    return strlen(s);
}

char *crt_strcpy(char *dst,const char *src)
{
    return strcpy(dst,src);
}

int32_t crt_strcmp(const char *s1,const char * s2)
{
    return strcmp(s1,s2);
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

int32_t crt_strlen_w(const wchar_t *str)
{
    int32_t s = 0;
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

int32_t crt_wvsprintf(wchar_t *buf, const wchar_t *format, crt_va_list param)
{
    return wvsprintfW(buf,format, param);
}

FILE *crt_fopen_w(const wchar_t *fn, const wchar_t *mode)
{
    return _wfopen(fn,mode);
}

static uint32_t old_timer = 0;
uint32_t crt_get_sys_timer()
{
    if(old_timer == 0) old_timer = timeGetTime();
    return timeGetTime() - old_timer;
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
    return '\\';
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
    WIN32_FIND_DATAW fd;
    HANDLE handle;
    wchar_t buf[MAX_PATH];

    crt_strcpy_w(buf,dir);
    crt_strcat_w(buf,L"\\*");

    handle = FindFirstFileW(buf,&fd);
    if(handle == INVALID_HANDLE_VALUE)
        return ERROR;
    
    p[0] = (void*)handle;
    crt_strcpy_w((wchar_t*)p[1],fd.cFileName);
    p[2] = (void*)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    p[3] = (void*)(fd.nFileSizeLow);
    p[10] = (void*)1;
    return OK;
}

status_t crt_read_dir_w(void **p)
{
    WIN32_FIND_DATAW fd;
    HANDLE handle;

    if(p[10] == (void*)1)
    {
        p[10] = 0;
        return OK;
    }
    
    handle = (HANDLE)p[0];
    if(FindNextFileW(handle,&fd) == 0)
        return ERROR;

    crt_strcpy_w((wchar_t*)p[1],fd.cFileName);
    p[2] = (void*)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    p[3] = (void*)(fd.nFileSizeLow);
    p[10] = (void*)0;

    return OK;
}

wchar_t crt_get_path_splitor_w()
{
    return L'\\';
}

status_t crt_get_cur_dir_w(wchar_t *path, int32_t max)
{
    ASSERT(0);
    return 0;
}

status_t crt_mkdir_w(const wchar_t *dir)
{
    return CreateDirectoryW(dir,NULL);
}

status_t crt_rmdir_w(const wchar_t *dir)
{
    return RemoveDirectoryW(dir);
}

status_t crt_putchar(int32_t ch)
{
    return putchar(ch);
}

void crt_msleep(int32_t ms)
{
    Sleep(ms);
}

wchar_t crt_gbk_char_to_unicode(const char *gbk)
{
    wchar_t unicode[16];
    UINT nCodePage = CP_ACP;
    MultiByteToWideChar(nCodePage,0, gbk, -1, (wchar_t*)unicode, 1);
    return unicode[0];
}

void crt_unicode_char_to_gbk(wchar_t uc,char *gb)
{
    wchar_t ustr[2];
    UINT nCodePage;

    ustr[0] = uc;ustr[1] = 0;
    gb[0] = 0;gb[1] = 0;gb[2] = 0;gb[3] = 0;
    nCodePage = CP_ACP;
    WideCharToMultiByte(nCodePage,0,(wchar_t*)ustr,-1,gb,4,NULL,NULL);       
}

///////////////////////////////////////////////////////////////////
#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////
#pragma comment(lib, "ws2_32.lib")

int32_t crt_socket( int32_t af, int32_t type, int32_t protocol )
{
    return socket(af,type,protocol);
}

int32_t crt_closesocket(int32_t s) 
{
    return closesocket(s);
}

int32_t crt_send(int32_t s, const char *buf, int32_t len, int32_t flag)
{
    return send(s,buf,len,flag);
}

int32_t crt_recv(int32_t s, char *buf, int32_t len, int32_t flag)
{
    return recv(s,buf,len,flag);
}

void crt_socket_start()
{
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
}

void crt_socket_end()
{
    WSACleanup();
}

int32_t crt_set_blocking_mode(int32_t s,status_t enable)
{
    u_long block = !enable;
    return ioctlsocket(s,FIONBIO,&block) == 0;
}

int32_t crt_select(int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    return select(nfds,readfds,writefds,exceptfds,timeout);
}

int32_t crt_gethostname(char *name, int32_t namelen) 
{
    return gethostname(name,namelen); 
}

struct hostent *crt_gethostbyname(const char * name)
{
    return gethostbyname(name);
}

char *crt_inet_ntoa (struct in_addr in)
{
    return inet_ntoa(in); 
}

uint16_t crt_htons( uint16_t hostshort)
{
    return htons(hostshort);
}

int32_t crt_bind ( int32_t s,const struct sockaddr * name, int32_t namelen)
{
    return bind(s,name,namelen);
}   

int32_t crt_listen(int32_t s, int32_t backlog)
{
    return listen(s,backlog);
}

int32_t crt_accept( int32_t s, struct sockaddr *addr,int32_t *addrlen)
{
    return accept(s,addr,addrlen);
}

int32_t crt_connect( int32_t s, const struct sockaddr* name,int32_t namelen)
{
    return connect(s,name,namelen);
}

status_t crt_is_connect_complete(int32_t s)
{
    struct timeval to;
    
    fd_set fs;  
    to.tv_sec = 0;  
    to.tv_usec = 10;
    
    FD_ZERO(&fs);   
    FD_SET(s,&fs);

    return crt_select((int32_t)(s+1), 0, &fs, 0, &to) > 0;
}
status_t crt_get_all_ip(char *out)
{
    char szHostName[128];   
    const char *pszAddr;   
    struct hostent *pHost;   
    int32_t i;     

    out[0] = 0;
    if(crt_gethostname(szHostName,128) == 0)   
    {   
        pHost = crt_gethostbyname(szHostName);     
        for(i = 0; pHost != NULL&&pHost->h_addr_list[i]!=NULL; i++)     
        {
            pszAddr=crt_inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i]);   
            crt_strcat(out,pszAddr);
            crt_strcat(out,"\n");
        }   
    }   
    return OK;
}

///////////////////////////////////////////////////////////////////
#endif //#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////

