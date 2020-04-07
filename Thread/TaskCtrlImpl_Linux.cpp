#include "TaskCtrlImpl_Linux.h"

TaskCtrlImpl::TaskCtrlImpl()
{
}

void TaskCtrlImpl::Init(int n)
{
	m_ctrl1.Init(0,1,n);
	m_ctrl2.Init(n);
}

void TaskCtrlImpl::SyncTaskOneTime()
{
	m_ctrl2.Signal();
	m_ctrl1.SemWait();
}

void TaskCtrlImpl::wait(void* param)
{
	m_ctrl2.Wait(param);
}

void TaskCtrlImpl::done()
{
	m_ctrl1.SemSignal();
}

void TaskCtrlImpl::AddThread(void* param)
{
	m_ctrl2.AddThread(param);
}

void TaskCtrlImpl::DelThread(void* param)
{
	m_ctrl2.DelThread(param);
}
