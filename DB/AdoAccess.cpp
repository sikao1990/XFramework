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
	catch (_com_error &e) {
		//printf("connectPtr execute failed:%s\n", );
		cout << "connectPtr execute failed:" << e.Description();
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
	pSql = Fromat(strSql, sql,sizeof(strSql),args, argTypeList, ap, '?', '%');
	va_end(ap);
	if (!strlen(pSql)||pSql != strSql)
		return -2;
	if (args.size() != argTypeList.size())
		return -3;
	//处理 ?
	try {
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		for (int i = 0; i < args.size(); i++)
		{
			if (0 == args[i].nFlag)continue;
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
					bool nflag = pAdoConn->SetAdoBinaryData(&varBin,args[i].val.ptr.pBlob, args[i].val.len);
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
	catch (_com_error &e)
	{
		cout << "connectPtr execute failed:" << e.Description();
		return 1;
	}
	return 0;
}
//test code
int GetBinaryByAdo(const _RecordsetPtr& pSet, const char* colName, char** ppBuf)
{
	long lDataSize = pSet->GetFields()->GetItem(colName)->ActualSize;
	if (lDataSize <= 0)
		return 0;
	*ppBuf = new char[lDataSize];
	_variant_t varBLOB;
	varBLOB = pSet->GetCollect(colName);
	if (varBLOB.vt == (VT_ARRAY | VT_UI1))
	{
		char* pBuf = NULL;
		SafeArrayAccessData(varBLOB.parray, (void **)&pBuf);
		memcpy(*ppBuf, pBuf, lDataSize);
		SafeArrayUnaccessData(varBLOB.parray);
		return lDataSize;
	}
	return 0;
}

int AdoAccess::Query(const char* sql, list< list<DBField> >& result)
{
#if 0
	int nRet = 0;
	_RecordsetPtr pSet = NULL;
	AdoConn* pAdoConn = dynamic_cast<AdoConn*>(m_connect);
	if (0!=(nRet=DBAccess::Query(sql,result)))
		return nRet;
	try {
		pSet.CreateInstance(__uuidof(Recordset));
		pSet = pAdoConn->m_pConn->Execute(_bstr_t(sql), NULL, adCmdText);

		pSet->MoveFirst();
		while (!pSet->GetAdoEOF()) 
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
				case adWChar:
					{
						string strVal = (_bstr_t)pSet->GetFields()->GetItem(i)->Value;
						tmpField.fieldType = DT_STRING;
						tmpField.len = strVal.size();
						tmpField.memType = 0;
						strcpy(tmpField.ptr.pStr = new char[tmpField.len + 1], strVal.c_str());
					}
					break;
				case adBinary:
					{
					long lDataSize = pSet->GetFields()->GetItem(tmpField.fieldName.c_str())->ActualSize;
					if (lDataSize <= 0)
						return 0;
						int c = pAdoConn->GetBinaryByAdo(pSet, tmpField.fieldName.c_str(), &tmpField, lDataSize);
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
#else//test code
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));

	_RecordsetPtr pRecordset;
	_variant_t cls;
	string last_pcls;

	if (FAILED(hr))
		return -1;
	try
	{
		// Open方法连接字串必须是BSTR或者_bstr_t类型  
		//Provider的属性对于access和sqlserver有所不同. access数据库用"Provider=Microsoft.ACE.OLEDB.12.0;Data Source=./data/studetn.mdb;"这种格式链接数据库，下面的格式是sqlserver格式
		HRESULT hr = m_pConnect->Open("Provider=SQLOLEDB.1;Password=zhubin123;Persist Security Info=True;User ID=sa;Initial Catalog=test;Data Source=localhost", "", "", -1);
		if (!SUCCEEDED(hr)) {
			cout << " connect failed! " << endl;
			return -1;
		}
	}
	catch (_com_error &e)
	{
		cout << "open sql server failed " << e.Description() << endl;
		return -1;
	}//发生链接错误 


	if (FAILED(pRecordset.CreateInstance("adodb.Recordset")))
	{
		return -1;
	}
	try
	{
		//将查询数据导入m_pRecordset数据容器
		pRecordset = m_pConnect->Execute(_bstr_t("select  * from example_blob"), NULL, adCmdText);
	}
	catch (_com_error &e)
	{
		cout << "execute sql failed " << e.Description() << endl;
		return -1;
	}
	cout << "connect sql server success" << endl;

	if (!pRecordset->GetAdoEOF())//有记录
	{
		try
		{
			//pRecordset->MoveFirst();//移动到第一条记录
			while (!pRecordset->GetAdoEOF())//还没到结尾
			{
				cls = pRecordset->GetFields()->GetItem("id")->Value;
				//cls = pRecordset->GetFields()->GetItem("aval")->Value;
				//last_pcls = _bstr_t(cls);
				long lDataSize = pRecordset->GetFields()->GetItem("pic")->ActualSize;
				char* pData = NULL;//new char[lDataSize];
				int result = 0;//int(cls);
							   //FieldPtr pField = pRecordset->Fields->GetItem("pic");
				puts("----------");
				//_variant_t varBLOB = pRecordset->GetFields()->GetItem("pic")->GetChunk(lDataSize);
#if 0                
				_variant_t varBLOB = pRecordset->GetCollect("pic");
				if (varBLOB.vt == (VT_ARRAY | VT_UI1))
				{
					char* pBuf = NULL;
					SafeArrayAccessData(varBLOB.parray, (void **)&pBuf);
					memcpy(pData, pBuf, lDataSize);
					SafeArrayUnaccessData(varBLOB.parray);
				}
#else
				result = GetBinaryByAdo(pRecordset, "pic", &pData);
#endif    
				if (lDataSize>0) {
					printf("---------->%x\n", pData[0]);
				}
				//cout << (last_pcls.c_str()) << " ";
				cout << "===>" << result << endl;
				// 取下一条记录  
				pRecordset->MoveNext();
			}
		}
		catch (_com_error &e)
		{
			cout << "get data error!" << e.Description() << endl;
			return -1;
		}
	}
#endif
	return 0;
}

int AdoAccess::Query(const char* sql, const vector<FieldInfo>& argTypeList,list< list<DBField> >& result, TagType type, ...)
{
	int nRet = 0;
	if (0!=(nRet=DBAccess::Query(sql,argTypeList,result,type)))
		return nRet;

	va_list ap;
	_RecordsetPtr pSet = NULL;
	AdoConn* pAdoConn = dynamic_cast<AdoConn*>(m_connect);
	vector<ArgType>	args;
	char strSql[512] = {};
	const char* pSql = NULL;

	va_start(ap, type);
	//拼接sql语句,处理 %
	pSql = Fromat(strSql, sql, sizeof(strSql), args, argTypeList, ap, '?', '%');
	va_end(ap);
	if (!strlen(pSql) || pSql != strSql)
		return -2;
	if (args.size() != argTypeList.size())
		return -3;

	//处理 ?
	try {
		pSet.CreateInstance(__uuidof(Recordset));
		_CommandPtr cmd;
		cmd.CreateInstance(__uuidof(Command));
		for (int i = 0; i < args.size(); i++)
		{
			if (0 == args[i].nFlag)continue;
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
				bool nflag = pAdoConn->SetAdoBinaryData(&varBin, args[i].val.ptr.pBlob, args[i].val.len);
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

		pSet = cmd->Execute(NULL, NULL, adCmdText);
		cmd.Release();
	}
	catch (...)
	{
	}
	try{
		//pSet->MoveFirst();
		while (!pSet->GetAdoEOF())
		{
			list<DBField> lstFields;
			int nFields = pSet->GetFields()->GetCount();
			for (LONG i = 0; i < nFields; i++)
			{
				DBField tmpField;
				FieldPtr columnPtr = pSet->GetFields()->GetItem(i);
				tmpField.fieldName = columnPtr->GetName();
				long lDataSize = columnPtr->ActualSize;
				int colType = columnPtr->GetType();
				if (lDataSize <= 0)
					return 0;
				switch (columnPtr->GetType())
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
				case adVarChar:
				case adWChar:
					{
						string strVal = (_bstr_t)pSet->GetFields()->GetItem(i)->Value;
						tmpField.fieldType = DT_STRING;
						tmpField.len = strVal.size();
						tmpField.memType = 0;
						strcpy(tmpField.ptr.pStr = new char[tmpField.len + 1],strVal.c_str());
						break;
					}			
				case adBinary:
					{
						tmpField.fieldType = DT_BLOB;
						int c = pAdoConn->GetBinaryByAdo(pSet, tmpField.fieldName.c_str(), &tmpField, lDataSize);
						if (c <= 0)
							throw "Get Binary data error";
						break;
					}
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

