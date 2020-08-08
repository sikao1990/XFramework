/*----------------------------------------------------------------
// 文件名：AdoAccess.h
// 文件功能描述：Ado访问数据库接口
// 创建者:zhuliwu
// 时间：2018/10/8 9:10:00
// 版本：V1.0.0
//----------------------------------------------------------------*/


#ifndef __AdoAccess_H__
#define __AdoAccess_H__

#include "../XDefine.h"
#include "DBAccess.h"

class FRAMEWORK_API AdoAccess : public DBAccess
{
public:
	AdoAccess();
	~AdoAccess();
public:
	virtual bool InitAccessParam(void* p);

	virtual int Execute(const char* sql);
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);

	virtual int Query(const char* sql, list< list<DBField> >& result);
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...);

private:
	DECLARENODE(AdoAccess);
};

#endif