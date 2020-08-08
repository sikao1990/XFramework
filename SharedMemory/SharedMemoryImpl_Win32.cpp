#include "SharedMemoryImpl_Win32.h"
#include <windows.h>

SharedMemoryImpl::SharedMemoryImpl():m_hMap(NULL),m_pStart(NULL)
{
}

bool SharedMemoryImpl::Open(const char* str,int size)const
{
	if(NULL!=m_hMap)
		return false;
	SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
    pThis->m_hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, str);
    if (NULL == m_hMap)
    {
		pThis->m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,size,str);
    }
    return true;
}

bool SharedMemoryImpl::Attach()const
{
	if(NULL==m_hMap)
		return false;
	SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
	pThis->m_pStart = ::MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(NULL==m_pStart)
		return false;	
	return true;
}

void SharedMemoryImpl::Detach()const
{
	SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
	if(NULL!=m_hMap&&NULL!=m_pStart){
		::UnmapViewOfFile(m_pStart);
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
	SharedMemoryImpl* pThis = const_cast<SharedMemoryImpl* >(this);
	if(NULL!=m_hMap)
	{	::CloseHandle(m_hMap);
		pThis->m_hMap = NULL;
	}
}
