#ifndef __Semaphore_H__
#define __Semaphore_H__

class XMem;
class SemaphoreImpl;
#include "../XDefine.h"
class FRAMEWORK_API Semaphore
{
public:
	Semaphore(int n=0,XMem* pMem=0);
	~Semaphore();
	void SemPost();
	void SemWait();
private:
	Semaphore(const Semaphore&);
	Semaphore& operator=(const Semaphore&);
private:
	SemaphoreImpl*	_Imp;
	XMem*			pAllocator;
};

#endif	// Semaphore.h
