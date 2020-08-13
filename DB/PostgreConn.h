#ifndef __PostgreConn_H__
#define __PostgreConn_H__
#include "DBConnect.h"
#include "../XDefine.h"

class PostgreAccess;
class FRAMEWORK_API PostgreConn : public DBConnect
{
	friend PostgreAccess;
public:
	PostgreConn();
	~PostgreConn();
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
	DECLARENODE(PostgreConn);
private:
	//TODO:

};

#endif	//PostgreConn.h