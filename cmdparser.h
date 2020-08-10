// CmdParser.h: interface for the CCmdParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDPARSER_H__55FDBFD3_7C81_4167_92D8_17F87B0A6987__INCLUDED_)
#define AFX_CMDPARSER_H__55FDBFD3_7C81_4167_92D8_17F87B0A6987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mem.h"
#include "memfile.h"
#include "memstk.h"

class CCmdParser{
public:
    CMemStk *cmd_line;
public:
    int HaveKey(char *key,int case_sensive);
    int SetCmdLine(char *line);
    CMem * ReadKeyValue(char *key,int case_sensive,char *defval);
    int GetArgc();
    CMem * ReadKeyValue(char *key,int case_sensive);
    CMem * GetArgv(int i);
    int SetCmdLine(int argc,char **argv);
    CCmdParser();
    virtual ~CCmdParser();
    int Init();
    int Destroy();
    int InitBasic();
};

#endif // !defined(AFX_CMDPARSER_H__55FDBFD3_7C81_4167_92D8_17F87B0A6987__INCLUDED_)
