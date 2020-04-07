#ifndef __MsTimerMgrImpl_H__
#define __MsTimerMgrImpl_H__
#include "Timer.h"
#include <windows.h>
#include <map>
#include <vector>
#include "../Mem/XAllocator.h"

using namespace std;

class MsTimerMgrImpl
{
	struct TimerInfo{
		HANDLE			hTimer;
		AbstractTimer*	pTimer;
		TimerInfo(AbstractTimer* Timer=NULL):pTimer(Timer), hTimer(NULL){}
	};
public:
	MsTimerMgrImpl();
	~MsTimerMgrImpl();
	bool Init();
	bool AddTimer(const char* str, AbstractTimer* timer);
	bool DelTimer(const char* str);
	bool ModifyRepeatTimer(const char* str, int len, int nInterval);
	void Stop();
private:
	MsTimerMgrImpl(const MsTimerMgrImpl&);
	MsTimerMgrImpl& operator=(const MsTimerMgrImpl&);	
private:
	static VOID CALLBACK TimerRoutine( PVOID lpParameter, BOOLEAN TimerOrWaitFired );
private:
	HANDLE				m_hTimerQueue;
	map<string,TimerInfo>	m_pTimers;
};

#endif	// MsTimerMgrImpl.h
