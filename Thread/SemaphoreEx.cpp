#include "SemaphoreEx.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "SemaphoreExImpl_Win32.h"
#else
#include "SemaphoreExImpl_Linux.h"
#endif

SemaphoreEx::SemaphoreEx(int nInit,int nWait,int nPost,XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(SemaphoreExImpl*)pMem->Alloc(sizeof(SemaphoreExImpl))))
		new (_Imp) SemaphoreExImpl;
	else
		_Imp = new SemaphoreExImpl;	
	_Imp->Init(nInit,nWait,nPost);
}

SemaphoreEx::~SemaphoreEx()
{
	if (NULL != pAllocator){
		_Imp->~SemaphoreExImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;	
}

void SemaphoreEx::SemSignal(int n)
{
	_Imp->SemSignal(n);
}

void SemaphoreEx::SemWait(int n)
{
	_Imp->SemWait(n);
}

