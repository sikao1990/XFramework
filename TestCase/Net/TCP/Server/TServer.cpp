#include "../NetFrame.h"
#include <iostream>
#include <thread>
#include <Windows.h>
#include "TSockSession.h"

using namespace std;

void send_thread(NetFrame* net)
{
	while(1){
		for (int i = CONNECT_COUNT;i>=0; i--)
			net->RecvClientData(i,NULL,i);
			//net->RemoveClient(i);
		std::chrono::milliseconds dura(1000);
		std::this_thread::sleep_for(dura);	
	}
}

int main()
{
#if 0
	NetFrame	net;
	for(int i=0;i<CONNECT_COUNT;i++)
	{
		net.AppendNewClient(i);
	}
	
	std::thread t(send_thread,&net);
    t.join();
#endif
	FILE* stream;
	//freopen_s(&stream, "d://data.txt", "w", stdout);
	TSockSession server;
	server.Init();
	while (1) {
		//char c = getchar();
		//if ('q' == c)
		//	break;
		//else
			Sleep(2000);
	}
	system("pause");
	return 0;
}
