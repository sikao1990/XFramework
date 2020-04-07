#ifndef __AtomMutexImpl_H__
#define __AtomMutexImpl_H__
#include <pthread.h>

class AtomMutexImpl
{
public:
	AtomMutexImpl();
	~AtomMutexImpl();
	void Lock();
	void UnLock();
private:
	AtomMutexImpl(const AtomMutexImpl&);
	AtomMutexImpl& operator=(const AtomMutexImpl&);
private:
	pthread_mutex_t m_lock;
};


#endif	//AtomMutexImpl.h
