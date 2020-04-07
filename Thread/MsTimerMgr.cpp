#include "MsTimerMgr.h"
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "MsTimerMgrImpl_Win32.h"
#else
#include "MsTimerMgrImpl_Linux.h"
#endif	

MsTimerMgr::MsTimerMgr(/*XMem* pMem*/)//:pAllocator(pMem)
{
	/*if(NULL!=pAllocator && NULL!=(_Imp=(TimerMgrImpl*)pMem->Alloc(sizeof(TimerMgrImpl))))
		new (_Imp) TimerMgrImpl;
	else*/
		_Imp = new MsTimerMgrImpl;
	if (!_Imp->Init())
		throw "sys error";
}

MsTimerMgr::~MsTimerMgr()
{/*
	if (NULL != pAllocator){
		_Imp->~TimerMgrImpl();
		pAllocator->Free(_Imp);
	}
	else*/
		delete _Imp;	
}

bool MsTimerMgr::AddTimer(const char* str, AbstractTimer* timer)
{
	return _Imp->AddTimer(str, timer);
}

bool MsTimerMgr::ModifyRepeatTimer(const char* str, int len, int nInterval)
{
	return _Imp->ModifyRepeatTimer(str,len, nInterval);
}

bool MsTimerMgr::DelTimer(const char* str)
{
	return _Imp->DelTimer(str);
}

