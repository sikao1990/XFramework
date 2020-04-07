#include "TimerMgr.h"
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "TimerMgrImpl_Win32.h"
#else
#include "TimerMgrImpl_Linux.h"
#endif	

TimerMgr::TimerMgr(/*XMem* pMem*/)//:pAllocator(pMem)
{
	/*if(NULL!=pAllocator && NULL!=(_Imp=(TimerMgrImpl*)pMem->Alloc(sizeof(TimerMgrImpl))))
		new (_Imp) TimerMgrImpl;
	else*/
		_Imp = new TimerMgrImpl;
	if (!_Imp->Init())
		throw "sys error";
}

TimerMgr::~TimerMgr()
{/*
	if (NULL != pAllocator){
		_Imp->~TimerMgrImpl();
		pAllocator->Free(_Imp);
	}
	else*/
		delete _Imp;	
}

bool TimerMgr::AddTimer(const char* str, AbstractTimer* timer)
{
	return _Imp->AddTimer(str, timer);
}

bool TimerMgr::ModifyRepeatTimer(const char* str, int len, int nInterval)
{
	return _Imp->ModifyRepeatTimer(str,len, nInterval);
}

bool TimerMgr::DelTimer(const char* str)
{
	return _Imp->DelTimer(str);
}

