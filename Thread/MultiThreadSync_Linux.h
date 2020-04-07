#ifndef __MultiThreadSync_H__
#define __MultiThreadSync_H__
#include <pthread.h>
#include <iostream>
#include <map>
#include "../Mem/XAllocator.h"

using namespace std;

class MultiThreadSync
{
public:
	MultiThreadSync();
	~MultiThreadSync();
	bool Init(int n);
	void AddThread(void* param);
	void DelThread(void* param);	
	void Wait(void* param);
	void Signal();
private:
	MultiThreadSync(const MultiThreadSync&);
	MultiThreadSync& operator=(const MultiThreadSync&);
private:
	unsigned short	m_nCount;
	unsigned short	m_nStrLen;
	string 			m_strTag;
	char*			m_pFlag;
	pthread_mutex_t	m_lock;
	pthread_cond_t	m_cond;
	map<void*,int>	m_threads;
};

#endif 	// MultiThreadSync.h
