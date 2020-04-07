#include "ThreadCtrlEx.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "ThreadCtrlExImpl_Win32.h"
#else
#include "ThreadCtrlExImpl_Linux.h"
#endif

ThreadCtrlEx::ThreadCtrlEx(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(ThreadCtrlExImpl*)pMem->Alloc(sizeof(ThreadCtrlExImpl))))
		new (_Imp) ThreadCtrlExImpl;
	else
		_Imp = new ThreadCtrlExImpl;
}

ThreadCtrlEx::~ThreadCtrlEx()
{
	if(NULL!=pAllocator){
		_Imp->~ThreadCtrlExImpl();
		pAllocator->Free(_Imp);
	}else
		delete _Imp;
}

void ThreadCtrlEx::Lock()
{
	_Imp->Lock();
}

void ThreadCtrlEx::Unlock()
{
	_Imp->Unlock();
}

void ThreadCtrlEx::WaitProduce()
{
	_Imp->WaitProduce();
}

void ThreadCtrlEx::WaitConsume()
{
	_Imp->WaitConsume();
}

void ThreadCtrlEx::WakeProduce(bool bAll)
{
	_Imp->WakeProduce(bAll);
}

void ThreadCtrlEx::WakeConsume(bool bAll)
{
	_Imp->WakeConsume(bAll);
}
