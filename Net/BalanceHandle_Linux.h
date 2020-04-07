#ifndef __BalanceHandle_Linux_H__
#define __BalanceHandle_Linux_H__

#include <list>
#include <vector>
#include <stdio.h>
#include "XNetCfg.h"
#include "SockHandle.h"
#include "NetPackage.h"
#include "SocketSession.h"
#include "../Thread/SemaphoreImpl_Linux.h"
#include "../Thread/RWLockImpl_Linux.h"
#include "../Thread/TimerMgrImpl_Linux.h"
#include "../Thread/ThreadCtrlExImpl_Linux.h"

class ThreadPri;
class SockBase;
class SessionInterface;

struct IterRange
{
	std::list<SocketSession*>::iterator 	itrStart;
	std::list<SocketSession*>::iterator 	itrEnd;
	IterRange() {}
	void SetStart(std::list<SocketSession*>::iterator iter){ itrStart = iter; }
	void SetEnd(std::list<SocketSession*>::iterator iter){ itrEnd = iter; }
};

class BalanceHandle{
	class SessionCampare
	{
	public:
		SessionCampare(const SockHandle& hSock):m_socket(hSock.m_sock),m_ip(hSock.m_ip){}
		SessionCampare(const SockHandle* hSock):m_socket(hSock->m_sock),m_ip(hSock->m_ip){}
		bool operator()(const SocketSession* other)const
		{
			return  other->getSock() == m_socket;
		}
	private:
		hSockFd m_socket;
		int	   	m_type;
		string 	m_ip;
	};		
public:	
	BalanceHandle();
	~BalanceHandle();

	bool Start(SockBase* pBase);
	bool Stop();
	bool SetNetParam(SessionInterface* Interface);
	void RecvClientData(const SockHandle& s,char* pBuf, int len, int nIndex, void* pPrev,void* pPostNode);
	void ActivateWrite(const SockHandle& s, int index, void* pBuf, int len,void* pPostNode);
	void KickClient(const SockHandle& s);
	//新客户端连入
	void AppendNewClient(const SockHandle& s);
	//客户端退出
	void RemoveClient(const SockHandle& s);
	void* Alloc(const SockHandle& s, int len);
	void Free(const SockHandle& s, void* p);
	//interface for user,copy to p,and will destroy source data;
	int RecvNetPackage(const SockHandle& s,NetPackage& p);
	//interface for user,p was alloc by user
	int SendNetPackage(const SockHandle& s,const NetPackage& p);
private:
	BalanceHandle(const BalanceHandle&){}
	BalanceHandle& operator=(const BalanceHandle&) { return *this; }
	
	void InitIterator(int count);
	void DealData(std::list<SocketSession*>::iterator begin, const std::list<SocketSession*>::iterator end, int id);

	void run(void* p);
	void runImpl(int index);
	void HeartBitHandle(void* p);
	void LoseHeartBitThread();
private:
	unsigned short			m_WorkorFlag;
	unsigned short 			m_bStart;
	unsigned short			m_HeartBitFlag;
	SessionInterface*		m_interface;
	SockBase*				m_pBase;
	list<SocketSession*>	m_clientInfos;
	IterRange				m_itrs[THREAD_COUNT];
	ThreadPri*				m_th[THREAD_COUNT];
	AbstractTimer*			m_heartbit[THREAD_COUNT];
	SemaphoreImpl			m_sem[THREAD_COUNT];
	vector<SockHandle>		m_lostHeartbit;
	RWLockImpl				m_rwLock;
	TimerMgrImpl			m_timerMgr;
	ThreadCtrlExImpl		m_heartCtrl;
	ThreadPri*				m_LoseBitTh;
};
#endif	// BalanceHandle_Linux.h
