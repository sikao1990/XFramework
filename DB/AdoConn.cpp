#include "AdoConn.h"
#include "DBDefine.h"

DEFINENODE(AdoConn, "AdoConn", DBConnect, ALLOCMEM);

bool AdoConn::InitLib()
{
	CoInitialize(NULL);
	return true;
}

bool AdoConn::UninitLib()
{
	CoUninitialize();
	return true;
}

AdoConn::AdoConn()
{
	m_pConn = NULL;
}

AdoConn::~AdoConn()
{
}

bool AdoConn::InitConnecion(void* p)
{
	DBParam* pParam = (DBParam*)p;
	m_DBIP = pParam->param._adoParam.m_dbIp;
	m_user = pParam->param._adoParam.m_user;
	m_passwd = pParam->param._adoParam.m_passwd;
	m_dbStr = pParam->param._adoParam.m_dbBase;
	m_DBName = pParam->param._adoParam.m_dbName;
	return true;
}

bool AdoConn::ConnectDB()
{
	try {
		HRESULT nRet;
		nRet = m_pConn.CreateInstance(__uuidof(Connection));
		if (FAILED(nRet))
			return false;
		char strConn[512] = {};
		sprintf(strConn,m_dbStr.c_str(), m_DBName.c_str(),m_DBIP.c_str());
		nRet = m_pConn->Open(strConn,m_user.c_str(), m_passwd.c_str(), -1);
		if (FAILED(nRet))
			return false;
	}
	catch (const _com_error& e)
	{
		return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void AdoConn::DisConnectDB()
{
	try {
		if (NULL != m_pConn)
		{
			m_pConn->Close();
			m_pConn.Release();
		}
	}
	catch (...)
	{
		return;
	}
}

int AdoConn::BeginTrans()
{
	try {
		m_pConn->BeginTrans();
	}
	catch (...) {
		return -1;
	}
	return 0;
}

int AdoConn::CommitTrans(bool b)
{
	try {
		b ? m_pConn->CommitTrans() : m_pConn->RollbackTrans();
	}
	catch (...) {
		return -1;
	}
	return 0;
}


bool AdoConn::SetAdoBinaryData(_variant_t* pVarChunk, void* lpData, int nBytes)
{
	HRESULT hr;
	long lngOffset = 0;
	UCHAR chData;
	SAFEARRAY FAR *psa = NULL;
	SAFEARRAYBOUND rgsabound[1];

	//Create a safe array to store the array of BYTES 
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = nBytes;
	psa = SafeArrayCreate(VT_UI1, 1, rgsabound);

	while (lngOffset < (long)nBytes)
	{
		chData = ((UCHAR*)lpData)[lngOffset];
		hr = SafeArrayPutElement(psa, &lngOffset, &chData);

		if (FAILED(hr))
			return false;

		lngOffset++;
	}
	lngOffset = 0;

	//Assign the Safe array  to a variant. 
	pVarChunk->vt = VT_ARRAY | VT_UI1;
	pVarChunk->parray = psa;

	return true;
}

int AdoConn::GetBinaryByAdo(const _RecordsetPtr& pSet, const char* colName, DBField* pStart, long lDataSize)
{
	try {
		pStart->ptr.pBlob = (unsigned char*)new char[lDataSize];
		pStart->len = lDataSize;
		pStart->memType = 0;
		pStart->fieldType = DT_BLOB;
		_variant_t varBLOB;
		varBLOB = pSet->GetCollect(colName);
		if (varBLOB.vt == (VT_ARRAY | VT_UI1))
		{
			char* pBuf = NULL;
			SafeArrayAccessData(varBLOB.parray, (void **)&pBuf);
			memcpy(pStart->ptr.pBlob, pBuf, lDataSize);
			SafeArrayUnaccessData(varBLOB.parray);
			return lDataSize;
		}
	}
	catch (_com_error &e) {
		cout << "connectPtr execute failed:" << e.Description();
	}
	return 0;
}
