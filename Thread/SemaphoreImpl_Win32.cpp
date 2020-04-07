#include "SemaphoreImpl_Win32.h"

#define NSEMACOUNT (10)

SemaphoreImpl::SemaphoreImpl(int n):m_hSema(NULL)
{
	m_hSema=CreateSemaphore(NULL,n,NSEMACOUNT,NULL);
}

SemaphoreImpl::~SemaphoreImpl()
{
	if(NULL!=m_hSema)CloseHandle(m_hSema);
}

void SemaphoreImpl::SemPost()
{
	if(NULL!=m_hSema)
		ReleaseSemaphore(m_hSema,1,NULL);
}

void SemaphoreImpl::SemWait()
{
	if(NULL!=m_hSema)
		WaitForSingleObject(m_hSema,INFINITE);
}
