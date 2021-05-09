#include "ProcessMutex_Win32.h"

ProcessMutex::ProcessMutex():m_hHandle(NULL)
{
    
}

ProcessMutex::~ProcessMutex()
{
    if(NULL!=m_hHandle)CloseHandle(m_hHandle);
}

bool ProcessMutex::CreateMutex(const char* strName)
{
    if(NULL!=m_hHandle)return false;
    return NULL!=(m_hHandle = ::CreateMutex(NULL,false,strName));
}

bool ProcessMutex::OpenMutex(const char* strName)
{
    if(NULL!=m_hHandle)return false;
    return NULL!=(m_hHandle = ::OpenMutex(MUTEX_ALL_ACCESS,false,strName));
}

void ProcessMutex::Lock()
{
    if(NULL==m_hHandle)return;
    WaitForSingleObject(m_hHandle,INFINITE);
}

void ProcessMutex::Unlock()
{
    if(NULL==m_hHandle)return;
    ReleaseMutex(m_hHandle);
}
