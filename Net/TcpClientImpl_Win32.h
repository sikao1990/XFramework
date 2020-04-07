#ifndef __TcpClientImpl_H__
#define __TcpClientImpl_H__

#include "XNetCfg.h"
#include "SockBase.h"
#include "NetPackage.h"

#pragma comment(lib,"ws2_32.lib")

class SocketSession;
class TcpClientImpl : public SockBase
{
public:
	TcpClientImpl();
	~TcpClientImpl();

	bool Start(const char* IP,int port,SocketSession* pSession);
	void Stop();

	int RecvNetPackage(NetPackage& p);
	void  RecycleMem(const NetPackage& p);

	const NetPackage AllocPackage(int len);
	int SendNetPackage(const NetPackage& p);
	hSockFd getSock()const;	

	bool ReConnect();
protected:
	virtual bool PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode = NULL );
	virtual bool PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode = NULL );
	virtual void Close(const hSockFd& fd);
private:
	void ConnServer();
	void DisconnServer();
	void threadWorker();
	void run();
	static unsigned WINAPI IOCPWoker(void* );
	static unsigned WINAPI HelperWorker(void* );
	static VOID CALLBACK TimerRoutine(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
private:
	char					m_flag;
	char					m_bStart;
	volatile long			m_lock;
	HANDLE					m_hTimerQueue;
	HANDLE					m_hHeartBit;
	HANDLE					m_hIocp;
	SOCKET					m_socket;
	SocketSession*			m_client;
	HANDLE					m_th[2];
	sockaddr_in				m_clientInfo;
};

#endif	// TcpClientImpl.h

