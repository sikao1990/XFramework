/*----------------------------------------------------------------
// 文件名：MysqlConn.h
// 文件功能描述：Mysql数据库访问链接接口
// 创建者:zhuliwu
// 时间：2018/10/8 9:10:00
// 版本：V1.0.0
//----------------------------------------------------------------*/

#ifndef __MysqlConn_H__
#define __MysqlConn_H__
#include "DBConnect.h"

#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif
#include <mysql.h>

#pragma comment(lib, "C:/Program Files/MySQL/MySQL Server 5.7/lib/libmysql.lib")  

class MysqlAccess;
class MysqlConn : public DBConnect
{
	friend MysqlAccess;
public:
	MysqlConn();
	~MysqlConn();
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
	DECLARENODE(MysqlConn);
private:
	string	m_host;
	string	m_user;
	string	m_passwd;
	string	m_dbName;
	MYSQL*	m_pConn;
};

#endif	//AdoConn.h

