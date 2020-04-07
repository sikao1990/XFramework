#include "TaskCtrlImpl_Win32.h"

TaskCtrlImpl::TaskCtrlImpl():m_hEvent(NULL)
{
}

TaskCtrlImpl::~TaskCtrlImpl()
{
	if(NULL!=m_hEvent)CloseHandle(m_hEvent);
}

void TaskCtrlImpl::Init(int n)
{
	if(NULL!=m_hEvent)
		return;
	m_sema.Init(0,n,1);
	m_hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
}

void TaskCtrlImpl::SyncTaskOneTime()
{
	if(NULL!=m_hEvent){
		PulseEvent(m_hEvent);
		m_sema.SemWait();
	}
}

void TaskCtrlImpl::wait(void* param)
{
	if(NULL!=m_hEvent)
		WaitForSingleObject(m_hEvent,INFINITE);
}

void TaskCtrlImpl::done()
{
	m_sema.SemSignal();
}

void TaskCtrlImpl::AddThread(void* param)
{
}

void TaskCtrlImpl::DelThread(void* param)
{
}

	