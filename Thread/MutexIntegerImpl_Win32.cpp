#include "MutexIntegerImpl_Win32.h"
#include <windows.h>

MutexIntegerImpl::MutexIntegerImpl():m_val(0)
{
}

void MutexIntegerImpl::SetVal(const int n)
{
	::InterlockedExchange(&m_val, n);
}

int MutexIntegerImpl::getVal()
{
	return ::InterlockedCompareExchange((LPLONG)&m_val, m_val, m_val);
}

bool MutexIntegerImpl::IsVal(const int n)
{
	return n==::InterlockedCompareExchange((LPLONG)&m_val, m_val, n);
}
