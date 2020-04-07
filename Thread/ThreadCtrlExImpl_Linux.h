#ifndef __ThreadCtrlExImpl_H__
#define __ThreadCtrlExImpl_H__
#include <pthread.h>

class ThreadCtrlExImpl
{
public:
	ThreadCtrlExImpl();
	~ThreadCtrlExImpl();
	void Lock();
	void Unlock();
	void WaitProduce();
	void WaitConsume();
	void WakeProduce(bool bAll);
	void WakeConsume(bool bAll);

	ThreadCtrlExImpl(const ThreadCtrlExImpl&);
	ThreadCtrlExImpl& operator=(const ThreadCtrlExImpl&);	
private:
	pthread_mutex_t	m_lock;
	pthread_cond_t 	m_condp;
	pthread_cond_t 	m_condc;
};

#endif	// ThreadCtrlExImpl.h
