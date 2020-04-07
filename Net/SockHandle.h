#ifndef __SockHandle_H__
#define __SockHandle_H__

#include <iostream>
#include <assert.h>
#include "XNetCfg.h"

using namespace std;
struct SockHandle
{
	hSockFd	m_sock;
	string	m_ip;
	int		m_type;	//0 tcp,1 udp
	int		m_port;
	SockHandle(int type=-1,const char* ip="", int port=0,hSockFd sock=-1) :m_type(type), m_ip(ip), m_port(port), m_sock(sock){}
	bool operator==(const SockHandle& hSock)const
	{
		return (m_sock == hSock.m_sock && m_ip == hSock.m_ip && m_type == hSock.m_type);
	}
	bool operator<(const SockHandle& hSock)const
	{
		assert(hSock.m_type == this->m_type);
		if (CONNTYPE_TCP == hSock.m_type)
			return m_sock < hSock.m_sock;
		else if (CONNTYPE_UDP == hSock.m_type)
			return m_ip < hSock.m_ip;
		else 
			return false;
	}
	SockHandle& operator=(const SockHandle& hSock)
	{
		this->m_sock = hSock.m_sock;
		this->m_ip = hSock.m_ip;
		this->m_type = hSock.m_type;
		this->m_port = hSock.m_port;
		return *this;
	}
};

#endif	// SockHandle.h
