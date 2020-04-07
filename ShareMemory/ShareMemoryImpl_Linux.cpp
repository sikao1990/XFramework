#include "ShareMemoryImpl_Linux.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

ShareMemoryImpl::ShareMemoryImpl():m_shmid(-1),m_pStart(NULL)
{
}

bool ShareMemoryImpl::Open(const char* str,int size)const
{
	key_t key;
	if(-1!=m_shmid)
		return false;
	key=ftok(str,size);
	ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
	if(-1==(pThis->m_shmid=shmget(key, size, 0600|IPC_CREAT)))
		return false;
	printf("shareID:%d\n",m_shmid);
	return true;
}

bool ShareMemoryImpl::Attach()const
{
	ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
	if(-1==m_shmid)
		return false;
	pThis->m_pStart=shmat(m_shmid,NULL,0);
	if(NULL==m_pStart)
		return false;
	return true;
}

void ShareMemoryImpl::Detach()const
{
	if(-1!=m_shmid&&NULL!=m_pStart){
		ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
		shmdt(m_pStart);
		pThis->m_pStart = NULL;
	}
}

void* ShareMemoryImpl::GetAddress()
{
	return m_pStart;
}

const void* ShareMemoryImpl::GetAddress()const
{
	return m_pStart;
}

void ShareMemoryImpl::Close()const
{
	if(-1!=m_shmid)
	{
		ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
		shmctl(m_shmid,IPC_RMID,NULL);
		pThis->m_shmid = -1;
	}
}
