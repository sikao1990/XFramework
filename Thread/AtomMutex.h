#ifndef __AtomMutex_H__
#define __AtomMutex_H__

class XMem;
class AtomMutexImpl;
#include "../XDefine.h"
class FRAMEWORK_API AtomMutex
{
public:
	AtomMutex(XMem* pMem=0);
	~AtomMutex();
	void Lock();
	void UnLock();
private:
	AtomMutex(const AtomMutex&);
	AtomMutex& operator=(const AtomMutex&);
private:
	AtomMutexImpl*	_Imp;
	XMem* 			pAllocator;
};

#endif	//AtomMutex.h

