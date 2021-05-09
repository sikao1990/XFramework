#include "MultiThreadSync_linux.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

MultiThreadSync::MultiThreadSync():m_nths(0)
{
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_cond,NULL);
}

MultiThreadSync::~MultiThreadSync()
{
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_cond);
}

bool MultiThreadSync::Init(int n)
{
	m_nths = n;
	m_threads.clear();
}

void MultiThreadSync::Wait(void* param)
{
	int nPos = -1;
	pthread_mutex_lock(&m_lock);
	if(m_threads.empty() || m_threads.end()==m_threads.find(param)){
		pthread_mutex_unlock(&m_lock);
		return;
	}
	while(0==m_threads.find(param)->second)
		pthread_cond_wait(&m_cond,&m_lock);
	m_threads[param]=0;
	pthread_mutex_unlock(&m_lock);
}

void MultiThreadSync::Signal()
{
	pthread_mutex_lock(&m_lock);
	if(m_threads.empty()){
		pthread_mutex_unlock(&m_lock);
		return;
	}
	for(map<void*,int>::iterator it=m_threads.begin();it!=m_threads.end();it++)
		it->second=1;
	pthread_cond_broadcast(&m_cond);
	pthread_mutex_unlock(&m_lock);
}

void MultiThreadSync::AddThread(void* param)
{
	pthread_mutex_lock(&m_lock);
	m_threads.insert(make_pair(param,0));
	pthread_mutex_unlock(&m_lock);
}

void MultiThreadSync::DelThread(void* param)
{
	pthread_mutex_lock(&m_lock);
	if(m_threads.end()!=m_threads.find(param)){
		m_threads.erase(m_threads.find(param));
	}
	pthread_mutex_unlock(&m_lock);
}
