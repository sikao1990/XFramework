#ifndef __XTcpServer_H__
#define __XTcpServer_H__

#include "SockHandle.h"
#include "NetPackage.h"

class SessionInterface;
class TcpServerImpl;
class XTcpServer
{
public:
	XTcpServer();
	~XTcpServer();
	bool SetNetParam(SessionInterface* Interface);
	bool Start(int port);
	bool Stop();

	int RecvNetPackage(const SockHandle& s, NetPackage& p);
	void  RecycleMem(const SockHandle& s, const NetPackage& p);

	const NetPackage AllocPackage(const SockHandle& s, int len);
	int SendNetPackage(const SockHandle& s, const NetPackage& p);

	void KickClient(const SockHandle& s);
protected:	
	XTcpServer(const XTcpServer& server){}
	XTcpServer& operator=(const XTcpServer& server){ return *this;}
private:
	TcpServerImpl*	_Imp;
};

#endif	// XTcpServer.h
