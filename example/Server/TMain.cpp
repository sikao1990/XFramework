#include <iostream>
#include <thread>
#include "TServerSession.h"

using namespace std;

int main()
{
	FILE* stream;
	//freopen_s(&stream, "d://data.txt", "w", stdout);
	TServerSession server;
	//server.Init(CONNTYPE_TCP);
	server.Init(CONNTYPE_UDP);
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
