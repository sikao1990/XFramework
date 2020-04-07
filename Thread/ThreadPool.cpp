#include "ThreadPool.h"

#ifdef	WIN32
#include "ThreadPoolImpl_Win32.h"
#else
#include "ThreadPoolImpl_Linux.h"
#endif


ThreadPool::ThreadPool(int limit)
{
	_Imp = new ThreadPoolImpl(limit);
}

ThreadPool::~ThreadPool()
{
	delete _Imp;	
}


int ThreadPool::init_pool()
{
	return _Imp->Init();
}

void ThreadPool::stop_pool()
{
	_Imp->Stop();
}

int ThreadPool::submit_job_req(Task* job)
{
	return _Imp->submit_job_req(job);
}

bool ThreadPool::Register_taskProcess(const jobtype& type,TaskProcess* process)
{
	return _Imp->Register_taskProcess(type,process);
}
	
