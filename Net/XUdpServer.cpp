#include "XUdpServer.h"

XUdpServer::XUdpServer()
{
    
}

XUdpServer::~XUdpServer()
{
   
}

bool XUdpServer::SetNetParam(SessionInterface* Interface)
{
     //TODO:
    return true;   
}

bool XUdpServer::EnableMulticast(const char* address,int port)
{
    //TODO:
    return true;    
}

bool XUdpServer::EnableBroadcast(int port)
{
    //TODO:
    return true;    
}

bool XUdpServer::Start(int port)
{
    //TODO:
    return true;    
}

bool XUdpServer::Stop()
{
    //TODO:
    return true;    
}

int XUdpServer::RecvNetPackage(const SockHandle& s, NetPackage& p)
{
    //TODO:
    return 0;
}

void  XUdpServer::RecycleMem(const SockHandle& s, const NetPackage& p)
{
    //TODO:
}

const NetPackage XUdpServer::AllocPackage(const SockHandle& s, int len)
{
    NetPackage package;
    //TODO:
    return package;
}

int XUdpServer::SendNetPackage(const SockHandle& s, const NetPackage& p)
{
    return 0;
}

void XUdpServer::KickClient(const SockHandle& s)
{
    //TODO:
}

void XUdpServer::SendMulticastData(const NetPackage& data)
{
    //TODO:
}

void XUdpServer::SendBroadcastData(const NetPackage& data)
{
    //TODO:
}
