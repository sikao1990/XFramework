#include "ShareMemory.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "ShareMemoryImpl_Win32.h"
#else
#include "ShareMemoryImpl_Linux.h"
#endif

ShareMemory::ShareMemory(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(ShareMemoryImpl*)pMem->Alloc(sizeof(ShareMemoryImpl))))
		new (_Imp) ShareMemoryImpl;
	else
		_Imp = new ShareMemoryImpl;	
}

ShareMemory::~ShareMemory()
{
	if (NULL != pAllocator){
		_Imp->~ShareMemoryImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;		
}

bool ShareMemory::Open(const char* str,int size)const
{
	return _Imp->Open(str,size);
}

bool ShareMemory::Attach()const
{
	return _Imp->Attach();
}

void ShareMemory::Detach()const
{
	_Imp->Detach();
}

void* ShareMemory::GetAddress()
{
	return _Imp->GetAddress();
}

void ShareMemory::Close()const
{
	_Imp->Close();
}

const void* ShareMemory::GetAddress()const
{
	return _Imp->GetAddress();
}
