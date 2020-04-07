/*----------------------------------------------------------------
// �ļ�����MysqlAccess.h
// �ļ�����������Mysql���ݿ���ʽӿ�
// ������:zhuliwu
// ʱ�䣺2018/10/8 9:10:00
// �汾��V1.0.0
//----------------------------------------------------------------*/

#ifndef __MysqlAccess_H__
#define __MysqlAccess_H__

#include "DBAccess.h"

class MysqlAccess : public DBAccess
{
	struct mysqlFieldinfo
	{
		string			name;
		unsigned long	type;
		long long 		len;
		mysqlFieldinfo(const char* name="",unsigned long type=-1,long long len=0):name(name),type(type),len(len) { }
	};
public:
	MysqlAccess();
	~MysqlAccess();
public:
	virtual bool InitAccessParam(void* p);

	virtual int Execute(const char* sql);
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);

	virtual int Query(const char* sql, list< list<DBField> >& result);
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, list< list<DBField> >& result, ...);
private:
	DECLARENODE(MysqlAccess);
};

#endif //MysqlAccess.h
