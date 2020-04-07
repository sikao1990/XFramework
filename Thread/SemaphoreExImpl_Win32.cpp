#include "SemaphoreExImpl_Win32.h"

SemaphoreExImpl::SemaphoreExImpl():m_hSema(NULL),m_nPost(1),m_nWait(1)
{
}

SemaphoreExImpl::~SemaphoreExImpl(){
	if(NULL!=m_hSema)CloseHandle(m_hSema);
}

void SemaphoreExImpl::Init(int nCount,int nWait,int nPost)
{
	if (NULL != m_hSema)
		return;
	m_hSema = CreateSemaphore(NULL, nCount, NSEMACOUNT, NULL);
	m_nCount = nCount,m_nWait=nWait,m_nPost=nPost;
}

void SemaphoreExImpl::SemSignal(int n){
	if(NULL!=m_hSema){
		m_nCount += (n>0&&n<m_nPost)?n:m_nPost;
		ReleaseSemaphore(m_hSema,m_nPost,NULL);
	}
}

void SemaphoreExImpl::SemWait(int n){
	if(NULL!=m_hSema){
		for(int i=0;(i<n>0&&n<m_nWait)?n:m_nWait;i++){
			WaitForSingleObject(m_hSema,INFINITE);//每次减1
			m_nCount--;
		}
	}
}
