#ifndef __TimerMgrImpl_H__
#define __TimerMgrImpl_H__
#include "Timer.h"
#include <windows.h>
#include <map>
#include <vector>
#include "../Mem/XAllocator.h"

using namespace std;

class TimerMgrImpl
{
	struct TimerInfo{
		HANDLE			hTimer;
		AbstractTimer*	pTimer;
		TimerInfo(AbstractTimer* Timer=NULL):pTimer(Timer), hTimer(NULL){}
	};
public:
	TimerMgrImpl();
	~TimerMgrImpl();
	bool Init();
	bool AddTimer(const char* str, AbstractTimer* timer);
	bool DelTimer(const char* str);
	bool ModifyRepeatTimer(const char* str, int len, int nInterval);
	void Stop();
private:
	TimerMgrImpl(const TimerMgrImpl&);
	TimerMgrImpl& operator=(const TimerMgrImpl&);	
private:
	static VOID CALLBACK TimerRoutine( PVOID lpParameter, BOOLEAN TimerOrWaitFired );
private:
	HANDLE				m_hTimerQueue;
	map<string,TimerInfo>	m_pTimers;
};

#endif	// TimerMgrImpl.h
