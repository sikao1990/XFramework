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
	//������ʵ��->access�����ȳ�ʼ�����ݿ����ӵĲ���
	return true;
}

bool DBConnect::ConnectDB()
{
	//������ʵ��->mysql��ado��oracle�����ݿ������
	return true;
}

void DBConnect::DisConnectDB()
{
	//������ʵ��->mysql��ado��oracle�����ݿ�ĶϿ�
}

int DBConnect::BeginTrans()
{
	return 0;
}

int DBConnect::CommitTrans(bool b)
{
	return 0;
}
