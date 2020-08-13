#ifndef __OracleAccess_H__
#define __OracleAccess_H__

#include "../XDefine.h"
#include "DBAccess.h"

class FRAMEWORK_API OracleAccess : public DBAccess
{
public:
	OracleAccess();
	~OracleAccess();
public:
	virtual bool InitAccessParam(void* p);

	virtual int Execute(const char* sql);
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);

	virtual int Query(const char* sql, list< list<DBField> >& result);
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...);

private:
	DECLARENODE(OracleAccess);
};

#endif