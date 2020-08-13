#ifndef __MangoConn_H__
#define __MangoConn_H__
#include "DBConnect.h"
#include "../XDefine.h"

class MangoAccess;
class FRAMEWORK_API MangoConn : public DBConnect
{
	friend MangoAccess;
public:
	MangoConn();
	~MangoConn();
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
	DECLARENODE(MangoConn);
private:
	//TODO:

};

#endif	//MangoConn.h