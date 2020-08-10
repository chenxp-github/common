// CmdParser.cpp: implementation of the CCmdParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdParser::CCmdParser()
{
    this->InitBasic();
}
CCmdParser::~CCmdParser()
{
    this->Destroy();
}
int CCmdParser::InitBasic()
{
    this->cmd_line = NULL;
    return OK;
}
int CCmdParser::Init()
{
    this->InitBasic();
    
    NEW(this->cmd_line,CMemStk);
    this->cmd_line->Init(100);

    return OK;
}
int  CCmdParser::Destroy()
{
    DEL(this->cmd_line);
    this->InitBasic();
    return OK;
}

int CCmdParser::SetCmdLine(int argc, char **argv)
{
    ASSERT(argv);
    
    int i;
    
    this->cmd_line->Clear();

    for(i = 0; i<argc; i++)
    {
        this->cmd_line->Push(argv[i]);
    }   

    return OK;
}

int CCmdParser::GetArgc()
{
    return this->cmd_line->GetLen();
}

CMem * CCmdParser::GetArgv(int i)
{
    return this->cmd_line->GetElem(i);
}

CMem * CCmdParser::ReadKeyValue(char *key,int case_sensive)
{
    int i,c;
    CMem *pmem;

    for(i = 0; i < this->GetArgc(); i++)
    {
        pmem = this->GetArgv(i);
        ASSERT(pmem);       
        c = case_sensive ? pmem->StrCmp(key) : pmem->StrICmp(key);
        if(c == 0)
            return this->GetArgv(i + 1);
    }

    return NULL;
}

CMem * CCmdParser::ReadKeyValue(char *key, int case_sensive, char *defval)
{
    CMem *pmem = this->ReadKeyValue(key,case_sensive);
    
    if(pmem)
    {
        return pmem;
    }
    else
    {
        static CMem mem;
        
        mem.Init();
        mem.SetP(defval,strlen(defval) + 1);

        return &mem;
    }
}

int CCmdParser::SetCmdLine(char *line)
{
    CMem mem_line,mem_buf;

    LOCAL_MEM(mem_buf);
    mem_line.Init();
    mem_line.SetP(line,strlen(line));

    this->cmd_line->Clear();

    mem_line.Seek(0);
    mem_line.SetSplitChars(" \r\n\t");
    while(mem_line.ReadWord(&mem_buf))
    {
        this->cmd_line->Push(&mem_buf);
    }

    return OK;
}

int CCmdParser::HaveKey(char *key, int case_sensive)
{
    int i,c;
    CMem *pmem;

    for(i = 0; i < this->GetArgc(); i++)
    {
        pmem = this->GetArgv(i);
        ASSERT(pmem);       
        c = case_sensive ? pmem->StrCmp(key) : pmem->StrICmp(key);
        if(c == 0)
            return TRUE;
    }

    return FALSE;
}
