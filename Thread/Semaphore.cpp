#include "Semaphore.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "SemaphoreImpl_Win32.h"
#else
#include "SemaphoreImpl_Linux.h"
#endif

Semaphore::Semaphore(int n,XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(SemaphoreImpl*)pMem->Alloc(sizeof(SemaphoreImpl))))
		new (_Imp) SemaphoreImpl(n);
	else
		_Imp = new SemaphoreImpl(n);	
}

Semaphore::~Semaphore()
{
	if (NULL != pAllocator) {
		_Imp->~SemaphoreImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;	
}

void Semaphore::SemPost()
{
	_Imp->SemPost();
}

void Semaphore::SemWait()
{
	_Imp->SemWait();
}
	