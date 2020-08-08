#ifndef __TcpServerImpl_Linux_H__
#define __TcpServerImpl_Linux_H__

#define EPOLLDEALCOUNT	2
#define POOLTHREADCOUNT	4

#include "XNetCfg.h"
#include "SockBase.h"
#include "SockHandle.h"
#include "NetPackage.h"
#include "BalanceHandle_Linux.h"
#include "AsyncServer.h"
#include "AsyncNetDefine.h"

class ThreadPri;
class SessionInterface;
class TcpServerImpl : public SockBase
{
public:
	TcpServerImpl();
	~TcpServerImpl();
	bool SetNetParam(SessionInterface* Interface);
	bool Start(int port);
	bool Stop();

	int RecvNetPackage(const SockHandle& s, NetPackage& p);
	void  RecycleMem(const SockHandle& s, const NetPackage& p);

	const NetPackage AllocPackage(const SockHandle& s, int len);
	int SendNetPackage(const SockHandle& s, const NetPackage& p);

	void KickClient(const SockHandle& s);	

    int getPort()const{ return m_port;}
protected:
	virtual bool PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode = NULL );
	virtual bool PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode = NULL );
	virtual void Close(const hSockFd& fd);
private:
	TcpServerImpl(const TcpServerImpl&) {}
	TcpServerImpl& operator=(const TcpServerImpl&) { return *this; }
	
	void EpollThread();
private:
	char					m_bStart;
    int                     m_port;
	AsyncServer*			m_pServer;
	SessionInterface*		m_interface;
	BalanceHandle			m_Manager;		
	ThreadPri*				m_th[EPOLLDEALCOUNT];
};

#endif	// TcpServerImpl_Linux.h
