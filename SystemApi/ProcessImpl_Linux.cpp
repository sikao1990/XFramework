#include "ProcessImpl_Linux.h"
#include "Process.h"
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

ProcessImpl::ProcessImpl(pProceeExitHandle pfuncPtr,Process* proxy):
    m_pid(-1),m_pFuncPtr(pfuncPtr)
{
    
}

ProcessImpl::~ProcessImpl()
{
    kill(m_pid,SIGTERM);
}

unsigned ProcessImpl::StartNewProcess(const char* pPath,char** argv,void* pCallObj)
{
    Process* pCall = (Process*)pCallObj;
    if(NULL==pCall)return -1;
    m_pid = vfork();
    m_cmdPath = pPath;
    signal(SIGCHLD,(pSigHandle)m_pFuncPtr);
    if(0==m_pid)
        pCall->NewProcessBody(pPath,argv);
    return m_pid;
}

hHandle ProcessImpl::GetProcessHandle()
{
    return m_pid;
}

unsigned ProcessImpl::NewProcessBody(const char* pPath,char** argv)
{
    return execvp(pPath,argv);
}

