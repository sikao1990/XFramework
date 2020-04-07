#include "XTcpServer.h"

#ifdef WIN32
#include "TcpServerImpl_Win32.h"
#else
#include "TcpServerImpl_Linux.h"
#endif

XTcpServer::XTcpServer()
{
	_Imp = new TcpServerImpl;
}

XTcpServer::~XTcpServer()
{
	delete _Imp;
}

bool XTcpServer::SetNetParam(SessionInterface* Interface)
{
	return _Imp->SetNetParam(Interface);
}

bool XTcpServer::Start(int port)
{
	return _Imp->Start(port);
}

bool XTcpServer::Stop()
{
	return _Imp->Stop();
}

int XTcpServer::RecvNetPackage(const SockHandle& s, NetPackage& p)
{
	return _Imp->RecvNetPackage(s,p);
}

void XTcpServer::RecycleMem(const SockHandle& s, const NetPackage& p)
{
	_Imp->RecycleMem(s,p);
}

const NetPackage XTcpServer::AllocPackage(const SockHandle& s, int len)
{
	return _Imp->AllocPackage(s,len);
}

int XTcpServer::SendNetPackage(const SockHandle& s, const NetPackage& p)
{
	return _Imp->SendNetPackage(s,p);
}

void XTcpServer::KickClient(const SockHandle& s)
{
	_Imp->KickClient(s);
}
