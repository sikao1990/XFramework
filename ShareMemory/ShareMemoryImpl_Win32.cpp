#include "ShareMemoryImpl_Win32.h"
#include <windows.h>

ShareMemoryImpl::ShareMemoryImpl():m_hMap(NULL),m_pStart(NULL)
{
}

bool ShareMemoryImpl::Open(const char* str,int size)const
{
	if(NULL!=m_hMap)
		return false;
	ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
    pThis->m_hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, str);
    if (NULL == m_hMap)
    {
		pThis->m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,size,str);
    }
    return true;
}

bool ShareMemoryImpl::Attach()const
{
	if(NULL==m_hMap)
		return false;
	ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
	pThis->m_pStart = ::MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(NULL==m_pStart)
		return false;	
	return true;
}

void ShareMemoryImpl::Detach()const
{
	ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
	if(NULL!=m_hMap&&NULL!=m_pStart){
		::UnmapViewOfFile(m_pStart);
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
	ShareMemoryImpl* pThis = const_cast<ShareMemoryImpl* >(this);
	if(NULL!=m_hMap)
	{	::CloseHandle(m_hMap);
		pThis->m_hMap = NULL;
	}
}
