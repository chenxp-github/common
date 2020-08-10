// CmdExe.cpp: implementation of the CCmdExe class.
//
//////////////////////////////////////////////////////////////////////
#include "cmdexe.h"
#include "syslog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCmdExe::CCmdExe()
{
    this->InitBasic();
}
CCmdExe::~CCmdExe()
{
    this->Destroy();
}
int CCmdExe::InitBasic()
{
    this->hSaveStdin = NULL;
    this->hSaveStdout = NULL;
    this->hChildStdoutRd = NULL;
    this->hChildStdoutWr = NULL;
    this->hChildStdinRd = NULL;
    this->hChildStdinWr = NULL;
    this->hChildStdinWrDup = NULL;
    this->hChildStdoutRdDup = NULL;
    
    memset(&this->siStartInfo,0,sizeof(STARTUPINFO));  
    memset(&this->piProcInfo,0,sizeof(PROCESS_INFORMATION));

    return OK;
}
int CCmdExe::Init()
{
    this->InitBasic();
    
    SECURITY_ATTRIBUTES   saAttr;       
    BOOL fSuccess;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);       
    saAttr.bInheritHandle = TRUE;       
    saAttr.lpSecurityDescriptor = NULL;   

    hSaveStdout   =   GetStdHandle(STD_OUTPUT_HANDLE);       
    hSaveStdin   =   GetStdHandle(STD_INPUT_HANDLE);       
    
    if(!CreatePipe(   &hChildStdoutRd,   &hChildStdoutWr,   &saAttr,   1024*1024))
    {           
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "Stdout   pipe   creation   failed\n"
        );
        return ERROR;           
    }   
    
    if(!CreatePipe(&hChildStdinRd,   &hChildStdinWr,   &saAttr,   1024*1024))
    {   
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "Stdin   pipe   creation   failed\n"
        );
        return ERROR;           
    }   
    
    if( !SetStdHandle(STD_INPUT_HANDLE,   hChildStdinRd))           
    {   
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "Redirecting   Stdin   failed\n"
        );          
        return   FALSE;         
    }   

    if( !SetStdHandle(STD_OUTPUT_HANDLE,   hChildStdoutWr) )        
    {   
        
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "Redirecting   STDOUT   failed\n"
        );
        return   FALSE;         
    }   

    fSuccess   =   DuplicateHandle( GetCurrentProcess(), hChildStdinWr,     
                                    GetCurrentProcess(), &hChildStdinWrDup,     
                                    0,   FALSE,                                     //   not   inherited                 
                                    DUPLICATE_SAME_ACCESS   );     
    if(!fSuccess)     
    {   
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "DuplicateWrHandle   failed\n"
        );
        return   FALSE;   
    }   

    fSuccess   =   DuplicateHandle( GetCurrentProcess(), hChildStdoutRd,   
                                    GetCurrentProcess(), &hChildStdoutRdDup   ,     
                                    0, FALSE,   
                                    DUPLICATE_SAME_ACCESS   );   
    if( !fSuccess )   
    {   
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "DuplicateRdHandle   failed\n"
        );
        return   FALSE;   
    }   
    
    return OK;
}
int  CCmdExe::Destroy()
{
    if( !SetStdHandle(STD_INPUT_HANDLE,   hSaveStdin))          
    {           
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "Re-redirecting   Stdin   failed\n"
        );
        return   FALSE;         
    }   

    if( !SetStdHandle(STD_OUTPUT_HANDLE,   hSaveStdout))        
    {           
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "Re-redirecting   Stdout   failed\n"
        );
        return   FALSE;         
    }   

    if(this->hChildStdoutRd)
        CloseHandle(hChildStdoutRd);
    if(this->hChildStdoutWr)
        CloseHandle(hChildStdoutWr);
    if(this->hChildStdinRd)
        CloseHandle(hChildStdinRd);
    if(this->hChildStdinWr)
        CloseHandle(hChildStdinWr);
    if(this->hChildStdinWrDup)
        CloseHandle(hChildStdinWrDup);
    if(this->hChildStdoutRdDup)
        CloseHandle(hChildStdoutRdDup);

    this->InitBasic();

    return OK;
}
int  CCmdExe::Print()
{
    return TRUE;
}

int CCmdExe::Exec(const char *cmd)
{
    siStartInfo.cb   =   sizeof(STARTUPINFO);       
    siStartInfo.dwFlags   =   STARTF_USESTDHANDLES;   
    siStartInfo.hStdInput   =   hChildStdinRd;   
    siStartInfo.hStdOutput   =   hChildStdoutWr;   
    siStartInfo.hStdError   =   hChildStdoutWr;   

    BOOL   ret   =   CreateProcessA(   
        NULL,   
        (char*)cmd,                     //   applicatin   name   
        NULL,                       //   process   security   attributes     
        NULL,                       //   primary   thread   security   attributes     
        TRUE,                       //   handles   are   inherited     
        DETACHED_PROCESS,           //   creation   flags     
        NULL,                       //   use   parent's   environment     
        NULL,                       //   use   parent's   current   directory     
        &siStartInfo,               //   STARTUPINFO   pointer     
        &piProcInfo);               //   receives   PROCESS_INFORMATION     

    if(ret == 0)
        return ERROR;

    return OK;
}

int CCmdExe::WaitForEnd()
{
    ASSERT(this->piProcInfo.hProcess);
    WaitForSingleObject(this->piProcInfo.hProcess,INFINITE);
    this->piProcInfo.hProcess = NULL;
    return OK;
}

long CCmdExe::Read(void *buf, long rs)
{
    ASSERT(buf);
    ASSERT(rs > 0);

    DWORD dwRead = 0;

    ReadFile(hChildStdoutRdDup,buf,rs,&dwRead,NULL);

    return dwRead;
}

long CCmdExe::Write(void *buf, long ws)
{
    ASSERT(buf);
    ASSERT(ws > 0);

    DWORD dwWrite = 0;

    WriteFile(this->hChildStdinWrDup,buf,ws,&dwWrite,NULL);

    return dwWrite;
}

int CCmdExe::PutsStdout(const char *str)
{
    ASSERT(str);

    DWORD dwWrite;

    WriteFile(hChildStdoutWr,str,strlen(str),&dwWrite,NULL);

    return dwWrite;
}

status_t CCmdExe::Terminate()
{
    if(this->piProcInfo.hProcess)
    {
        TerminateProcess(this->piProcInfo.hProcess,0);
        this->piProcInfo.hProcess = NULL;
    }

    return OK;
}
