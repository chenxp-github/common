#include "mx_array.h"
#include "syslog.h"
#include "mem_tool.h"

/*##Begin namespace_begin##*/
namespace matlab {
/*##End namespace_begin##*/

CMxArray::CMxArray()
{
    this->InitBasic();
}

CMxArray::~CMxArray()
{
    this->Destroy();
}

status_t CMxArray::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
/*##End InitBasic##*/
    m_flags = 0;
    m_mx_array = NULL;
    return OK;
}

status_t CMxArray::Init()
{
/*##Begin Init##*/
    this->InitBasic();
/*##End Init##*/
    return OK;
}

status_t CMxArray::Destroy()
{
    this->Free();
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    this->InitBasic();
/*##End Destroy##*/
    return OK;
}

status_t CMxArray::Copy(CMxArray *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    ASSERT(0);
/*##Begin Copy##*/
/*##End Copy##*/
    return OK;
}

int CMxArray::Comp(CMxArray *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CMxArray::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
/*##End Print##*/
    return OK;
}

/*@@ Insert Function Here @@*/

status_t CMxArray::Free()
{
    if(!IsAttached() && m_mx_array)
    {
        mxDestroyArray(m_mx_array);        
        m_mx_array = NULL;
    }
    this->SetIsAttached(false);
    return OK;
}
status_t CMxArray::Set(mxArray *arr)
{
    ASSERT(arr);    
    this->Free();
    m_mx_array = arr;
    return OK;
}

status_t CMxArray::Attach(mxArray *arr)
{
    ASSERT(arr);
    this->Free();
    this->SetIsAttached(true);
    m_mx_array = arr;
    return OK;
}

status_t CMxArray::CreateDoubleMatrix(int m, int n, mxComplexity complexity)
{
    this->Free();
    m_mx_array = mxCreateDoubleMatrix(m,n,complexity);
    return m_mx_array!=NULL;
}

status_t CMxArray::CreateString(const char *str)
{
    ASSERT(str);
    this->Free();
    m_mx_array = mxCreateString(str);
    return m_mx_array != NULL;
}

mxArray *CMxArray::GetMxArray()
{
    return m_mx_array;
}

double *CMxArray::GetPr()
{
    ASSERT(m_mx_array);
    return mxGetPr(m_mx_array);
}

status_t CMxArray::GetString(CMem *out)
{
    ASSERT(out);    
    ASSERT(!out->IsReadOnly());
    ASSERT(m_mx_array);
    int ret = mxGetString(m_mx_array, out->GetRawBuf(), out->GetMaxSize());
    if(ret != 0)return ERROR;
    int len = strlen(out->GetRawBuf());
    out->SetSize(len);
    return OK;
}

int CMxArray::GetNumberOfDimensions()
{
    ASSERT(m_mx_array);
    return mxGetNumberOfDimensions(m_mx_array);
}

const mwSize *CMxArray::GetDimensions()
{
    ASSERT(m_mx_array);
    return mxGetDimensions(m_mx_array);
}
int CMxArray::GetNumberOfElements()
{
    ASSERT(m_mx_array);
    return mxGetNumberOfElements(m_mx_array);
}

status_t CMxArray::SetField(int i, const char *fieldname, CMxArray *value)
{
    ASSERT(fieldname && value);
    ASSERT(m_mx_array);
    value->SetIsAttached(true); //giveup ownership
    mxSetField(m_mx_array, i, fieldname,value->GetMxArray());
    return OK;
}

status_t CMxArray::CreateStructMatrix(int m, int n, int nfields, const char **fieldnames)
{
    ASSERT(fieldnames);
    this->Free();
    m_mx_array = mxCreateStructMatrix(m,n,nfields,fieldnames);
    return m_mx_array != NULL;
}

status_t CMxArray::CreateDoubleScalar(double v)
{
    this->Free();
    m_mx_array = mxCreateDoubleScalar(v);
    return m_mx_array != NULL;
}

status_t CMxArray::SetFieldByNumber(int index, int field, CMxArray *value)
{
    ASSERT(value);
    ASSERT(m_mx_array);
    mxSetFieldByNumber(m_mx_array,index,field,value->GetMxArray());
    return OK;
}

/*##Begin namespace_end##*/
}  //name space matlab
/*##End namespace_end##*/

