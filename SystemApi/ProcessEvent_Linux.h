#ifndef __ProcessEvent_Linux_H__
#define __ProcessEvent_Linux_H__

class ProcessEvent
{
public:
    ProcessEvent();
    ~ProcessEvent();
    bool CreateEvent(const char* strName,int mode=0600);
    bool OpenEvent(const char* strName,int mode=0600);
    void Wait();
    void Post();
    void DeleteEvent();
private:
    int m_id;
};

#endif
