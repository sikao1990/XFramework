#ifndef __Process_H__
#define __Process_H__

#include "../XDefine.h"

class ProcessImpl;
class Process
{
public:
    Process(pProceeExitHandle pfuncPtr);
    ~Process();
    unsigned StartNewProcess(const char* pPath,char** argv);
    hHandle GetProcessHandle();
protected:
    virtual unsigned NewProcessBody(const char* pPath,char** argv);
private:
    ProcessImpl*    _Impl;
};

#endif  // Process.h
