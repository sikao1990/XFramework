#ifndef __OracleConn_H__
#define __OracleConn_H__
#include "DBConnect.h"
#include "../XDefine.h"

class OracleAccess;
class FRAMEWORK_API OracleConn : public DBConnect
{
	friend OracleAccess;
public:
	OracleConn();
	~OracleConn();
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
	DECLARENODE(OracleConn);
private:
	//TODO:

};

#endif	//OracleConn.h