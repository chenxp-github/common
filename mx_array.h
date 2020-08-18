#ifndef __MX_ARRAY_H
#define __MX_ARRAY_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "mat.h"

/*##Begin namespace_begin##*/
namespace matlab {
/*##End namespace_begin##*/

class CMxArray
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

/*##End Members##*/
    uint32_t m_flags;
    mxArray *m_mx_array;
public:
    CMxArray();
    virtual ~CMxArray();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CMxArray *_p);
    int Comp(CMxArray *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/

    status_t Set(mxArray *arr);
    status_t Attach(mxArray *arr);
    status_t Free();

    status_t CreateDoubleMatrix(int m, int n, mxComplexity complexity=mxREAL);
    status_t CreateStructMatrix(int m, int n, int nfields, const char **fieldnames);
    status_t CreateString(const char *str);
    status_t CreateDoubleScalar(double v);
    
    mxArray *GetMxArray();
    double *GetPr();
    status_t GetString(CMem *out);
    int GetNumberOfDimensions();
    int GetNumberOfElements();
    const mwSize *GetDimensions();

    status_t SetField(int i, const char *fieldname, CMxArray *value);
    status_t SetFieldByNumber(int index, int field, CMxArray *value);

    FLAG_FUNC(m_flags,IsAttached,0x00000001);
};


/*##Begin namespace_end##*/
}  //name space matlab
/*##End namespace_end##*/

#endif
