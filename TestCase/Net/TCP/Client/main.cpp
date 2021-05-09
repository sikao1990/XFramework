#include <iostream>
#include <stdio.h>

#include "../../Framework/Net/NetPackage.h"
#include "../../Framework/Net/XNetCfg.h"
#include "../Server/TMySockSession.h"
#include "../../Framework/Net/XTcpClient.h"
#include "../Server/NetMsg.h"

using namespace std;

#pragma comment(lib,"ws2_32.lib")

struct SendFrame : public MsgHead
{
	char			nData[PERIOPOSTSIZE];
	MsgFrame		frame;
	SendFrame(){
		frame.len = PERIOPOSTSIZE;
		frame.m_buf = nData;
	}
};

//1000个客户端	10 * 100，单个线程处理100 socket
#define NCLIENTS				1//1000
#define NTHREADS				1//50
#define USELIB					1

SOCKET							theSocks[NCLIENTS];
TMySockSession					theSession[NCLIENTS];
PackageProxy<TMySockSession>	parseProxy[NCLIENTS];
HANDLE							theWorker[NTHREADS];
XTcpClient						theClients[NCLIENTS];

SendFrame						testFrames[NCLIENTS][2] = {};
vector<MsgFrame* >				theFrames[NCLIENTS];

bool nflag = true;
bool gflag = 1;
HANDLE g_hSync = INVALID_HANDLE_VALUE;
int totalFrames = 0;

unsigned WINAPI worker(void* p)
{
	int index = (int)p;
	int i = 0;
	int SendCount = 0;
	time_t nPrevt = time(0);
	int	   nPrevC = 0;
	int total = sizeof(NetPackage) / (TRANSFRAME - sizeof(Msg)) + (sizeof(NetPackage) - (sizeof(NetPackage) / (TRANSFRAME - sizeof(Msg))) * (TRANSFRAME - sizeof(Msg)) > 0 ? 1 : 0);
	printf("thread ---------------------------------------------->index:%d,total:%d\n",index,total);
	while(1){
		//测试列子1：小于一帧的包
		if (!nflag)break;
		if (!gflag) {
			WaitForSingleObject(g_hSync, INFINITE);
			continue;
		}
		for (int thi = 0; thi <NCLIENTS/NTHREADS; thi++) 
		{
			MsgFrame pPrev;
			int pos = index*(NCLIENTS / NTHREADS) + thi;
#if !USELIB
			pPrev.m_buf = theFrames[pos][0]->m_buf;
			//sprintf_s(theFrames[pos][0]->m_buf+1048, 40, "hello server %d", i);//发送1500字节的数据打印
			SOCKET client = theSocks[pos];
			for (int index = 0; index < totalFrames; index++)
			{
				char* pTmp = (char*)pPrev.m_buf;
				int offset = 0;
				offset += pPrev.m_pMsg->nFrameLen;

				pPrev.m_pMsg->ntimestamp = i;// ComFunction::getTimeStamp();
				
				sprintf_s((char*)pPrev.m_pMsg->nData+4, 40, "send large frame :%d,index:%d\n", i,index);//发送小数据打印
				printf("index:%d,type:%d,totalFrame:%d,len:%d\n",index,*((unsigned*)pPrev.m_pMsg->nData), totalFrames, theFrames[pos][index]->GetValidDataLen());

				if (sizeof(NetMsg) > TRANSFRAME) {//此处暂时只考虑 大于一倍的TRANSFRAME
					MsgFrame tmpf;
					tmpf.m_buf = (pTmp + offset);
					tmpf.m_pMsg->ntimestamp = i;
				}

				int ret = send(client, (const char*)pPrev.m_buf, theFrames[pos][index]->GetValidDataLen(), 0);
				if (ret == theFrames[pos][index]->GetValidDataLen())
				{
					printf("[sock %d] %d success send size :%d,frames:%d,timestamp:%lld\n", client, i, theFrames[pos][index]->GetValidDataLen(), pPrev.m_pMsg->tFrames, pPrev.m_pMsg->ntimestamp);
				}
				else {
					printf(" send error %d,ret:%d\n", i,ret);
					system("pause");
				}

				if (PERIOPOSTSIZE > TRANSFRAME) {
					pPrev.m_buf = (pTmp + offset);
				}
				else {
					if (index + 1 == totalFrames)
						break;
					pPrev.m_buf = theFrames[pos][index + 1]->m_buf;
				}
			}
#else
			NetMsg theMsg;
			memset(&theMsg, 0, sizeof(theMsg));
			//NetPackage tmpMsg;
			//int ret = theClients[pos].RecvNetPackage(tmpMsg);
			//if (0 == ret) {
			//	memcpy(&theMsg, tmpMsg.GetData(), tmpMsg.GetLen());
			//	printf("[sock:%d] val:%s\n", theClients[pos].getSock(), theMsg.Data.comBuf);
			//	theClients[pos].RecycleMem(tmpMsg);
			//}

			
			//theMsg.type = MT_LOGON;
			sprintf_s(theMsg.Data.comBuf, 40,"hello server %d", i);
			//sprintf_s(theMsg.Data.tData.other,40,"hello server %d", i);
			NetPackage tmpVal = theClients[pos].AllocPackage(sizeof(NetMsg));
			tmpVal.CopyData(&theMsg, sizeof(theMsg));
			theClients[pos].SendNetPackage(tmpVal);

			printf("complate send:%d\n",i);
#endif
		}
		SendCount = ++i;
		//time_t tmpT = time(0);
		//if (1 == tmpT - nPrevt) {
		//	printf("%d------------------->cost:%d\n",i,SendCount - nPrevC);
		//	nPrevC = SendCount;
		//	nPrevt = tmpT;
		//}
		Sleep(30);
	}
	return 0;
}

void main()
{
	NetPackage	package;
	package.m_pData = new char[sizeof(NetMsg)];
	package.m_len = sizeof(NetMsg);
	NetMsg* p = (NetMsg*)package.m_pData;
	//p->type = MT_LOGON;

	NetPackage	package2;
	package2.m_pData = new char[sizeof(NetMsg)];
	package2.m_len = sizeof(NetMsg);
	p = (NetMsg*)package2.m_pData;
	//p->type = MT_LOGOFF;

	deque<NetPackage>	SendDatas;
	SendDatas.push_back(package);
	//SendDatas.push_back(package2);

	for (int i = 0; i < NCLIENTS; i++)
	{
		theFrames[i].push_back(&testFrames[i][0].frame);
		theFrames[i].push_back(&testFrames[i][1].frame);
	}

	g_hSync = CreateEvent(NULL, true, false, NULL);
#if !USELIB
	for (int i = 0; i < NCLIENTS; i++)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(5863);
		server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

		theSocks[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		int flag;
		flag = connect(theSocks[i], (sockaddr*)&server, sizeof(server));
		if (flag < 0) {
			cout << "error!" << endl;
			return;
		}
		theSession[i].Init(theSocks[i]);
		parseProxy[i].Init(0,(void*)(theSession + i));
		printf("client %d connected sock:!\n",i,theSocks[i]);
		Sleep(10);
	}
	int frameCount = 0;
	for (int i = 0; i < NCLIENTS; i++) 
	{
		int total = parseProxy[0].Packaging(theFrames[i], SendDatas, frameCount);
		for (int j = 0; j < frameCount; j++)
		{
			printf("frame %d count:%d validLen:%d\n",j, frameCount,theFrames[i][j]->GetValidDataLen());
		}
	}
	totalFrames = frameCount;
#else
	for (int i = 0; i < NCLIENTS; i++)
	{
		//theClients[i].Start("127.0.0.1", SERVERPORT, theSession + i);
		theClients[i].Start("127.0.0.1", SERVERPORT, theSession + i);
		Sleep(10);
	}
	Sleep(1000);
#endif
	printf("%d client has connect\n", NCLIENTS);

	for (int i = 0; i < NTHREADS; i++)
	{
		theWorker[i]=(HANDLE)_beginthreadex(NULL, 0, worker, (void*)i, 0, NULL);
	}
	while (true) {
		char c = getchar();
		if ('q' == c) {
			nflag = false;
			break;
		}
		else if ('b' == c)
			gflag = false;
		else if ('g' == c) {
			gflag = true;
			PulseEvent(g_hSync);
		}
	}
	//wait
	//close
	WSACleanup();
	system("pause");
}