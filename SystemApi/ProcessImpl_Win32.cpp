#include "ProcessImpl_Win32.h"
#include "Process.h"
#include <stdlib.h>

ProcessImpl::ProcessImpl(pProceeExitHandle pfuncPtr,Process* proxy):
    m_pFuncPtr(pfuncPtr),m_hWait(NULL),m_pProxy(proxy)
{
    memset(&m_info,0,sizeof(m_info));
}

ProcessImpl::~ProcessImpl()
{
    if(m_info.hThread) CloseHandle(m_info.hThread);
    if(m_hWait) 
    {
        ::UnregisterWaitEx(m_hWait, INVALID_HANDLE_VALUE); // INVALID_HANDLE_VALUE means "Wait for pending callbacks"
        m_hWait = NULL;
    }
    if(m_info.hProcess)
    {
        DWORD code;
        if (GetExitCodeProcess(m_info.hProcess, &code))
        {
            if (code == STILL_ACTIVE) TerminateProcess(m_info.hProcess, 1);
        }

        CloseHandle(m_info.hProcess);
    }
}

unsigned ProcessImpl::StartNewProcess(const char* pPath,char** argv,void* pCallObj)
{
    Process* pCall = (Process*)pCallObj;
    if(NULL==pCall)return 0;
    m_cmdPath = pPath;
    return pCall->NewProcessBody(pPath,argv);
}

hHandle ProcessImpl::GetProcessHandle()
{
    return m_info.hProcess;
}

unsigned ProcessImpl::NewProcessBody(const char* pPath,char** argv)
{
	 = NULL;
	STARTUPINFO si = { sizeof(si) };
	if(TRUE == CreateProcess(NULL,
		m_cmdPath.c_str(),
		NULL,
		NULL,
		FALSE,
		NULL,
		NULL,
		NULL,
		&si,
		&m_info
	))
    BOOL result = RegisterWaitForSingleObject(&m_hWait, m_info.hProcess, OnExited, this, INFINITE, WT_EXECUTEONLYONCE);
    if (!result)
    {
        DWORD error = GetLastError();
        CloseHandle(m_info.hThread);
        CloseHandle(m_info.hProcess);
        printf("RegisterWaitForSingleObject() failed with error code %d\n",error);
        return -1;
    }
        return m_info.dwProcessId;
    else
        return -1;
}

void ProcessImpl::OnExited(void* context, BOOLEAN isTimeOut)
{
    ((ProcessImpl*)context)->OnExited();
}

void ProcessImpl::OnExited()
{
    m_pFuncPtr(m_pProxy);
}

