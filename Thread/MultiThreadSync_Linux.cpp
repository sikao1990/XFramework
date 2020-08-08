#include "MultiThreadSync_linux.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

MultiThreadSync::MultiThreadSync():m_pFlag(NULL),m_nCount(0),m_strTag("")
{
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_cond,NULL);
}

MultiThreadSync::~MultiThreadSync()
{
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_cond);
	delete [] m_pFlag;
}

bool MultiThreadSync::Init(int n)
{
	m_pFlag = new char[n+1];
	m_nStrLen = n;
	m_nCount = 0;
	for(int i=0;i<n;i++)
		m_strTag.push_back('0');
}

void MultiThreadSync::Wait(void* param)
{
	int nPos = -1;
	if(m_nStrLen!=m_nCount)
		return;
	if(m_threads.end()==m_threads.find(param))
		return;
	pthread_mutex_lock(&m_lock);
	while('0'==m_pFlag[nPos=m_threads.find(param)->second])
		pthread_cond_wait(&m_cond,&m_lock);
	m_pFlag[nPos]='0';
	pthread_mutex_unlock(&m_lock);
}

void MultiThreadSync::Signal()
{
	if(m_nStrLen!=m_nCount)
		return;	
	pthread_mutex_lock(&m_lock);
	for(map<void*,int>::iterator it=m_threads.begin();it!=m_threads.end();it++)
		m_pFlag[it->second]='1';
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_lock);
}

void MultiThreadSync::AddThread(void* param)
{
	m_threads.insert(make_pair(param,m_nCount));
	m_pFlag[m_nCount++]='0';//须使用编号分配管理(对象分配管理)
}

void MultiThreadSync::DelThread(void* param)
{
	if(m_threads.end()!=m_threads.find(param)){
		m_threads.erase(m_threads.find(param));
		m_pFlag[m_threads.find(param)->second]='1';
	}
	if(0==m_threads.size()){
		m_nCount = 0;
		memset(m_pFlag,0,m_nStrLen);
	}
}
