#ifndef __DBConnPool_H__
#define __DBConnPool_H__
#include "../XDefine.h"
#include "DBConnect.h"
#include "DBDefine.h"
#include "../Mem/ObjPool.h"
#include <thread>
#include <map>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
using namespace std;

#define REGISTER_DB_ENGINE(strDBinfo,ConnType,ProxyInfo)	\
	DBConnPool::AddSupportDBengine(strDBinfo,#ConnType,#ProxyInfo,&ConnType::InitLib,&ConnType::UninitLib)

#define UNREGISTER_DB_ENGINE(strDBinfo)	\
	DBConnPool::DeleteSupportDBengine(strDBinfo)

class FRAMEWORK_API DBConnPool : public ObjPool<DBConnect,true>
{
public:
	DBConnPool();
	virtual ~DBConnPool();
	bool Init(const DBParam& param,int size = ALLOCCOUNT);
	DBConnect* GetNewConnect();
	void ReleaseConn(DBConnect* pConn);
	void ReCreatePool(DBConnect* pConn);
	static void AddSupportDBengine(const char* dbInfo,const char* connName,const char* proxyName, pFuncDB pInit = NULL, pFuncDB pUninit = NULL);
	static void DeleteSupportDBengine(const char* dbInfo);
	static const map<string, DBInfo>& GetDBInfo() { return m_setDb; }
protected:
	virtual bool InitDBlib();
	bool InitObj(DBConnect** ppObj, void* pParam);
	void run();
private:
	static map<string,DBInfo>	m_setDb;
private:
	bool	m_RorganizeFlag;
	bool	m_DBInitFlag;
	DBParam m_param;
#ifdef WIN32
	HANDLE	m_hEvent;
#endif
};

#endif	//DBConnPool.h

