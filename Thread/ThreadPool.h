#ifndef __ThreadPool_H__
#define __ThreadPool_H__

#include "Task.h"

class TaskProcess;
class ThreadPoolImpl;
class ThreadPool
{
public:	
	ThreadPool(int limit);
	~ThreadPool();
	int  init_pool();
	void stop_pool();
	int submit_job_req(Task* job);
	bool Register_taskProcess(const jobtype& type,TaskProcess* process);
private:
	ThreadPool(ThreadPool&);
	ThreadPool& operator=(const ThreadPool& );	
private:
	ThreadPoolImpl*	_Imp;
};

#endif	//ThreadPool.h
