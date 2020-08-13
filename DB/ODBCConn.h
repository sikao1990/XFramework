#ifndef __ODBCConn_H__
#define __ODBCConn_H__
#include "DBConnect.h"
#include "../XDefine.h"

class ODBCAccess;
class FRAMEWORK_API ODBCConn : public DBConnect
{
	friend ODBCAccess;
public:
	ODBCConn();
	~ODBCConn();
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
	DECLARENODE(ODBCConn);
private:
	//TODO:

};

#endif	//ODBCConn.h