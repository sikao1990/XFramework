#ifndef __TMySockSession_H__
#define __TMySockSession_H__
#include "../../Framework/Net/SocketSession.h"

class TMySockSession : public SocketSession
{
public:
	TMySockSession();
	~TMySockSession();

	int code(const char* src, int srclen, int len = 0);
	int compress(const char* src, int srclen, int len = 0);
	void* getData(int& len);

	bool SetPackageInfo(void* buf, int len, int srcLen);
	bool encode();
	int uncompress();

	void* Alloc(int len);
	void Free(void* p);
private:
	int		m_Size;
	//for deal recv	,must diff from send buf
	char*	m_pOwnbuf;

	//for deal send
	char*	m_pTmpBuf;
	int		m_capacity;
	int		m_srcLen;
	int						m_count;
	volatile long			m_poolLock; 
	volatile int			m_MaxFreeIndex;
	char*					m_rMemPoolBuf;
	char*					m_rMemPool[50];					
private:
	TMySockSession(const TMySockSession&) {}
	TMySockSession& operator=(const TMySockSession&) { return *this; }
};


#endif	//TMySockSession.h
