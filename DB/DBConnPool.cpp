#include "DBConnPool.h"

map<string,DBInfo>	DBConnPool::m_setDb;

DBConnPool::DBConnPool():ObjPool<DBConnect, true>()
{
	m_RorganizeFlag = true;
	m_DBInitFlag = false;
#ifdef WIN32
	m_hEvent = NULL;
#endif
	m_tagDBInfo = "";
}

DBConnPool::~DBConnPool()
{
	pFuncDB pUninit = NULL; 
	pUninit = m_setDb.find(m_tagDBInfo)->second.m_pFuncUnInit;
	if(pUninit)pUninit();
	m_RorganizeFlag = false;
	m_DBInitFlag = false;
}

bool DBConnPool::Init(int size )
{
#ifdef WIN32
	m_hEvent = CreateEvent(NULL, false, false, NULL);
#endif
	std::thread t(&DBConnPool::run, this);
	t.detach();
	return ObjPool<DBConnect, true>::Init(size);
}

bool DBConnPool::InitDBlib(const char* p)
{
	m_tagDBInfo = p;
	if (m_setDb.find(m_tagDBInfo) == m_setDb.end())return false;
	pFuncDB pInit = m_setDb.find(m_tagDBInfo)->second.m_pFuncInit;
	return pInit();
}

DBConnect* DBConnPool::GetNewConnect()
{
	DBConnect* pConn = (DBConnect*)Alloc();
	if(NULL!=pConn){
		pConn->SetDBInfo(m_tagDBInfo.c_str(),this);
		return pConn;
	}
	return NULL;
}

void DBConnPool::ReleaseConn(DBConnect* pConn)
{
	pConn->~DBConnect();
	Free((void*)pConn);
}

bool DBConnPool::InitObj(DBConnect** ppObj, void* pParam)
{
	if(m_setDb.find(m_tagDBInfo)!=m_setDb.end()){
		*ppObj = DBConnect::getInstance(m_setDb.find(m_tagDBInfo)->second.DBConnName);
		//new (*ppObj) DBConnect;
		return (*ppObj)->InitConnecion(pParam) && (*ppObj)->ConnectDB();
	}else
		return false;
}

void DBConnPool::AddSupportDB(const char* dbInfo,const char* connName,const char* proxyName, pFuncDB pInit, pFuncDB pUninit)
{
	m_setDb.insert(make_pair(dbInfo,DBInfo(connName,proxyName, pInit,pUninit)) );
}

void DBConnPool::DeleteSupportDB(const char* dbInfo)
{
	if(m_setDb.find(dbInfo)!= m_setDb.end())
		m_setDb.erase(m_setDb.find(dbInfo));
}

void DBConnPool::ReCreatePool(DBConnect* pConn)
{
	ReleaseConn(pConn);
#ifdef WIN32
	SetEvent(m_hEvent);//最好使用c++11信号量
#endif
}

void DBConnPool::run()
{
	while (m_RorganizeFlag)
	{
#ifdef WIN32
		WaitForSingleObject(m_hEvent, INFINITE);
#endif
		ClearPool();
		RetryInitObj();
	}
}
