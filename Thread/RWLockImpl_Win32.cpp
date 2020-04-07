#include "RWLockImpl_Win32.h"

RWLockImpl::RWLockImpl()
{
	InitializeSRWLock(&m_lock);
}

RWLockImpl::~RWLockImpl()
{
}

RWLockImpl::RWLockImpl(const RWLockImpl&)
{
	InitializeSRWLock(&m_lock);
}

RWLockImpl& RWLockImpl::operator=(const RWLockImpl&)
{
	return *this;
}

void RWLockImpl::LockWrite()
{
	AcquireSRWLockExclusive(&m_lock);
}

void RWLockImpl::UnlockWrite()
{
	ReleaseSRWLockExclusive(&m_lock);
}

void RWLockImpl::LockRead()
{
	AcquireSRWLockShared(&m_lock);
}

void RWLockImpl::UnlockRead()
{
	ReleaseSRWLockShared(&m_lock);
}
