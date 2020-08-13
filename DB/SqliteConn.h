#ifndef __SqliteConn_H__
#define __SqliteConn_H__
#include "DBConnect.h"
#include "../XDefine.h"

class SqliteAccess;
class FRAMEWORK_API SqliteConn : public DBConnect
{
	friend SqliteAccess;
public:
	SqliteConn();
	~SqliteConn();
public:
	virtual bool InitConnecion(void* p);
	virtual bool ConnectDB();
	virtual void DisConnectDB();
	virtual int BeginTrans();
	virtual int CommitTrans(bool b);
public:
	static bool InitLib();
	static bool UninitLib();
private:
	DECLARENODE(SqliteConn);
private:
	//TODO:

};

#endif	//SqliteConn.h