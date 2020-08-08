#include "SharedMemoryImpl_Linux.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

SharedMemoryImpl::SharedMemoryImpl():m_shmid(-1),m_pStart(NULL)
{
}

bool SharedMemoryImpl::Open(const char* str,int size)const
{
	key_t key;
	if(-1!=m_shmid)
		return false;
	key=ftok(str,size);
	SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
	if(-1==(pThis->m_shmid=shmget(key, size, 0600|IPC_CREAT)))
		return false;
	printf("shareID:%d\n",m_shmid);
	return true;
}

bool SharedMemoryImpl::Attach()const
{
	SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
	if(-1==m_shmid)
		return false;
	pThis->m_pStart=shmat(m_shmid,NULL,0);
	if(NULL==m_pStart)
		return false;
	return true;
}

void SharedMemoryImpl::Detach()const
{
	if(-1!=m_shmid&&NULL!=m_pStart){
		SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
		shmdt(m_pStart);
		pThis->m_pStart = NULL;
	}
}

void* SharedMemoryImpl::GetAddress()
{
	return m_pStart;
}

const void* SharedMemoryImpl::GetAddress()const
{
	return m_pStart;
}

void SharedMemoryImpl::Close()const
{
	if(-1!=m_shmid)
	{
		SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
		shmctl(m_shmid,IPC_RMID,NULL);
		pThis->m_shmid = -1;
	}
}
