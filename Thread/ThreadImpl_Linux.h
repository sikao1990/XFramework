#ifndef __ThreadImpl_H__
#define __ThreadImpl_H__
#include <pthread.h>
#include <unistd.h>
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
	ThreadImpl(T* pProxy,pFuncRun pFunc=NULL):m_pProxy(pProxy),m_hThread(0),m_bRuning(false){m_funcptr.m_pCallback=pFunc;}
    ThreadImpl(T* pProxy,PfuncRunEx pFunc=NULL,void* pParam=NULL):m_pProxy(pProxy),m_hThread(0),m_bRuning(false),
        m_ExtendParam(pParam){ m_funcptr.m_pCallbackEx=pFunc;}
	~ThreadImpl(){
		stop();
	}
	bool start(){
	    m_ExtendParam.pThis = this;
		if(pthread_create(&m_hThread,NULL,threadProc,(void*)&m_ExtendParam))
			return false;
		pthread_detach(m_hThread);
		return true;
	}
	void stop(){
		m_hThread = 0;
	}
	size_t GetThreadID(){ return (size_t)m_hThread; }
	bool IsRuning(){ return m_bRuning; }
	static void mSleep(unsigned ms){ usleep(ms*1000); }
	ThreadImpl(const ThreadImpl& th){
		m_pProxy 	= th.m_pProxy;
		m_hThread 	= 0;
		m_bRuning 	= false;
		m_funcptr.m_pCallback = th.m_funcptr.m_pCallback;
        m_ExtendParam.pExtend = th.m_ExtendParam.pExtend;        
	}
	ThreadImpl& operator=(const ThreadImpl& th){
		m_pProxy 	= th.m_pProxy;
		m_hThread 	= 0;
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
	static void* threadProc(void* p){
	    thInParam* pParam = (thInParam*)p;
		pParam->pThis->m_bRuning = true;
		pParam->pThis->Run(pParam->pExtend);
		pParam->pThis->m_bRuning = false;
		return p;
	}
protected:
	T* 			m_pProxy;
    union {
	    pFuncRun	m_pCallback;
        PfuncRunEx  m_pCallbackEx;
    }m_funcptr;
	pthread_t	m_hThread;
	bool		m_bRuning;
    thInParam   m_ExtendParam;
};

#endif //threadImp.h


