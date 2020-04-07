#include "AtomMutexEx_Win32.h"
	
AtomMutexEx::AtomMutexEx()
{
	InitializeCriticalSection(&m_lock);
}

AtomMutexEx::~AtomMutexEx()
{
	DeleteCriticalSection(&m_lock);
}

void AtomMutexEx::Lock()
{
	EnterCriticalSection(&m_lock);
}

void AtomMutexEx::UnLock()
{
	LeaveCriticalSection(&m_lock);
}
