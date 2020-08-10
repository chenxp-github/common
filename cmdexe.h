// CmdExe.h: interface for the CCmdExe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDEXE_H__AB6C6B2D_D3FA_44EA_93EF_A9C342A04795__INCLUDED_)
#define AFX_CMDEXE_H__AB6C6B2D_D3FA_44EA_93EF_A9C342A04795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"

class CCmdExe{
public:
    HANDLE hSaveStdin,hSaveStdout;
    HANDLE hChildStdoutRd,hChildStdoutWr;
    HANDLE hChildStdinRd,hChildStdinWr;
    HANDLE hChildStdinWrDup,hChildStdoutRdDup;
    PROCESS_INFORMATION  piProcInfo;   
    STARTUPINFOA  siStartInfo;  
public:
    status_t Terminate();
    int PutsStdout(const char *str);
    long Write(void *buf,long ws);
    long Read(void *buf,long rs);
    int WaitForEnd();
    int Exec(const char *cmd);
    CCmdExe();
    virtual ~CCmdExe();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
};

#endif // !defined(AFX_CMDEXE_H__AB6C6B2D_D3FA_44EA_93EF_A9C342A04795__INCLUDED_)
