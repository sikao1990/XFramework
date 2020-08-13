#ifndef __ODBCAccess_H__
#define __ODBCAccess_H__

#include "../XDefine.h"
#include "DBAccess.h"

class FRAMEWORK_API ODBCAccess : public DBAccess
{
public:
	ODBCAccess();
	~ODBCAccess();
public:
	virtual bool InitAccessParam(void* p);

	virtual int Execute(const char* sql);
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);

	virtual int Query(const char* sql, list< list<DBField> >& result);
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...);

private:
	DECLARENODE(ODBCAccess);
};

#endif