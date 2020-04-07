#ifndef __TaskCtrlImpl_H__
#define __TaskCtrlImpl_H__
#include <windows.h>
#include "SemaphoreExImpl_Win32.h"

class TaskCtrlImpl
{
public:
	TaskCtrlImpl();
	~TaskCtrlImpl();
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
	SemaphoreExImpl	m_sema;
	HANDLE			m_hEvent;
};

#endif	// TaskCtrlImpl.h

