#ifndef __TaskCtrl_H__
#define __TaskCtrl_H__

class XMem;
class TaskCtrlImpl;

#include "../XDefine.h"
class FRAMEWORK_API TaskCtrl
{
public:
	TaskCtrl(int n=1,XMem* pMem=0);
	~TaskCtrl();
	void AddThread(void* param);
	void DelThread(void* param);
	void SyncTaskOneTime();
	void wait(void* param);
	void done();
private:
	TaskCtrl(const TaskCtrl&);
	TaskCtrl& operator=(const TaskCtrl&);		
private:
	TaskCtrlImpl*	_Imp;
	XMem*			pAllocator;
};

#endif	// TaskCtrl

