#ifndef __RWLock_H__
#define __RWLock_H__
#include <stdarg.h>

class XMem;
class RWLockImpl;

#include "../XDefine.h"
class FRAMEWORK_API RWLock
{
public:
	RWLock(XMem* pMem=0);
	virtual ~RWLock();

	void LockWrite();
	void UnlockWrite();
	void LockRead();
	void UnlockRead();	

private:
	RWLockImpl*	_Imp;
	XMem*		pAllocator;
};

#endif	//RWLock.h
