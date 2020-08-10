#include "jsonreader.h"
#include "encoder.h"
#include "misc.h"

#define JSON_SPLIT_CHARS ",:{}[] \r\n\t"

#define VALUE_TYPE_UNKNOWN  0
#define VALUE_TYPE_NUMBER   1
#define VALUE_TYPE_BOOL     2
#define VALUE_TYPE_STRING   3
#define VALUE_TYPE_OBJECT   4
#define VALUE_TYPE_NULL     5
#define VALUE_TYPE_ARRAY    6

CJsonReader::CJsonReader()
{
    this->InitBasic();
}
CJsonReader::~CJsonReader()
{
    this->Destroy();
}
status_t CJsonReader::InitBasic()
{
    this->iSrcFile = NULL;
    this->mNameStack = NULL;
    this->mValue = NULL;
    return OK;
}
status_t CJsonReader::Init()
{
    this->InitBasic();
    
    NEW(this->mNameStack,CMemStk);
    this->mNameStack->Init(1024);
    
    NEW(this->mValue,CMem);
    this->mValue->Init();
    this->mValue->Malloc(512*1024);

    return OK;
}
status_t CJsonReader::Destroy()
{
    DEL(this->mValue);
    DEL(this->mNameStack);
    this->InitBasic();
    return OK;
}
status_t CJsonReader::Skip(const char *str)
{
    CMem mem_buf;
    LOCAL_MEM(mem_buf);

    this->iSrcFile->ReadWord(&mem_buf);
    if(strcmp(mem_buf.p,str) == 0)
        return OK;
    LOG("syntax error£¬%s is expected.\n",str);
    return ERROR;
}
status_t CJsonReader::GetCurPath(CFileBase *file)
{
    return this->mNameStack->GetPath(file);
}
status_t CJsonReader::SetSrcFile(CFileBase *iFile)
{
    ASSERT(iFile);
    iFile->SetSplitChars(JSON_SPLIT_CHARS);
    iFile->Seek(0);
    this->iSrcFile = iFile;
    return OK;
}
char CJsonReader::Getc_NotWhite(CFileBase *file)
{
    ASSERT(file);   
    char ch;    

    while(!file->IsEnd())
    {
        ch = file->Getc();
        if(!CFileBase::IsEmptyChar(ch))
            return ch;
    }
    return 0;
}

#define RET(code) do{ret_code = code;goto ok;}while(0)

status_t CJsonReader::Parse()
{
    ASSERT(this->iSrcFile);

    fsize_t old_off;
    char ch,ch1;

    status_t ret_code = JSON_TYPE_ERROR;
    this->mValue->SetSize(0);

    while(!this->iSrcFile->IsEnd())
    {
        ch = this->Getc_NotWhite(this->iSrcFile);
        if(ch == '{' || ch == '[')
        {
            char tmp[2];
            tmp[0] = ch;
            tmp[1] = 0;
            this->mNameStack->Push(tmp);
            RET(JSON_TYPE_BRACE_ENTER);
        }
        else if(ch == '}' || ch == ']')
        {
            if(this->IsTopBrace())
            {
                this->mNameStack->DelTop();
                RET(JSON_TYPE_BRACE_LEAVE);
            }
            else
            {
                ASSERT(!this->mNameStack->IsEmpty());
                this->iSrcFile->UnGetc();
                RET(JSON_TYPE_VALUE);
            }
        }
        else if(ch ==',')
        {
            if(!this->IsTopBrace())
            {
                RET(JSON_TYPE_VALUE);
            }
        }

        else if(ch == '"')
        {
            this->iSrcFile->ReadCStr(this->mValue);
            old_off = this->iSrcFile->GetOffset();
            ch1 = this->Getc_NotWhite(this->iSrcFile);
            if(ch1 == ':')
            {
                this->mNameStack->Push(this->mValue);
                RET(JSON_TYPE_KEY);
            }
            else if(ch1 == ',')
            {
                RET(JSON_TYPE_VALUE);
            }
            else 
            {
                this->iSrcFile->Seek(old_off);
                RET(JSON_TYPE_VALUE);
            }
        }
        else if(ch != 0)
        {
            this->iSrcFile->UnGetc();
            this->iSrcFile->ReadString(this->mValue);
            old_off = this->iSrcFile->GetOffset();
            ch1 = this->Getc_NotWhite(this->iSrcFile);
            if(ch1 == ':')
            {
                this->mNameStack->Push(this->mValue);
                RET(JSON_TYPE_KEY);
            }
            else if(ch1 == ',' )
            {
                RET(JSON_TYPE_VALUE);
            }
            else if(ch1 == ']' || ch1 == '}')
            {
                this->iSrcFile->Seek(old_off);
                RET(JSON_TYPE_VALUE);
            }
            else
            {
                goto syntax_error;
            }
        }
    }
    return ret_code;

syntax_error:
    ret_code = JSON_TYPE_ERROR;
    LOG("syntax error, parse stopped at offset 0x%x.",this->iSrcFile->GetOffset());
ok:
    return ret_code;
}

CMem *CJsonReader::GetTopName()
{
    if(this->IsTopBrace())
        return NULL;
    return this->mNameStack->GetTopPtr();   
}

status_t CJsonReader::PopName()
{
    if(this->IsTopBrace())
        return ERROR;
    return this->mNameStack->DelTop();
}
bool CJsonReader::IsTopBrace()
{
    CMem *ptop = this->mNameStack->GetTopPtr();
    if(ptop == NULL)return false;
    return ptop->p[0] == '{' || ptop->p[0] == '[';
}

bool CJsonReader::IsPreTopBrace()
{
    int len = this->mNameStack->GetLen();   
    CMem *ptop = this->mNameStack->GetElem(len-2);
    if(ptop == NULL)return false;
    return ptop->p[0] == '{' || ptop->p[0] == '[';
}
CMem * CJsonReader::GetValue()
{
    return this->mValue;
}

status_t CJsonReader::JsonToXml(CFileBase *json,CFileBase *xml)
{
    ASSERT(xml && json);
    
    xml->SetSize(0);
    xml->Puts("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
    xml->Puts("<JSON>\r\n");
    
    CJsonReader js;
    js.Init();
    js.SetSrcFile(json);

    status_t ret;
    while(ret=js.Parse())
    {
        if(ret == JSON_TYPE_KEY)
        {
            CMem *top = js.GetTopName();
            ASSERT(top);
            xml->Printf("<%s type=\"%s\">",top->p,js.ValueTypeToString(js.GetNextValueType(js.iSrcFile)));
        }

        if(ret == JSON_TYPE_VALUE)
        {           
            CMem *top = js.GetTopName();
            if(top == NULL)
            {
                CMem *value = js.GetValue();
                ASSERT(value);
                value->Seek(0);
                xml->Printf("<e type=\"%s\">",js.ValueTypeToString(js.GetValueType(value)));
                CEncoder::EscapeXmlStr(value);
                xml->Puts(value);
                xml->Printf("</e>\r\n");
            }
            else
            {           
                CEncoder::EscapeXmlStr(js.GetValue());
                xml->Puts(js.GetValue());
                xml->Printf("</%s>\r\n",top->p);
            }
            js.PopName();   
        }

        if(ret == JSON_TYPE_BRACE_ENTER)
        {
            if(js.IsPreTopBrace())
                xml->Printf("<e type=\"object\">");
        }

        if(ret == JSON_TYPE_BRACE_LEAVE)
        {
            if(js.IsTopBrace())
                xml->Printf("</e>");
        }

    }
    xml->Printf("</JSON>\r\n");
    return OK;
}


int CJsonReader::GetNextValueType(CFileBase *file)
{
    ASSERT(file);
    fsize_t oldOff = file->GetOffset();
    
    int type = VALUE_TYPE_UNKNOWN;
    char ch = this->Getc_NotWhite(file);
    if(ch == '{')
    {
        type = VALUE_TYPE_OBJECT;
    }
    else if(ch == '[')
    {
        type = VALUE_TYPE_ARRAY;
    }
    else if(ch == '\"')
    {
        type = VALUE_TYPE_STRING;
    }
    else
    {
        CMem mem;
        LOCAL_MEM(mem);
        file->Seek(oldOff);
        file->ReadString(&mem);
        if(mem.StrICmp("true") == 0 || mem.StrICmp("false") == 0)
        {
            type = VALUE_TYPE_BOOL;
        }
        else if(mem.StrICmp("null") == 0)
        {
            type = VALUE_TYPE_NULL;
        }
        else if(CMisc::is_float(mem.p))
        {
            type = VALUE_TYPE_NUMBER;
        }
    }
    file->Seek(oldOff);
    return type;
}

const char *CJsonReader::ValueTypeToString(int valueType)
{
    if(valueType == VALUE_TYPE_STRING)
        return "string";
    if(valueType == VALUE_TYPE_BOOL)
        return "boolean";
    if(valueType == VALUE_TYPE_NUMBER)
        return "number";
    if(valueType == VALUE_TYPE_NULL)
        return "null";
    if(valueType == VALUE_TYPE_OBJECT)
        return "object";
    if(valueType == VALUE_TYPE_ARRAY)
        return "array";

    return "unknown";
}

int CJsonReader::GetValueType(CFileBase *value)
{
    value->Seek(0);
    value->SetSplitChars(JSON_SPLIT_CHARS);
    int type = this->GetNextValueType(value);   
    if(type == VALUE_TYPE_UNKNOWN)
    {
        if(value->StrLen() > 0)
            type = VALUE_TYPE_STRING;
    }
    return type;
}
