#include "BalanceHandle_Win32.h"
#include "SessionInterface.h"
#include <process.h>
#include <algorithm>

BalanceHandle::BalanceHandle() :m_WorkorFlag(TRUE), m_HeartBitFlag(TRUE), m_bStart(false)
{	
	InitializeSRWLock(&m_lock); 
	InitializeConditionVariable(&m_HeartbitSig);
	InitializeCriticalSection(&m_lostLock);
	for(int i=0;i<THREAD_COUNT;i++)
		InitializeConditionVariable(m_HasData+i);

	for(int i=0;i<THREAD_COUNT;i++){
		m_itrs[i].SetStart(m_clientInfos.end());
		m_itrs[i].SetEnd(m_clientInfos.end());
	}
	
	m_hTimerQueue = NULL;
	if(NULL == (m_hTimerQueue = CreateTimerQueue()) )
		throw "createTimerQueue failed";
}

BalanceHandle::~BalanceHandle()
{	
	if(m_bStart)Stop();
}

bool BalanceHandle::Start(SockBase* pBase)
{
	for (int i = 0; i<THREAD_COUNT; i++)
		m_params[i].pthis = this, m_params[i].index = i, m_params[i].hTimer = NULL;
	for (int i = 0; i<THREAD_COUNT; i++)
		m_th[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (void*)(m_params + i), 0, NULL);
#if 0
	m_HeartbitTh = (HANDLE)_beginthreadex(NULL, 0, HeartbitMonitor, (void*)this, 0, NULL);
	for (int i = 0; i<THREAD_COUNT; i++)
		if (!CreateTimerQueueTimer(&m_params[i].hTimer, m_hTimerQueue, WAITORTIMERCALLBACK(TimerRoutine), (void*)(m_params + i), 2000, HEARTBIT_INTERVAL * 1000, NULL))
			return FALSE;
#endif
	m_bStart = true;
	m_pBase = pBase;
	return TRUE;
}

bool BalanceHandle::Stop()
{
	for (int i = 0; i < THREAD_COUNT; i++)
		DeleteTimerQueueTimer(m_hTimerQueue, m_params[i].hTimer, NULL);
	m_HeartBitFlag = FALSE;
	WakeConditionVariable(&m_HeartbitSig);
	WaitForSingleObject(m_HeartbitTh, INFINITE);
	DeleteTimerQueue(m_hTimerQueue);
	DeleteCriticalSection(&m_lostLock);

	m_WorkorFlag = FALSE;
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		if (m_itrs[i].itrStart != m_itrs[i].itrEnd) {
			WakeAllConditionVariable(m_HasData + i);
		}
	}
	WaitForMultipleObjects(THREAD_COUNT, m_th, TRUE, INFINITE);

	list<SocketSession*>::iterator it = m_clientInfos.begin();
	for (; it != m_clientInfos.end(); it++)
		delete *it;
	m_clientInfos.clear();
	m_bStart = false;
	return TRUE;
}

bool BalanceHandle::SetNetParam(SessionInterface* Interface)
{ 
	if (NULL == Interface )
		return FALSE;
	m_interface = Interface; 
	return TRUE;
}

void BalanceHandle::InitIterator(int count)
{
	int pos = m_clientInfos.size()/count;
	std::list<SocketSession*>::iterator it=m_clientInfos.begin();
	std::list<SocketSession*>::iterator istart=m_clientInfos.begin();
	std::list<SocketSession*>::iterator iend=m_clientInfos.end();
	for(int i=0;i<count;i++)
	{
		m_itrs[i].SetStart(m_clientInfos.end());
		m_itrs[i].SetEnd(m_clientInfos.end());
	}
	for(int i=0;i<count;i++){
		if(0==i)
			istart = m_clientInfos.begin();
		else
			istart = it;
		for(int j=0;j<pos;j++)
			it++;		
		if(count-1 == i)
			iend = m_clientInfos.end();
		else
			iend = it;
		m_itrs[i].SetStart(istart);
		m_itrs[i].SetEnd(iend);
	}
}

unsigned BalanceHandle::ThreadProc(void* p)
{
	threadParam* pParam = reinterpret_cast<threadParam*>(p);
	BalanceHandle* pthis=pParam->pthis;
	if (NULL == pthis->m_interface)return 1;
	puts("The workor thread was start------------------->");
	while(pthis->m_WorkorFlag){
		(pthis->run)(pParam->index);
		Sleep(1);
	}
	return 0;
}

void BalanceHandle::DealData(std::list<SocketSession*>::iterator begin, const std::list<SocketSession*>::iterator end, int id)
{
	for (; begin != end;) {
		(*begin)->ParseData();
		(*begin++)->PostSend();
	}
}

void BalanceHandle::run(int index)
{
	AcquireSRWLockShared(&m_lock);
	if (m_itrs[index].itrStart == m_itrs[index].itrEnd) {
		//cout << "thread:" << GetCurrentThreadId() << " not data" << endl;
		puts("not data and sleep");
		SleepConditionVariableSRW(m_HasData + index, &m_lock, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
	}
	DealData(m_itrs[index].itrStart, m_itrs[index].itrEnd, GetCurrentThreadId());
	ReleaseSRWLockShared(&m_lock);
}

void BalanceHandle::KickClient(const SockHandle& hSock)
{
	closesocket(hSock.m_sock);
	AcquireSRWLockExclusive(&m_lock);
	std::list<SocketSession*>::iterator it = m_clientInfos.end();
	it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) {
		printf("the client:%d logoff\n", (*it)->getSock());	
		m_interface->DestroySocketSession(*it);
		m_clientInfos.erase(it);
		InitIterator(THREAD_COUNT);
		for (int i = 0; i < THREAD_COUNT; i++)
		{
			if (m_itrs[i].itrStart != m_itrs[i].itrEnd) {
				WakeAllConditionVariable(m_HasData + i);
			}
		}
	}
	ReleaseSRWLockExclusive(&m_lock);
	m_interface->ClientExit(hSock);
}

void BalanceHandle::AppendNewClient(const SockHandle& hSock)
{
	SocketSession* pSession = m_interface->GetNetSocketSession();
	if (NULL == pSession) 
		throw "can't new Client where connect";

	pSession->Init(hSock, SESSION_SERVER,m_pBase);
	AcquireSRWLockExclusive(&m_lock);
	m_clientInfos.push_back(pSession);
	InitIterator(THREAD_COUNT);
	for(int i=0;i<THREAD_COUNT;i++)
	{
		if(m_itrs[i].itrStart!=m_itrs[i].itrEnd){		
			WakeAllConditionVariable(m_HasData+i);
		}
	}
	ReleaseSRWLockExclusive(&m_lock);
	m_interface->ClientEnter(hSock);
	pSession->PostRecv();
}

void BalanceHandle::RemoveClient(const SockHandle& hSock)
{
	closesocket(hSock.m_sock);
	AcquireSRWLockExclusive(&m_lock);
	std::list<SocketSession*>::iterator it = m_clientInfos.end();
	it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) {
		printf("the client:%d logoff\n", (*it)->getSock());
		m_interface->DestroySocketSession(*it);
		m_clientInfos.erase(it);
		InitIterator(THREAD_COUNT);
		for (int i = 0; i < THREAD_COUNT; i++)
		{
			if (m_itrs[i].itrStart != m_itrs[i].itrEnd) {
				WakeAllConditionVariable(m_HasData + i);
			}
		}
	}
	ReleaseSRWLockExclusive(&m_lock);
	m_interface->ClientExit(hSock);
}

void BalanceHandle::RecvClientData(const SockHandle& hSock,char* pBuf, int len, int nIndex, void* pPrev,void* pPostNode)
{
	AcquireSRWLockShared(&m_lock);

	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		int ret = (*it)->DispatchData(pBuf,len,nIndex,pPrev,pPostNode);
	}				
	ReleaseSRWLockShared(&m_lock);		
}

void BalanceHandle::ActivateWrite(const SockHandle& hSock, int index, void* pBuf, int len,void* pPostNode)
{
	AcquireSRWLockShared(&m_lock);
	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		(*it)->NoticeComplateSend(index,pBuf,len,pPostNode);
	}			
	ReleaseSRWLockShared(&m_lock);		
}

int BalanceHandle::RecvNetPackage(const SockHandle& hSock,NetPackage& p)
{
	int ret = -1;
	AcquireSRWLockShared(&m_lock);

	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		ret=(*it)->RecvNetPackage(p);
	}	
		
	ReleaseSRWLockShared(&m_lock);		
	return ret;
}

void* BalanceHandle::Alloc(const SockHandle& hSock, int len)
{
	void* pNew = NULL;
	AcquireSRWLockShared(&m_lock);

	std::list<SocketSession*>::iterator it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) {
		pNew = (*it)->Alloc(len);
	}

	ReleaseSRWLockShared(&m_lock);
	return pNew;
}

void BalanceHandle::Free(const SockHandle& hSock, void* p)
{
	AcquireSRWLockShared(&m_lock);

	std::list<SocketSession*>::iterator it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) 
		(*it)->Free(p);

	ReleaseSRWLockShared(&m_lock);
}

int BalanceHandle::SendNetPackage(const SockHandle& hSock,const NetPackage& p)
{
	int ret = -1;
	AcquireSRWLockShared(&m_lock);

	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		ret=(*it)->SendNetPackage(p);
	}		
		
	ReleaseSRWLockShared(&m_lock);		
	return ret;
}

VOID BalanceHandle::TimerRoutine(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	threadParam* pParam = reinterpret_cast<threadParam*>(lpParameter);
	BalanceHandle* pthis = pParam->pthis;
	EnterCriticalSection(&pthis->m_lostLock);
	AcquireSRWLockShared(&pthis->m_lock);
	std::list<SocketSession*>::iterator it = pthis->m_itrs[pParam->index].itrStart;
	for (; it != pthis->m_itrs[pParam->index].itrEnd; it++)
	{
		if (time(0) - (*it)->getLastTime() > HEARTBIT_INTERVAL + HEARTBIT_INTERVAL / 10) {
			printf("%d lost heartbit\n", (*it)->getSock());
			pthis->m_lostHeartbit.push_back((*it)->GetSockHandle());
		}
	}
	WakeConditionVariable(&pthis->m_HeartbitSig);
	ReleaseSRWLockShared(&pthis->m_lock);
	LeaveCriticalSection(&pthis->m_lostLock);
}

unsigned BalanceHandle::HeartbitMonitor(void* p)
{
	BalanceHandle* pthis = reinterpret_cast<BalanceHandle*>(p);
	int count = 0;
	while (pthis->m_HeartBitFlag) {
		EnterCriticalSection(&pthis->m_lostLock);
		while(!pthis->m_lostHeartbit.size())SleepConditionVariableCS(&pthis->m_HeartbitSig, &pthis->m_lostLock, INFINITE);
			
		for (int i = 0; i < pthis->m_lostHeartbit.size(); i++) 
			pthis->KickClient(pthis->m_lostHeartbit[i]);
		pthis->m_lostHeartbit.clear();

		LeaveCriticalSection(&pthis->m_lostLock);
	}
	return 0;
}
