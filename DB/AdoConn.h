/*----------------------------------------------------------------
// �ļ�����AdoConn.h
// �ļ�����������Ado���ݿ�������ӽӿ�
// ������:zhuliwu
// ʱ�䣺2018/10/8 9:10:00
// �汾��V1.0.0
//----------------------------------------------------------------*/

#ifndef __AdoConn_H__
#define __AdoConn_H__
#include "DBConnect.h"
#include "../XDefine.h"

#import "./msado15.dll" no_namespace rename("EOF","AdoEOF") 

class AdoAccess;
class FRAMEWORK_API AdoConn : public DBConnect
{
	friend AdoAccess;
public:
	AdoConn();
	~AdoConn();
public:
	virtual bool InitConnecion(void* p);
	virtual bool ConnectDB();
	virtual void DisConnectDB();
	virtual int BeginTrans();
	virtual int CommitTrans(bool b);
public:
	static bool InitLib();
	static bool UninitLib();
	bool SetAdoBinaryData(_variant_t* pVart,void* pStart, int count);
	int GetBinaryByAdo(const _RecordsetPtr& pSet, const char* colName, DBField* pStart,long lDataSize);
private:
	DECLARENODE(AdoConn);
private:
	string			m_dbStr;
	string			m_user;
	string			m_passwd;
	string			m_DBIP;
	string			m_DBName;
	_ConnectionPtr	m_pConn;
};

#endif	//AdoConn.h

