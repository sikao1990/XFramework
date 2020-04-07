#include "DBAccess.h"
#include "DBConnect.h"
#include <stdarg.h>

DEFINEBASEMETHOD(DBAccess);
DEFINENODE(DBAccess, "DBAccess", DBAccess, ALLOCMEM);

bool DBAccess::InitAccessParam(void* p)
{
	return true;
}

int DBAccess::Execute(const char* sql)
{ 
	if (!m_connect->IsValidConnect())
		return -1;
	return 0; 
}

int DBAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{ 
	if (!m_connect->IsValidConnect())
		return -1;
	/*
	try{
	}catch(...){
		m_connect->ReleaseInvalidConn();
	}
	*/
	return 0; 
}

int DBAccess::Query(const char* sql, list< list<DBField> >& result)
{ 
	if (!m_connect->IsValidConnect())
		return -1;
	return 0; 
}

int DBAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, list< list<DBField> >& result, ...)
{ 
	if (!m_connect->IsValidConnect())
		return -1;
	return 0; 
}

char* DBAccess::Fromat(const char* sql, char* destr, int totalCount,vector<ArgType>& args, const vector<FieldInfo>& argTypeList, va_list ap, char c1, char c2)
{
	char* pStart = destr;
	const char* pTmp = NULL;
	char tmpFromat[256] = {};
	while (NULL != pTmp && pTmp < strlen(sql) + sql)
	{
		pTmp = strchr(sql, c1);// '?'
		args.push_back(ArgType{ pTmp,1 });
	}
	pTmp = NULL;
	while (NULL != pTmp && pTmp < strlen(sql) + sql)
	{
		pTmp = strchr(sql, c2);// '%'
		args.push_back(ArgType{ pTmp,0 });
	}
	if (args.size() != argTypeList.size())
		return NULL;
	for (int i = 0; i < argTypeList.size(); i++)
	{
		if (1 == args.size())
			strcpy_s(tmpFromat,strlen(sql),sql);
		else if (i == args.size() - 1)
			strncpy_s(tmpFromat, args[i].pStart, sql + strlen(sql) - args[i].pStart);
		else
			strncpy_s(tmpFromat, args[i].pStart, args[i + 1].pStart - args[i].pStart);

		switch (argTypeList[i].fieldType)
		{
		case DT_INT:
			args[i].val.fieldType = DT_INT;
			args[i].val.len = sizeof(int);
			args[i].val.ptr.nInt32 = va_arg(ap, int);
			sprintf_s(destr + strlen(destr), totalCount - strlen(destr), tmpFromat, args[i].val.ptr.nInt32);
			break;
		case DT_INT64:		
			args[i].val.fieldType = DT_INT64;
			args[i].val.len = sizeof(long long);
			args[i].val.ptr.nInt64 = va_arg(ap, long long);
			sprintf_s(destr + strlen(destr), totalCount - strlen(destr), tmpFromat, va_arg(ap, long long));
			break;
		case DT_DOUBLE:
			args[i].val.fieldType = DT_DOUBLE;
			args[i].val.len = sizeof(double);
			args[i].val.ptr.lfDouble = va_arg(ap, double);
			sprintf_s(destr + strlen(destr), totalCount - strlen(destr), tmpFromat, va_arg(ap, double));
			break;
		case DT_STRING:
			{
				const char* pTmp = va_arg(ap, char*);
				args[i].val.fieldType = DT_STRING;
				args[i].val.len = strlen(pTmp);
				args[i].val.memType = 0;
				strcpy_s(args[i].val.ptr.pStr = new char[strlen(pTmp) + 1], strlen(pTmp),pTmp);
				sprintf_s(destr + strlen(destr), totalCount - strlen(destr), tmpFromat, args[i].val.ptr.pStr);
			}
			break;
		case DT_BLOB:
			{
				unsigned char* pVal = NULL;
				int len = 0;
				pVal = va_arg(ap, unsigned char*);
				len = va_arg(ap, int);
				args[i].val.fieldType = DT_BLOB;
				args[i].val.len = len;
				args[i].val.memType = 0;
				memcpy(args[i].val.ptr.pBlob =(unsigned char*)new char[len],pVal,len);
			}
			break;
		}
		args[i].val.fieldName = argTypeList[i].fieldName;
		if (0 == args[i].nFlag)
			args[i].nFlag = -1;
	}
	return pStart;
}
