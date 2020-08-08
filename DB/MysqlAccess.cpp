#include "MysqlAccess.h"
#include "MysqlConn.h"
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
typedef my_bool MYBOOL;
#elif __linux__
typedef my_bool MYBOOL;
#elif __APPLE__
typedef bool MYBOOL;
#endif

DEFINENODE(MysqlAccess, "MysqlAccess", DBAccess, ALLOCMEM);

MysqlAccess::MysqlAccess()
{

}

MysqlAccess::~MysqlAccess()
{

}

bool MysqlAccess::InitAccessParam(void* p)
{
	//TODO:
	return true;
}

int MysqlAccess::Execute(const char* sql)
{
	int nRet = 0;
	MysqlConn*	pMysqlConn = dynamic_cast<MysqlConn*>(m_connect);
	if (0 != (nRet = DBAccess::Execute(sql)))
		return nRet;
	try {
		if (0 == mysql_query(pMysqlConn->m_pConn, sql))
			return -2;
	}
	catch (...)
	{
		return 1;
	}
	return 0;
}

int MysqlAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{
	int nRet = 0;
	if (0 != (nRet = DBAccess::Execute(sql, argTypeList, type)))
		return nRet;
	va_list ap;
	MysqlConn* pMysqlConn = dynamic_cast<MysqlConn*>(m_connect);
	vector<ArgType>	args;
	char strSql[1024] = {};
	const char* pSql = NULL;

	va_start(ap, type);
	//ƴ��sql���,���� %
	pSql = Fromat(sql, strSql, sizeof(strSql), args, argTypeList, ap, '?', '%');
	va_end(ap);
	if (pSql != strSql)
		return -2;
	if (args.size() != argTypeList.size())
		return -3;
	//ȥ�� % ����
	for (vector<ArgType>::iterator it = args.begin(); !args.empty();)
	{
		if (-1 == it->nFlag)
			it = args.erase(it);
		else
			it++;
	}
	MYSQL_BIND* pBindParam = new MYSQL_BIND[args.size()];
	memset(pBindParam, 0, sizeof(MYSQL_BIND)*args.size());
	MYSQL_STMT *stmt = mysql_stmt_init(pMysqlConn->m_pConn);
	if (mysql_stmt_prepare(stmt, pSql, strlen(pSql)))
		return 1;

	for (int i = 0; i < args.size(); i++)
	{
		pBindParam[i].length = &args[i].val.len;//TODO:
		switch (args[i].val.fieldType)
		{
		case DT_INT:
			pBindParam[i].buffer = (int *)&args[i].val.ptr.nInt32;
			pBindParam[i].buffer_length = sizeof(args[i].val.ptr.nInt32);
			pBindParam[i].buffer_type = MYSQL_TYPE_LONG;
			break;
		case DT_INT64:
			pBindParam[i].buffer = (long long *)&args[i].val.ptr.nInt64;
			pBindParam[i].buffer_length = sizeof(args[i].val.ptr.nInt64);
			pBindParam[i].buffer_type = MYSQL_TYPE_LONGLONG;
			break;
		case DT_DOUBLE:
			pBindParam[i].buffer = (double *)&args[i].val.ptr.lfDouble;
			pBindParam[i].buffer_length = sizeof(args[i].val.ptr.lfDouble);
			pBindParam[i].buffer_type = MYSQL_TYPE_DOUBLE;
			break;
		case DT_STRING:
			pBindParam[i].buffer = (char *)&args[i].val.ptr.pStr;
			pBindParam[i].buffer_length = args[i].val.len;
			pBindParam[i].buffer_type = MYSQL_TYPE_STRING;
			break;
		case DT_BLOB:
			pBindParam[i].buffer = (char *)&args[i].val.ptr.pStr;
			pBindParam[i].buffer_length = args[i].val.len;
			pBindParam[i].buffer_type = MYSQL_TYPE_BLOB;
			break;
		}
	}
	mysql_stmt_bind_param(stmt, pBindParam);
	mysql_stmt_execute(stmt);
	mysql_stmt_close(stmt);
	delete[] pBindParam;
	return 0;
}

int MysqlAccess::Query(const char* sql, list< list<DBField> >& result)
{
	int nRet = 0;
	if (0 != (nRet = DBAccess::Query(sql, result)))
		return nRet;
	MYSQL_RES *SqlResult = NULL;
	MysqlConn*	pMysqlConn = dynamic_cast<MysqlConn*>(m_connect);
	if (0 == mysql_query(pMysqlConn->m_pConn, sql)) {
		SqlResult = mysql_store_result(pMysqlConn->m_pConn);
		int rowcount = mysql_num_rows(SqlResult);

		vector<mysqlFieldinfo>	fieldInfos;
		unsigned long* pLengths = NULL;
		unsigned int fieldcount = mysql_num_fields(SqlResult);
		pLengths = mysql_fetch_lengths(SqlResult);
		MYSQL_FIELD *field = NULL;

		MYSQL_ROW		row = NULL;
		list<DBField>	lstFields;

		for (unsigned int i = 0; i < fieldcount; i++) {
			field = mysql_fetch_field_direct(SqlResult, i);
			fieldInfos.push_back(mysqlFieldinfo(field->name, field->type,pLengths[i]));
		}

		row = mysql_fetch_row(SqlResult);
		while (NULL != row) {
			DBField tmpFields;
			for (int i = 0; i < fieldcount; i++) {
				tmpFields.fieldName = fieldInfos[i].name;
				tmpFields.len = fieldInfos[i].len;
				switch (fieldInfos[i].type)
				{
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_LONG:
					tmpFields.fieldType = DT_INT;
					tmpFields.ptr.nInt32 = atoi(row[i]);
					break;
				case MYSQL_TYPE_LONGLONG:
					tmpFields.fieldType = DT_INT64;
					tmpFields.ptr.nInt64 = atoll(row[i]);
					break;
				case MYSQL_TYPE_FLOAT:
				case MYSQL_TYPE_DOUBLE:
					tmpFields.fieldType = DT_DOUBLE;
					tmpFields.ptr.lfDouble = atof(row[i]);
					break;
				case MYSQL_TYPE_VARCHAR:
				case MYSQL_TYPE_STRING:
				case MYSQL_TYPE_VAR_STRING:
					tmpFields.fieldType = DT_STRING;
					tmpFields.memType = 0;
					//strcpy_s(tmpFields.ptr.pStr = new char[fieldInfos[i].len+1], fieldInfos[i].len,row[i]);
					strcpy(tmpFields.ptr.pStr = new char[fieldInfos[i].len + 1], row[i]);
					break;
				case MYSQL_TYPE_BLOB:
				case MYSQL_TYPE_LONG_BLOB:
					tmpFields.fieldType = DT_BLOB;
					tmpFields.memType = 0;
					memcpy(tmpFields.ptr.pBlob = new unsigned char[tmpFields.len], row[i],tmpFields.len);
					break;
				case MYSQL_TYPE_TIME:
				case MYSQL_TYPE_DATE:
				case MYSQL_TYPE_DATETIME:
					break;
				}
				lstFields.push_back(tmpFields);
			}
			result.push_back(lstFields);
			row = mysql_fetch_row(SqlResult);
		}
	}
	else
		return -2;
	return 0;
}

int MysqlAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, list< list<DBField> >& result, TagType type, ...)
{
	int nRet = 0;
	if (0 != (nRet = DBAccess::Query(sql, argTypeList, result,type)))
		return nRet;
	va_list ap;
	MysqlConn* pMysqlConn = dynamic_cast<MysqlConn*>(m_connect);
	vector<ArgType>	args;
	char strSql[1024] = {};
	const char* pSql = NULL;

	MYSQL_RES*	prepare_meta_result = NULL;
	MYSQL_BIND*	pOutBind = NULL;
	MYBOOL*	pBool = NULL;
	unsigned long*	pLength = NULL;
	int  param_count = 0, column_count = 0;
	MYSQL_FIELD *field = NULL;
	vector<int>	vecTypes;
	vector<DBField>	tmpFields;

	va_start(ap, type);//
	//ƴ��sql���,���� %
	pSql = Fromat(sql, strSql, sizeof(strSql), args, argTypeList, ap, '?', '%');
	va_end(ap);
	if (pSql != strSql)
		return -2;
	if (args.size() != argTypeList.size())
		return -3;
	//ȥ�� % ����
	for (vector<ArgType>::iterator it = args.begin(); !args.empty();)
	{
		if (-1 == it->nFlag)
			it = args.erase(it);
		else
			it++;
	}
	MYSQL_BIND* pBindParam = new MYSQL_BIND[args.size()];
	memset(pBindParam, 0, sizeof(MYSQL_BIND)*args.size());
	MYSQL_STMT *stmt = mysql_stmt_init(pMysqlConn->m_pConn);
	if (NULL == stmt)
		return -4;
	if (mysql_stmt_prepare(stmt, pSql, strlen(pSql))) {
		nRet = 1;
		goto NoFreeEnd;
	}

	for (int i = 0; i < args.size(); i++)
	{
		pBindParam[i].length = &args[i].val.len;
		switch (args[i].val.fieldType)
		{
		case DT_INT:
			pBindParam[i].buffer = (int *)&args[i].val.ptr.nInt32;
			pBindParam[i].buffer_length = sizeof(args[i].val.ptr.nInt32);
			pBindParam[i].buffer_type = MYSQL_TYPE_LONG;
			break;
		case DT_INT64:
			pBindParam[i].buffer = (long long *)&args[i].val.ptr.nInt64;
			pBindParam[i].buffer_length = sizeof(args[i].val.ptr.nInt64);
			pBindParam[i].buffer_type = MYSQL_TYPE_LONGLONG;
			break;
		case DT_DOUBLE:
			pBindParam[i].buffer = (double *)&args[i].val.ptr.lfDouble;
			pBindParam[i].buffer_length = sizeof(args[i].val.ptr.lfDouble);
			pBindParam[i].buffer_type = MYSQL_TYPE_DOUBLE;
			break;
		case DT_STRING:
			pBindParam[i].buffer = (char *)&args[i].val.ptr.pStr;
			pBindParam[i].buffer_length = args[i].val.len;
			pBindParam[i].buffer_type = MYSQL_TYPE_STRING;
			break;
		case DT_BLOB:
			pBindParam[i].buffer = (char *)&args[i].val.ptr.pStr;
			pBindParam[i].buffer_length = args[i].val.len;
			pBindParam[i].buffer_type = MYSQL_TYPE_BLOB;
			break;
		}
	}
	//�󶨲���
	mysql_stmt_bind_param(stmt, pBindParam);

	param_count = mysql_stmt_param_count(stmt);
	if (param_count != args.size()) {
		nRet = 2;
		goto NoFreeEnd;
	}
	prepare_meta_result = mysql_stmt_result_metadata(stmt);
	if (NULL == prepare_meta_result) {
		nRet = 3;
		goto NoFreeEnd;
	}
	//ȡ��Ҫ��ѯ���ֶ�����
	column_count = mysql_num_fields(prepare_meta_result);


	//ȡ��Ҫ��ѯ�����ֶε�����
	for (unsigned int i = 0; i < column_count; i++) {
		field = mysql_fetch_field_direct(prepare_meta_result, i);
		vecTypes.push_back(field->type);
	}
	pOutBind = new MYSQL_BIND[column_count];
	pBool = new MYBOOL[column_count];
	pLength = new unsigned long[column_count];
	memset(pOutBind, 0, sizeof(MYSQL_BIND)*column_count);
	memset(pBool, 0, sizeof(MYBOOL)*column_count);
	memset(pLength, 0, sizeof(unsigned long)*column_count);

	if (mysql_stmt_execute(stmt)) {
		nRet = 4;
		goto end;
	}
	for (int i = 0; i < vecTypes.size(); i++)
	{
		DBField tmpField;
		switch (vecTypes[i])
		{
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_LONG:
			tmpField.fieldType = DT_INT;
			tmpField.ptr.nInt32 = 0;//��ʼֵ
			break;
		case MYSQL_TYPE_LONGLONG:
			tmpField.fieldType = DT_INT64;
			tmpField.ptr.nInt64 = 0;//��ʼֵ
			break;
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			tmpField.fieldType = DT_DOUBLE;
			tmpField.ptr.lfDouble = 0;//��ʼֵ
			break;
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			tmpField.fieldType = DT_STRING;
			tmpField.memType = 0;
			tmpField.len = 1024;//Ԥ���ռ�->�������ݿ��ֶ��ַ����1023
			tmpField.ptr.pStr = new char[tmpField.len];
			break;
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
			tmpField.fieldType = DT_BLOB;
			tmpField.memType = 0;
			tmpField.len = 8 * 1024;//Ԥ���ռ�->�������ݿ��ֶεĶ�������� 8*1024
			tmpField.ptr.pBlob = new unsigned char[tmpField.len];
			break;
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_DATETIME:
			break;
		}
		tmpFields.push_back(tmpField);
	}
	//�������������ֶλ���
	for (int i = 0; i < column_count; i++)
	{
		pOutBind[i].buffer_type = (enum enum_field_types)vecTypes[i];
		pOutBind[1].buffer_length = tmpFields[i].len;
		pOutBind[1].is_null = &pBool[i];
		pOutBind[1].length = &pLength[i];
		switch (tmpFields[i].fieldType)
		{
		case DT_INT:
		case DT_INT64:
		case DT_DOUBLE:
			pOutBind[1].buffer = (char*)&tmpFields[i].ptr.pBase;
			break;
		case DT_STRING:
		case DT_BLOB:
			pOutBind[1].buffer = (char*)tmpFields[i].ptr.pStr;
			break;
		}
	}
	//���н��������
	if (mysql_stmt_bind_result(stmt, pOutBind)) {
		nRet = 5;
		goto end;
	}
	
	if (mysql_stmt_store_result(stmt)) {
		nRet = 6;
		goto end;
	}
	//��������
	while (!mysql_stmt_fetch(stmt))
	{
		list<DBField> lstFields;
		for (int i = 0; i < column_count; i++)
		{
			if (!pBool[i])
			{
				lstFields.push_back(tmpFields[i]);
			}
			else {
				nRet = 7;
				goto end;
			}
		}
	}
end:
	delete[] pOutBind;
	delete[] pBool;
	delete[] pLength;
	mysql_free_result(prepare_meta_result);
NoFreeEnd:
	mysql_stmt_close(stmt);
	return nRet;
}
