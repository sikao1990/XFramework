#include "TaskCtrlImpl_Linux.h"

TaskCtrlImpl::TaskCtrlImpl()
{
}

void TaskCtrlImpl::Init(int n)
{
	m_semaEx.Init(0,n,1);
	m_multSync.Init(n);
}

void TaskCtrlImpl::SyncTaskOneTime()
{
	m_multSync.Signal();//+n
	m_semaEx.SemWait();//-m
}

void TaskCtrlImpl::wait(void* param)
{
	m_multSync.Wait(param);//-1
}

void TaskCtrlImpl::done()
{
	m_semaEx.SemSignal();//+1
}

void TaskCtrlImpl::AddThread(void* param)
{
	m_multSync.AddThread(param);
}

void TaskCtrlImpl::DelThread(void* param)
{
	m_multSync.DelThread(param);
}
