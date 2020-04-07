/*----------------------------------------------------------------
// �ļ�����AdoAccess.h
// �ļ�����������Ado�������ݿ�ӿ�
// ������:zhuliwu
// ʱ�䣺2018/10/8 9:10:00
// �汾��V1.0.0
//----------------------------------------------------------------*/


#ifndef __AdoAccess_H__
#define __AdoAccess_H__

#include "DBAccess.h"

class AdoAccess : public DBAccess
{
public:
	AdoAccess();
	~AdoAccess();
public:
	virtual bool InitAccessParam(void* p);

	virtual int Execute(const char* sql);
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);

	virtual int Query(const char* sql, list< list<DBField> >& result);
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, list< list<DBField> >& result, ...);

private:
	DECLARENODE(AdoAccess);
};

#endif