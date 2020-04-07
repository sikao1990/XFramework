#ifndef __SemaphoreImpl_H__
#define __SemaphoreImpl_H__
#include <semaphore.h>

class SemaphoreImpl
{
public:
	SemaphoreImpl(int n=0);
	~SemaphoreImpl();
	void SemPost();
	void SemWait();
private:
	SemaphoreImpl(const SemaphoreImpl&);
	SemaphoreImpl& operator=(const SemaphoreImpl&);		
private:
	sem_t	m_sync;
};

#endif	// SemaphoreImpl.h
