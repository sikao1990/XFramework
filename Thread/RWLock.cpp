#include "RWLock.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "RWLockImpl_Win32.h"
#else
#include "RWLockImpl_Linux.h"
#endif

RWLock::RWLock(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(RWLockImpl*)pMem->Alloc(sizeof(RWLockImpl))))
		new(_Imp) RWLockImpl;
	else
		_Imp = new RWLockImpl;	
}

RWLock::~RWLock()
{
	if (NULL != pAllocator) {
		_Imp->~RWLockImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;	
}

void RWLock::LockWrite()
{
	_Imp->LockWrite();
}

void RWLock::UnlockWrite()
{
	_Imp->UnlockWrite();
}

void RWLock::LockRead()
{
	_Imp->LockRead();
}

void RWLock::UnlockRead()
{
	_Imp->UnlockRead();
}
