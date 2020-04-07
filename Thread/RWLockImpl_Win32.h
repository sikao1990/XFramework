#ifndef __RWLockImpl_Win32_H__
#define __RWLockImpl_Win32_H__
#include <windows.h>

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
	SRWLOCK	m_lock;
};

#endif	// RWLockImpl.h
