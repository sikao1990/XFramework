#include "MutexInteger.h"
#include <new>

#include "../Mem/XMem.h"
#ifdef WIN32
#include "MutexIntegerImpl_Win32.h"
#else
#include "MutexIntegerImpl_Linux.h"
#endif	

MutexInterger::MutexInterger(XMem* pMem):pAllocator(pMem)
{
	if (NULL != pAllocator && NULL != (_Imp = (MutexIntegerImpl*)pMem->Alloc(sizeof(MutexIntegerImpl))))
		new (_Imp) MutexIntegerImpl;
	else
		_Imp = new MutexIntegerImpl;
}

MutexInterger::MutexInterger(int i, XMem* pMem )
{
	if (NULL != pAllocator && NULL != (_Imp = (MutexIntegerImpl*)pMem->Alloc(sizeof(MutexIntegerImpl))) )
		new (_Imp) MutexIntegerImpl;
	else
		_Imp = new MutexIntegerImpl;
	_Imp->SetVal(i);
}

MutexInterger::~MutexInterger()
{
	if (NULL != pAllocator) {
		_Imp->~MutexIntegerImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;		
}

void MutexInterger::SetVal(const int n)
{
	return _Imp->SetVal(n);
}

int MutexInterger::getVal()const
{
	return _Imp->getVal();
}

bool MutexInterger::IsVal(const int n)const
{
	return _Imp->IsVal(n);
}
