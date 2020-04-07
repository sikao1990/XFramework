#ifndef __BalanceHandle_Win32_H__
#define __BalanceHandle_Win32_H__

#include <list>
#include <vector>
#include "SockHandle.h"
#include "NetPackage.h"
#include "SocketSession.h"

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

class BalanceHandle
{
	struct threadParam 		{
		threadParam() :pthis(NULL), hTimer(NULL) {}
		BalanceHandle*	pthis;
		int				index;
		HANDLE			hTimer;
	};
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

	void run(int index);
	static unsigned WINAPI HeartbitMonitor(void* p);
	static unsigned WINAPI ThreadProc(void* p);
	static VOID CALLBACK TimerRoutine( PVOID lpParameter, BOOLEAN TimerOrWaitFired );	
private:
	unsigned				m_WorkorFlag : 8,
							m_HeartBitFlag : 8,
							m_bStart:8,
							:8;
	SRWLOCK 				m_lock;
	HANDLE					m_hTimerQueue;
	CRITICAL_SECTION		m_lostLock;
	
	HANDLE					m_HeartbitTh;
	CONDITION_VARIABLE		m_HeartbitSig;
	SessionInterface*		m_interface;
	SockBase*				m_pBase;
	list<SocketSession*>	m_clientInfos;
	vector<SockHandle>		m_lostHeartbit;

	IterRange				m_itrs[THREAD_COUNT];
	threadParam				m_params[THREAD_COUNT];
	CONDITION_VARIABLE		m_HasData[THREAD_COUNT];
	HANDLE 					m_th[THREAD_COUNT];	
};


#endif	// BalanceHandle_Win32.h
