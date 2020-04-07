#include "RWLockImpl_Linux.h"

RWLockImpl::RWLockImpl()
{
	pthread_rwlock_init(&m_lock,NULL);
}

RWLockImpl::~RWLockImpl()
{
	pthread_rwlock_destroy(&m_lock);
}

RWLockImpl::RWLockImpl(const RWLockImpl&)
{
	pthread_rwlock_init(&m_lock,NULL);
}

RWLockImpl& RWLockImpl::operator=(const RWLockImpl&)
{
	return *this;
}

void RWLockImpl::LockWrite()
{
	pthread_rwlock_wrlock(&m_lock);
}

void RWLockImpl::UnlockWrite()
{
	pthread_rwlock_unlock(&m_lock);
}

void RWLockImpl::LockRead()
{
	pthread_rwlock_rdlock(&m_lock);
}

void RWLockImpl::UnlockRead()
{
	pthread_rwlock_unlock(&m_lock);
}

	