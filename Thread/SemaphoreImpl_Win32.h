#ifndef __SemaphoreImpl_H__
#define __SemaphoreImpl_H__
#include <windows.h>

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
	HANDLE	m_hSema;
};

#endif	// SemaphoreImpl.h
