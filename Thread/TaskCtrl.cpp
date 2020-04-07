#include "TaskCtrl.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "TaskCtrlImpl_Win32.h"
#else
#include "TaskCtrlImpl_Linux.h"
#endif	

TaskCtrl::TaskCtrl(int n,XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(TaskCtrlImpl*)pMem->Alloc(sizeof(TaskCtrlImpl))))
		new (_Imp) TaskCtrlImpl;
	else
		_Imp = new TaskCtrlImpl;
	_Imp->Init(n);
}

TaskCtrl::~TaskCtrl()
{
	if(NULL!=pAllocator){
		_Imp->~TaskCtrlImpl();
		pAllocator->Free(_Imp);
	}else
		delete _Imp;
}

void TaskCtrl::SyncTaskOneTime()
{
	_Imp->SyncTaskOneTime();
}

void TaskCtrl::wait(void* param)
{
	_Imp->wait(param);
}

void TaskCtrl::done()
{
	_Imp->done();
}

void TaskCtrl::AddThread(void* param)
{
	_Imp->AddThread(param);
}

void TaskCtrl::DelThread(void* param)
{
	_Imp->DelThread(param);
}
	