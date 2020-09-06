#include "MyService.h"

MyService::MyService()
{

}

bool MyService::OnInitInstance(int argc,char** argv)
{
    return true;
}

DWORD MyService::Handle(const char* SvrName,DWORD dwControl,DWORD dwEventType,LPVOID lpEventData, SERVICE_STATUS* pServiceStatus)
{
	Log(SvrName,"MyService Handle");
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
		Log(SvrName, "Monitoring stopped.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_STOPPED;
		return NO_ERROR;
	case SERVICE_CONTROL_SHUTDOWN:
		Log(SvrName, "Monitoring stopped shutdown.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_STOPPED;
		return NO_ERROR;
	case SERVICE_CONTROL_PAUSE:
		Log(SvrName, "Monitoring pause.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_PAUSED;
		return NO_ERROR;
	case SERVICE_CONTROL_CONTINUE:
		Log(SvrName, "Monitoring continue.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_RUNNING;
		return NO_ERROR;
	default:
		break;
	}
	return -1;
}

void MyService::Logcat(const char* str)
{
    FILE* fp = fopen(LOGFILE_PATH, "a+");
    if(fp == NULL)
    {
        printf("error to open file:%s msg:%s err:%d\n", LOGFILE_PATH,str, GetLastError());
        return;
    }
    
    fprintf(fp, "%s\n",str);
    fflush(fp);
    fclose(fp);    
}
