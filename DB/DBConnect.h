#ifndef	__DBConnect_H__
#define __DBConnect_H__
#include "../XDefine.h"
#include "DBAccess.h"
#include "../Mem/ObjPool.h"
#include "../Pattern/TreeRelation.h"
#include "DBDefine.h"

class DBConnPool;
class FRAMEWORK_API DBConnect : public ObjPool<DBAccess>
{
	friend class DBConnPool;
public:
	DBConnect():ObjPool<DBAccess>(){ m_bValid = false; }
	virtual ~DBConnect(){}
	const string GetDBType()const { return m_dbType; }
	DBAccess* GetDBAccessProxy();
	void ReleaseDBAccessProxy(DBAccess* pProxy);
	bool IsValidConnect() { return m_bValid; }
	virtual int BeginTrans();
	virtual int CommitTrans(bool b);
protected:
	void ReleaseInvalidConn();
	void SetDBInfo(const char* str, DBConnPool* pPool) { m_dbType = str; m_pFromPool = pPool; m_bValid = true; }
	virtual bool InitConnecion(void* p);
	virtual bool ConnectDB();
	virtual void DisConnectDB();
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

