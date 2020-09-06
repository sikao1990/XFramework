#ifndef __ProcessImpl_Win32_H__
#define __ProcessImpl_Win32_H__
#include <windows.h>

class Process;
class ProcessImpl
{
public:
    ProcessImpl(pProceeExitHandle pfuncPtr,Process* proxy);
    ~ProcessImpl();
    unsigned StartNewProcess(const char* pPath,char** argv,void* pCallObj);
    hHandle GetProcessHandle();
    unsigned NewProcessBody(const char* pPath,char** argv);
private:
    static void CALLBACK OnExited(void* context, BOOLEAN isTimeOut);
    void OnExited();
private:
    std::string             m_cmdPath;
    PROCESS_INFORMATION     m_info;
    pProceeExitHandle       m_pFuncPtr;
    HANDLE                  m_hWait;
    Process*                m_pProxy;
};

#endif  // ProcessImpl_Win32.h
