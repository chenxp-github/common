#ifndef __SQLITE3DATABASE_H
#define __SQLITE3DATABASE_H

#include "sqlite3.h"
#include "sqlite3stmt.h"

class CSQLite3DataBase{
public:
	sqlite3 *mDB;
public:
	bool IsTableExist(const char *table_name);
	status_t Prepare(const char *sqlCmd, CSQLite3Stmt *outStmt);
	status_t RunSqlCommand(const char *sqlCmd);
	status_t ReportError();
	status_t OpenDataBase(const char *dbFile);
	CSQLite3DataBase();
	virtual ~CSQLite3DataBase();
	status_t Init();
	status_t Destroy();
	status_t InitBasic();
};

#endif
