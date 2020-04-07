#ifndef __ThreadImpll_H__
#define __ThreadImpll_H__
#include <windows.h>
#include <process.h>
#include "ThreadPri.h"

template < typename T >
class ThreadImpl : public ThreadPri
{
    struct thInParam{
        ThreadImpl* pThis;
        void*       pExtend;
        thInParam(void* pParam=NULL,ThreadImpl*p=NULL):pThis(p),pExtend(pParam){}
        bool HasInThParam(){ return NULL!=pExtend; }
    };
	typedef void (T::*pFuncRun)();
    typedef void (T::*PfuncRunEx)(void* );
public:
	ThreadImpl(T* pProxy,pFuncRun pFunc=NULL):m_pProxy(pProxy),m_hThread(NULL),
		m_dwThreadID(0), m_bRuning(false){ m_funcptr.m_pCallback=pFunc; }
    ThreadImpl(T* pProxy,PfuncRunEx pFunc=NULL,void* pParam=NULL):m_pProxy(pProxy),m_hThread(0),m_bRuning(false),
		m_ExtendParam(pParam){ m_funcptr.m_pCallbackEx=pFunc;}
	~ThreadImpl(){
		if(NULL!=m_hThread)
			stop();
	}
	bool start(){
	    m_ExtendParam.pThis = this;
		if(m_hThread=(HANDLE)_beginthreadex(NULL,0,threadProc,(void*)&m_ExtendParam,0,&m_dwThreadID))
			return true;
		else
			return false;
	}
	void stop(){
		WaitForSingleObject(m_hThread,INFINITE);
		m_hThread = NULL;
	}
	unsigned GetThreadID(){ return m_dwThreadID; }
	bool IsRuning(){ return m_bRuning; }
	static void mSleep(unsigned ms){ Sleep(ms); }
	ThreadImpl(const ThreadImpl& th){
		m_pProxy 	= th.m_pProxy;
		m_hThread 	= 0;
		m_dwThreadID= 0;
		m_bRuning 	= false;
		m_funcptr.m_pCallback = th.m_funcptr.m_pCallback;
        m_ExtendParam.pExtend = th.m_ExtendParam.pExtend;        
	}
	ThreadImpl& operator=(const ThreadImpl& th){
		m_pProxy 	= th.m_pProxy;
		m_hThread 	= 0;
		m_dwThreadID= 0;
		m_bRuning 	= false;	
   		m_funcptr.m_pCallback = th.m_funcptr.m_pCallback;
        m_ExtendParam.pExtend = th.m_ExtendParam.pExtend;     
		return *this;
	}	
private:
	void Run(void* pExtend){
        if(NULL!=m_funcptr.m_pCallbackEx){
            if(m_ExtendParam.HasInThParam()){
                (m_pProxy->*(m_funcptr.m_pCallbackEx))(pExtend);
            }else{
    		    (m_pProxy->*(m_funcptr.m_pCallback))();
            }
        }
	}
	static unsigned WINAPI threadProc(void* p){
	    thInParam* pParam = (thInParam*)p;
		pParam->pThis->m_bRuning = true;
		pParam->pThis->Run(pParam->pExtend);
		pParam->pThis->m_bRuning = false;
		return 0;
	}
protected:
	T* 			m_pProxy;
    union {
	    pFuncRun	m_pCallback;
        PfuncRunEx  m_pCallbackEx;
    }m_funcptr;
	HANDLE		m_hThread;
	unsigned 	m_dwThreadID;
	bool		m_bRuning;
    thInParam   m_ExtendParam;
};

#endif //ThreadImpl.h


