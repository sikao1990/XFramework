#include "AdoAccess.h"
#include "AdoConn.h"
#include <stdarg.h>

DEFINENODE(AdoAccess, "AdoAccess", DBAccess, ALLOCMEM);

AdoAccess::AdoAccess()
{

}

AdoAccess::~AdoAccess()
{

}

bool AdoAccess::InitAccessParam(void* p)
{
	//TODO:
	return true;
}

int AdoAccess::Execute(const char* sql)
{
	int nRet = 0;
	AdoConn* pAdoConn = dynamic_cast<AdoConn*>(m_connect);
	if (0!=(nRet=DBAccess::Execute(sql)))
		return nRet;
	try {
		pAdoConn->m_pConn->Execute(_bstr_t(sql), NULL, adCmdText);
	}
	catch (...) {
		return 1;
	}
	return 0;
}
//暂时仅支持sql，不处理存储过程
int AdoAccess::Execute(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, ...)
{
	int nRet = 0;
	va_list ap;
	AdoConn* pAdoConn = dynamic_cast<AdoConn*>(m_connect);
	vector<ArgType>	args;
	char strSql[1024] = {};
	const char* pSql = NULL;
	
	if (0!=(nRet=DBAccess::Execute(sql,argTypeList,type)) )
		return nRet;

	va_start(ap, type);
	//拼接sql语句,处理 %
	pSql = Fromat(sql, strSql,sizeof(strSql),args, argTypeList, ap, '?', '%');
	va_end(ap);
	if (pSql != strSql)
		return -2;
	if (args.size() != argTypeList.size())
		return -3;
	//去除 % 参数
	for (vector<ArgType>::iterator  it = args.begin();!args.empty();)
	{
		if (-1 == it->nFlag)
			it = args.erase(it);
		else
			it++;
	}
	//处理 ?
	try {
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		for (int i = 0; i < args.size(); i++)
		{
			_ParameterPtr param;
			param.CreateInstance("ADODB.Parameter");
			switch (args[i].val.fieldType) {
			case DT_INT:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adInteger, adParamInput, sizeof(int), _variant_t(args[i].val.ptr.nInt32));
				break;
			case DT_INT64:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adBigInt, adParamInput, sizeof(long long), _variant_t(args[i].val.ptr.nInt64));
				break;
			case DT_DOUBLE:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adDouble, adParamInput, sizeof(double), _variant_t(args[i].val.ptr.lfDouble));
				break;
			case DT_STRING:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adBSTR, adParamInput, args[i].val.len, _variant_t(args[i].val.ptr.pStr));
				break;
			case DT_BLOB:
				{
					_variant_t varBin;
					bool nflag = pAdoConn->GetAdoBinaryData(&varBin,args[i].val.ptr.pBlob, args[i].val.len);
					if (nflag)
						param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adBinary, adParamInput, args[i].val.len, varBin);
					else
						throw "Write Binary data error";
				}
				break;
			}
			cmd->Parameters->Append(param);
		}
		cmd->ActiveConnection = pAdoConn->m_pConn;
		cmd->CommandText = _bstr_t(pSql);
		cmd->Execute(NULL, NULL, adCmdText);
	}
	catch (...)
	{
		return 1;
	}
	return 0;
}

int AdoAccess::Query(const char* sql, list< list<DBField> >& result)
{
	int nRet = 0;
	_RecordsetPtr pSet = NULL;
	AdoConn* pAdoConn = dynamic_cast<AdoConn*>(m_connect);
	if (0!=(nRet=DBAccess::Query(sql,result)))
		return nRet;
	try {
		pSet.CreateInstance(__uuidof(Recordset));
		pSet = pAdoConn->m_pConn->Execute(_bstr_t(sql), NULL, adCmdText);

		pSet->MoveFirst();
		while (pSet->GetAdoEOF()) 
		{
			list<DBField> lstFields;
			int nFields = pSet->GetFields()->GetCount();
			for (LONG i = 0; i < nFields; i++)
			{
				DBField tmpField;
				tmpField.fieldName = pSet->GetFields()->GetItem(i)->GetName();
				switch (pSet->GetFields()->GetItem(i)->GetType())
				{
				case adInteger:					
					tmpField.fieldType = DT_INT;
					tmpField.ptr.nInt32 = pSet->GetFields()->GetItem(i)->Value;
					break;
				case adBigInt:
					tmpField.fieldType = DT_INT64;
					tmpField.ptr.nInt64 = pSet->GetFields()->GetItem(i)->Value;
					break;
				case adDouble:
					tmpField.fieldType = DT_DOUBLE;
					tmpField.ptr.lfDouble = pSet->GetFields()->GetItem(i)->Value;
					break;
				case adBSTR:
					{
						string strVal = (_bstr_t)pSet->GetFields()->GetItem(i)->Value;
						tmpField.fieldType = DT_STRING;
						tmpField.len = strVal.size();
						tmpField.memType = 0;
						strcpy_s(tmpField.ptr.pStr = new char[tmpField.len + 1], tmpField.len, strVal.c_str());
					}
					break;
				case adBinary:
					{
						int c = pAdoConn->GetBinaryByAdo(pSet->GetFields()->GetItem(i), &tmpField);
						if (c <= 0)
							throw "Get Binary data error";
					}
					break;
				}
				lstFields.push_back(tmpField);
			}
			result.push_back(lstFields);
			pSet->MoveNext();
		}
		pSet->Close();
	}
	catch (...) {
		return 1;
	}
	return 0;
}

int AdoAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList, TagType type, list< list<DBField> >& result, ...)
{
	int nRet = 0;
	if (0!=(nRet=DBAccess::Query(sql,argTypeList,type,result)))
		return nRet;

	va_list ap;
	_RecordsetPtr pSet = NULL;
	AdoConn* pAdoConn = dynamic_cast<AdoConn*>(m_connect);
	vector<ArgType>	args;
	char strSql[1024] = {};
	const char* pSql = NULL;

	va_start(ap, type);
	//拼接sql语句,处理 %
	pSql = Fromat(sql, strSql, sizeof(strSql), args, argTypeList, ap, '?', '%');
	va_end(ap);
	if (pSql != strSql)
		return -2;
	if (args.size() != argTypeList.size())
		return -3;
	//去除 % 参数
	for (vector<ArgType>::iterator it = args.begin(); !args.empty();)
	{
		if (-1 == it->nFlag)
			it = args.erase(it);
		else
			it++;
	}
	//处理 ?
	try {
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		for (int i = 0; i < args.size(); i++)
		{
			_ParameterPtr param;
			param.CreateInstance("ADODB.Parameter");
			switch (args[i].val.fieldType) {
			case DT_INT:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adInteger, adParamInput, sizeof(int), _variant_t(args[i].val.ptr.nInt32));
				break;
			case DT_INT64:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adBigInt, adParamInput, sizeof(long long), _variant_t(args[i].val.ptr.nInt64));
				break;
			case DT_DOUBLE:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adDouble, adParamInput, sizeof(double), _variant_t(args[i].val.ptr.lfDouble));
				break;
			case DT_STRING:
				param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adBSTR, adParamInput, args[i].val.len, _variant_t(args[i].val.ptr.pStr));
				break;
			case DT_BLOB:
			{
				_variant_t varBin;
				bool nflag = pAdoConn->GetAdoBinaryData(&varBin, args[i].val.ptr.pBlob, args[i].val.len);
				if (nflag)
					param = cmd->CreateParameter((_bstr_t)(args[i].val.fieldName.c_str()), adBinary, adParamInput, args[i].val.len, varBin);
				else
					throw "Write Binary data error";
			}
			break;
			}
			cmd->Parameters->Append(param);
		}
		cmd->ActiveConnection = pAdoConn->m_pConn;
		cmd->CommandText = _bstr_t(pSql);
		pSet.CreateInstance(__uuidof(Recordset));
		pSet = cmd->Execute(NULL, NULL, adCmdText);

		pSet->MoveFirst();
		while (pSet->GetAdoEOF())
		{
			list<DBField> lstFields;
			int nFields = pSet->GetFields()->GetCount();
			for (LONG i = 0; i < nFields; i++)
			{
				DBField tmpField;
				tmpField.fieldName = pSet->GetFields()->GetItem(i)->GetName();
				switch (pSet->GetFields()->GetItem(i)->GetType())
				{
				case adInteger:
					tmpField.fieldType = DT_INT;
					tmpField.ptr.nInt32 = pSet->GetFields()->GetItem(i)->Value;
					break;
				case adBigInt:
					tmpField.fieldType = DT_INT64;
					tmpField.ptr.nInt64 = pSet->GetFields()->GetItem(i)->Value;
					break;
				case adDouble:
					tmpField.fieldType = DT_DOUBLE;
					tmpField.ptr.lfDouble = pSet->GetFields()->GetItem(i)->Value;
					break;
				case adBSTR:
				{
					string strVal = (_bstr_t)pSet->GetFields()->GetItem(i)->Value;
					tmpField.fieldType = DT_STRING;
					tmpField.len = strVal.size();
					tmpField.memType = 0;
					strcpy_s(tmpField.ptr.pStr = new char[tmpField.len + 1], tmpField.len, strVal.c_str());
				}
				break;
				case adBinary:
				{
					int c = pAdoConn->GetBinaryByAdo(pSet->GetFields()->GetItem(i), &tmpField);
					if (c <= 0)
						throw "Get Binary data error";
				}
				break;
				}
				lstFields.push_back(tmpField);
			}
			result.push_back(lstFields);
			pSet->MoveNext();
		}
		pSet->Close();
	}
	catch (...)
	{
		return 1;
	}
	return 0;
}

