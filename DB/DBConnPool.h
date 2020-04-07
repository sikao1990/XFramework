#ifndef __DBConnPool_H__
#define __DBConnPool_H__
#include "DBConnect.h"
#include "../Mem/ObjPool.h"
#include <thread>
#include <map>
#include <windows.h>

using namespace std;

typedef bool (*pFuncDB)();

#define REGISTER_DB(strDBinfo,ConnType,ProxyType)	\
	DBConnPool::AddSupportDB(strDBinfo,#ConnType,#ProxyType,&ConnType::InitLib,&ConnType::UninitLib)

#define UNREGISTER_DB(strDBinfo)	\
	DBConnPool::DeleteSupportDB(strDBinfo)

struct DBInfo
{
	DBInfo(const char* pc=NULL,const char* pp=NULL, pFuncDB pInit=NULL, pFuncDB pUninit=NULL)
	{
		if (NULL !=pc)DBConnName = pc;
		if (NULL !=pp)DBProxyName = pp;
		if (NULL != pInit)m_pFuncInit = pInit;
		if (NULL != pUninit)m_pFuncUnInit = pUninit;
	}
	string 	DBConnName;
	string 	DBProxyName;
	pFuncDB	m_pFuncInit;
	pFuncDB	m_pFuncUnInit;
};

class DBConnPool : public ObjPool<DBConnect,true>
{
public:
	DBConnPool();
	virtual ~DBConnPool();
	bool Init(int size = ALLOCCOUNT);
	virtual bool InitDBlib(const char* p=NULL);
	DBConnect* GetNewConnect();
	void ReleaseConn(DBConnect* pConn);
	void ReCreatePool(DBConnect* pConn);
	static void AddSupportDB(const char* dbInfo,const char* connName,const char* proxyName, pFuncDB pInit = NULL, pFuncDB pUninit = NULL);
	static void DeleteSupportDB(const char* dbInfo);
	static const map<string, DBInfo>& GetDBInfo() { return m_setDb; }
protected:
	bool InitObj(DBConnect** ppObj, void* pParam);
	void run();
private:
	static map<string,DBInfo>	m_setDb;
private:
	string	m_tagDBInfo;
	bool	m_RorganizeFlag;
	bool	m_DBInitFlag;
	HANDLE	m_hEvent;
};

#endif	//DBConnPool.h

