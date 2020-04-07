#ifndef __TimerMgr_H__
#define __TimerMgr_H__
#include "Timer.h"

class XMem;
class TimerMgrImpl;
#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API TimerMgr
#else
class TimerMgr
#endif
{
public:
	TimerMgr(/*XMem* pMem=NULL*/);
	~TimerMgr();
	bool AddTimer(const char* str,AbstractTimer* timer);
	bool DelTimer(const char* str);
	bool ModifyRepeatTimer(const char* str,int len,int nInterval);	
private:
	TimerMgr(const TimerMgr&);
	TimerMgr& operator=(const TimerMgr&);
private:
	TimerMgrImpl*	_Imp;
	//XMem*			pAllocator;
};

#endif	//TimerMgr.h
