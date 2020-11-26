#include "jnilib_filebase.h"
#include "mem_tool.h"
#include "syslog.h"

#define THIS_JAVA_CLASS_PATH JAVA_CLASS_PATH_FILEBASE

JNI_GET_OBJ_FUNC(CFileBase,filebase)
JNI_GC_FUNC(CFileBase,filebase)
JNI_CREATE_JAVA_OBJ_FUNC(CFileBase,filebase)

static jboolean filebase_isend(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->IsEnd();


    return _ret0;
}

static jbyte filebase_charat(JNIEnv* _env,jobject _this_obj, jlong _index)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t index = (fsize_t)_index;

    jbyte _ret0 = _this->CharAt(index);


    return _ret0;
}

static jbyte filebase_getlastchar(JNIEnv* _env,jobject _this_obj, jboolean _empty_included)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    bool empty_included = (bool)_empty_included;

    jbyte _ret0 = _this->GetLastChar(empty_included);


    return _ret0;
}

static jbyte filebase_getc(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jbyte _ret0 = _this->Getc();


    return _ret0;
}

static jbyte filebase_ungetc(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jbyte _ret0 = _this->UnGetc();


    return _ret0;
}

static jlong filebase_getmaxsize(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetMaxSize();


    return _ret0;
}

static jlong filebase_getoffset(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetOffset();


    return _ret0;
}

static jlong filebase_getsize(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->GetSize();


    return _ret0;
}

static jlong filebase_putc(JNIEnv* _env,jobject _this_obj, jbyte _ch)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    char ch = (char)_ch;

    jlong _ret0 = _this->Putc(ch);


    return _ret0;
}

static jlong filebase_puts_v1(JNIEnv* _env,jobject _this_obj, jobject _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *str = get_filebase(_env,_str);
    ASSERT(str);

    jlong _ret0 = _this->Puts(str);


    return _ret0;
}

static jlong filebase_puts_v2(JNIEnv* _env,jobject _this_obj, jstring _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    jlong _ret0 = _this->Puts(str);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jboolean filebase_readleftstr(JNIEnv* _env,jobject _this_obj, jobject _file, jboolean _skip_empty)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);
    bool skip_empty = (bool)_skip_empty;

    jboolean _ret0 = _this->ReadLeftStr(file,skip_empty);


    return _ret0;
}

static jboolean filebase_readword(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadWord(file);


    return _ret0;
}

static jboolean filebase_readwordwithemptychar(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadWordWithEmptyChar(file);


    return _ret0;
}

static jboolean filebase_readwordwithemptychar_reverse(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadWordWithEmptyChar_Reverse(file);


    return _ret0;
}

static jboolean filebase_readword_reverse(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadWord_Reverse(file);


    return _ret0;
}

static jlong filebase_searchbin(JNIEnv* _env,jobject _this_obj, jobject _file_bin)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file_bin = get_filebase(_env,_file_bin);
    ASSERT(file_bin);

    jlong _ret0 = _this->SearchBin(file_bin);


    return _ret0;
}

static jlong filebase_searchstr(JNIEnv* _env,jobject _this_obj, jstring _str, jboolean _case_sensive, jboolean _word_only)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);
    bool case_sensive = (bool)_case_sensive;
    bool word_only = (bool)_word_only;

    jlong _ret0 = _this->SearchStr(str,case_sensive,word_only);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jlong filebase_seek(JNIEnv* _env,jobject _this_obj, jlong _off)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t off = (fsize_t)_off;

    jlong _ret0 = _this->Seek(off);


    return _ret0;
}

static jlong filebase_seekback(JNIEnv* _env,jobject _this_obj, jlong _back_bytes)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t back_bytes = (fsize_t)_back_bytes;

    jlong _ret0 = _this->SeekBack(back_bytes);


    return _ret0;
}

static jlong filebase_seekend(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->SeekEnd();


    return _ret0;
}

static jlong filebase_split(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jlong _ret0 = _this->Split(file);


    return _ret0;
}

static jlong filebase_writefile_v1(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jlong _ret0 = _this->WriteFile(file);


    return _ret0;
}

static jlong filebase_writefile_v2(JNIEnv* _env,jobject _this_obj, jobject _file, jlong _start, jlong _ws)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);
    fsize_t start = (fsize_t)_start;
    fsize_t ws = (fsize_t)_ws;

    jlong _ret0 = _this->WriteFile(file,start,ws);


    return _ret0;
}

static jlong filebase_writetofile_v1(JNIEnv* _env,jobject _this_obj, jstring _fn)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *fn = _env->GetStringUTFChars(_fn,NULL);
    ASSERT(fn);

    jlong _ret0 = _this->WriteToFile(fn);

    _env->ReleaseStringUTFChars(_fn,fn);

    return _ret0;
}

static jlong filebase_writetofile_v2(JNIEnv* _env,jobject _this_obj, jstring _fn, jlong _start, jlong _wsize)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *fn = _env->GetStringUTFChars(_fn,NULL);
    ASSERT(fn);
    fsize_t start = (fsize_t)_start;
    fsize_t wsize = (fsize_t)_wsize;

    jlong _ret0 = _this->WriteToFile(fn,start,wsize);

    _env->ReleaseStringUTFChars(_fn,fn);

    return _ret0;
}

static jlong filebase_writetofile_v3(JNIEnv* _env,jobject _this_obj, jobject _file, jlong _start, jlong _wsize)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);
    fsize_t start = (fsize_t)_start;
    fsize_t wsize = (fsize_t)_wsize;

    jlong _ret0 = _this->WriteToFile(file,start,wsize);


    return _ret0;
}

static jint filebase_strcmp_v1(JNIEnv* _env,jobject _this_obj, jobject _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *str = get_filebase(_env,_str);
    ASSERT(str);

    jint _ret0 = _this->StrCmp(str);


    return _ret0;
}

static jint filebase_strcmp_v2(JNIEnv* _env,jobject _this_obj, jstring _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    jint _ret0 = _this->StrCmp(str);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jint filebase_stricmp_v1(JNIEnv* _env,jobject _this_obj, jobject _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *str = get_filebase(_env,_str);
    ASSERT(str);

    jint _ret0 = _this->StrICmp(str);


    return _ret0;
}

static jint filebase_stricmp_v2(JNIEnv* _env,jobject _this_obj, jstring _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    jint _ret0 = _this->StrICmp(str);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jlong filebase_read(JNIEnv* _env,jobject _this_obj, jobject _buf, jlong _n)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *buf = get_filebase(_env,_buf);
    ASSERT(buf);
    int_ptr_t n = (int_ptr_t)_n;

    jlong _ret0 = _this->WriteToFile(buf,_this->GetOffset(),n);


    return _ret0;
}

static jlong filebase_readzeroendstring(JNIEnv* _env,jobject _this_obj, jobject _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *str = get_filebase(_env,_str);
    ASSERT(str);

    jlong _ret0 = _this->ReadZeroEndString(str);


    return _ret0;
}

static jlong filebase_strlen(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jlong _ret0 = _this->StrLen();


    return _ret0;
}

static jlong filebase_write(JNIEnv* _env,jobject _this_obj, jobject _buf, jlong _n)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *buf = get_filebase(_env,_buf);
    ASSERT(buf);
    int_ptr_t n = (int_ptr_t)_n;

    jlong _ret0 = _this->Write(buf,n);


    return _ret0;
}

static jboolean filebase_isemptychar(JNIEnv* _env, jbyte _ch)
{
    char ch = (char)_ch;

    jboolean _ret0 = CFileBase::IsEmptyChar(ch);


    return _ret0;
}

static jboolean filebase_delete(JNIEnv* _env,jobject _this_obj, jlong _del_size)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t del_size = (fsize_t)_del_size;

    jboolean _ret0 = _this->Delete(del_size);


    return _ret0;
}

static jboolean filebase_deletelast(JNIEnv* _env,jobject _this_obj, jlong _bytes)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t bytes = (fsize_t)_bytes;

    jboolean _ret0 = _this->DeleteLast(bytes);


    return _ret0;
}

static jboolean filebase_dump(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Dump();


    return _ret0;
}

static jboolean filebase_endwith(JNIEnv* _env,jobject _this_obj, jstring _str, jboolean _case_sensive, jboolean _skip_empty)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);
    bool case_sensive = (bool)_case_sensive;
    bool skip_empty = (bool)_skip_empty;

    jboolean _ret0 = _this->EndWith(str,case_sensive,skip_empty);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jboolean filebase_fillblock(JNIEnv* _env,jobject _this_obj, jlong _fill_size, jbyte _fill_ch)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t fill_size = (fsize_t)_fill_size;
    char fill_ch = (char)_fill_ch;

    jboolean _ret0 = _this->FillBlock(fill_size,fill_ch);


    return _ret0;
}

static jboolean filebase_insert_v1(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->Insert(file);


    return _ret0;
}

static jboolean filebase_insert_v2(JNIEnv* _env,jobject _this_obj, jobject _file, jlong _start, jlong _size)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);
    fsize_t start = (fsize_t)_start;
    fsize_t size = (fsize_t)_size;

    jboolean _ret0 = _this->Insert(file,start,size);


    return _ret0;
}

static jboolean filebase_isspchar(JNIEnv* _env,jobject _this_obj, jbyte _ch)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    char ch = (char)_ch;

    jboolean _ret0 = _this->IsSpChar(ch);


    return _ret0;
}

static jboolean filebase_loadfile(JNIEnv* _env,jobject _this_obj, jstring _fn)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *fn = _env->GetStringUTFChars(_fn,NULL);
    ASSERT(fn);

    jboolean _ret0 = _this->LoadFile(fn);

    _env->ReleaseStringUTFChars(_fn,fn);

    return _ret0;
}

static jboolean filebase_readcstr(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadCStr(file);


    return _ret0;
}

static jboolean filebase_readline(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadLine(file);


    return _ret0;
}

static jboolean filebase_readline_reverse(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadLine_Reverse(file);


    return _ret0;
}

static jboolean filebase_readquotestr(JNIEnv* _env,jobject _this_obj, jstring _begin_and_end_char, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *begin_and_end_char = _env->GetStringUTFChars(_begin_and_end_char,NULL);
    ASSERT(begin_and_end_char);
    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadQuoteStr(begin_and_end_char[0],begin_and_end_char[1],file);

    _env->ReleaseStringUTFChars(_begin_and_end_char,begin_and_end_char);

    return _ret0;
}

static jboolean filebase_readstring(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadString(file);


    return _ret0;
}

static jboolean filebase_readstring_reverse(JNIEnv* _env,jobject _this_obj, jobject _file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);

    jboolean _ret0 = _this->ReadString_Reverse(file);


    return _ret0;
}

static jboolean filebase_replacestr(JNIEnv* _env,jobject _this_obj, jstring _src_str, jstring _des_str, jboolean _case_sensive, jboolean _word_only, jobject _new_file)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *src_str = _env->GetStringUTFChars(_src_str,NULL);
    ASSERT(src_str);
    const char *des_str = _env->GetStringUTFChars(_des_str,NULL);
    ASSERT(des_str);
    bool case_sensive = (bool)_case_sensive;
    bool word_only = (bool)_word_only;
    CFileBase *new_file = get_filebase(_env,_new_file);
    ASSERT(new_file);

    jboolean _ret0 = _this->ReplaceStr(src_str,des_str,case_sensive,word_only,new_file);

    _env->ReleaseStringUTFChars(_src_str,src_str);
    _env->ReleaseStringUTFChars(_des_str,des_str);

    return _ret0;
}

static jboolean filebase_reverse(JNIEnv* _env,jobject _this_obj, jlong _start, jlong _end)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t start = (fsize_t)_start;
    fsize_t end = (fsize_t)_end;

    jboolean _ret0 = _this->Reverse(start,end);


    return _ret0;
}

static jboolean filebase_setchar(JNIEnv* _env,jobject _this_obj, jlong _index, jbyte _ch)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t index = (fsize_t)_index;
    char ch = (char)_ch;

    jboolean _ret0 = _this->SetChar(index,ch);


    return _ret0;
}

static jboolean filebase_setfilename(JNIEnv* _env,jobject _this_obj, jstring _fn)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *fn = _env->GetStringUTFChars(_fn,NULL);
    ASSERT(fn);

    jboolean _ret0 = _this->SetFileName(fn);

    _env->ReleaseStringUTFChars(_fn,fn);

    return _ret0;
}

static jboolean filebase_setsize(JNIEnv* _env,jobject _this_obj, jlong _ssize)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    fsize_t ssize = (fsize_t)_ssize;

    jboolean _ret0 = _this->SetSize(ssize);


    return _ret0;
}

static jboolean filebase_setsplitchars(JNIEnv* _env,jobject _this_obj, jstring _sp_str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *sp_str = _env->GetStringUTFChars(_sp_str,NULL);
    ASSERT(sp_str);

    int len = strlen(sp_str);

    char *sp_str_malloc;
    MALLOC(sp_str_malloc,char,len+1);
    crt_strcpy(sp_str_malloc,sp_str);

    jboolean _ret0 = (int)_this->SetSplitChars(sp_str_malloc);
    _this->is_sp_chars_malloc = true;

    _env->ReleaseStringUTFChars(_sp_str,sp_str);
    return _ret0;
}

static jboolean filebase_skipemptychars(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->SkipEmptyChars();


    return _ret0;
}

static jboolean filebase_skipemptychars_reverse(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->SkipEmptyChars_Reverse();


    return _ret0;
}

static jboolean filebase_startwith(JNIEnv* _env,jobject _this_obj, jstring _str, jboolean _case_sensive, jboolean _skip_empty)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);
    bool case_sensive = (bool)_case_sensive;
    bool skip_empty = (bool)_skip_empty;

    jboolean _ret0 = _this->StartWith(str,case_sensive,skip_empty);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jboolean filebase_strcat(JNIEnv* _env,jobject _this_obj, jstring _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    jboolean _ret0 = _this->StrCat(str);

    _env->ReleaseStringUTFChars(_str,str);

    return _ret0;
}

static jboolean filebase_strlwr(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->StrLwr();


    return _ret0;
}

static jboolean filebase_strupr(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->StrUpr();


    return _ret0;
}

static jboolean filebase_trim(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    jboolean _ret0 = _this->Trim();


    return _ret0;
}

static jstring filebase_getfilename(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    const char* _ret0 = _this->GetFileName();
    ASSERT(_ret0);
    jstring ret0 = _env->NewStringUTF(_ret0);


    return ret0;
}

static jboolean filebase_readlinewitheol(JNIEnv* _env,jobject _this_obj, jobject _file, jobject _eol)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    CFileBase *file = get_filebase(_env,_file);
    ASSERT(file);
    CFileBase *eol = get_filebase(_env,_eol);
    ASSERT(eol);

    jboolean _ret0 = _this->ReadLineWithEol(file,eol);


    return _ret0;
}

static jboolean filebase_putint8(JNIEnv* _env,jobject _this_obj, jbyte _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int8_t i = (int8_t)_i;

    jboolean _ret0 = _this->Putc(i);


    return _ret0;
}

static jbyte filebase_getint8(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int8_t ret0 = 0;
    _this->Read(&ret0,sizeof(ret0));
    return ret0;
}

static jboolean filebase_putint16(JNIEnv* _env,jobject _this_obj, jshort _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int16_t i = (int16_t)_i;

    jboolean _ret0 = _this->Write(&i,sizeof(i));

    return _ret0;
}

static jshort filebase_getint16(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int16_t ret0 = 0;
    _this->Read(&ret0,sizeof(ret0));
    jshort _ret0 = ret0;


    return _ret0;
}

static jboolean filebase_putint32(JNIEnv* _env,jobject _this_obj, jint _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int32_t i = (int32_t)_i;

    jboolean _ret0 = _this->Write(&i,sizeof(i));


    return _ret0;
}

static jint filebase_getint32(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int32_t ret0 = 0;
   	_this->Read(&ret0,sizeof(ret0));

    jint _ret0 = ret0;


    return _ret0;
}

static jboolean filebase_putint64(JNIEnv* _env,jobject _this_obj, jlong _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int64_t i = (int64_t)_i;

    jboolean _ret0 = _this->Write(&i,sizeof(i));


    return _ret0;
}

static jlong filebase_getint64(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    int64_t ret0 = 0;
   	_this->Read(&ret0,sizeof(ret0));

    jlong _ret0 = ret0;


    return _ret0;
}

static jboolean filebase_putuint8(JNIEnv* _env,jobject _this_obj, jbyte _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint8_t i = (uint8_t)_i;

    jboolean _ret0 = _this->Write(&i,sizeof(i));


    return _ret0;
}

static jbyte filebase_getuint8(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint8_t ret0 = 0;
  	_this->Read(&ret0,sizeof(ret0));
    jbyte _ret0 = ret0;


    return _ret0;
}

static jboolean filebase_putuint16(JNIEnv* _env,jobject _this_obj, jshort _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint16_t i = (uint16_t)_i;

    jboolean _ret0 = _this->Write(&i,sizeof(i));


    return _ret0;
}

static jshort filebase_getuint16(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint16_t ret0 = 0;
  	_this->Read(&ret0,sizeof(ret0));
    jshort _ret0 = ret0;


    return _ret0;
}

static jboolean filebase_putuint32(JNIEnv* _env,jobject _this_obj, jint _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint32_t i = (uint32_t)_i;

    jboolean _ret0 = _this->Write(&i,sizeof(i));


    return _ret0;
}

static jint filebase_getuint32(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint32_t ret0 = 0;
   	_this->Read(&ret0,sizeof(ret0));
    jint _ret0 = ret0;


    return _ret0;
}

static jboolean filebase_putuint64(JNIEnv* _env,jobject _this_obj, jlong _i)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint64_t i = (uint64_t)_i;

    jboolean _ret0 = _this->Write(&i,sizeof(i));


    return _ret0;
}

static jlong filebase_getuint64(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    uint64_t ret0 = 0;
   	_this->Read(&ret0,sizeof(ret0));
    jlong _ret0 = ret0;


    return _ret0;
}

static jboolean filebase_putzeroendstring(JNIEnv* _env,jobject _this_obj, jstring _str)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    const char *str = _env->GetStringUTFChars(_str,NULL);
    ASSERT(str);

    int len = strlen(str);
    _this->Write(str,len);
	uint8_t zero = 0;
	_this->Write(&zero,sizeof(zero));

    _env->ReleaseStringUTFChars(_str,str);

    return true;
}

static jstring filebase_getzeroendstring(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    LOCAL_MEM(tmp);

    while(!_this->IsEnd())
	{
		char ch = _this->Getc();
		if(ch == 0)break;
		tmp.Putc(ch);
	}

    jstring ret0 = _env->NewStringUTF(tmp.CStr());

    return ret0;
}

static jstring filebase_nextstring(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);


    LOCAL_MEM_WITH_SIZE(mem,LBUF_SIZE*8);
	if(_this->ReadString(&mem))
	{
		jstring ret0 = _env->NewStringUTF(mem.CStr());
		return ret0;
	}

	return NULL;
}

static jstring filebase_nextword(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    LOCAL_MEM_WITH_SIZE(mem,LBUF_SIZE*8);
	if(_this->ReadWord(&mem))
	{
		jstring ret0 = _env->NewStringUTF(mem.CStr());
		return ret0;
	}

	return NULL;
}

static jstring filebase_nextline(JNIEnv* _env,jobject _this_obj)
{
    CFileBase *_this = get_filebase(_env,_this_obj);
    ASSERT(_this);

    LOCAL_MEM_WITH_SIZE(mem,LBUF_SIZE*8);
	if(_this->ReadLine(&mem))
	{
		jstring ret0 = _env->NewStringUTF(mem.CStr());
		return ret0;
	}
	return NULL;
}


static const JNINativeMethod filebase_native_methods[]={
    {"_gc","()I",(void*)filebase__gc},
    {"isEnd","()Z",(void*)filebase_isend},
    {"charAt","(J)B",(void*)filebase_charat},
    {"getLastChar","(Z)B",(void*)filebase_getlastchar},
    {"getc","()B",(void*)filebase_getc},
    {"unGetc","()B",(void*)filebase_ungetc},
    {"getMaxSize","()J",(void*)filebase_getmaxsize},
    {"getOffset","()J",(void*)filebase_getoffset},
    {"getSize","()J",(void*)filebase_getsize},
    {"putc","(B)J",(void*)filebase_putc},
    {"puts","(Lcom/jni/common/CFileBase;)J",(void*)filebase_puts_v1},
    {"puts","(Ljava/lang/String;)J",(void*)filebase_puts_v2},
    {"readLeftStr","(Lcom/jni/common/CFileBase;Z)Z",(void*)filebase_readleftstr},
    {"readWord","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readword},
    {"readWordWithEmptyChar","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readwordwithemptychar},
    {"readWordWithEmptyCharReverse","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readwordwithemptychar_reverse},
    {"readWordReverse","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readword_reverse},
    {"searchBin","(Lcom/jni/common/CFileBase;)J",(void*)filebase_searchbin},
    {"searchStr","(Ljava/lang/String;ZZ)J",(void*)filebase_searchstr},
    {"seek","(J)J",(void*)filebase_seek},
    {"seekBack","(J)J",(void*)filebase_seekback},
    {"seekEnd","()J",(void*)filebase_seekend},
    {"split","(Lcom/jni/common/CFileBase;)J",(void*)filebase_split},
    {"writeFile","(Lcom/jni/common/CFileBase;)J",(void*)filebase_writefile_v1},
    {"writeFile","(Lcom/jni/common/CFileBase;JJ)J",(void*)filebase_writefile_v2},
    {"writeToFile","(Ljava/lang/String;)J",(void*)filebase_writetofile_v1},
    {"writeToFile","(Ljava/lang/String;JJ)J",(void*)filebase_writetofile_v2},
    {"writeToFile","(Lcom/jni/common/CFileBase;JJ)J",(void*)filebase_writetofile_v3},
    {"strCmp","(Lcom/jni/common/CFileBase;)I",(void*)filebase_strcmp_v1},
    {"strCmp","(Ljava/lang/String;)I",(void*)filebase_strcmp_v2},
    {"strICmp","(Lcom/jni/common/CFileBase;)I",(void*)filebase_stricmp_v1},
    {"strICmp","(Ljava/lang/String;)I",(void*)filebase_stricmp_v2},
    {"read","(Lcom/jni/common/CFileBase;J)J",(void*)filebase_read},
    {"readZeroEndString","(Lcom/jni/common/CFileBase;)J",(void*)filebase_readzeroendstring},
    {"strLen","()J",(void*)filebase_strlen},
    {"write","(Lcom/jni/common/CFileBase;J)J",(void*)filebase_write},
    {"isEmptyChar","(B)Z",(void*)filebase_isemptychar},
    {"delete","(J)Z",(void*)filebase_delete},
    {"deleteLast","(J)Z",(void*)filebase_deletelast},
    {"dump","()Z",(void*)filebase_dump},
    {"endWith","(Ljava/lang/String;ZZ)Z",(void*)filebase_endwith},
    {"fillBlock","(JB)Z",(void*)filebase_fillblock},
    {"insert","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_insert_v1},
    {"insert","(Lcom/jni/common/CFileBase;JJ)Z",(void*)filebase_insert_v2},
    {"isSpChar","(B)Z",(void*)filebase_isspchar},
    {"loadFile","(Ljava/lang/String;)Z",(void*)filebase_loadfile},
    {"readCStr","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readcstr},
    {"readLine","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readline},
    {"readLineReverse","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readline_reverse},
    {"readQuoteStr","(Ljava/lang/String;Lcom/jni/common/CFileBase;)Z",(void*)filebase_readquotestr},
    {"readString","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readstring},
    {"readStringReverse","(Lcom/jni/common/CFileBase;)Z",(void*)filebase_readstring_reverse},
    {"replaceStr","(Ljava/lang/String;Ljava/lang/String;ZZLcom/jni/common/CFileBase;)Z",(void*)filebase_replacestr},
    {"reverse","(JJ)Z",(void*)filebase_reverse},
    {"setChar","(JB)Z",(void*)filebase_setchar},
    {"setFileName","(Ljava/lang/String;)Z",(void*)filebase_setfilename},
    {"setSize","(J)Z",(void*)filebase_setsize},
    {"setSplitChars","(Ljava/lang/String;)Z",(void*)filebase_setsplitchars},
    {"skipEmptyChars","()Z",(void*)filebase_skipemptychars},
    {"skipEmptyCharsReverse","()Z",(void*)filebase_skipemptychars_reverse},
    {"startWith","(Ljava/lang/String;ZZ)Z",(void*)filebase_startwith},
    {"strCat","(Ljava/lang/String;)Z",(void*)filebase_strcat},
    {"strLwr","()Z",(void*)filebase_strlwr},
    {"strUpr","()Z",(void*)filebase_strupr},
    {"trim","()Z",(void*)filebase_trim},
    {"getFileName","()Ljava/lang/String;",(void*)filebase_getfilename},
    {"readLineWithEol","(Lcom/jni/common/CFileBase;Lcom/jni/common/CFileBase;)Z",(void*)filebase_readlinewitheol},
    {"putInt8","(B)Z",(void*)filebase_putint8},
    {"getInt8","()B",(void*)filebase_getint8},
    {"putInt16","(S)Z",(void*)filebase_putint16},
    {"getInt16","()S",(void*)filebase_getint16},
    {"putInt32","(I)Z",(void*)filebase_putint32},
    {"getInt32","()I",(void*)filebase_getint32},
    {"putInt64","(J)Z",(void*)filebase_putint64},
    {"getInt64","()J",(void*)filebase_getint64},
    {"putUInt8","(B)Z",(void*)filebase_putuint8},
    {"getUInt8","()B",(void*)filebase_getuint8},
    {"putUInt16","(S)Z",(void*)filebase_putuint16},
    {"getUInt16","()S",(void*)filebase_getuint16},
    {"putUInt32","(I)Z",(void*)filebase_putuint32},
    {"getUInt32","()I",(void*)filebase_getuint32},
    {"putUInt64","(J)Z",(void*)filebase_putuint64},
    {"getUInt64","()J",(void*)filebase_getuint64},
    {"putZeroEndString","(Ljava/lang/String;)Z",(void*)filebase_putzeroendstring},
    {"getZeroEndString","()Ljava/lang/String;",(void*)filebase_getzeroendstring},
    {"nextString","()Ljava/lang/String;",(void*)filebase_nextstring},
    {"nextWord","()Ljava/lang/String;",(void*)filebase_nextword},
    {"nextLine","()Ljava/lang/String;",(void*)filebase_nextline},
};
status_t register_filebase_native_methods(JNIEnv* env)
{
    jclass clazz;
    clazz = env->FindClass(THIS_JAVA_CLASS_PATH);
    ASSERT(clazz);
    int32_t size = sizeof(filebase_native_methods)/sizeof(filebase_native_methods[0]);
    ASSERT(env->RegisterNatives(clazz, filebase_native_methods , size) == JNI_OK);
    return OK;
}
