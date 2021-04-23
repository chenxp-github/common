#ifndef __SQLITE3STMT_H
#define __SQLITE3STMT_H

#include "cruntime.h"
#include "sqlite3.h"
#include "mem.h"
#include "encoder.h"

class CSQLite3Stmt{
public:
    sqlite3_stmt *mStmt;
public:
    status_t Reset();
    status_t ColumnBlob(int col, CMem *out);
    int ColumnBytes(int col);
    double ColumnDouble(int col);
    int ColumnInt(int col);
    int64_t ColumnInt64(int col);
    status_t ColumnText(int col,CMem *out);
    status_t BindBlob(int index, const void *blob, int size);
    status_t BindBlob(int index, CMem *blob);
    status_t BindDouble(int index, double val);
    status_t BindInt(int index, int val);
    status_t BindInt64(int index, int64_t val);
    status_t BindText(int index, const char *text);
    status_t BindText(int index, CMem *text);
    status_t BindText(int index, const char  *text, int encoding);
    status_t BindText(int index,CMem *text, int encoding);
    status_t Step();
    status_t Next();
    status_t Finish();
    CSQLite3Stmt();
    virtual ~CSQLite3Stmt();
    status_t Init();
    status_t Destroy();
    status_t Copy(CSQLite3Stmt *p);
    status_t Comp(CSQLite3Stmt *p);
    status_t Print();
    status_t InitBasic();
};

#endif
