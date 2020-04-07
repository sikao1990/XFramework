#include "ThreadPoolImpl_Win32.h"
#include <stdio.h>
#include <process.h>
#include <algorithm>

#define NSEMACOUNT	(10)

ThreadPoolImpl::ThreadPoolImpl(int limit):m_thread_sum(limit),m_pools(NULL), m_LowPriority("-1"), m_MaxPriority("-1")
	,m_flag(false),m_ctrl(NULL), m_taskCmp(NULL)
{
	m_ctrl=CreateSemaphore(NULL,0,NSEMACOUNT,NULL);
	m_lock = 0;
}

ThreadPoolImpl::~ThreadPoolImpl()
{
	if(NULL!=m_ctrl){
		CloseHandle(m_ctrl);
		m_ctrl = NULL;
	}	
	Stop();
}

int	ThreadPoolImpl::Init()
{
	if(NULL!=m_pools)return -1;
	m_pools=new HANDLE[m_thread_sum];
	memset(m_pools,0,sizeof(HANDLE)*m_thread_sum);
	if(m_pools){
		for(int i=0;i<m_thread_sum;i++){
			m_pools[i]=(HANDLE)_beginthreadex(NULL,0,&ThreadPoolImpl::thread_func,(void*)this,0,NULL);
		}
		m_flag = true;
		return 0;
	}else
		return -1;
}

int ThreadPoolImpl::submit_job_req(Task* job)
{
	if (!m_flag)return -1;
	bool bFlag = false;
	while (::InterlockedCompareExchange((LPLONG)&m_lock, 1, 0) != 0) Sleep(0);
	if ("-1" != m_LowPriority&&m_jobs.end() != std::find_if(m_jobs.begin(), m_jobs.end(), TaskTypeCompare(m_LowPriority, job, m_taskCmp))){
		bFlag = true;
		delete job;
	}
	else
		m_jobs.push_back(job);
	InterlockedExchange(&m_lock, 0);
	if (bFlag)return -2;
	ReleaseSemaphore(m_ctrl,1,NULL);//计数器加1
	return job->wait();
}

void ThreadPoolImpl::run()
{
	Task* t = NULL;
	list<Task* >::iterator it = m_jobs.end();
	WaitForSingleObject(m_ctrl,INFINITE);//等待�?
	while (::InterlockedCompareExchange((LPLONG)&m_lock, 1, 0) != 0) Sleep(0);
	if ("-1" != m_MaxPriority && m_jobs.end() != (it = std::find_if(m_jobs.begin(), m_jobs.end(), TaskTypeCompare(m_MaxPriority)))) {
		t = *it;//优先处理高优先级任务
		m_jobs.erase(it);
	}
	else {
		t = m_jobs.front();//提取任务
		m_jobs.pop_front();
	}
	InterlockedExchange(&m_lock, 0);//解锁
	if(t&&m_tasks.end()!=m_tasks.find(t->m_type)){
		(*(m_tasks.find(t->m_type)->second))(t);
		t->done(0);
		delete t;
	}
}

unsigned ThreadPoolImpl::thread_func(void *param)
{
	ThreadPoolImpl* obj=(ThreadPoolImpl*)param;
	while(obj->m_flag){
		obj->run();
	}
	return 0;
}	

bool ThreadPoolImpl::Register_taskProcess(const jobtype& type,TaskProcess* process)
{
	return m_tasks.insert(make_pair(type,process)).second;
}

void ThreadPoolImpl::Stop()
{
	if(!m_flag)return;
	m_flag=false;
	for(int i=0;i<m_thread_sum;i++)
		ReleaseSemaphore(m_ctrl,1,NULL);
	std::map<jobtype,TaskProcess* >::iterator it=m_tasks.begin();
	for(;it!=m_tasks.end();it++)	
		delete it->second;//TODO:
	m_tasks.clear();
	while(!m_jobs.empty()){
		Task* pTask = m_jobs.front();
		pTask->done(1);
		delete pTask;//TODO:释放问题
		m_jobs.pop_front();
	}
	delete [] m_pools;
	m_pools = NULL;
}

void ThreadPoolImpl::setMaxPriorityType(jobtype t)
{
	m_MaxPriority = t;
}

void ThreadPoolImpl::setMergeTaskType(jobtype t)
{
	m_LowPriority = t;
}

void ThreadPoolImpl::setTaskCmp(TCompare f)
{
	m_taskCmp = f;
}
