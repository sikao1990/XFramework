#ifndef __XUdpClient_H__
#define __XUdpClient_H__
#include "../XDefine.h"
#include "XNetCfg.h"
#include "NetPackage.h"

class SocketSession;
class FRAMEWORK_API XUdpClient
{
public:
	XUdpClient();
	~XUdpClient();

	bool Start(const char* IP,int port,SocketSession* pSession);
    bool SetRecvMulticast(const char* address,int port);
    bool SetRecvBroadcast(int port);
	void Stop();

	int RecvNetPackage(NetPackage& p);
	void  RecycleMem(const NetPackage& p);

	const NetPackage AllocPackage(int len);
	int SendNetPackage(const NetPackage& p);
	hSockFd getSock()const;	
//protected:    
private:
	XUdpClient(const XUdpClient& other){}
	XUdpClient& operator=(const XUdpClient& other){ return *this;}
private:
	//UdpClientImpl*	_Impl;
};

#endif  //XUdpClient.h
