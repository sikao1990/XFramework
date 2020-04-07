#include "AtomMutex.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "AtomMutexImpl_Win32.h"
#else
#include "AtomMutexImpl_Linux.h"
#endif

AtomMutex::AtomMutex(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(AtomMutexImpl*)pMem->Alloc(sizeof(AtomMutexImpl))))
		new (_Imp) AtomMutexImpl;
	else
		_Imp = new AtomMutexImpl;
}

AtomMutex::~AtomMutex()
{
	if (NULL != pAllocator) {
		_Imp->~AtomMutexImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;
}

void AtomMutex::Lock()
{
	_Imp->Lock();
}

void AtomMutex::UnLock()
{
	_Imp->UnLock();
}

