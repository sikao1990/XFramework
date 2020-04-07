#include "SemaphoreImpl_Linux.h"
#include <stdio.h>

SemaphoreImpl::SemaphoreImpl(int n)
{
	sem_init(&m_sync, 0, n);
}

SemaphoreImpl::~SemaphoreImpl()
{
	sem_destroy(&m_sync);
}

void SemaphoreImpl::SemPost()
{
	sem_post(&m_sync);
}

void SemaphoreImpl::SemWait()
{
	sem_wait(&m_sync);
}
	