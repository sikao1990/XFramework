#include "XUdpClient.h"

//#ifdef WIN32
//#else
//#endif

XUdpClient::XUdpClient()
{
   //TODO: 
}

XUdpClient::~XUdpClient()
{
    //TODO:
}

bool XUdpClient::Start(const char* IP,int port,SocketSession* pSession)
{
    //TODO:
    return true;
}

bool XUdpClient::SetRecvMulticast(const char* address,int port)
{
    //TODO:
    return true;    
}

bool XUdpClient::SetRecvBroadcast(int port)
{
    //TODO:
    return true;
}

void XUdpClient::Stop()
{
    //TODO:
}

int XUdpClient::RecvNetPackage(NetPackage& p)
{
    //TODO:
	return 0;
}

void  XUdpClient::RecycleMem(const NetPackage& p)
{
    //TODO:
}

const NetPackage XUdpClient::AllocPackage(int len)
{
    NetPackage package;
    //TODO:
    return package;
}

int XUdpClient::SendNetPackage(const NetPackage& p)
{
    //TODO:
    return 0;
}

hSockFd XUdpClient::getSock()const
{
    hSockFd fd = 0;
	//TODO:
    return fd;
}
