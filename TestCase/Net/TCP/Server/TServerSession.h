#ifndef __TServerSession_H__
#define __TServerSession_H__
#include "../../Framework/Net/SessionInterface.h"
#include <list>
#include <map>
#include "TPlayerSession.h"
#include "../../Framework/Net/XTcpServer.h"
#include "TMySockSession.h"

using namespace std;

/*
sample code for test
*/
class TServerSession : public SessionInterface
{
public:
	TServerSession();
public:
	bool Init(int type);
	virtual bool ClientEnter(const SockHandle& s);
	virtual void ClientExit(const SockHandle& s);
	int RecvNetPackage(const SockHandle& s, NetPackage& p);
	int SendNetPackage(const SockHandle& s, const NetPackage& p);
	void KickClient(const SockHandle& s);
	void FreeRecvPackage(const SockHandle& s, const NetPackage& p);
	const NetPackage& AllocSendPackage(const SockHandle& s,int len);
	virtual SocketSession* GetNetSocketSession();
	virtual void DestroySocketSession(SocketSession* pSession);
private:
	void run(int i);
	static unsigned WINAPI WinThread(void* p);
private:
	map<SockHandle,TPlayerSession* >	m_clients;
	XTcpServer	m_network;
	HANDLE		m_th;
	//map<session_str,SOCKET>
	CRITICAL_SECTION		m_lock;
	int m_port;
};


#endif

