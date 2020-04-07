#include "TimerMgrImpl_Win32.h"

TimerMgrImpl::TimerMgrImpl():m_hTimerQueue(NULL)
{
	if(NULL == (m_hTimerQueue = CreateTimerQueue()) )
		throw "createTimerQueue failed";	
}

TimerMgrImpl::~TimerMgrImpl()
{
	while (!m_pTimers.empty()) {
		DelTimer(m_pTimers.begin()->first.c_str());
	}
	DeleteTimerQueue(m_hTimerQueue);
}

VOID TimerMgrImpl::TimerRoutine(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	TimerInfo* pTag = (TimerInfo*)lpParameter;
	pTag->pTimer->TimerProc();
}

bool TimerMgrImpl::AddTimer(const char* str, AbstractTimer* timer)
{
	if (m_pTimers.end() != m_pTimers.find(str))
		return false;
	else {
		TimerInfo info;
		info.pTimer = timer;
		m_pTimers.insert(make_pair(str, info));
		if (!CreateTimerQueueTimer(&(m_pTimers.find(str)->second.hTimer), m_hTimerQueue, WAITORTIMERCALLBACK(TimerRoutine),
			(void*)&(m_pTimers.find(str)->second), timer->GetStart()*1000, timer->GetInterval()*1000, 0))
			return false;
		return true;
	}
}

bool TimerMgrImpl::DelTimer(const char* str)
{
	if (m_pTimers.end() != m_pTimers.find(str))
	{
		DeleteTimerQueueTimer(m_hTimerQueue, m_pTimers.find(str)->second.hTimer, NULL);
		delete m_pTimers.find(str)->second.pTimer;
		m_pTimers.erase(m_pTimers.find(str));
		return true;
	}
	return false;
}

bool TimerMgrImpl::ModifyRepeatTimer(const char* str, int len, int nInterval)
{
	if (m_pTimers.end() == m_pTimers.find(str))
		return false;
	m_pTimers.find(str)->second.pTimer->SetTimer(len, nInterval);
	ChangeTimerQueueTimer(m_hTimerQueue, &(m_pTimers.find(str)->second.hTimer), len*1000, nInterval*1000);
	return true;
}

bool TimerMgrImpl::Init()
{
	return true;
}

void TimerMgrImpl::Stop()
{

}
