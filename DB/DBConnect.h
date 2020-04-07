#ifndef	__DBConnect_H__
#define __DBConnect_H__
#include "DBAccess.h"
#include "../Mem/ObjPool.h"
#include "../Pattern/TreeRelation.h"
#include "DBDefine.h"

class DBConnPool;
class DBConnect : public ObjPool<DBAccess>
{
public:
	DBConnect() { m_bValid = false; }
	virtual ~DBConnect(){}
	void SetDBInfo(const char* str, DBConnPool* pPool) { m_dbType = str; m_pFromPool = pPool; m_bValid = true; }
	const string GetDBType()const { return m_dbType; }
	DBAccess* GetDBAccessProxy();
	void ReleaseDBAccessProxy(DBAccess* pProxy);
	bool IsValidConnect() { return m_bValid; }
	void ReleaseInvalidConn();
	virtual bool InitConnecion(void* p);
	virtual bool ConnectDB();
	virtual void DisConnectDB();
	virtual int BeginTrans();
	virtual int CommitTrans(bool b);
public:
	//static bool InitLib();	//the child must be defined this function
	//static bool UninitLib();	//the child must be defined this function
protected:
	bool InitObj(DBAccess** ppObj, void* pParam);
protected:
	string	m_dbType;
	bool	m_bValid;
	DECLAREBASEMETHOD(DBConnect);
	DECLARENODE(DBConnect);	
private:
	DBConnPool*	m_pFromPool;
};

#endif	//DBConnect.h

