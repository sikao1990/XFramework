#include "Service_Win32.h"
#include "ForceConvert.h"
#include <utility>

using namespace std;

std::vector<SERVICE_TABLE_ENTRY>    Service::m_tabVec;
std::map<std::string,SvrInfo>       Service::m_ctrlMap;
Service* Service::pThis;

Service::Service()
{
    pThis = this;
    m_tabVec.reserve(MAX_SERVICE_COUNT);
}

Service::~Service()
{
    
}

void Service::RegisterService(const char* pStrName,LPSERVICE_MAIN_FUNCTION pfunc, pLogFunc logFuncPtr)
{
    SERVICE_TABLE_ENTRY entry;
    entry.lpServiceName = (LPSTR)pStrName;
    entry.lpServiceProc = pfunc;
    if (m_tabVec.capacity() == m_tabVec.size() - 1)
    {
        return;
    }
    if( m_tabVec.capacity() > m_tabVec.size() )
    {
        if( m_tabVec.empty() )
            m_tabVec.push_back(entry);
        else
            m_tabVec[m_tabVec.size()-1] = entry;
    }
    entry.lpServiceName = NULL;
    entry.lpServiceProc = NULL;
    m_tabVec.push_back(entry);

    SvrInfo info = {};
	info.pLogFuncPtr = logFuncPtr;
    m_ctrlMap.insert( make_pair(pStrName, info) );
}

bool Service::RegisterHandle(const char* strSvrName,unsigned uType)
{
	BOOL bRet = TRUE;
    char szbuf[32] = {};

    std::map<std::string,SvrInfo>::iterator it = m_ctrlMap.find(strSvrName);
    if ( m_ctrlMap.end() == it )
        return false;

	it->second.ServiceStatus.dwWin32ExitCode = 0;
	it->second.ServiceStatus.dwCheckPoint = 0;
	it->second.ServiceStatus.dwWaitHint = 0;
	it->second.ServiceStatus.dwServiceType = uType;//SERVICE_WIN32;
	it->second.ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	it->second.ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	it->second.ServiceStatus.dwServiceSpecificExitCode = 0;
    const char* pStr = m_ctrlMap.find(strSvrName)->first.c_str();
	it->second.hStatus = RegisterServiceCtrlHandlerEx(strSvrName, (LPHANDLER_FUNCTION_EX)CtrlHandleEx,ForceCast<LPVOID>(pStr));
	if (it->second.hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		// log failed
		return false;
	}

    sprintf(szbuf,"RegisterHandle hStatus %d",it->second.hStatus->unused);
    it->second.pLogFuncPtr(szbuf);
	//service status update
	it->second.ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(it->second.hStatus, &(it->second.ServiceStatus));
    return true;
}

DWORD Service::Handle(const char* SvrName,DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,SERVICE_STATUS* pServiceStatus)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
		Log(SvrName,"Monitoring stopped.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_STOPPED;
        OnStop(SvrName, dwEventType, lpEventData);
		return NO_ERROR;
	case SERVICE_CONTROL_SHUTDOWN:
		Log(SvrName, "Monitoring stopped shutdown.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_STOPPED;
        OnShutDown(SvrName, dwEventType, lpEventData);
		return NO_ERROR;
	case SERVICE_CONTROL_PAUSE:
		Log(SvrName, "Monitoring pause.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_PAUSED;
        OnPause(SvrName, dwEventType, lpEventData);
		return NO_ERROR;
	case SERVICE_CONTROL_CONTINUE:
		Log(SvrName, "Monitoring continue.");
		pServiceStatus->dwWin32ExitCode = 0;
		pServiceStatus->dwCurrentState = SERVICE_RUNNING;
        OnContinue(SvrName, dwEventType, lpEventData);
		return NO_ERROR;
	default:
		break;
	}
    return -1;
}

DWORD Service::CtrlHandleEx(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext)
{
    if(NULL==pThis)
        return -1;
    const char* str = (const char*)lpContext;
	std::map<std::string, SvrInfo>::iterator it = m_ctrlMap.find(str);
	if ( m_ctrlMap.end() == it ) 
    {
		Log(str,"con't find register server name");
		return -1;
	}
    SERVICE_STATUS* pStatus = &(it->second.ServiceStatus);
    DWORD dwRet = NO_ERROR;
    dwRet = pThis->Handle(str,dwControl,dwEventType,lpEventData,pStatus);

    char szbuf[64]={};
    sprintf(szbuf,"CtrlHandleEx hStatus %d ,ret:%u",it->second.hStatus->unused,dwRet);
    Log(str, szbuf);
    
    SetServiceStatus(it->second.hStatus, &(it->second.ServiceStatus));
    return dwRet;
}

bool Service::InitInstance(const char* pszSvrName)
{
    StartServiceCtrlDispatcher(&(m_tabVec[0]));
	if (NULL == pThis)
        return false;
    return true;
}

bool Service::Create(const char* svrName,const char* svrShowName,const char* strDesc,const char* strSvrCmdPath,unsigned dwStartStyle)
{
    SC_HANDLE scm = NULL;
    SC_HANDLE scv = NULL;
    DWORD dwErrorCode;
    printf("Creating Service .... ");
    scm = OpenSCManager(0/*localhost*/,
                        NULL/*SERVICES_ACTIVE_DATABASE*/,
						SC_MANAGER_CREATE_SERVICE/*SC_MANAGER_ALL_ACCESS*//*ACCESS*/);
    if (NULL == scm)
    {
        printf("OpenSCManager error:%d\n", GetLastError());
        return false;
    }
    scv = CreateService(scm,//句柄
						svrName,//服务开始名
						svrShowName,//显示服务名
						SERVICE_ALL_ACCESS, //服务访问类型
						SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,//服务类型
						dwStartStyle, //自动启动服务
						SERVICE_ERROR_IGNORE,//忽略错误
                        strSvrCmdPath,//启动的文件名
						NULL,//name of load ordering group (载入组名)
						0,//标签标识符
						NULL,//相关性数组名
						NULL,//帐户(当前)
						NULL); //密码(当前)

    if (NULL == scv)
    {
        dwErrorCode = GetLastError();
		CloseServiceHandle(scm);
        if(ERROR_SERVICE_EXISTS != dwErrorCode)
        {
            printf("Failure !\n");
        }
        else
        {
            printf("already Exists !\n");
        }
		return false;
    }
    else
    {
        SERVICE_DESCRIPTIONA info;
        info.lpDescription = (LPSTR)strDesc;
        ChangeServiceConfig2(scv, SERVICE_CONFIG_DESCRIPTION, &info);
        printf("Success !\n");
        CloseServiceHandle(scv);
    }

    CloseServiceHandle(scm);
    return true;
}

void Service::Delete(const char* strSvrName)
{
    SC_HANDLE scm = NULL;
    SC_HANDLE scv = NULL;
    SERVICE_STATUS              ServiceStatus;

    scm = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if (NULL != scm)
    {
        scv=OpenService(scm,strSvrName,SERVICE_ALL_ACCESS);
        if (NULL != scv)
        {
            QueryServiceStatus(scv,&(ServiceStatus));
            if (ServiceStatus.dwCurrentState==SERVICE_RUNNING)
            {
                ControlService(scv,SERVICE_CONTROL_STOP,&(ServiceStatus));
            }
			DeleteService(scv);
			CloseServiceHandle(scv);
            printf("Uninstall %s success!\n",strSvrName);
        }
		CloseServiceHandle(scm);
    }   
}

bool Service::Start(const char* strSvrName,int argc,char** argv)
{
    SERVICE_STATUS              ServiceStatus;
    DWORD dwErrorCode = NO_ERROR;
    SC_HANDLE scm = NULL;
    SC_HANDLE scv = NULL;
    //Starting Service
    scm = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(NULL != scm)
    {
        scv = OpenService(scm, strSvrName, SERVICE_ALL_ACCESS);
        if (NULL != scv)
        {
            if(StartService(scv, argc, (LPCSTR*)argv)==0)
            {
                dwErrorCode = GetLastError();
				CloseServiceHandle(scv);
				CloseServiceHandle(scm);
                if(dwErrorCode == ERROR_SERVICE_ALREADY_RUNNING)
                {
                    printf("already Running ! errCode:%u\n", dwErrorCode);
                }
                return false;
            }
			else
			{
				printf("Pending ... \n");
			}

			//wait until the servics started
			while(QueryServiceStatus(scv,&ServiceStatus)!=0)
			{
				if(SERVICE_START_PENDING == ServiceStatus.dwCurrentState)
				{
					Sleep(100);
				}
				else
				{
					printf("start complate:%d\n", SERVICE_RUNNING);
					break;
				}
			}
			CloseServiceHandle(scv);
			CloseServiceHandle(scm);
			return true;
        }
        else
        {
            //error to OpenService
            printf("error to OpenService\n");
			CloseServiceHandle(scm);
            return false;
        }
    }
    else
    {
        puts("fail to OpenSCManager");
        return false;
    }
    return false;
}

void Service::Stop(const char* strSvrName)
{
    SC_HANDLE scm = NULL;
    SC_HANDLE scv = NULL;
    SERVICE_STATUS              ServiceStatus;
    scm = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(NULL != scm)
    {
        scv = OpenService(scm,strSvrName,SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (NULL != scv)
        {
            QueryServiceStatus(scv,&(ServiceStatus));
            if (SERVICE_RUNNING == ServiceStatus.dwCurrentState)
            {
				Log(strSvrName, "stop service");
                ControlService(scv,SERVICE_CONTROL_STOP,&(ServiceStatus));
            }
            CloseServiceHandle(scv);
        }
        CloseServiceHandle(scm);
    }    
}

void Service::SetServiceStopStatus(const char* strSvrName)
{
    std::map<std::string,SvrInfo>::iterator it = m_ctrlMap.find(strSvrName);
	if (m_ctrlMap.end() == it)return;
    it->second.ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    it->second.ServiceStatus.dwWin32ExitCode = -1;
    SetServiceStatus(it->second.hStatus, &(it->second.ServiceStatus));    
}

bool Service::IsServiceStopStatus(const char* strSvrName)
{
    std::map<std::string,SvrInfo>::iterator it = m_ctrlMap.find(strSvrName);
	if (m_ctrlMap.end() == it)return false;
    return !(SERVICE_RUNNING == it->second.ServiceStatus.dwCurrentState);
}

void Service::Log(const char* strSvrName, const char* str)
{
	if (NULL != pThis && m_ctrlMap.end()!= m_ctrlMap.find(strSvrName))
	{
		m_ctrlMap.find(strSvrName)->second.pLogFuncPtr(str);
	}
}

int Service::GetServiceStatus(const char* strSvrName)
{
	std::map<std::string, SvrInfo>::iterator it = m_ctrlMap.find(strSvrName);
	if (m_ctrlMap.end() == it)return -1;
	return it->second.ServiceStatus.dwCurrentState;
}


bool Service::IsInstalled(const char* strSvrName)
{
	bool bResult = FALSE;

	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL != hSCM)
	{
		//打开服务
		SC_HANDLE hService = ::OpenService(hSCM, strSvrName, SERVICE_QUERY_CONFIG);
		if (NULL != hService)
		{
			bResult = TRUE;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bResult;
}

void Service::SetExitCode(SERVICE_STATUS* pStatus, int nCode)
{
	if(NULL!= pStatus)
		pStatus->dwWin32ExitCode = nCode;
}

void Service::SetCurrentStatus(SERVICE_STATUS* pStatus, unsigned dwStatus)
{
	if (NULL != pStatus)
		pStatus->dwCurrentState = dwStatus;
}

void Service::OnStop(const char* SvrName, DWORD dwEventType, LPVOID lpEventData)
{

}

void Service::OnPause(const char* SvrName, DWORD dwEventType, LPVOID lpEventData)
{

}

void Service::OnContinue(const char* SvrName, DWORD dwEventType, LPVOID lpEventData)
{

}

void Service::OnShutDown(const char* SvrName, DWORD dwEventType, LPVOID lpEventData)
{

}
