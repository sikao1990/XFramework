#ifndef __Service_Win32_H__
#define __Service_Win32_H__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsvc.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include "../XDefine.h"

#define MAX_SERVICE_COUNT   5

typedef void(*pLogFunc)(const char*);

struct SvrInfo{
    SERVICE_STATUS              ServiceStatus;
    SERVICE_STATUS_HANDLE       hStatus;
	pLogFunc					pLogFuncPtr;
    SvrInfo(){memset(this,0,sizeof(*this));}
};

//the child Inherits Servuce and must be single class
class FRAMEWORK_API Service
{
public:
    Service();
    virtual ~Service();

    //pSvrfunc the service entry point
    static void RegisterService(const char* pStrName,LPSERVICE_MAIN_FUNCTION pSvrfunc, pLogFunc logFuncPtr);
    //call at main function 
    static bool InitInstance(int argc,char** argv);
    static bool Create(const char* strSvrName,const char* svrShowName,const char* strDesc,unsigned dwStartStyle);
    static void Delete(const char* strSvrName);
	//argv array was end of NULL
	static bool Start(const char* strSvrName,int argc,char** argv);
	static void Stop(const char* strSvrName);
	static bool IsInstalled(const char* strSvrName);
    
    //the pSvrfunc must be call this function register when defined
    static bool RegisterHandle(const char* pStrSvrName,unsigned uType=SERVICE_WIN32);
    static void SetServiceStopStatus(const char* strSvrName);
    static bool IsServiceStopStatus(const char* strSvrName);
	/* 
	status:
		SERVICE_STOPPED                        0x00000001
		SERVICE_START_PENDING                  0x00000002
		SERVICE_STOP_PENDING                   0x00000003
		SERVICE_RUNNING                        0x00000004
		SERVICE_CONTINUE_PENDING               0x00000005
		SERVICE_PAUSE_PENDING                  0x00000006
		SERVICE_PAUSED                         0x00000007
	*/
	static int GetServiceStatus(const char* strSvrName);
    static void Log(const char* strSvrName,const char* str);
protected:
	static void SetExitCode(SERVICE_STATUS* pStatus, int nCode);
	static void SetCurrentStatus(SERVICE_STATUS* pStatus, unsigned dwStatus);
    virtual bool OnInitInstance(int argc,char** argv);
    virtual DWORD Handle(const char* strSvrName,DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,SERVICE_STATUS* pStatus);
private:
    static DWORD WINAPI CtrlHandleEx(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext);
protected:
    static std::vector<SERVICE_TABLE_ENTRY> m_tabVec;
    static std::map<std::string,SvrInfo>    m_ctrlMap;
    static Service*							pThis;
};

#endif // Service_Win32.h
