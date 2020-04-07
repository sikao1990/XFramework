#include "MutexIntegerImpl_Linux.h"

MutexIntegerImpl::MutexIntegerImpl():m_val(0)
{
}

void MutexIntegerImpl::SetVal(const int n)
{
	m_lock.Lock();
	m_val = n;
	m_lock.UnLock();
}

int MutexIntegerImpl::getVal()
{
	int val = 0;
	m_lock.Lock();
	val = m_val;
	m_lock.UnLock();
	return val;
}

bool MutexIntegerImpl::IsVal(const int n)
{
	int val = 0;
	m_lock.Lock();
	val = m_val;
	m_lock.UnLock();
	return val==n;
}

