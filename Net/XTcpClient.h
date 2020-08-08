#ifndef __XTcpClient_H__
#define __XTcpClient_H__

#include "XNetCfg.h"
#include "NetPackage.h"

class SocketSession;
class TcpClientImpl;
class FRAMEWORK_API XTcpClient
{
public:
	XTcpClient();
	~XTcpClient();

	bool Start(const char* IP,int port,SocketSession* pSession);
	void Stop();

	int RecvNetPackage(NetPackage& p);
	void  RecycleMem(const NetPackage& p);

	const NetPackage AllocPackage(int len);
	int SendNetPackage(const NetPackage& p);
	hSockFd getSock()const;	
private:
	XTcpClient(const XTcpClient& other){}
	XTcpClient& operator=(const XTcpClient& other){ return *this;}
private:
	TcpClientImpl*	_Impl;
};

#endif	// XTcpClient.h
