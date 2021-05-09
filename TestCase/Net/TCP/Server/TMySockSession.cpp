#include "TMySockSession.h"
#include "NetMsg.h"

TMySockSession::TMySockSession():m_poolLock(0) {
	m_capacity = 4096;
	m_srcLen = 0;
	m_pTmpBuf = new char[4096];
	m_rMemPoolBuf = new char[50 * NETMSGSIZE];
	m_count = m_MaxFreeIndex = 49;
	memset(m_rMemPool, 0, sizeof(m_rMemPool));
	for (int i = 0; i < 50; i++)
		m_rMemPool[i]=(m_rMemPoolBuf + i * NETMSGSIZE);
}

TMySockSession::~TMySockSession()
{
	delete[] m_pTmpBuf;
	delete[] m_rMemPoolBuf;
}

bool TMySockSession::SetPackageInfo(void* buf, int len, int srcLen)
{
	m_pOwnbuf = (char*)buf;
	m_Size = len;
	return true;
}

bool TMySockSession::encode()
{
	return true;
}
int TMySockSession::uncompress()
{
	return 1;
}


int TMySockSession::code(const char* src, int srclen, int len)
{
	//MsgFrame* pFrmae = (MsgFrame*)src;
	memcpy(m_pTmpBuf, src, m_srcLen = srclen);
	//¼ÓÃÜ
	//printf("Handle::code data:%s,len:%d\n", m_pTmpBuf,srclen);
	return m_srcLen;
}

int TMySockSession::compress(const char* src, int srclen, int len)
{
	//TODO:Ñ¹Ëõ
	return m_srcLen;
}

void* TMySockSession::getData(int& len)
{
	len = m_srcLen;
	return m_pTmpBuf;
}

void* TMySockSession::Alloc(int len)
{
	void* p = NULL;
	while (::InterlockedCompareExchange((LPLONG)&m_poolLock, 1, 0) != 0) Sleep(0);
	if (m_count > 0) {
		p = m_rMemPool[--m_count];//TODO:
	}
	printf("Alloc MemPool recv count:%d,use:%d\n", GetRecvPackageCount(), m_count);
	::InterlockedExchange(&m_poolLock, 0);
	if (NULL == p)
		throw "memBuf empty";
	memset(p, 0, NETMSGSIZE);
	return p;
}

void TMySockSession::Free(void* p)
{
	while (::InterlockedCompareExchange((LPLONG)&m_poolLock, 1, 0) != 0) Sleep(0);
	++m_count;
	printf("Free MemPool recv count:%d,use:%d\n", GetRecvPackageCount(), m_count);
	::InterlockedExchange(&m_poolLock, 0);
}
