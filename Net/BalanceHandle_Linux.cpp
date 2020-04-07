#include "BalanceHandle_Linux.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "../SystemApi/ForceConvert.h"
#include "../Thread/ThreadImpl_Linux.h"
#include "SessionInterface.h"

BalanceHandle::BalanceHandle()
{
	for(int i=0;i<THREAD_COUNT;i++){
		m_itrs[i].SetStart(m_clientInfos.end());
		m_itrs[i].SetEnd(m_clientInfos.end());
	}
	m_timerMgr.Init();
	char szbuf[4]={};
	TimeSet test(2,HEARTBIT_INTERVAL);
	for(int i=0;i<THREAD_COUNT;i++){
		sprintf(szbuf,"%d",i);
		m_heartbit[i] = new Timer<BalanceHandle>(this,&BalanceHandle::HeartBitHandle,test,reinterpret_cast<void*>(i));
		m_th[i] = new ThreadImpl<BalanceHandle>(this,&BalanceHandle::run,reinterpret_cast<void*>(i));
		m_timerMgr.AddTimer(szbuf,m_heartbit[i]);
	}
	m_LoseBitTh = new ThreadImpl<BalanceHandle>(this,&BalanceHandle::LoseHeartBitThread);
}

BalanceHandle::~BalanceHandle()
{
	m_timerMgr.Stop();
	for(int i=0;i<THREAD_COUNT;i++){
		delete m_th[i];
		delete m_heartbit[i];
	}
	delete m_LoseBitTh;
}

bool BalanceHandle::Start(SockBase* pBase)
{
	if(m_bStart)return false;
	m_LoseBitTh->start();
	for(int i=0;i<THREAD_COUNT;i++)
		m_th[i]->start();
	m_bStart = true;
	m_pBase = pBase;
	return true;
}

bool BalanceHandle::Stop()
{
	if(!m_bStart)return false;
	m_bStart = false;
	m_HeartBitFlag = false;
	m_WorkorFlag = false;
	for(int i=0;i<THREAD_COUNT;i++)
		m_sem[i].SemPost();
	m_heartCtrl.WakeConsume(false);	
	m_LoseBitTh->stop();
	for(int i=0;i<THREAD_COUNT;i++){
		m_th[i]->stop();
	}
	list<SocketSession*>::iterator it = m_clientInfos.begin();
	for(;it!=m_clientInfos.end();){
		close( (*it)->getSock() );
		m_interface->DestroySocketSession(*it);
		it = m_clientInfos.erase(it);
	}
	return true;
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

void BalanceHandle::DealData(std::list<SocketSession*>::iterator begin, const std::list<SocketSession*>::iterator end, int id)
{
	for (; begin != end;) {
		(*begin)->ParseData();
		(*begin++)->PostSend();
	}
}

bool BalanceHandle::SetNetParam(SessionInterface* Interface)
{ 
	if (NULL == Interface )
		return false;
	m_interface = Interface; 
	return true;
}

void BalanceHandle::KickClient(const SockHandle& hSock)
{
	if(!m_bStart)return;
	close(hSock.m_sock);
	m_rwLock.LockWrite();
	std::list<SocketSession*>::iterator it = m_clientInfos.end();
	it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) {
		printf("the client:%d logoff\n", (*it)->getSock());
		m_interface->DestroySocketSession(*it);
		m_clientInfos.erase(it);
		InitIterator(THREAD_COUNT);
		for (int i = 0; i < THREAD_COUNT; i++)
		{
			if (m_itrs[i].itrStart != m_itrs[i].itrEnd)
				m_sem[i].SemPost();
		}
	}
	m_rwLock.UnlockWrite();
	m_interface->ClientExit(hSock);
}

void BalanceHandle::AppendNewClient(const SockHandle& hSock)
{
	if(!m_bStart)return;
	SocketSession* pSession = m_interface->GetNetSocketSession();
	if (NULL == pSession) 
		throw "can't new Client where connect";

	pSession->Init(hSock, SESSION_SERVER,m_pBase);
	m_rwLock.LockWrite();
	m_clientInfos.push_back(pSession);
	InitIterator(THREAD_COUNT);
	for(int i=0;i<THREAD_COUNT;i++)
	{
		if(m_itrs[i].itrStart!=m_itrs[i].itrEnd)	
			m_sem[i].SemPost();
	}
	m_rwLock.UnlockWrite();
	m_interface->ClientEnter(hSock);
	pSession->PostRecv();
}

void BalanceHandle::RemoveClient(const SockHandle& hSock)
{
	if(!m_bStart)return;
	close(hSock.m_sock);
	m_rwLock.LockWrite();
	std::list<SocketSession*>::iterator it = m_clientInfos.end();
	it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) {
		printf("the client:%d logoff\n", (*it)->getSock());
		m_interface->DestroySocketSession(*it);
		m_clientInfos.erase(it);
		InitIterator(THREAD_COUNT);
		for (int i = 0; i < THREAD_COUNT; i++)
		{
			if (m_itrs[i].itrStart != m_itrs[i].itrEnd)
				m_sem[i].SemPost();
		}
	}
	m_rwLock.UnlockWrite();
	m_interface->ClientExit(hSock);
}

void BalanceHandle::RecvClientData(const SockHandle& hSock,char* pBuf, int len, int nIndex, void* pPrev,void* pPostNode)
{
	if(!m_bStart)return;
	m_rwLock.LockRead();
	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		int ret = (*it)->DispatchData(pBuf,len,nIndex,pPrev,pPostNode);
	}				
	m_rwLock.UnlockRead();		
}

void BalanceHandle::ActivateWrite(const SockHandle& hSock, int index, void* pBuf, int len,void* pPostNode)
{
	if(!m_bStart)return;
	m_rwLock.LockRead();
	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		(*it)->NoticeComplateSend(index,pBuf,len,pPostNode);
	}			
	m_rwLock.UnlockRead();		
}

int BalanceHandle::RecvNetPackage(const SockHandle& hSock,NetPackage& p)
{
	int ret = -1;
	if(!m_bStart)return ret;
	m_rwLock.LockRead();

	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		ret=(*it)->RecvNetPackage(p);
	}	
		
	m_rwLock.UnlockRead();			
	return ret;
}

void* BalanceHandle::Alloc(const SockHandle& hSock, int len)
{
	void* pNew = NULL;
	if(!m_bStart)return pNew;
	m_rwLock.LockRead();

	std::list<SocketSession*>::iterator it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) {
		pNew = (*it)->Alloc(len);
	}

	m_rwLock.UnlockRead();	
	return pNew;
}

void BalanceHandle::Free(const SockHandle& hSock, void* p)
{
	if(!m_bStart)return;
	m_rwLock.LockRead();

	std::list<SocketSession*>::iterator it = std::find_if(m_clientInfos.begin(), m_clientInfos.end(), SessionCampare(hSock));
	if (it != m_clientInfos.end()) 
		(*it)->Free(p);

	m_rwLock.UnlockRead();	
}

int BalanceHandle::SendNetPackage(const SockHandle& hSock,const NetPackage& p)
{
	int ret = -1;
	if(!m_bStart)return ret;
	m_rwLock.LockRead();

	std::list<SocketSession*>::iterator it=std::find_if(m_clientInfos.begin(),m_clientInfos.end(),SessionCampare(hSock));
	if(it!=m_clientInfos.end()){
		ret=(*it)->SendNetPackage(p);
	}		
		
	m_rwLock.UnlockRead();		
	return ret;
}

void BalanceHandle::runImpl(int index)
{
	m_rwLock.LockRead();
	if (m_itrs[index].itrStart == m_itrs[index].itrEnd) {
		//cout << "thread:" << GetCurrentThreadId() << " not data" << endl;
		puts("not data and sleep");
		m_rwLock.UnlockRead();
		m_sem[index].SemWait();
		m_rwLock.LockRead();
	}
	DealData(m_itrs[index].itrStart, m_itrs[index].itrEnd, m_th[index]->GetThreadID());
	m_rwLock.UnlockRead();
}

void BalanceHandle::run(void* p)
{
	int index = -1;
	ForceConvert(index,p);
	while(m_WorkorFlag){
		runImpl(index);
		ThreadImpl<BalanceHandle>::mSleep(1);
	}
}

void BalanceHandle::HeartBitHandle(void* p)
{
	int index = -1;
	ForceConvert(index,p);
	m_heartCtrl.Lock();
	m_rwLock.LockRead();
	std::list<SocketSession*>::iterator it = m_itrs[index].itrStart;
	for (; it != m_itrs[index].itrEnd; it++)
	{
		if (time(0) - (*it)->getLastTime() > HEARTBIT_INTERVAL + HEARTBIT_INTERVAL / 10) {
			printf("%d lost heartbit\n", (*it)->getSock());
			m_lostHeartbit.push_back((*it)->GetSockHandle());
		}
	}
	if(m_lostHeartbit.size())m_heartCtrl.WakeConsume(false);
	m_rwLock.UnlockRead();
	m_heartCtrl.Unlock();
}

void BalanceHandle::LoseHeartBitThread()
{
	while(m_HeartBitFlag){
		m_heartCtrl.Lock();
		while(m_lostHeartbit.empty())m_heartCtrl.WaitConsume();
			for (int i = 0; i < m_lostHeartbit.size(); i++) 
				KickClient(m_lostHeartbit[i]);
			m_lostHeartbit.clear();	
		m_rwLock.UnlockRead();
	}
}
