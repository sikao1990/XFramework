#include "MysqlConn.h"
#include "DBDefine.h"

DEFINENODE(MysqlConn, "MysqlConn", DBConnect, ALLOCMEM);

bool MysqlConn::InitLib()
{
	if (0 != mysql_library_init(0, NULL, NULL))
		return false;
	return true;
}

bool MysqlConn::UninitLib()
{
	mysql_server_end();
	return true;
}

MysqlConn::MysqlConn()
{
}

MysqlConn::~MysqlConn()
{
	if (m_pConn)mysql_close(m_pConn);
}

bool MysqlConn::InitConnecion(void* p)
{
	DBParam* pParam = NULL;
	Init(ALLOCCOUNT);
	m_pConn = mysql_init(NULL);
	if (NULL == m_pConn)
		return false;
	mysql_autocommit(m_pConn, 1);
	pParam = (DBParam*)p;
	if (NULL == pParam)return false;
	m_host = pParam->param._mysqlParam.m_dbIp;
	m_user = pParam->param._mysqlParam.m_user;
	m_passwd = pParam->param._mysqlParam.m_passwd;
	m_dbName = pParam->param._mysqlParam.m_dbName;
	if (0 != mysql_options(m_pConn, MYSQL_SET_CHARSET_NAME, "gbk"))
		return false;
	return true;
}

bool MysqlConn::ConnectDB()
{
	try {
		if (!mysql_real_connect(m_pConn, m_host.c_str(), m_user.c_str(),m_passwd.c_str(), m_dbName.c_str(), 3306, NULL, 0))
			return false;
	}
	catch (...) {
		return false;
	}
	return true;
}

void MysqlConn::DisConnectDB()
{
	//派生类实现->mysql、ado、oracle等数据库的断开
}

int MysqlConn::BeginTrans()
{
	mysql_autocommit(m_pConn, 0);
	return 0;
}

int MysqlConn::CommitTrans(bool b)
{
	b ? mysql_commit(m_pConn): mysql_rollback(m_pConn); 
	mysql_autocommit(m_pConn, 1);
	return 0;
}

