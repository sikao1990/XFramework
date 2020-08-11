#include "DBConnect.h"
#include "DBConnPool.h"

DEFINEBASEMETHOD(DBConnect);
DEFINENODE(DBConnect, "DBConnect", DBConnect, ALLOCMEM);

DBAccess* DBConnect::GetDBAccessProxy()
{
	if (!m_bValid)return NULL;
	DBAccess* pAccess = (DBAccess*)Alloc();
	if(NULL!= pAccess){
		pAccess->SetConnect(this);
		return pAccess;
	}
	return NULL;
}

void DBConnect::ReleaseDBAccessProxy(DBAccess* pProxy)
{
	if (!m_bValid)return;
	pProxy->~DBAccess();
	Free((void*)pProxy);
}

bool DBConnect::InitObj(DBAccess** ppObj, void* pParam)
{
	const map<string, DBInfo>& dbInfos = DBConnPool::GetDBInfo();
	if(dbInfos.find(m_dbType)!= dbInfos.end()){
		*ppObj = DBAccess::getInstance(dbInfos.find(m_dbType)->second.DBProxyName,NULL);
		if (NULL == *ppObj)return false;
		return (*ppObj)->InitAccessParam(pParam);
	}else
		return false;	
}

void DBConnect::ReleaseInvalidConn()
{
	m_bValid = false;
	DisConnectDB();
	//m_pFromPool->ReCreatePool(this);
}

bool DBConnect::InitConnecion(void* p)
{
	//派生类实现->access数量等初始化数据库连接的参数
	return true;
}

bool DBConnect::ConnectDB()
{
	//派生类实现->mysql、ado、oracle等数据库的连接
	return true;
}

void DBConnect::DisConnectDB()
{
	//派生类实现->mysql、ado、oracle等数据库的断开
}

int DBConnect::BeginTrans()
{
	return 0;
}

int DBConnect::CommitTrans(bool b)
{
	return 0;
}
