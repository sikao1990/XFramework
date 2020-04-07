#include "ThreadPoolImpl_Linux.h"
#include <stdio.h>
#include <algorithm>

ThreadPoolImpl::ThreadPoolImpl(int limit):m_thread_sum(limit),m_pools(NULL),m_flag(true), 
    m_LowPriority("-1"), m_MaxPriority("-1"),m_taskCmp(NULL)
{
	sem_init(&m_ctrl, 0, 0);
	pthread_mutex_init(&m_lock,NULL);
}

ThreadPoolImpl::~ThreadPoolImpl()
{
	sem_destroy(&m_ctrl);
	pthread_mutex_destroy(&m_lock);
	Stop();
}

int	ThreadPoolImpl::Init()
{
	if(NULL!=m_pools)return -1;
	m_pools=new pthread_t[m_thread_sum];
	if(m_pools){
		for(int i=0;i<m_thread_sum;i++){
			pthread_create(m_pools+i,0,&ThreadPoolImpl::thread_func,(void*)this);
			pthread_detach(*(m_pools+i));
		}
		m_flag = true;
		return 0;
	}else
		return -1;
}

int ThreadPoolImpl::submit_job_req(Task* job)
{
	if(!m_flag)return -1;
	int bFlag = false;
	pthread_mutex_lock(&m_lock);
	if("-1"!=m_LowPriority&&m_jobs.end()!=std::find_if(m_jobs.begin(),m_jobs.end(),TaskTypeCompare(m_LowPriority,job,m_taskCmp))){
		bFlag=true;
		delete job;
	}else
		m_jobs.push_back(job);
	pthread_mutex_unlock(&m_lock);
	if(bFlag)return -2;
	sem_post(&m_ctrl);//计数器加1
	return job->wait();
}

void ThreadPoolImpl::run()
{
	Task* t = NULL;
	list<Task*,XAllocator<Task* >  >::iterator it = m_jobs.end();
	sem_wait(&m_ctrl);//等待�?
	pthread_mutex_lock(&m_lock);//
	if("-1"!=m_MaxPriority && m_jobs.end()!=(it=std::find_if(m_jobs.begin(),m_jobs.end(),TaskTypeCompare(m_MaxPriority)))){
		t = *it;//优先处理高优先级任务
		m_jobs.erase(it);
	}else{
		t = m_jobs.front();//提取任务
		m_jobs.pop_front();
	}
	pthread_mutex_unlock(&m_lock);//解锁
	if(t&&m_tasks.end()!=m_tasks.find(t->m_type)){
		(*(m_tasks.find(t->m_type)->second))(t);
		t->done(0);
		delete t;
	}
}

void* ThreadPoolImpl::thread_func(void *param)
{
	ThreadPoolImpl* obj=(ThreadPoolImpl*)param;
	while(obj->m_flag){
		obj->run();
	}
	return obj;
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
		sem_post(&m_ctrl);
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
