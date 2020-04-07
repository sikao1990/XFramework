#ifndef __ThreadPoolImpl_H__
#define __ThreadPoolImpl_H__
#include <pthread.h>
#include <semaphore.h>
#include <list>
#include <map>
#include "../Mem/XAllocator.h"
#include "Task.h"
#include "TaskProcess.h"

using namespace std;

class ThreadPoolImpl
{
public:
	ThreadPoolImpl(int limit);
	virtual ~ThreadPoolImpl();
	int  Init();
	void Stop();
	int submit_job_req(Task* job);
	bool Register_taskProcess(const jobtype& type,TaskProcess* process); 
	//设置高优先级任务
	void setMaxPriorityType(jobtype t);
	//设置可合并的低优先级任务,如果任务队列中已有该类型任务，则可忽略新增同类型任务
	void setMergeTaskType(jobtype t);
	void setTaskCmp(TCompare f);
private:
	ThreadPoolImpl(ThreadPoolImpl&);
	ThreadPoolImpl& operator=(const ThreadPoolImpl& );
private:
	void run();
	static void* thread_func(void *);	
private:
	unsigned char	m_pool_sum;
	unsigned char 	m_flag;
	unsigned char	m_thread_sum;
	jobtype			m_MaxPriority;
	jobtype			m_LowPriority;
	TCompare		m_taskCmp;//查找任务的函数
	pthread_t*		m_pools;
		
	pthread_mutex_t m_lock;
	sem_t 			m_ctrl;
	list<Task*,XAllocator<Task* > >	m_jobs;
	std::map<jobtype,TaskProcess* >	m_tasks;
};

#endif	// ThreadPoolImpl.h
