#include "DBConnPool.h"

#ifdef WIN32
#include "../Thread/ThreadImpl_Win32.h"
#else
#include "../Thread/ThreadImpl_Linux.h"
#endif

map<string,DBInfo>	DBConnPool::m_setDb;

DBConnPool::DBConnPool():ObjPool<DBConnect, true>()
{
	m_RorganizeFlag = true;
	m_DBInitFlag = false;
#ifdef WIN32
	m_hEvent = NULL;
#endif
}

DBConnPool::~DBConnPool()
{
	pFuncDB pUninit = NULL; 
	pUninit = m_setDb.find(m_param.m_strDBinfo)->second.m_pFuncUnInit;
	if(pUninit)pUninit();
	m_RorganizeFlag = false;
	m_DBInitFlag = false;
}

bool DBConnPool::Init(const DBParam& param, int size )
{
#ifdef WIN32
	m_hEvent = CreateEvent(NULL, false, false, NULL);
#endif
	m_param = param;
	ThreadImpl<DBConnPool> t(this,&DBConnPool::run);
	//std::thread t(&DBConnPool::run,this);
	//t.detach();
	return ObjPool<DBConnect, true>::Init(size,&m_param)&& InitDBlib();
}

bool DBConnPool::InitDBlib()
{
	if (m_setDb.find(m_param.m_strDBinfo) == m_setDb.end())return false;
	pFuncDB pInit = m_setDb.find(m_param.m_strDBinfo)->second.m_pFuncInit;
	return pInit();
}

DBConnect* DBConnPool::GetNewConnect()
{
	DBConnect* pConn = (DBConnect*)Alloc();
	if(NULL!=pConn){
		pConn->SetDBInfo(m_param.m_strDBinfo,this);
		if(pConn->Init(ALLOCCOUNT/10,&m_param)&&pConn->InitConnecion(&m_param)&&pConn->ConnectDB())
			return pConn;
		pConn->~DBConnect();
		Free((void*)pConn);
	}
	return NULL;
}

void DBConnPool::ReleaseConn(DBConnect* pConn)
{
	if (NULL == pConn)return;
	pConn->DisConnectDB();
	pConn->~DBConnect();
	Free((void*)pConn);
}

bool DBConnPool::InitObj(DBConnect** ppObj, void* pParam)
{
	if(m_setDb.find(m_param.m_strDBinfo)!=m_setDb.end()){
		*ppObj = DBConnect::getInstance(m_setDb.find(m_param.m_strDBinfo)->second.DBConnName,NULL);
		return NULL!=*ppObj;
	}else
		return false;
}

void DBConnPool::AddSupportDBengine(const char* dbInfo,const char* connName,const char* proxyName, pFuncDB pInit, pFuncDB pUninit)
{
	m_setDb.insert(make_pair(dbInfo,DBInfo(connName,proxyName, pInit,pUninit)) );
}

void DBConnPool::DeleteSupportDBengine(const char* dbInfo)
{
	if(m_setDb.find(dbInfo)!= m_setDb.end())
		m_setDb.erase(m_setDb.find(dbInfo));
}

void DBConnPool::ReCreatePool(DBConnect* pConn)
{
	ReleaseConn(pConn);
	//TODO:
	//m_bReachableDBServer = false;
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
#else
		pause();
#endif
		ClearPool();
		RetryInitObj();
		//m_bReachableDBServer = true;
	}
}
