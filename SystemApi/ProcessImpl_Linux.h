#ifndef __ProcessImpl_Linux_H__
#define __ProcessImpl_Linux_H__
#include <string>
#include "../XDefine.h"

class Process;
class ProcessImpl
{
    typedef void (*pSigHandle)(int);
public:
    ProcessImpl(pProceeExitHandle pfuncPtr,Process* proxy);
    ~ProcessImpl();
    unsigned StartNewProcess(const char* pPath,char** argv,void* pCallObj);
    hHandle GetProcessHandle();
    unsigned NewProcessBody(const char* pPath,char** argv);
private:
    int                 m_pid;
    std::string         m_cmdPath;
    pProceeExitHandle   m_pFuncPtr;
};

#endif  // ProcessImpl_Linux.h
