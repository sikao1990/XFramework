#ifndef	__DBAccess_H__
#define __DBAccess_H__
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <stdarg.h>
#include "../XDefine.h"
#include "../Pattern/TreeRelation.h"
#include "DBDefine.h"

using namespace std;

class DBConnect;
class FRAMEWORK_API DBAccess
{
	friend class DBConnect;
protected:
	struct ArgType {
		const char* pStart;
		int			nFlag;// ? or %
		DBField		val;
		ArgType(const char* p=NULL,int nflag=-1):pStart(p),nFlag(nflag){}
		bool operator<(const ArgType& th)const {
			return this->pStart < th.pStart;
		}
	};
	struct TmpInfo {
		const char* pStart;
		int nIndex;
		TmpInfo():pStart(NULL),nIndex(-1){}
	};
public:
	DBAccess(){}
	virtual ~DBAccess() { m_connect = NULL; }
	//insert update delete not return
	//return: 0、success; 1、failed; -1、表示该数据库连接已失效，需要重新获取
	//非参数化接口
	virtual int Execute(const char* sql);//该接口不适合操作二进制数据,inert,update中操作二进制数据
	//参数化sql接口,在使用时尽量%不要出现在sql中
	virtual int Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...);
	
	//select 
	//return: 0 not result、n the row of result
	//非参数化sql接口
	virtual int Query(const char* sql, list< list<DBField> >& result);
	//参数化sql接口,在使用时尽量%不要出现在sql中
	virtual int Query(const char* sql, const vector<FieldInfo>& argTypeList,list< list<DBField> >& result, TagType type, ...);
protected:
	virtual bool InitAccessParam(void* p);
	void SetConnect(DBConnect* pConn) { m_connect = pConn; }
	char* Fromat(char* destr, const char* sql,  int totalCount, vector<ArgType>& args, const vector<FieldInfo>& argTypeList, va_list ap, char c1='?', char c2='%');
protected:
	DBConnect*	m_connect;
	
	DECLAREBASEMETHOD(DBAccess);
	DECLARENODE(DBAccess);	
};


#endif	//DBAccess.h

