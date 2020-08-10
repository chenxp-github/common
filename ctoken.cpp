// CToken.cpp: implementation of the CCToken class.
//
//////////////////////////////////////////////////////////////////////
#include "ctoken.h"
#include "mem.h"
#include "mem_tool.h"
#include "syslog.h"

static const char *c_ignore_words[] = {
    "static",
    "struct",
    "const",
    "[",
    "]",
    "(",
    ")",
    "&",
};

static const char *java_ignore_words[] = {
    "public",
    "static",
    "private",
    "[",
    "]",
    "(",
    ")",
    "&",
};

const int c_ignore_words_size = sizeof(c_ignore_words)/sizeof(c_ignore_words[0]);
const int java_ignore_words_size = sizeof(java_ignore_words)/sizeof(java_ignore_words[0]);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCToken::CCToken()
{
    this->InitBasic();
}
CCToken::~CCToken()
{
    this->Destroy();
}
status_t CCToken::InitBasic()
{
    this->i_file_src = NULL;
    this->mf_value = NULL;
    this->line = 0;
    this->ignore_words = NULL;
    this->ignore_words_size = 0;
    return OK;
}
status_t CCToken::Init()
{
    this->InitBasic();
    
    NEW(this->mf_value,CMemFile);
    this->mf_value->Init();

    this->SetIgnoreTypeC();
    return OK;
}
status_t  CCToken::Destroy()
{
    DEL(this->mf_value);
    this->InitBasic();
    return OK;
}
char CCToken::Getc()
{
    ASSERT(this->i_file_src);
    char ch;
    ch = this->i_file_src->Getc();  
    
    if(ch == '\r')
        ch = this->i_file_src->Getc();  
    if(ch == '\n')
        line ++;
    return ch;
}
status_t CCToken::GetNextTag()
{
    char ch;
    if(this->i_file_src->IsEnd())
        return CTYPE_ERR;
    ch = this->Getc();
    
    this->mf_value->SetSize(0);
    /////////////////////////////////
    if(CFileBase::IsEmptyChar(ch))
    {
        this->mf_value->Putc(ch);
        return CTYPE_WHITE;
    }
    /////////////////////////////////
    //comments
    if(ch == '/')
    {
        this->mf_value->Putc(ch);
        ch = this->Getc();
        if(ch == '/')
        {
            this->mf_value->Putc(ch);
            //single line comments
            while(!this->IsEnd())
            {
                ch = this->Getc();
                if(ch == '\n')
                {           
                    this->UnGetc();
                    break;
                }
                this->mf_value->Putc(ch);
            }
            return CTYPE_SINGLE_COMMENT;
        }
        
        else if(ch == '*')
        {
            this->mf_value->Putc(ch);
            //multi line comments
            char last_char;
            while(!this->IsEnd())
            {   
                last_char = ch;
                ch = this->Getc();
                if(last_char=='*' && ch == '/')
                {
                    mf_value->Puts("/");
                    break;
                }
                else
                {
                    mf_value->Putc(ch);
                }
            }
            return CTYPE_MULTI_COMMENT;
        }
    }
    /////////////////////////////////
    //string
    if(ch =='\"')
    {
        LOCAL_MEM(mem);
        this->mf_value->Putc(ch);
        this->i_file_src->ReadQuoteStr('\\','\"',&mem);
        this->mf_value->Puts(&mem);
        this->mf_value->Putc(ch);
        return CTYPE_STRING;
    }
    /////////////////////////////////
    if(ch == '\'')
    {
        LOCAL_MEM(mem);
        this->mf_value->Putc(ch);
        this->i_file_src->ReadQuoteStr('\\','\'',&mem);
        this->mf_value->Puts(&mem);
        this->mf_value->Putc(ch);
        return CTYPE_STRING_SINGLE; 
    }
    /////////////////////////////////
    if(ch == '#')
    {
        char last_char1=0,last_char2=0;
        this->mf_value->Putc(ch);
        while(!this->IsEnd())
        {
            last_char2 = last_char1;
            last_char1 = ch;
        
            ch = this->Getc();
            this->mf_value->Putc(ch);
            if(ch == '\n')
            {
                if(last_char1 =='\r')
                {
                    if(last_char2 != '\\')
                        break;
                }
                else
                {
                    if(last_char1 != '\\')
                        break;
                }
            }
        }
        return CTYPE_PREPROCESS;
    }
    /////////////////////////////////
    //only single puncture
    if(this->i_file_src->IsSpChar(ch))
    {
        this->mf_value->Putc(ch);
        return CTYPE_PUNC;
    }
    /////////////////////////////////
    //normal worlds
    this->mf_value->Putc(ch);
    while(!this->IsEnd())
    {
        ch = this->Getc();
        if(this->i_file_src->IsSpChar(ch) || CFileBase::IsEmptyChar(ch))
        {
            this->UnGetc();
            return CTYPE_NORMAL;
        }       
        this->mf_value->Putc(ch);
    }
    return CTYPE_NORMAL;
}
status_t CCToken::IsEnd()
{
    ASSERT(this->i_file_src);
    return this->i_file_src->IsEnd();
}
char CCToken::UnGetc()
{
    char ch = this->i_file_src->UnGetc();
    if(ch == '\n')
        line --;
    return ch;
}
status_t CCToken::SplitToBlocks(CMemStk *stk)
{
    ASSERT(this->i_file_src && stk);
    CMemFile mf;
    char ch;
    int32_t bracket_big = 0;
    int32_t bracket_small = 0;
    mf.Init();
    this->i_file_src->Seek(0);
    stk->Clear();
    int32_t ret;
    while((ret = this->GetNextTag()))
    {
        if(ret == CTYPE_PUNC)
        {
            ch = this->mf_value->CharAt(0);
            if(ch == '(')
            {
                bracket_small++;
            }
            else if(ch == ')')
            {
                bracket_small--;
            }
            else if(ch == '{')
            {
                if(bracket_big == 0)
                {
                    stk->PushFile(&mf);
                    mf.SetSize(0);
                }
                bracket_big ++;
            }
            else if(ch == '}')
            {
                bracket_big --;
                if(bracket_big == 0)
                {
                    mf.WriteFile(this->mf_value);
                    stk->PushFile(&mf);
                    mf.SetSize(0);
                    continue;
                }
            }
            else if(ch == ';')
            {
                if(bracket_big == 0 && bracket_small == 0)
                {
                    mf.WriteFile(this->mf_value);
                    stk->PushFile(&mf);
                    mf.SetSize(0);
                    continue;
                }
            }
        }
        
        if(bracket_big == 0)
        {
            if(ret == CTYPE_PREPROCESS)
            {
                if(mf.GetSize() > 0)
                    stk->PushFile(&mf);
                mf.SetSize(0);
                stk->PushFile(this->mf_value);
                
            }
            else if(ret == CTYPE_SINGLE_COMMENT || ret == CTYPE_MULTI_COMMENT)
            {
                int32_t hit_block_end = 0;
                if(mf.GetSize() <= 0)
                    hit_block_end = 1;
                if(mf.CharAt(0) == '#')
                    hit_block_end = 1;
                ch = mf.GetLastChar(0);
                if(ch == '}' || ch == ';' || CFileBase::IsEmptyChar(ch))
                    hit_block_end = 1;
                if(hit_block_end)
                {
                    if(mf.GetSize() > 0)
                        stk->PushFile(&mf);
                    mf.SetSize(0);
                    stk->PushFile(this->mf_value);
                }
                else
                {
                    mf.WriteFile(this->mf_value);
                }
            }
            else
            {
                mf.WriteFile(this->mf_value);
            }           
        }
        else
        {
            mf.WriteFile(this->mf_value);
        }
    }
    if(mf.GetSize() > 0)
    {
        stk->PushFile(&mf);
        mf.SetSize(0);
    }
    return OK;
}
status_t CCToken::IsBracketBlock(CFileBase *block)
{
    ASSERT(block);
    return block->StartWith("{",0,1);
}
status_t CCToken::IsFuncBlock(CFileBase *block)
{
    ASSERT(block);
    
    char ch;
    int32_t hit = 0,bracket = 0;
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);
    block->Seek(0);
    while(!block->IsEnd())
    {
        buf.StrCpy(&mem);
        block->ReadWord(&mem);
        if(mem.StrCmp("(") == 0)
        {
            if(CCToken::IsKeyWord(buf.CStr()))
                return FALSE;
            break;
        }
    }
    block->Seek(0);
    block->SkipEmptyChars();
    ch = block->Getc();
    if(ch == '#' || ch == '/')
        return FALSE;
    
    CCToken ct;
    ct.Init();
    ct.i_file_src = block;
    
    block->Seek(0);
    
    int32_t ret;
    while((ret = ct.GetNextTag()))
    {   
        if(ret == CTYPE_PUNC)
        {
            ch = ct.mf_value->CharAt(0);
            if(ch =='(')
            {
                hit = 1;
                bracket++;
            }
            else if(ch == ')')
            {
                bracket --;
            }
            //括号外不能有等号
            else if(ch == '=')
            {
                if(bracket == 0)
                    return FALSE;
            }
            else if(ch == ';')
            {
                return FALSE;
            }
        }
    }
    
    if(hit == 0)
        return FALSE;
    else if(bracket != 0)
        return FALSE;
    return TRUE;
}
status_t CCToken::StripBracket(CFileBase *src, CFileBase *dst)
{
    ASSERT(src && dst);
    char ch;
    int32_t ret,bracket = 0;
    CCToken ct;
    ct.Init();
    ct.i_file_src = src;
    src->Seek(0);
    while((ret=ct.GetNextTag()))
    {
        if(ret == CTYPE_PUNC)
        {
            ch = ct.mf_value->CharAt(0);
            if(ch =='}')
            {
                bracket --;
                if(bracket <= 0)
                    break;
            }
            if(bracket >= 1)
            {
                dst->WriteFile(ct.mf_value);
            }
            if(ch == '{')
            {
                bracket ++;
            }
        }
        else
        {
            dst->WriteFile(ct.mf_value);
        }
    }
    return OK;
}
status_t CCToken::IsKeyWord(const char *str)
{
    ASSERT(str);
    const char *keywords[] = {
        "if","else","while","for","do","return","switch","case","break",
        "struct","typedef","int32_t","float","double","char","unsigned","signed",
        "short","const","static","try","catch"
    };
    int32_t size = sizeof(keywords) / sizeof(char*);
    for(int32_t i = 0; i < size; i++)
    {
        if(strcmp(str,keywords[i]) == 0)
            return TRUE;
    }
    return FALSE;
}
status_t CCToken::IsCommentBlock(CFileBase *file)
{
    ASSERT(file);
    return file->StartWith("/",0,1);
}
status_t CCToken::IsPreProcessBlock(CFileBase *file)
{
    ASSERT(file);
    return file->StartWith("#",0,1);
}
status_t CCToken::ParseFuncBlock(CFileBase *block, CFileBase *name, CFileBase *ret_type, CFileBase *param)
{
    ASSERT(block && name && ret_type && param);
    name->SetSize(0);
    ret_type->SetSize(0);
    param->SetSize(0);
    const char *old_sp_chars = block->sp_chars;
    block->SetSplitChars("[]() ,*\t\r\n");
    block->Seek(0);
    fsize_t pos = block->SearchStr("(",0,0);
    if(pos <= 0)
        return ERROR;
    block->Seek(pos);
    block->ReadString_Reverse(name);
    fsize_t pos2 = block->GetOffset();
    ret_type->WriteFile(block,0,pos2);
    ret_type->Trim();
    char ch;
    int32_t bracket = 0;
    block->Seek(pos);
    while(!block->IsEnd())
    {
        ch = block->Getc();
        if(ch == '(')
        {
            bracket ++;
            if(bracket == 1)
                continue;
        }
        else if(ch == ')')
        {
            bracket --;
            if(bracket == 0)
                break;
        }
        if(bracket > 0)
        {
            param->Putc(ch);
        }
    }
    block->SetSplitChars(old_sp_chars);
    return OK;
}

status_t CCToken::ReadExpression(CFileBase *file,status_t *has_bracket)
{
    ASSERT(file && has_bracket);
    int32_t ret,bs = 0;
    int32_t meet_bracket = 0;

    if(this->i_file_src->IsEnd())
        return ERROR;
    
    file->SetSize(0);
    while((ret=this->GetNextTag()))
    {   
        file->Puts(this->mf_value);
        if(ret == CTYPE_PUNC)
        {
            if(this->mf_value->CharAt(0) == '(')
                bs++;
            if(this->mf_value->CharAt(0) == ')')
                bs--;

            if(bs == 0 && this->mf_value->CharAt(0) == ';')
                break;
            if(bs == 0 && this->mf_value->CharAt(0) == '{')
            {
                meet_bracket = 1;
                break;
            }
        }
        
    }

    if(!meet_bracket)
    {
        *has_bracket = FALSE;
        return OK;
    }

    while((ret=this->GetNextTag()))
    {   
        file->Puts(this->mf_value);
        if(ret == CTYPE_PUNC)   
        {
            if(this->mf_value->CharAt(0) =='{')
                meet_bracket ++;
            else if(this->mf_value->CharAt(0) =='}')
                meet_bracket --;

            if(meet_bracket == 0)
                break;
        }
    }
    *has_bracket = TRUE;
    return OK;
}

status_t CCToken::Split_IF_Expression(CFileBase *source, CFileBase *part1, CFileBase *part2)
{
    ASSERT(source && part1 && part2);

    part1->SetSize(0);
    part2->SetSize(0);

    int32_t ret;
    int32_t bracket = 0;
        
    LOCAL_MEM(mem);

    CCToken ct;
    ct.Init();
    ct.i_file_src = source;
    source->Seek(0);
    while((ret=ct.GetNextTag()))
    {
        part1->Puts(ct.mf_value);
        if(ret == CTYPE_NORMAL)
        {
            if(ct.mf_value->StrCmp("else") == 0)
            {
                fsize_t old_off = source->GetOffset();
                source->ReadWord(&mem);
                if(mem.StrCmp("if") == 0)
                {
                    part1->Puts(" if");
                }
                else
                {
                    source->Seek(old_off);
                    break;
                }
            }
            if(ct.mf_value->StrCmp("do") == 0)
                break;
        }
        if(ret == CTYPE_PUNC)   
        {
            if(ct.mf_value->CharAt(0) =='(')
                bracket ++;
            else if(ct.mf_value->CharAt(0) ==')')
                bracket --;
            if(bracket == 0)
                break;
        }
    }
    source->ReadLeftStr(part2,FALSE);
    return OK;
}

status_t CCToken::ParseParams(CFileBase *line, CMemStk *params)
{
    ASSERT(line && params);
    
    line->Seek(0);
    line->SetSplitChars(" \t,[]()*&;");

    params->Clear();

    LOCAL_MEM(type);
    LOCAL_MEM(name);
    LOCAL_MEM(mem1);
    LOCAL_MEM(mem2);

    line->ReadWord(&mem1);
    while(line->ReadWord(&mem2))
    {
        if(mem2.StrCmp(",") == 0)
        {                       
            params->Push(&type);
            params->Push(&mem1);
            type.SetSize(0);
            mem1.SetSize(0);
        }
        else if(mem2.StrCmp(";") != 0)
        {
            if(mem1.StrLen() > 0)
            {
                if(type.GetSize() > 0)
                    type.Putc(' ');
                type.Puts(&mem1);
                
            }
            mem1.StrCpy(&mem2);
        }       
    }

    if(type.StrLen() > 0)
    {
        params->Push(&type);
        params->Push(&mem1);
    }

    return OK;
}

bool CCToken::IsStaticType(CFileBase *type)
{
    ASSERT(type);
    type->Seek(0);
    return type->SearchStr("static",1,1) >= 0;
}

bool CCToken::IsIgnoreWord(const char *tab[], int tabSize, CFileBase *word)
{
    ASSERT(word);

    for(int32_t i = 0; i < tabSize; i++)
    {
        if(word->StrICmp(tab[i]) == 0)
            return true;
    }

    return false;
}

bool CCToken::IsIgnoreWord(CFileBase *word)
{
    return this->IsIgnoreWord(this->ignore_words,this->ignore_words_size,word);
}

status_t CCToken::SimplifyTypeName(CFileBase *type, CFileBase *out)
{
    ASSERT(type && out);

    out->SetSize(0);
    type->Seek(0);
    type->SetSplitChars(" \t[](),*&");

    LOCAL_MEM(mem);

    while(type->ReadWord(&mem))
    {
        if(!IsIgnoreWord(&mem) && mem.StrICmp("*")!=0)
        {
            if(out->GetSize() > 0)
                out->Putc(' ');
            out->Puts(&mem);
        }
    }
    return OK;
}

status_t CCToken::GetStdType(CFileBase *type, CFileBase *out)
{
    ASSERT(type && out);

    out->SetSize(0);
    type->Seek(0);
    type->SetSplitChars(" \t[](),*&");

    LOCAL_MEM(mem);

    while(type->ReadWord(&mem))
    {
        if(!IsIgnoreWord(&mem))
        {
            out->Puts(&mem);
        }
    }
    return OK;
}

status_t CCToken::GetStdType(const char *type, CFileBase *out)
{
    CMem _type(type);
    return this->GetStdType(&_type,out);
}

status_t CCToken::SetIgnoreTypeC()
{
    this->ignore_words = c_ignore_words;
    this->ignore_words_size = c_ignore_words_size;
    return OK;
}

status_t CCToken::SetIgnoreTypeJava()
{
    this->ignore_words = java_ignore_words;
    this->ignore_words_size = java_ignore_words_size;
    return OK;
}

bool CCToken::IsVoidType(CFileBase *type)
{
    ASSERT(type);
    type->Seek(0);
    return type->SearchStr("void",1,1) >=0;
}

status_t CCToken::SetIgnoreWords(const char **words, int32_t size)
{
    this->ignore_words = words;
    this->ignore_words_size = size;
    return OK;
}

bool CCToken::IsArrayType(CFileBase *type)
{
    ASSERT(type);
    type->Seek(0);
    return type->SearchStr("[",0,0) >=0;
}
bool CCToken::IsVoidRetType(CFileBase *ret)
{
    ASSERT(ret);
    ret->Seek(0);
    return ret->SearchStr("void",1,1) >=0;
}
