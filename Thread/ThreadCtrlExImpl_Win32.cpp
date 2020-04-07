#include "ThreadCtrlExImpl_Win32.h"

ThreadCtrlExImpl::ThreadCtrlExImpl()
{
	InitializeSRWLock(&m_lock);
	//InitializeCriticalSection(&m_lock);
	InitializeConditionVariable(&m_Condp);
	InitializeConditionVariable(&m_Condc);
}

ThreadCtrlExImpl::~ThreadCtrlExImpl()
{
	//DeleteCriticalSection(&m_lock);
	WakeAllConditionVariable(&m_Condp);
	WakeAllConditionVariable(&m_Condc);
}

ThreadCtrlExImpl::ThreadCtrlExImpl(const ThreadCtrlExImpl&)
{
	InitializeSRWLock(&m_lock);
	//InitializeCriticalSection(&m_lock);
	InitializeConditionVariable(&m_Condp);
	InitializeConditionVariable(&m_Condc);	
}

ThreadCtrlExImpl& ThreadCtrlExImpl::operator=(const ThreadCtrlExImpl&)
{
	return *this;
}

void ThreadCtrlExImpl::Lock()
{
	AcquireSRWLockExclusive(&m_lock);
	//EnterCriticalSection(&m_lock);
}

void ThreadCtrlExImpl::Unlock()
{
	ReleaseSRWLockExclusive(&m_lock);
	//LeaveCriticalSection(&m_lock);
}

void ThreadCtrlExImpl::WaitProduce()
{
	SleepConditionVariableSRW(&m_Condp, &m_lock, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
	//SleepConditionVariableCS(&m_Condp, &m_lock, INFINITE);
}

void ThreadCtrlExImpl::WaitConsume()
{
	SleepConditionVariableSRW(&m_Condc, &m_lock, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
	//SleepConditionVariableCS(&m_Condc, &m_lock, INFINITE);
}

void ThreadCtrlExImpl::WakeProduce(bool bAll)
{
	bAll?WakeAllConditionVariable(&m_Condp):WakeConditionVariable(&m_Condp);
}

void ThreadCtrlExImpl::WakeConsume(bool bAll)
{
	bAll?WakeAllConditionVariable(&m_Condc):WakeConditionVariable(&m_Condc);
}
