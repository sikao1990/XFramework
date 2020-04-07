#ifndef __Timer_H__
#define __Timer_H__
#include <string.h>
#include "../SystemApi/TimeInfo.h"

struct TimeSet
{
	unsigned short	nStart;		//ms
	unsigned short	nInterval;	//ms
	TimeSet(int Start=0,int internal=0):nStart(Start)
		,nInterval(internal){}	
};

#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API AbstractTimer
#else
class AbstractTimer
#endif
{
public:
	AbstractTimer(int Start=0,int internal=0):m_nCur(0),m_param(Start,internal){}
	virtual ~AbstractTimer(){}
	virtual void TimerProc()=0;
	void SetTimer(int start,int interval){
		m_param.nStart 		= start;
		m_param.nInterval 	= interval;
	}
	void SetTimer(const TimeSet& p){ m_param=p;}
	int GetStart()const{ return m_param.nStart;}
	int GetInterval()const{ return m_param.nInterval; }
	TimeSet GetTimerInfo()const{ return m_param; }
	void ResetState(){ nFinish = 1; }
	bool IsTimeout()const{ return nFinish>1; }
	void SetStartTime(){ m_nCur = TimeInfo::getTimeStamp();}
	int Increment() { return ++nCount; }
	void RestCount() { nCount = 0; }
	void SetFinish() { nFinish = 2; }
	int GetState()const { return nFinish; }
private:
	TimeSet			m_param;
	unsigned short 	nCount;		//统计计数
	unsigned short	nFinish;	//结束标记->对于重复定时器，则表示第一次是否完成	
	long long 		m_nCur;		//当前时间
};

template < class Type >
class Timer : public AbstractTimer
{
public:
	typedef void (Type::* pFunc)();
    typedef void (Type::* pFuncExt)(void*);
	Timer(Type* pObj,pFunc f,const TimeSet& tSet):m_pObj(pObj),m_pExtend(NULL),
		AbstractTimer(tSet.nStart,tSet.nInterval)
	{
	    m_pfunptr.m_pFunc = f;
		SetTimer(tSet);
	}
	Timer(Type* pObj,pFuncExt f,const TimeSet& tSet,void* pExt):m_pObj(pObj),m_pExtend(pExt),
		AbstractTimer(tSet.nStart,tSet.nInterval)
	{
	    m_pfunptr.m_pFuncExt = f;
		SetTimer(tSet);
	}        
	virtual void TimerProc(){
		NULL==m_pExtend?(m_pObj->*(m_pfunptr.m_pFunc))():(m_pObj->*(m_pfunptr.m_pFuncExt))(m_pExtend);
	}
private:
	Type* 	m_pObj;
    union{
	    pFunc	    m_pFunc;
        pFuncExt    m_pFuncExt;
    }m_pfunptr;
    void*   m_pExtend;
};

#endif	// Timer.h
