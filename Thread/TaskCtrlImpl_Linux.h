#ifndef __TaskCtrlImpl_H__
#define __TaskCtrlImpl_H__

#include "SemaphoreExImpl_Linux.h"
#include "MultiThreadSync_Linux.h"

class TaskCtrlImpl
{
public:
	TaskCtrlImpl();
	void Init(int n);
	void AddThread(void* param);
	void DelThread(void* param);	
	void SyncTaskOneTime();
	void wait(void* param);
	void done();
private:
	TaskCtrlImpl(const TaskCtrlImpl&);
	TaskCtrlImpl& operator=(const TaskCtrlImpl&);
private:
	SemaphoreExImpl	m_semaEx;
	MultiThreadSync	m_multSync;
};

#endif	// TaskCtrlImpl.h

