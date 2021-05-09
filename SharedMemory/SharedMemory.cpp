#include "SharedMemory.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "SharedMemoryImpl_Win32.h"
#else
#include "SharedMemoryImpl_Linux.h"
#endif

SharedMemory::SharedMemory(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(SharedMemoryImpl*)pMem->Alloc(sizeof(SharedMemoryImpl))))
		new (_Imp) SharedMemoryImpl;
	else
		_Imp = new SharedMemoryImpl;	
}

SharedMemory::~SharedMemory()
{
	if (NULL != pAllocator){
		_Imp->~SharedMemoryImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;		
}

bool SharedMemory::Open(const char* str,int size)const
{
	return _Imp->Open(str,size)&&_Imp->Attach();
}

void* SharedMemory::GetAddress()
{
	return _Imp->GetAddress();
}

void SharedMemory::Close()const
{
	_Imp->Detach();
	_Imp->Close();
}

const void* SharedMemory::GetAddress()const
{
	return _Imp->GetAddress();
}
