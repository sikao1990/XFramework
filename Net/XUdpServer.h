#ifndef __XUdpServer_H__
#define __XUdpServer_H__
#include "../XDefine.h"
#include "XNetCfg.h"
#include "NetPackage.h"

class SessionInterface;
class FRAMEWORK_API XUdpServer
{
public:
	XUdpServer();
	~XUdpServer();
	bool SetNetParam(SessionInterface* Interface);
	//single,Multicast,broadcast port must be different
	//base service
	bool Start(int port);
	//Optional service enable when need on anytime(after base Start) 
	bool EnableMulticast(const char* address, int port);
	//Optional service enable when need on anytime(after base Start) 
	bool EnableBroadcast(int port);
	bool Stop();

	int RecvNetPackage(const SockHandle& s, NetPackage& p);
	void  RecycleMem(const SockHandle& s, const NetPackage& p);

	const NetPackage AllocPackage(const SockHandle& s, int len);
	int SendNetPackage(const SockHandle& s, const NetPackage& p);

	void KickClient(const SockHandle& s);
    
    void SendMulticastData(const NetPackage& data);
    void SendBroadcastData(const NetPackage& data);
protected:	
	XUdpServer(const XUdpServer& server){}
	XUdpServer& operator=(const XUdpServer& server){ return *this;}
private:
};

#endif  //XUdpServer.h
