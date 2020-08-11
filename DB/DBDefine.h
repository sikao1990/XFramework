#ifndef __DBDefine_H__
#define __DBDefine_H__

#include "../XDefine.h"
#include <iostream>
#include <string.h>

using namespace std;

struct adoParam
{
	char 	m_dbIp[32];
	char	m_user[16];
	char	m_passwd[16];
	char	m_dbName[16];
	char	m_dbBase[128];
};

struct mysqlParam
{
	char 	m_dbIp[16];
	char 	m_user[16];
	char	m_passwd[16];
	char	m_dbName[16];
	//char m_textType;"gbk"
};

struct DBParam
{
	char	m_strDBinfo[16];
	union{
		adoParam	_adoParam;
		mysqlParam	_mysqlParam;
	}param;
	DBParam() { memset(this, 0, sizeof(*this));}
	DBParam(const char* DBInfo) {
		memset(this, 0, sizeof(*this)); strcpy(m_strDBinfo, DBInfo);
	}
	DBParam(const DBParam& that) {
		memcpy(this, &that, sizeof(*this));
	}
	DBParam& operator=(const DBParam& that) {
		memcpy(this, &that, sizeof(*this));
		return *this;
	}
};

typedef bool(*pFuncDB)();
struct DBInfo
{
	DBInfo(const char* pc = NULL, const char* pp = NULL, pFuncDB pInit = NULL, pFuncDB pUninit = NULL)
	{
		if (NULL != pc)DBConnName = pc;
		if (NULL != pp)DBProxyName = pp;
		if (NULL != pInit)m_pFuncInit = pInit;
		if (NULL != pUninit)m_pFuncUnInit = pUninit;
	}
	string 	DBConnName;
	string 	DBProxyName;
	pFuncDB	m_pFuncInit;
	pFuncDB	m_pFuncUnInit;
};

enum DBFieldType {
	DT_INT,
	DT_INT64,
	DT_DOUBLE,
	DT_STRING,
	DT_BLOB,
};

enum TagType
{
	TT_Sql,		//sql ���
	TT_Store,	//�洢����
};

struct FieldInfo
{
	DBFieldType		fieldType;	//�ֶ�����
	std::string		fieldName;	//�ֶ�����
};

//�ֶ���
struct DBField
{
	DBFieldType		fieldType;	//�ֶ�����
	std::string		fieldName;	//�ֶ�����
	unsigned short	offset;		//��Ӧ�ṹ���Աƫ��
	unsigned short	memType;	//0 ��̬���䡢1 ʹ�ð󶨵�
	unsigned long	len;		//���ݳ���
	union
	{
		void*			pBase;		//���ݻ���ַ
		int				nInt32;
		long long		nInt64;
		double			lfDouble;
		char*			pStr;
		unsigned char*	pBlob;
	}ptr;
	DBField() { memset(this, 0, sizeof(*this)); }
	~DBField() {
		Delete();
	}
	void Delete() {
		if (0 == memType)return;
		switch (fieldType)
		{
		case DT_STRING:
			delete[] ptr.pStr;
			break;
		case DT_BLOB:
			delete[] ptr.pBlob;
			break;
		}
		ptr.pBase = NULL;
	}
	DBField(const DBField& other) {
		if (1 == memType) {
			this->fieldType = other.fieldType;
			this->fieldName = other.fieldName;
			this->offset = other.offset;
			this->len = other.len;
			memcpy(this->ptr.pBase = (void*)new char[other.len], other.ptr.pBase, other.len);
		}
		else
			memcpy(this, &other, sizeof(*this));
	}
	const DBField& operator=(const DBField& other) {
		if (1 == memType) {
			if (NULL != ptr.pBase)
				Delete();
			this->fieldType = other.fieldType;
			this->fieldName = other.fieldName;
			this->offset = other.offset;
			this->len = other.len;
			memcpy(this->ptr.pBase = (void*)new char[other.len], other.ptr.pBase, other.len);
		}
		else
			memcpy(this, &other, sizeof(*this));
		return *this;
	}
};

#endif	//DBDefine.h

