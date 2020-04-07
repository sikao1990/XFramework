#ifndef __TcpServerImpl_Win32_H__
#define __TcpServerImpl_Win32_H__

#include "SockBase.h"
#include <stdio.h>
#include <assert.h>
#include "PerIOData.h"
#include "SessionInterface.h"
#include "BalanceHandle_Win32.h"

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

	int getPort()const { return m_port; }
protected:
	virtual bool PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode = NULL );
	virtual bool PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode = NULL );
	virtual void Close(const hSockFd& fd);
private:
	TcpServerImpl(const BalanceHandle&) {}
	TcpServerImpl& operator=(const TcpServerImpl&) { return *this; }	
	
	BOOL InitializeIOCP();
	void DeInitializeIOCP();
	BOOL InitializeSocket(int port);
	void DeInitializeSocket();
	BOOL GetAcceptExFuction();

	static unsigned WINAPI WorkerThread(LPVOID lpParam);
	void PostAcceptEx(PerIOData *listeniodata);
	BOOL DoAcceptEx(PerIOData *listeniodata,string& strRemoteIp,int& port);
	
	PerIOData* GetNewIOData();
	void RemoveIOData(PerIOData* speriodata);
private:
	char						m_bStart;
	short						m_WorkerCount;
	int							m_port;
	HANDLE						m_completeport;
	LPFN_ACCEPTEX				m_lpfnacceptex;
	LPFN_GETACCEPTEXSOCKADDRS	m_lpfngetacceptexsockaddrs;

	list<PerIOData*>			m_AsyncReqs;
	SOCKET						m_listenSocket;
	HANDLE*						m_thiocp;
	SessionInterface*			m_interface;
	BalanceHandle				m_Manager;	
};

#endif	// TcpServerImpl_Win32.h
