#include "TServerSession.h"
#include "NetMsg.h"
/*
 sample code for test
*/

//the port

TServerSession::TServerSession()
{
	m_port = SERVERPORT;
}

bool TServerSession::Init(int type)
{
	InitializeCriticalSection(&m_lock);
	m_th = (HANDLE)_beginthreadex(NULL, 0, WinThread, this, 0, NULL);
	m_network.SetNetParam(this);
	m_network.Start(type);
	return true;
}

bool TServerSession::ClientEnter(const SockHandle& s)
{
	puts("one client has connect!");
	EnterCriticalSection(&m_lock);
	m_clients.insert(make_pair(s, new TPlayerSession(s, this)));
	LeaveCriticalSection(&m_lock);
	return true;
}
void TServerSession::ClientExit(const SockHandle& s)
{
	EnterCriticalSection(&m_lock);
	delete m_clients.find(s)->second;
	m_clients.erase(m_clients.find(s)); 
	LeaveCriticalSection(&m_lock);
	return ;
}

int TServerSession::RecvNetPackage(const SockHandle& s, NetPackage& p)
{
	return m_network.RecvNetPackage(s, p);
}

int TServerSession::SendNetPackage(const SockHandle& s, const NetPackage& p)
{
	return m_network.SendNetPackage(s, p);
}
void TServerSession::KickClient(const SockHandle& s)
{
	return m_network.KickClient(s);
}

void TServerSession::run(int i)
{
	map<SockHandle, TPlayerSession* >::iterator it = m_clients.begin();
	for (; it != m_clients.end(); it++)
	{
		it->second->update(i);
	}
}

unsigned TServerSession::WinThread(void* p)
{
	TServerSession* pthis = (TServerSession*)p;
	int i = 0;
	while (1) {
		EnterCriticalSection(&pthis->m_lock);
		pthis->run(i++);
		LeaveCriticalSection(&pthis->m_lock);
		Sleep(1);
	}
}

void TServerSession::FreeRecvPackage(const SockHandle& s, const NetPackage& p)
{
	return m_network.RecycleMem(s, p);
}

const NetPackage& TServerSession::AllocSendPackage(const SockHandle& s, int len)
{
	return m_network.AllocPackage(s, len);
}

SocketSession* TServerSession::GetNetSocketSession()
{
	return new TMySockSession;
}

void TServerSession::DestroySocketSession(SocketSession* pSession)
{
	delete pSession;
}
