#ifndef __ProcessMutex_Win32_H__
#define __ProcessMutex_Win32_H__
#include <windows.h>

class ProcessMutex
{
public:
    ProcessMutex();
    ~ProcessMutex();
    bool CreateMutex(const char* strName);
    bool OpenMutex(const char* strName);
    void Lock();
    void Unlock();
private:
    HANDLE  m_hHandle;
};

#endif // ProcessMutex_Win32.h
