#ifndef __AtomMutex_H__
#define __AtomMutex_H__

class XMem;
class AtomMutexImpl;
#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API AtomMutex
#else
class AtomMutex
#endif
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

