#ifndef __TaskProcess_H__
#define __TaskProcess_H__
#include "Task.h"
#include "TaskCtrl.h"

#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API TaskProcess
#else
class TaskProcess
#endif
{
public:
	TaskProcess(TaskCtrl* pCtrl=NULL):m_ctrl(pCtrl){}
	void SetTaskCtrl(TaskCtrl* pCtrl) { m_ctrl = pCtrl; }
	virtual ~TaskProcess(){}
	virtual void operator()(Task* task)=0;
protected:
	TaskCtrl* m_ctrl;	
};

#endif	// TaskProcess.h
