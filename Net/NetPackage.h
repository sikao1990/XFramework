#ifndef __NetPackage_H__
#define __NetPackage_H__

#include "SockHandle.h"
#include <string.h>

struct NetPackage  
{
public:
	NetPackage(void* p=NULL,int len=0):m_pData(p),m_len(len), m_timestamp(0),m_nBaseProtol(0), m_nState(0), m_nVerifyLen(0){}
	~NetPackage() { }
	int GetLen() const { return m_len; }
	void* GetData()const { return m_pData; }
	bool CopyData(void* pStart, int len) {
		return (len <= m_len && NULL!=m_pData && NULL!=pStart) ? 
			( memcpy(m_pData, pStart, len),true ) :	false;
	}
	long long GetTimestamp()const { return m_timestamp; }
	bool IsValid()const{ return NULL!=m_pData; }
	void SetBaseProtol(unsigned char nProtol) { m_nBaseProtol = nProtol; }
	void SetPackageState(unsigned char nState) { m_nState = nState; }
	void SetUserProtol(unsigned short nProtol) { m_nUserProtol = nProtol; }
	void SetVerifyLen(int nlen) { m_nVerifyLen = nlen; }
	bool IsBaseProtol()const { return m_nBaseProtol; }
public:
	int			m_len;
	unsigned	m_nBaseProtol	: 8,
				m_nState		: 8,
				m_nUserProtol	: 16;
	int			m_nVerifyLen;
	void*		m_pData;
	long long	m_timestamp;
};

#endif	// NetPackage.h
