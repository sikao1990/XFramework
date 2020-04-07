#ifndef __ThreadCtrlExImpl_H__
#define __ThreadCtrlExImpl_H__
#include <windows.h>

class ThreadCtrlExImpl
{
public:
	ThreadCtrlExImpl();
	~ThreadCtrlExImpl();
	void Lock();
	void Unlock();
	void WaitProduce();
	void WaitConsume();
	void WakeProduce(bool bAll);
	void WakeConsume(bool bAll);

	ThreadCtrlExImpl(const ThreadCtrlExImpl&);
	ThreadCtrlExImpl& operator=(const ThreadCtrlExImpl&);
private:
	CONDITION_VARIABLE		m_Condp;
	CONDITION_VARIABLE		m_Condc;
	SRWLOCK 				m_lock;
	//CRITICAL_SECTION		m_lock;
};

#endif	// ThreadCtrlExImpl.h
