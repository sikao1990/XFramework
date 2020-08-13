#ifndef __SqliteAccess_H__
#define __SqliteAccess_H__

#include "../XDefine.h"
#include "DBAccess.h"

class FRAMEWORK_API SqliteAccess : public DBAccess
{
public:
	SqliteAccess();
	~SqliteAccess();
public:
	virtual bool InitAccessParam(void* p);

	virtual int Execute(const char* sql);
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);

	virtual int Query(const char* sql, list< list<DBField> >& result);
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...);

private:
	DECLARENODE(SqliteAccess);
};

#endif