#ifndef __MAT_FILE_H
#define __MAT_FILE_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "mat.h"
#include "mx_array.h"

/*##Begin namespace_begin##*/
namespace matlab {
/*##End namespace_begin##*/

class CMatFile
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

/*##End Members##*/

    MATFile *m_mat_file;
public:
    CMatFile();
    virtual ~CMatFile();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CMatFile *_p);
    int Comp(CMatFile *_p);
    status_t Print(CFileBase *_buf);

    status_t Open(const char *filename , const char *mode);
    status_t Close();
    MATFile* GetMatFile();
/*##Begin Getter_H##*/
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/

    status_t PutVariable(const char *name, CMxArray *var);
    status_t PutVariableAsGlobal(const char *name, CMxArray *var);
    status_t GetVariable(const char *name, CMxArray *out);
};


/*##Begin namespace_end##*/
}  //name space matlab
/*##End namespace_end##*/

#endif
