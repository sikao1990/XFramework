#ifndef __MyService_H__
#define __MyService_H__
#include "Singleton.h"
#include "../../SystemApi/Service_Win32.h"

#define LOGFILE_PATH	"e:/test.log"

class MyService : public Service , public Singleton<MyService>
{
protected:
	friend Singleton<MyService>;
	MyService(); 
public:
    virtual bool OnInitInstance(int argc,char** argv);
    virtual DWORD Handle(const char* SvrName,DWORD dwControl,DWORD dwEventType,LPVOID lpEventData, SERVICE_STATUS* pServiceStatus);
    static void Logcat(const char*);
};

#endif  // MyService.h
