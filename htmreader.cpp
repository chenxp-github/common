// HtmReader.cpp: implementation of the CHtmReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HtmReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHtmReader::CHtmReader()
{
    this->InitBasic();
}
CHtmReader::~CHtmReader()
{
    this->Destroy();
}
status_t CHtmReader::InitBasic()
{
    this->i_file_htm = NULL;
    this->mem_stk = NULL;
    this->mf_attrib = NULL;
    this->mf_value = NULL;
    this->status = HTM_STATUS_NORMAL;

    return OK;
}
status_t CHtmReader::Init()
{
    this->InitBasic();
    
    NEW(this->mem_stk,CMemStk);
    this->mem_stk->Init(1024);
    
    NEW(this->mf_attrib,CMemFile);
    this->mf_attrib->Init();

    NEW(this->mf_value,CMemFile);
    this->mf_value->Init();

    return OK;
}
status_t  CHtmReader::Destroy()
{
    DEL(this->mem_stk);
    DEL(this->mf_attrib);
    DEL(this->mf_value);

    this->InitBasic();
    return OK;
}

status_t CHtmReader::GetCurPath(CFileBase *file)
{
    return this->mem_stk->GetPath(file);
}

status_t CHtmReader::IsFileEmpty(CFileBase *file)
{
    ASSERT(file);

    file->Seek(0);
    while(!file->IsEnd())
    {
        if(! CFileBase::IsEmptyChar(file->Getc()))
            return FALSE;
    }
    return TRUE;
}

status_t CHtmReader::EnterNextTag()
{
    ASSERT(this->i_file_htm);

    char ch;
    int32_t ret = HTM_RET_ERR,old_off;
    CMem buf,tmp;

    LOCAL_MEM(tmp);
    LOCAL_MEM(buf);

    this->mf_attrib->SetSize(0);
    this->mf_value->SetSize(0);

    while(!this->i_file_htm->IsEnd())
    {
        /*======================================================*/
        if(this->status == HTM_STATUS_NORMAL)
        {
            this->i_file_htm->SkipEmptyChars();
            while(! this->i_file_htm->IsEnd() )
            {           
                ch = this->i_file_htm->Getc();              
                if(ch =='<')
                {
                    this->status = HTM_STATUS_TAG;
                    break;
                }
                this->mf_value->Putc(ch);               
            }
        }
        /*======================================================*/
        if(this->status == HTM_STATUS_TAG)
        {
            this->i_file_htm->SkipEmptyChars();
            ch = this->i_file_htm->Getc();
            if(ch == '!')
            {
                this->i_file_htm->UnGetc();
                this->i_file_htm->ReadWord(&buf);
                this->mem_stk->Push(&buf);
                if(buf.StrICmp("!--") == 0)
                {
                    if(this->ProcessComment())
                    {
                        ret = HTM_RET_SINGLE;
                        goto end;
                    }
                }
                this->i_file_htm->SkipEmptyChars();
                while(!this->i_file_htm->IsEnd())
                {
                    ch = this->i_file_htm->Getc();
                    if(ch =='>')
                    {
                        this->status = HTM_STATUS_NORMAL;
                        break;
                    }
                    this->mf_value->Putc(ch);
                }
                
                ret = HTM_RET_SINGLE;
                goto end;
            }
            else if(ch =='/')
            {
                this->i_file_htm->ReadWord(&buf);
                if(this->Skip(">") == ERROR)
                {
                    ret = HTM_RET_ERR;
                    goto end;
                }
                ret = HTM_RET_LEAVE;
                goto end;
            }
            else
            {
                this->i_file_htm->UnGetc();
                this->i_file_htm->ReadWord(&buf);
                this->mem_stk->Push(&buf);

                while(!this->i_file_htm->IsEnd())
                {
                    this->i_file_htm->ReadWord(&buf);
                    
                    if(buf.p[0] == '/')
                    {       
                        old_off = this->i_file_htm->GetOffset();
                        this->i_file_htm->ReadWord(&tmp);
                        if(tmp.p[0] == '>')                     
                        {
                            ret = HTM_RET_SINGLE;
                            this->status = HTM_STATUS_NORMAL;
                            goto end;
                        }
                        else
                        {
                            this->i_file_htm->Seek(old_off);    
                        }
                    }
                    else if(buf.p[0] == '>')
                    {                       
                        CMem *pmem = this->mem_stk->GetTopPtr();
                        ASSERT(pmem);
                        if(pmem->StrICmp("script") == 0)
                        {
                            if(this->ProcessScript())
                            {
                                this->status = HTM_STATUS_NORMAL;
                                ret = HTM_RET_SINGLE;
                                goto end;           
                            }
                        }

                        else if(pmem->StrICmp("style") == 0)
                        {
                            if(this->ProcessStyle())
                            {
                                this->status = HTM_STATUS_NORMAL;
                                ret = HTM_RET_SINGLE;
                                goto end;           
                            }
                        }
                        else
                        {
                            this->status = HTM_STATUS_NORMAL;
                            ret = HTM_RET_ENTER;
                            goto end;
                        }
                    }
                    else
                    {
                        this->mf_attrib->Puts(buf.p);                       
                        if(buf.p[0] == '=')
                        {
                            old_off = this->i_file_htm->GetOffset();
                            this->i_file_htm->ReadWord(&buf);
                                                        
                            if(buf.p[0] == '\"' || buf.p[0]=='\'')
                            {
                                this->i_file_htm->ReadQuoteStr('\\',buf.p[0],&buf);
                            }
                            else
                            {
                                this->i_file_htm->Seek(old_off);
                                this->ReadAttribStr(&buf);
                            }
                            this->mf_attrib->Puts(buf.p);
                            this->mf_attrib->Puts("\r\n");
                        }
                    }
                }
            }
        }
    }
end:
    return ret;
}

status_t CHtmReader::SetHtmFile(CFileBase *file)
{
    ASSERT(file);

    this->i_file_htm = file;
    this->i_file_htm->SetSplitChars(" \t\r\n<>=\"\'/");
    this->i_file_htm->Seek(0);

    return OK;
}

status_t CHtmReader::Skip(const char *str)
{
    CMem mem_buf;

    LOCAL_MEM(mem_buf);
    this->i_file_htm->ReadWord(&mem_buf);
    if(mem_buf.StrICmp(str) == 0)
        return OK;
    LOG("syntax error: '%s' expected AT %d.\n",str,this->i_file_htm->GetOffset());
    return ERROR;
}

status_t CHtmReader::DelStackTop()
{
    return this->mem_stk->DelTop();
}

status_t CHtmReader::ProcessScript()
{
    int32_t start = this->i_file_htm->GetOffset();
    int32_t pos = this->SearchWordList(this->i_file_htm , "< / script >",0);
    int32_t off = this->i_file_htm->GetOffset();
    if(pos < 0)
        return ERROR;

    this->mf_value->SetSize(0);
    this->i_file_htm->WriteToFile(this->mf_value,start,pos - start);
    this->i_file_htm->Seek(off);
    return OK;
}
status_t CHtmReader::ProcessStyle()
{
    int32_t start = this->i_file_htm->GetOffset();

    int32_t pos = this->SearchWordList(this->i_file_htm , "< / style >",0);
    int32_t off = this->i_file_htm->GetOffset();
    if(pos < 0)
        return ERROR;

    this->mf_value->SetSize(0);
    this->i_file_htm->WriteToFile(this->mf_value,start,pos - start);
    this->i_file_htm->Seek(off);
    return OK;
}

status_t CHtmReader::ProcessComment()
{
    int32_t start = this->i_file_htm->GetOffset();

    int32_t pos = this->SearchWordList(this->i_file_htm , "-- >",0);
    int32_t off = this->i_file_htm->GetOffset();
    if(pos < 0)
        return ERROR;

    this->mf_value->SetSize(0);
    this->i_file_htm->WriteToFile(this->mf_value,start,pos - start);
    this->i_file_htm->Seek(off);
    return OK;
}

status_t CHtmReader::ReadAttribStr(CFileBase *file)
{
    ASSERT(file);
    
    const char *old_sp = this->i_file_htm->sp_chars;

    this->i_file_htm->SetSplitChars("\t <>");
    this->i_file_htm->ReadWord(file);       
    this->i_file_htm->SetSplitChars(old_sp);
    
    return OK;
}

CMem * CHtmReader::GetStackTop()
{
    CMem *pmem = this->mem_stk->GetTopPtr();
    if(pmem)return pmem;
    static CMem mem;
    const char *p = "@empty@";
    mem.Init();
    mem.SetP((char*)p);

    return &mem;
}

status_t CHtmReader::GetAllLink(CFileBase *file_htm, CFileBase *file_links)
{
    ASSERT(file_htm && file_links);
    
    CHtmReader htm;
    CMem mem_buf,mem;
    int32_t ret,n;

    LOCAL_MEM(mem);
    LOCAL_MEM(mem_buf);

    file_links->SetSize(0);

    htm.Init();
    htm.SetHtmFile(file_htm);

    while(1)
    {
        ret = htm.EnterNextTag();
        if(ret == HTM_RET_ERR)
            break;

        if(ret == HTM_RET_LEAVE)
            htm.DelStackTop();
        
        if(ret == HTM_RET_ENTER || ret == HTM_RET_SINGLE)
        {
            htm.mf_attrib->Seek(0);
            while(htm.mf_attrib->ReadLine(&mem_buf))
            {
                if(mem_buf.StartWith("href",0,1) || mem_buf.StartWith("src",0,1))
                {
                    mem_buf.Seek(0);
                    n = mem_buf.SearchStr("=",0,0);
                    if(n >= 0)
                    {

                        mem_buf.Seek(n+1);
                        mem_buf.ReadLeftStr(&mem,1);
                        file_links->Puts(&mem);
                        file_links->Putc('\n');
                    }
                }
            }
            if(ret == HTM_RET_SINGLE)
                htm.DelStackTop();
        }
    }

    return OK;
}
 
status_t CHtmReader::Htm2Txt(CFileBase *file_htm, CFileBase *file_txt)
{
    ASSERT(file_htm && file_txt);
    
    CHtmReader htm;
    int32_t ret;

    htm.Init();
    htm.SetHtmFile(file_htm);

    while(1)
    {
        ret = htm.EnterNextTag();
        if(ret == HTM_RET_ERR)
            break;

        if(ret == HTM_RET_SINGLE)
            htm.DelStackTop();

        if(ret == HTM_RET_ENTER)
        {
            if(htm.mf_value->GetSize() > 0)
            {
                if(file_txt->GetSize() > 0) 
                    file_txt->Puts("\r\n");
                file_txt->WriteFile(htm.mf_value);
            }
        }
        if(ret == HTM_RET_LEAVE)
        {
            if(htm.mf_value->GetSize() > 0)
            {
                if(file_txt->GetSize() > 0) 
                    file_txt->Puts("\r\n");
                file_txt->WriteFile(htm.mf_value);              
            }
            htm.DelStackTop();
        }       
    }

    return OK;
}

int32_t CHtmReader::SearchWordList(CFileBase *file,const char *list, status_t case_sensitive)
{
    CMem mem,buf,buf1;
    int pos = -1,c,flag = 0;

    LOCAL_MEM(buf);
    LOCAL_MEM(buf1);

    mem.Init();
    mem.SetSplitChars(" \t");
    mem.SetP((char*)list);
    while(!file->IsEnd())
    {
        mem.Seek(0);
        mem.ReadWord(&buf);
        pos = file->SearchStr(buf.p,case_sensitive,TRUE);
        if(pos < 0) return -1;

        file->Seek(pos);
        file->ReadWord(&buf); //skip first word
        flag = 1;
        while(!mem.IsEnd())
        {
            mem.ReadWord(&buf);
            if(buf.p[0] == 0) break;
            file->ReadWord(&buf1);
            c = case_sensitive ? buf.StrCmp(buf1.p):buf.StrICmp(buf1.p);
            if(c != 0)
            {
                file->Seek(pos + 1);
                flag = 0;
                break;
            }
        }
        if(flag)return pos;
    }
    return -1;
}
