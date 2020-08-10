#include "ftppartfileinfo.h"
CFtpPartFileInfo::CFtpPartFileInfo()
{
    this->InitBasic();
}
CFtpPartFileInfo::~CFtpPartFileInfo()
{
    this->Destroy();
}
status_t CFtpPartFileInfo::InitBasic()
{
    this->size = 0;
    this->md5 = NULL;
    this->version = 0.0f;
    this->fileName = NULL;
    this->totalSize = 0;
    return OK;
}
status_t CFtpPartFileInfo::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CFtpPartFileInfo::Destroy()
{
    FREE(this->md5);
    FREE(this->fileName);
    this->InitBasic();
    return OK;
}
status_t CFtpPartFileInfo::Copy(CFtpPartFileInfo *p)
{
    if(this == p)return OK;
    this->size = p->size;
    this->SetMd5(p->GetMd5());
    this->version = p->version;
    this->SetFileName(p->GetFileName());
    this->totalSize = p->totalSize;
    return OK;
}
status_t CFtpPartFileInfo::Comp(CFtpPartFileInfo *p)
{
    return 0;
}
status_t CFtpPartFileInfo::Print()
{
    char str[128];
    crt_fsize_to_str(size,str);
    LOG("size=%s\r\n",str);
    LOG("md5=%s\r\n",this->md5);
    LOG("version=%f\r\n",this->version);
    LOG("fileName=%s\r\n",this->fileName);
    crt_fsize_to_str(this->totalSize,str);
    LOG("totalSize=%s\r\n",this->totalSize);
    return TRUE;
}
status_t CFtpPartFileInfo::SetMd5(const char *md5)
{
    ASSERT(md5);
    FREE(this->md5);
    MALLOC(this->md5,char,strlen(md5)+1);
    strcpy(this->md5,md5);
    return OK;
}
const char *CFtpPartFileInfo::GetMd5()
{
    return (const char*)this->md5;
}
status_t CFtpPartFileInfo::SetFileName(const char *fileName)
{
    ASSERT(fileName);
    FREE(this->fileName);
    MALLOC(this->fileName,char,strlen(fileName)+1);
    strcpy(this->fileName,fileName);
    return OK;
}
const char *CFtpPartFileInfo::GetFileName()
{
    return (const char*)this->fileName;
}
status_t CFtpPartFileInfo::SaveToXml(CFileBase *xml)
{
    ASSERT(xml);

    char str[128];
    crt_fsize_to_str(this->size,str);
    xml->Puts("<size>");
    xml->Printf("%s",str);
    xml->Puts("</size>\r\n");
    xml->Puts("<md5>");
    ASSERT(this->md5);
    xml->Puts(this->md5);
    xml->Puts("</md5>\r\n");
    xml->Puts("<version>");
    xml->Printf("%f",this->version);
    xml->Puts("</version>\r\n");
    xml->Puts("<fileName>");
    ASSERT(this->fileName);
    xml->Puts(this->fileName);
    xml->Puts("</fileName>\r\n");
    xml->Puts("<totalSize>");
    crt_fsize_to_str(this->totalSize,str);
    xml->Printf("%s",str);
    xml->Puts("</totalSize>\r\n");
    return OK;
}
status_t CFtpPartFileInfo::LoadFromXml(CXmlNode *root)
{
    ASSERT(root);
    CXmlNode *px;
    LOCAL_MEM(mem);

    px = root->GetChildByName("size");
    ASSERT(px);
    px->GetStringValue(&mem);
    this->size = crt_str_to_fsize(mem.CStr());
    px = root->GetChildByName("md5");
    ASSERT(px);
    px->GetStringValue(&mem);
    this->SetMd5(mem.CStr());
    px = root->GetChildByName("version");
    ASSERT(px);
    px->GetStringValue(&mem);
    this->version = atof(mem.CStr());
    px = root->GetChildByName("fileName");
    ASSERT(px);
    px->GetStringValue(&mem);
    this->SetFileName(mem.CStr());
    px = root->GetChildByName("totalSize");
    ASSERT(px);
    px->GetStringValue(&mem);
    this->totalSize = crt_str_to_fsize(mem.CStr());
    return OK;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int32_t CFtpPartFileInfo::CompVersion(CFtpPartFileInfo *info)
{
    ASSERT(info);
    if(this->version < info->version)
        return -1;
    if(this->version > info->version)
        return 1;
    return 0;
}

status_t CFtpPartFileInfo::Clear()
{
    this->totalSize = 0;
    this->size = 0;
    this->version = 0.0f;   
    this->SetMd5("");
    this->SetFileName("");
    return OK;
}
bool CFtpPartFileInfo::HasMd5()
{
    if(this->md5 == NULL)
        return false;
    return strlen(this->md5) > 0;
}

bool CFtpPartFileInfo::IsComplete()
{
    return this->size == this->totalSize;
}
