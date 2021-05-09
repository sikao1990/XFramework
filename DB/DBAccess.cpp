#include "DBAccess.h"
#include "DBConnect.h"
#include <stdarg.h>
#include <algorithm>
#ifdef  WIN32
#include <windows.h>
#endif

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

int DBAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList,list< list<DBField> >& result, TagType type, ...)
{ 
	if (!m_connect->IsValidConnect())
		return -1;
	return 0; 
}

char* DBAccess::Fromat(char* destr, const char* sql, int totalCount,vector<ArgType>& args, const vector<FieldInfo>& argTypeList, va_list ap, char c1, char c2)
{
	const char* pTmp = strchr(sql, c1);// '?'
	int nPercentCount = 0;
	if (0 == strlen(sql)||NULL==destr||totalCount<strlen(sql))return NULL;
	while (NULL != pTmp && pTmp < strlen(sql) + sql)
	{
		args.push_back(ArgType(pTmp,1 ));
		pTmp = strchr(pTmp+1, c1);// '?'
	}
	pTmp = strchr(sql, c2);// '%'
	while (NULL != pTmp && pTmp < strlen(sql) + sql)
	{
		nPercentCount += 1;
		args.push_back(ArgType( pTmp,0 ));
		pTmp = strchr(pTmp+1, c2);// '%'
	}
	std::sort(args.begin(), args.end());
	if (args.size() != argTypeList.size())
		return NULL;
	for (int i = 0; i < argTypeList.size(); i++)
	{
		args[i].val.fieldName = argTypeList[i].fieldName;
		switch (argTypeList[i].fieldType)
		{
		case DT_INT:
			args[i].val.fieldType = DT_INT;
			args[i].val.len = sizeof(int);
			args[i].val.ptr.nInt32 = va_arg(ap, int);
			break;
		case DT_INT64:		
			args[i].val.fieldType = DT_INT64;
			args[i].val.len = sizeof(long long);
			args[i].val.ptr.nInt64 = va_arg(ap, long long);
			break;
		case DT_DOUBLE:
			args[i].val.fieldType = DT_DOUBLE;
			args[i].val.len = sizeof(double);
			args[i].val.ptr.lfDouble = va_arg(ap, double);
			break;
		case DT_STRING:
			{
				const char* pTmp = va_arg(ap, char*);
				args[i].val.fieldType = DT_STRING;
				args[i].val.len = strlen(pTmp);
				args[i].val.memType = 0;
				strcpy(args[i].val.ptr.pStr = new char[strlen(pTmp) + 1],pTmp);//TODO:
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
				memcpy(args[i].val.ptr.pBlob =(unsigned char*)new char[len],pVal,len);//TODO:
			}
			break;
		}
	}
	if (nPercentCount > 0) {
		vector<TmpInfo> infoVec;		
		for (int i = 0; i < args.size(); i++) {
			if (0 == args[i].nFlag) {
				TmpInfo tmp;
				tmp.pStart = args[i].pStart;
				tmp.nIndex = i;
				infoVec.push_back(tmp);
			}
		}
		vector<string> tmpVec;
		char szbuf[256] = {};
		strncpy(szbuf, sql, infoVec[0].pStart - sql);
		tmpVec.push_back(szbuf);
		for (int i = 0; i < infoVec.size() - 1; i++){
			memset(szbuf, 0, sizeof(szbuf));
			strncpy(szbuf, infoVec[i].pStart, infoVec[i + 1].pStart - infoVec[i].pStart);
			tmpVec.push_back(szbuf);
		}
		memset(szbuf, 0, sizeof(szbuf));
		strncpy(szbuf, infoVec[infoVec.size() - 1].pStart, sql + strlen(sql) - infoVec[infoVec.size() - 1].pStart);
		//OutputDebugString("");
		tmpVec.push_back(szbuf);
		memset(szbuf, 0, sizeof(szbuf));
		for (int i = 1; i < tmpVec.size(); i++) {
			switch (args[infoVec[i - 1].nIndex].val.fieldType)
			{
			case DT_INT:
				sprintf(szbuf, tmpVec[i].c_str(), args[infoVec[i - 1].nIndex].val.ptr.nInt32);
				break;
			case DT_INT64:
				sprintf(szbuf, tmpVec[i].c_str(), args[infoVec[i - 1].nIndex].val.ptr.nInt64);
				break;
			case DT_DOUBLE:
				sprintf(szbuf, tmpVec[i].c_str(), args[infoVec[i - 1].nIndex].val.ptr.lfDouble);
				break;
			case DT_STRING: 
				{
					sprintf(szbuf, tmpVec[i].c_str(), args[infoVec[i - 1].nIndex].val.ptr.pStr);
				}
				break;
			}
			tmpVec[i] = szbuf;
		}
		for (int i = 0; i < tmpVec.size(); i++)
			strcat(destr, tmpVec[i].c_str());
	}
	else
		strcpy(destr, sql);
	return destr;
}
