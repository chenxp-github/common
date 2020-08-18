#include "mat_file.h"
#include "syslog.h"
#include "mem_tool.h"

/*##Begin namespace_begin##*/
namespace matlab {
/*##End namespace_begin##*/

CMatFile::CMatFile()
{
    this->InitBasic();
}

CMatFile::~CMatFile()
{
    this->Destroy();
}

status_t CMatFile::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
/*##End InitBasic##*/
    m_mat_file = NULL;
    return OK;
}

status_t CMatFile::Init()
{
/*##Begin Init##*/
    this->InitBasic();
/*##End Init##*/
    return OK;
}

status_t CMatFile::Destroy()
{
    this->Close();
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    this->InitBasic();
/*##End Destroy##*/
    return OK;
}

status_t CMatFile::Copy(CMatFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
/*##End Copy##*/
    return OK;
}

int CMatFile::Comp(CMatFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CMatFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
/*##End Print##*/
    return OK;
}

/*@@ Insert Function Here @@*/

status_t CMatFile::Open(const char *filename , const char *mode)
{
    ASSERT(filename && mode);
    ASSERT(m_mat_file == NULL);

    m_mat_file = matOpen(filename, mode);
    if(!m_mat_file)
    {
        return ERROR;
    }
    return OK;
}

status_t CMatFile::Close()
{
    if(m_mat_file)
    {
        matClose(m_mat_file);
        m_mat_file = NULL;
    }
    return OK;
}

MATFile* CMatFile::GetMatFile()
{
    return m_mat_file;
}

status_t CMatFile::PutVariable(const char *name, CMxArray *var)
{
    ASSERT(name && var);
    ASSERT(m_mat_file);
    return matPutVariable(m_mat_file,name,var->GetMxArray()) == 0;
}

status_t CMatFile::PutVariableAsGlobal(const char *name, CMxArray *var)
{
    ASSERT(name && var);
    ASSERT(m_mat_file);
    return matPutVariableAsGlobal(m_mat_file,name,var->GetMxArray()) == 0;
}

status_t CMatFile::GetVariable(const char *name, CMxArray *out)
{
    ASSERT(name && out);
    ASSERT(m_mat_file);

    mxArray *arr = matGetVariable(m_mat_file,name);
    if(arr)
    {
        out->Set(arr);
        return OK;
    }

    return ERROR;
}

/*##Begin namespace_end##*/
}  //name space matlab
/*##End namespace_end##*/

