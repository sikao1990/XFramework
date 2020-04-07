#ifndef __DBDefine_H__
#define __DBDefine_H__

#include <iostream>

using namespace std;

struct adoParam
{
	string 	m_dbIp;
	string	m_user;
	string	m_passwd;
	string	m_dbName;
	string	m_dbBase;
};

struct mysqlParam
{
	string 	m_dbIp;
	string 	m_user;
	string	m_passwd;
	string	m_dbName;
	//string m_textType;"gbk"
};

struct DBParam
{
	string	m_strDBinfo;
	union{
		adoParam	_adoParam;
		mysqlParam	_mysqlParam;
	}param;
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

