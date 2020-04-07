#ifndef __SemaphoreExImpl_H__
#define __SemaphoreExImpl_H__
#include <pthread.h>

class SemaphoreExImpl
{
public:
	SemaphoreExImpl();
	~SemaphoreExImpl();
	void Init(int nCount=0,int nWait=1,int nPost=1);
	void SemSignal(int n=-1);
	void SemWait(int n=-1);
private:
	SemaphoreExImpl(const SemaphoreExImpl& sem);
	SemaphoreExImpl& operator=(const SemaphoreExImpl& sem);
private:
	unsigned char  	m_nCount;
	unsigned char	m_nWait;
	unsigned char	m_nPost;
	pthread_cond_t	m_cond;
	pthread_mutex_t m_lock;
};

#endif	// SemaphoreExImpl.h

