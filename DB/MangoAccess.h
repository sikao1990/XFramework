#ifndef __MangoAccess_H__
#define __MangoAccess_H__

#include "../XDefine.h"
#include "DBAccess.h"

class FRAMEWORK_API MangoAccess : public DBAccess
{
public:
	MangoAccess();
	~MangoAccess();
public:
	virtual bool InitAccessParam(void* p);

	virtual int Execute(const char* sql);
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);

	virtual int Query(const char* sql, list< list<DBField> >& result);
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...);

private:
	DECLARENODE(MangoAccess);
};

#endif