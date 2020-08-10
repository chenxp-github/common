#include "sqlite3stmt.h"
#include "syslog.h"
#include "encoder.h"

CSQLite3Stmt::CSQLite3Stmt()
{
    this->InitBasic();
}
CSQLite3Stmt::~CSQLite3Stmt()
{
    this->Destroy();
}
status_t CSQLite3Stmt::InitBasic()
{
    this->mStmt = NULL;
    return OK;
}
status_t CSQLite3Stmt::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CSQLite3Stmt::Destroy()
{
    this->Finish();
    this->InitBasic();
    return OK;
}
status_t CSQLite3Stmt::Copy(CSQLite3Stmt *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
status_t CSQLite3Stmt::Comp(CSQLite3Stmt *p)
{
    return 0;
}
status_t CSQLite3Stmt::Print()
{
    //add your code
    return TRUE;
}

status_t CSQLite3Stmt::Finish()
{
    if(this->mStmt)
    {
        sqlite3_finalize(this->mStmt);
        this->mStmt = NULL;
    }

    return OK;
}

status_t CSQLite3Stmt::Next()
{
    int ret = sqlite3_step(this->mStmt);
    if(ret == SQLITE_ROW)return OK;
    return ERROR;
}

status_t CSQLite3Stmt::Step()
{
    return sqlite3_step(this->mStmt) == SQLITE_DONE;
}

status_t CSQLite3Stmt::BindText(int index, CMem *text)
{
    ASSERT(text);
    return sqlite3_bind_text(this->mStmt,index,text->CStr(),text->StrLen(),NULL) == SQLITE_OK;
}

status_t CSQLite3Stmt::BindText(int index, const char  *text, int encoding)
{
    CMem mem(text);
    return this->BindText(index,&mem,encoding);
}

status_t CSQLite3Stmt::BindText(int index,CMem *text, int encoding)
{
    ASSERT(text);
    LOCAL_MEM(tmp);
    CEncoder::EncodingConvert(encoding,ENCODING_UTF8,text,&tmp);
    return sqlite3_bind_text(this->mStmt,index,tmp.CStr(),tmp.StrLen(),NULL) == SQLITE_OK;
}

status_t CSQLite3Stmt::BindText(int index, const char *text)
{
    ASSERT(text);
    return sqlite3_bind_text(this->mStmt,index,text,-1,NULL) == SQLITE_OK;
}

status_t CSQLite3Stmt::BindInt(int index, int val)
{
    return sqlite3_bind_int(this->mStmt, index, val) == SQLITE_OK;
}   

status_t CSQLite3Stmt::BindDouble(int index, double val)
{
    return sqlite3_bind_double(this->mStmt, index, val) == SQLITE_OK;
}

status_t CSQLite3Stmt::BindBlob(int index, const void *blob, int size)
{
    ASSERT(blob);
    return sqlite3_bind_blob(this->mStmt,index,blob,size,NULL) == SQLITE_OK;
}

status_t CSQLite3Stmt::BindBlob(int index, CMem *blob)
{
    ASSERT(blob);
    return this->BindBlob(index,(const void*)blob->GetRawBuf(),(int)blob->GetSize());
}

status_t CSQLite3Stmt::ColumnText(int col, CMem *out)
{
    ASSERT(out);
    out->SetSize(0);
    const unsigned char *buf;
    buf = sqlite3_column_text(this->mStmt,col);
    ASSERT(buf);
    int size = this->ColumnBytes(col);
    ASSERT(size <= out->GetMaxSize());
    out->Write(buf,size);
    return OK;
}

int CSQLite3Stmt::ColumnInt(int col)
{
    return sqlite3_column_int(this->mStmt,col);
}

double CSQLite3Stmt::ColumnDouble(int col)
{
    return sqlite3_column_double(this->mStmt,col);
}

int CSQLite3Stmt::ColumnBytes(int col)
{
    return sqlite3_column_bytes(this->mStmt,col);
}

status_t CSQLite3Stmt::ColumnBlob(int col, CMem *out)
{
    ASSERT(out);
    out->SetSize(0);
    int size = this->ColumnBytes(col);
    ASSERT(size > 0 && size <= out->GetMaxSize());
    const void *buf;
    buf = sqlite3_column_blob(this->mStmt,col);
    ASSERT(buf);
    out->Write(buf,size);
    return OK;
}

status_t CSQLite3Stmt::Reset()
{
    return sqlite3_reset(this->mStmt) == SQLITE_OK;
}
