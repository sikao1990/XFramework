#ifndef __ProcessEvent_Win32_H__
#define __ProcessEvent_Win32_H__
#include <windows.h>

class ProcessEvent
{
public:
    ProcessEvent();
    ~ProcessEvent();
    bool CreateEvent(const char* strName,bool bManual=false);
    bool OpenEvent(const char* strName);
    void Wait();
    void Post();
    void ManualReset();
private:
    HANDLE m_hHandle;
    bool m_bManual;
};

#endif  // ProcessEvent_Win32.h
