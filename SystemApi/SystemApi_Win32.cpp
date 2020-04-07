#include "SystemApi_Win32.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
#include <stdio.h>

#pragma comment( lib , "shlwapi.lib" )

unsigned SystemApi::GetProcessID(const char* CmdName)
{
	DWORD dwRet = 0;
	HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		printf("\n获得进程快照失败,返回的GetLastError():%d", ::GetLastError());
		return dwRet;
	}
 
	PROCESSENTRY32 pe32;//声明进程入口对象 
	pe32.dwSize = sizeof(PROCESSENTRY32);//填充进程入口对象大小 
	::Process32First(hSnapShot, &pe32);//遍历进程列表 
	do
	{
		if (PathMatchSpec(pe32.szExeFile, CmdName))//查找指定进程名的PID 
		{
			dwRet = pe32.th32ProcessID;
			break;
		}
	} while (::Process32Next(hSnapShot, &pe32));
	::CloseHandle(hSnapShot);
	return dwRet; 
}

