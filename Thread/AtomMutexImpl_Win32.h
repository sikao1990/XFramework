#ifndef __AtomMutexImpl_H__
#define __AtomMutexImpl_H__

class AtomMutexImpl
{
public:
	AtomMutexImpl();
	void Lock();
	void UnLock();
private:
	AtomMutexImpl(const AtomMutexImpl&);
	AtomMutexImpl& operator=(const AtomMutexImpl&);	
private:
	unsigned long m_lock;
};

#endif	//AtomMutexImpl.h
