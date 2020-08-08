#ifndef __MsTimerMgr_H__
#define __MsTimerMgr_H__
#include "Timer.h"

class XMem;
class MsTimerMgrImpl;

#include "../XDefine.h"
class FRAMEWORK_API MsTimerMgr
{
public:
	//MsTimerMgr(/*XMem* pMem=NULL*/);
	~MsTimerMgr();
	bool AddTimer(const char* str,AbstractTimer* timer);
	bool DelTimer(const char* str);
	bool ModifyRepeatTimer(const char* str,int len,int nInterval);	
    static MsTimerMgr& GetInstance(){
        static MsTimerMgr mgr;
        return mgr;
    }
private:
    MsTimerMgr();
	MsTimerMgr(const MsTimerMgr&);
	MsTimerMgr& operator=(const MsTimerMgr&);
private:
	MsTimerMgrImpl*	_Imp;
	//XMem*			pAllocator;
};

#endif	//MsTimerMgr.h
