#ifndef __TcpClientImpl_H__
#define __TcpClientImpl_H__

#include "../Thread/Timer.h"
#include "../Thread/TimerMgrImpl_Linux.h"
#include "XNetCfg.h"
#include "SockBase.h"
#include "NetPackage.h"
#include "AsyncClient.h"
#include "AsyncNetDefine.h"

class ThreadPri;
class SocketSession;
class TcpClientImpl
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
protected:
	virtual bool PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode = NULL );
	virtual bool PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode = NULL );
	virtual void Close(const hSockFd& fd);
private:
	void ConnServer();
	void DisconnServer();

	void Run();
	void SessionRun();
	void TimerProc();
private:
	char				m_flag;
	char				m_bStart;
	SocketSession*		m_client;
	ThreadPri*			m_th[2];
	AbstractTimer*		m_timer;
	AsyncClient*		m_pNet;
    TimerMgrImpl        m_timerMgr;
};

#endif	// TcpClientImpl.h

