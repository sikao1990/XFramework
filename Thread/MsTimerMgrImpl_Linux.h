#ifndef __MsTimerMgrImpl_Linux_H__
#define __MsTimerMgrImpl_Linux_H__
#include <signal.h>
#include <list>
#include <map>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include "Timer.h"
#include "Task.h"
#include "TaskCtrl.h"
#include "ThreadPool.h"
#include "TaskProcess.h"

class MsTimerMgrImpl
{
	class OnceProcess : public TaskProcess
	{
	public:
		OnceProcess(MsTimerMgrImpl* pTimer):m_pTimer(pTimer),m_bFlag(true){}
		void operator()(Task* task){
			while(m_bFlag){
				m_pTimer->WaitOnce();
				m_pTimer->DealOnceHandle();
			}
		}
		void stop()
		{
			m_bFlag = false;
			m_pTimer->WakeOne();
		}

	private:
		bool 			m_bFlag;
		MsTimerMgrImpl* 	m_pTimer;
	};
	class RepeatProcess : public TaskProcess
	{
	public:
		RepeatProcess(MsTimerMgrImpl* pTimer):m_pTimer(pTimer),m_bFlag(true){}
		void operator()(Task* task){
			while(m_bFlag){
				m_pTimer->WaitRepeat();
				m_pTimer->DealRepeatHandle();
			}			
		}
		void stop()
		{
			m_bFlag = false;
			m_pTimer->WakeRepeat();
		}
	private:
		bool 			m_bFlag;
		MsTimerMgrImpl* 	m_pTimer;
	};
public:
	MsTimerMgrImpl();
	~MsTimerMgrImpl();
	bool Init();
	bool AddTimer(const char* str,AbstractTimer* timer);
	bool DelTimer(const char* str);
	bool ModifyRepeatTimer(const char* str,int len,int nInterval);
	void Stop();
protected:
	friend class OnceProcess;
	friend class RepeatProcess;
	void WaitOnce();
	void WakeOne();
	void DealOnceHandle();
	void WakeRepeat();
	void WaitRepeat();
	void DealRepeatHandle();
	void TimerHandle();
	static void sigHandle(int sig);
private:
	static MsTimerMgrImpl* pThis;
	MsTimerMgrImpl(const MsTimerMgrImpl& );
	MsTimerMgrImpl& operator=(const MsTimerMgrImpl& );
private:
	unsigned short						bFlagOnce;
	unsigned short						bFlagRepeat;//避免压栈和弹栈
	
	sem_t								m_SemOnce;
	sem_t								m_SemRepeat;
	pthread_mutex_t						m_OnceLock;
	pthread_rwlock_t					m_Repeatlock;
	OnceProcess*						m_pOncePrecess;
	RepeatProcess*						m_pRepeatProcess;
	
	list<AbstractTimer*>::iterator 			itOnce;//避免压栈和弹栈
	map<string,AbstractTimer*>::iterator	itRepeat;
	
	list<AbstractTimer*>					onceLst;
	map<string,AbstractTimer*>				repeatSet;
	ThreadPool								m_pools;
};

#endif	// MsTimerMgrImpl_Linux.h
