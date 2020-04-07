#include "AtomMutexImpl_Linux.h"

AtomMutexImpl::AtomMutexImpl()
{
	pthread_mutex_init(&m_lock,NULL);
}

AtomMutexImpl::~AtomMutexImpl()
{
	pthread_mutex_destroy(&m_lock);
}

void AtomMutexImpl::Lock()
{
	pthread_mutex_lock(&m_lock);
}

void AtomMutexImpl::UnLock()
{
	pthread_mutex_unlock(&m_lock);
}

