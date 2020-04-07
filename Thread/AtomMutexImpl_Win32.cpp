#include "AtomMutexImpl_Win32.h"
#include <windows.h>

AtomMutexImpl::AtomMutexImpl():m_lock(0)
{
}

void AtomMutexImpl::Lock()
{
	while (::InterlockedCompareExchange((LPLONG)&m_lock, 1, 0) != 0) Sleep(0);
}

void AtomMutexImpl::UnLock()
{
	InterlockedExchange(&m_lock, 0);
}

