#include "sqlite3database.h"
#include "syslog.h"

#if HAVE_WINDOWS_H
#pragma comment(lib, "sqlite3.lib")
#endif


CSQLite3DataBase::CSQLite3DataBase()
{
	this->InitBasic();
}
CSQLite3DataBase::~CSQLite3DataBase()
{
	this->Destroy();
}
status_t CSQLite3DataBase::InitBasic()
{
	this->mDB = NULL;
	return OK;
}
status_t CSQLite3DataBase::Init()
{
	this->InitBasic();
	//add your code
	return OK;
}
status_t CSQLite3DataBase::Destroy()
{
	if(this->mDB)
	{
		sqlite3_close(this->mDB);
		this->mDB = NULL;
	}
	this->InitBasic();
	return OK;
}
status_t CSQLite3DataBase::OpenDataBase(const char *dbFile)
{
	ASSERT(dbFile);
	ASSERT(this->mDB == NULL);

	if(sqlite3_open(dbFile, &mDB) != SQLITE_OK)
	{
		this->ReportError();
		return ERROR;
	}

	return OK;
}

status_t CSQLite3DataBase::ReportError()
{
	XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
		"sqlite3 error:%s\n",sqlite3_errmsg(mDB));
	return OK;
}

status_t CSQLite3DataBase::RunSqlCommand(const char *sqlCmd)
{
	ASSERT(sqlCmd);
	char* errorMsg = NULL;
	int ret = sqlite3_exec(mDB, sqlCmd, NULL, NULL, &errorMsg);

	if(ret != SQLITE_OK)
	{
		this->ReportError();
		sqlite3_free(&errorMsg);
		return ERROR;
	}

	return OK;
}

status_t CSQLite3DataBase::Prepare(const char *sqlCmd, CSQLite3Stmt *outStmt)
{
	ASSERT(sqlCmd && outStmt);
	outStmt->Finish();
	int ret = sqlite3_prepare_v2(this->mDB, sqlCmd, -1, &outStmt->mStmt, NULL);
	if (ret != SQLITE_OK) 
	{
		this->ReportError();
		return ERROR;
	}
	return OK;
}
