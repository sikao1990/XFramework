#include "ThreadCtrlExImpl_Linux.h"

ThreadCtrlExImpl::ThreadCtrlExImpl()
{
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_condp,NULL);
	pthread_cond_init(&m_condc,NULL);
}

ThreadCtrlExImpl::~ThreadCtrlExImpl()
{
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_condp);
	pthread_cond_destroy(&m_condc);
}

ThreadCtrlExImpl::ThreadCtrlExImpl(const ThreadCtrlExImpl&)
{
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_condp,NULL);
	pthread_cond_init(&m_condc,NULL);	
}

ThreadCtrlExImpl& ThreadCtrlExImpl::operator=(const ThreadCtrlExImpl&)
{
	return *this;
}
	
void ThreadCtrlExImpl::Lock()
{
	pthread_mutex_lock(&m_lock);
}

void ThreadCtrlExImpl::Unlock()
{
	pthread_mutex_unlock(&m_lock);
}

void ThreadCtrlExImpl::WaitProduce()
{
	pthread_cond_wait(&m_condp,&m_lock);
}

void ThreadCtrlExImpl::WaitConsume()
{
	pthread_cond_wait(&m_condc,&m_lock);
}

void ThreadCtrlExImpl::WakeProduce(bool bAll)
{
	bAll?pthread_cond_broadcast(&m_condp):pthread_cond_signal(&m_condp);
}

void ThreadCtrlExImpl::WakeConsume(bool bAll)
{
	bAll?pthread_cond_broadcast(&m_condc):pthread_cond_signal(&m_condc);
}
	