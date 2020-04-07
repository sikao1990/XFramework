#ifndef __RWLockImpl_Linux_H__
#define __RWLockImpl_Linux_H__
#include <pthread.h>
#include <stdarg.h>

class RWLock;
class RWLockImpl
{
public:	
	RWLockImpl();
	~RWLockImpl();

	void LockWrite();
	void UnlockWrite();
	void LockRead();
	void UnlockRead();

	RWLockImpl(const RWLockImpl&);
	RWLockImpl& operator=(const RWLockImpl&);
private:
	pthread_rwlock_t	m_lock;
};

#endif	// RWLockImpl.h
