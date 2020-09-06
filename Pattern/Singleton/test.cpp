#include <iostream>
#include "Singleton.h"
#include "MyService.h"
using namespace std;

#define MYSERVICE   "TestSvr"
#define SHOWNAME    "example"
#define DESCSTR     "example service"

#pragma comment(lib,"advapi32.lib")

#define TESTSERVICE	"SubSvr"
#define SSHOWNAME	"examplesub"
#define SDESCSTR	"sub example service"
#define SUBLOGSTR	"e:/test.txt"

class Myclass : public Singleton<Myclass>
{
protected:
	friend Singleton<Myclass>;
	Myclass();
public:
	void show(){
		cout <<"Myclass show"<<endl;
	}
};
//service entry point
void WINAPI ServiceMain(int argc, char** argv)//
{
    char szbuf[32]={};
    int nCount = 0;
    Service::RegisterHandle(MYSERVICE);
    while(SERVICE_STOPPED != Service::GetServiceStatus(MYSERVICE))//sample
    {    
        sprintf(szbuf,"%d",nCount++);
		MyService::Log(MYSERVICE,szbuf);
        Sleep(2000);//do work
        //if(xx)
            //Service::SetServiceStopStatus(MYSERVICE);
    }
	MyService::Log(MYSERVICE,"end");
}
void WINAPI SubServiceMain(int argc, char** argv)//Mybe run int another process,but handle was 
{
	char szbuf[32] = {};
	int nCount = 0;
	Service::RegisterHandle(TESTSERVICE);
	while (SERVICE_STOPPED != Service::GetServiceStatus(TESTSERVICE))//sample
	{
		sprintf(szbuf, "%d", nCount++);
		MyService::Log(TESTSERVICE, szbuf);
		Sleep(2000);//do work
					//if(xx)
					//Service::SetServiceStopStatus(MYSERVICE);
	}
	MyService::Log(TESTSERVICE, "end");
}

void LogCat(const char* str)
{
	FILE* fp = fopen(SUBLOGSTR, "a+");
	if (fp == NULL)
	{
		printf("error to open file:%s msg:%s err:%d\n", SUBLOGSTR, str, GetLastError());
		return;
	}

	fprintf(fp, "%s\n", str);
	fflush(fp);
	fclose(fp);
}

int main(int argc,char** argv)
{
#if 0
	Myclass& singleObj= Myclass::Instance();
	singleObj.show();
#endif
    MyService& singleSvr = MyService::Instance();
	singleSvr.RegisterService(MYSERVICE, (LPSERVICE_MAIN_FUNCTION)ServiceMain,(pLogFunc)&MyService::Logcat);
	singleSvr.RegisterService(TESTSERVICE, (LPSERVICE_MAIN_FUNCTION)SubServiceMain, (pLogFunc)LogCat);
    singleSvr.InitInstance(argc,argv);
	if (!singleSvr.Create(MYSERVICE, SHOWNAME, DESCSTR, SERVICE_DEMAND_START))
	{
		puts("create service failed");
		return 0;
	}
	if (!singleSvr.Start(MYSERVICE, 0, NULL))
	{
		puts("start service failed");
		return 0;
	}
	if (!singleSvr.Create(TESTSERVICE, SSHOWNAME, SDESCSTR, SERVICE_DEMAND_START))
	{
		puts("create service failed");
		return 0;
	}
	if (!singleSvr.Start(TESTSERVICE, 0, NULL))
	{
		puts("start service failed");
		return 0;
	}
	while (1)
	{
		char c = getchar();
		if (c == 's') {
			singleSvr.Stop(MYSERVICE);
			Sleep(500);
			singleSvr.Delete(MYSERVICE);
			Sleep(500);
		}
		else if (c == 'd') {
			singleSvr.Stop(TESTSERVICE);
			Sleep(500);
			singleSvr.Delete(TESTSERVICE);
			Sleep(500);
		}//bug:MYSERVICE stop first,when TESTSERVICE stop,the MYSERVICE was restart,other was normal
		else if ('q' == c)
			break;
		Sleep(1000);
	}
	return 0;
}
