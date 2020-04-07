#include "XTcpClient.h"

#ifdef WIN32
#include "TcpClientImpl_Win32.h"
#else
#include "TcpClientImpl_Linux.h"
#endif

XTcpClient::XTcpClient()
{
	_Impl = new TcpClientImpl;
}

XTcpClient::~XTcpClient()
{
	delete _Impl;
}

bool XTcpClient::Start(const char* IP,int port,SocketSession* pSession)
{
	return _Impl->Start(IP,port,pSession);
}

void XTcpClient::Stop()
{
	_Impl->Stop();
}

int XTcpClient::RecvNetPackage(NetPackage& p)
{
	return _Impl->RecvNetPackage(p);
}

void  XTcpClient::RecycleMem(const NetPackage& p)
{
	_Impl->RecycleMem(p);
}

const NetPackage XTcpClient::AllocPackage(int len)
{
	return _Impl->AllocPackage(len);
}

int XTcpClient::SendNetPackage(const NetPackage& p)
{
	return _Impl->SendNetPackage(p);
}

hSockFd XTcpClient::getSock()const
{
	return _Impl->getSock();
}
