#include "SemaphoreExImpl_Linux.h"

SemaphoreExImpl::SemaphoreExImpl():m_nPost(1),m_nWait(1)
{
	pthread_cond_init(&m_cond,NULL);
	pthread_mutex_init(&m_lock,NULL);
}

SemaphoreExImpl::~SemaphoreExImpl(){
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_cond);
}

void SemaphoreExImpl::Init(int nCount,int nWait,int nPost)
{
	m_nCount = nCount,m_nWait=nWait,m_nPost=nPost;
}

void SemaphoreExImpl::SemSignal(int n){
	pthread_mutex_lock(&m_lock);
	m_nCount+=(n>0&&n<m_nPost)?n:m_nPost;
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_lock);
}

void SemaphoreExImpl::SemWait(int n){
	pthread_mutex_lock(&m_lock);
	while(m_nCount<m_nWait)
		pthread_cond_wait(&m_cond,&m_lock);
	m_nCount -= n>0&&n<m_nWait?n:m_nWait;
	pthread_mutex_unlock(&m_lock);	
}
	