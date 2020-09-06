#include "Process.h"

#ifdef WIN32
#include "ProcessImpl_Win32.h"
#else
#include "ProcessImol_Linux.h"
#endif

Process::Process(pProceeExitHandle pfuncPtr)
{
    _Impl = new ProcessImpl(pfuncPtr,this);
}

Process::~Process()
{
    delete _Impl;
}

unsigned Process::StartNewProcess(const char* pPath,char** argv)
{
    if(0==strlen(pPath))return -1;
    return _Impl->StartNewProcess(pPath,argv,(void*)this);
}

hHandle Process::GetProcessHandle()
{
    return _Impl->GetProcessHandle();
}

unsigned Process::NewProcessBody(const char* pPath,char** argv)
{
    return _Impl->NewProcessBody(pPath,argv);
}
