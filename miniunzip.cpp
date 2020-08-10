#include "miniunzip.h"

CMiniUnzip::CMiniUnzip()
{
    this->InitBasic();
}
CMiniUnzip::~CMiniUnzip()
{
    this->Destroy();
}
status_t CMiniUnzip::InitBasic()
{
    this->iZipFile = NULL;
    this->mUnFunc = NULL;
    this->uFile = NULL;
    return OK;
}
status_t CMiniUnzip::Init()
{
    this->InitBasic();
    MALLOC(this->mUnFunc,zlib_filefunc_def,1);
    fill_filebase_filefunc (this->mUnFunc);
    return OK;
}
status_t CMiniUnzip::Destroy()
{
    if(this->uFile)
    {
        unzClose(this->uFile);
        this->uFile = NULL;
    }
    FREE(this->mUnFunc);
    this->InitBasic();
    return OK;
}

status_t CMiniUnzip::SetZipFile(CFileBase *file)
{
    ASSERT(file);
    ASSERT(this->uFile == NULL);

    this->iZipFile = file;
    this->uFile = unzOpen2((const char *)this->iZipFile,this->mUnFunc);
    ASSERT(this->uFile);
    return OK;
}

int32_t CMiniUnzip::GetFileCount()
{
    unz_global_info gi;
    int err = unzGetGlobalInfo(this->uFile,&gi);
    ASSERT(err==UNZ_OK);
    return gi.number_entry; 
}


status_t CMiniUnzip::GotoFirstFile()
{
    int err = unzGoToFirstFile(this->uFile);
    return err==UNZ_OK;
}


status_t CMiniUnzip::GotoNextFile()
{
    int err = unzGoToNextFile(this->uFile);
    return err==UNZ_OK;
}

status_t CMiniUnzip::GetCurFileName(CMem *name)
{
    ASSERT(name);

    char fn[256];
    unz_file_info file_info;
    int err = unzGetCurrentFileInfo(this->uFile,&file_info,fn,sizeof(fn),NULL,0,NULL,0);
    ASSERT(err == UNZ_OK);
    name->StrCpy(fn);
    return OK;
}

fsize_t CMiniUnzip::GetCurFileSize()
{
    char fn[256];
    unz_file_info file_info;
    int err = unzGetCurrentFileInfo(this->uFile,&file_info,fn,sizeof(fn),NULL,0,NULL,0);
    ASSERT(err == UNZ_OK);  
    return file_info.uncompressed_size;
}

status_t CMiniUnzip::LocateFile(const char *fileName, bool caseSensitivy)
{
    ASSERT(fileName);
    if(fileName[0] == '/') fileName++;
    return unzLocateFile(this->uFile,fileName,caseSensitivy) == UNZ_OK;
}

fsize_t CMiniUnzip::ReadCurFile(void *buf, fsize_t len)
{
    ASSERT(buf);
    return unzReadCurrentFile(this->uFile,buf,(uint32_t)len);
}

status_t CMiniUnzip::OpenCurFile()
{
    return unzOpenCurrentFile(this->uFile) == UNZ_OK;
}

status_t CMiniUnzip::ExtractCurFile(CFileBase *out)
{
    ASSERT(out);
    out->SetSize(0);
    char buf[32*1024];

    fsize_t size;

    this->OpenCurFile();
    while(true)
    {
        size = this->ReadCurFile(buf,sizeof(buf));
        if(size <= 0) break;
        out->Write(buf,(int32_t)size);
    }
    
    this->CloseCurFile();
    return OK;
}

status_t CMiniUnzip::CloseCurFile()
{
    return unzCloseCurrentFile(this->uFile) == UNZ_OK;
}

bool CMiniUnzip::IsCurFileDir()
{
    CMem mem;
    LOCAL_MEM(mem);
    this->GetCurFileName(&mem);
    char ch = mem.GetLastChar(0);
    return ch =='/' || ch== '\\';
}
