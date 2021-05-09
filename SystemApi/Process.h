#ifndef __Process_H__
#define __Process_H__

#include "../XDefine.h"

class ProcessImpl;
class Process
{
	friend class ProcessImpl;
public:
    Process(pProceeExitHandle pfuncPtr);
    virtual ~Process();
	//success processId return,error return -1
    unsigned StartNewProcess(const char* pPath,char** argv);
	//linux:return new processID
	//Win32:return handle of new process
    hHandle GetProcessHandle();
protected:
    virtual unsigned NewProcessBody(const char* pPath,char** argv);
private:
    ProcessImpl*    _Impl;
};

#endif  // Process.h
