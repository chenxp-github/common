#include "common_utils.h"
#include "dirmgr.h"
#include "xlog.h"

status_t get_self_exe_path(CMem *fullpath)
{
    ASSERT(fullpath);
    fullpath->SetSize(0);
    int n=readlink( "/proc/self/exe",fullpath->GetRawBuf(),fullpath->GetMaxSize()-1);
    if(n > 0)
    {
        fullpath->SetSize(n);
        return OK;
    }
    return ERROR;
}

status_t make_full_path_with_exe(const char *rpath, CMem *out)
{
    ASSERT(rpath && out);
    out->SetSize(0);
    
    if(CDirMgr::IsAbsPath(rpath))
    {
        out->StrCpy(rpath);
        return OK;
    }
    
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);
    get_self_exe_path(&mem);
    CDirMgr::GetFileName(&mem,&buf,FN_PATH);
    
    out->Puts(&buf);
    out->Putc(crt_get_path_splitor());
    out->Puts(rpath);
    CDirMgr::ToAbsPath("",out);
    return OK;
}
