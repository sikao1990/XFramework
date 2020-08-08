#include "TcpServerImpl_Win32.h"
#include <process.h>
#include "../SystemApi/TimeInfo.h"

#define MAX_POST_ACCEPT 10
#define BACKLOG 200

TcpServerImpl::TcpServerImpl() :
	m_completeport(NULL),
	m_lpfnacceptex(NULL),
	m_WorkerCount(0),
	m_lpfngetacceptexsockaddrs(NULL),
	m_bStart(false),
	m_port(0)
{
	printf("enter TcpServerImpl\n");
 
	WSADATA wsaData;
	if(NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		printf("WSAStartup WinSock 2.2 failed\n");
		assert(0);
	}
}
 
TcpServerImpl::~TcpServerImpl()
{
	printf("enter ~TcpServerImpl\n");
	if (m_bStart)
		Stop();
	list <PerIOData*>::iterator myIterator = m_AsyncReqs.begin();
	for (; myIterator != m_AsyncReqs.end(); myIterator++)
	{
		delete *myIterator;
		*myIterator = NULL;
	}
	m_AsyncReqs.clear();
	WSACleanup();
}

bool TcpServerImpl::Start(int port)
{
	printf("enter Start\n");
	if (NULL == m_interface)
		return FALSE;
	m_Manager.Start(this);
	m_port = port;
	if(FALSE == InitializeIOCP())
	{
		printf("InitializeIOCP() failed: %d\n", GetLastError());
		DeInitializeIOCP();
		return FALSE;
	}
	printf("InitializeIOCP() succeed\n");
 
	if (FALSE == InitializeSocket(port))
	{
		printf("InitializeSocket() failed: %d\n", GetLastError());
		DeInitializeSocket();
		return FALSE;
	}
	printf("InitializeSocket() succeed\n");

	if (FALSE == GetAcceptExFuction())
	{
		printf("GetAcceptExFuction() failed: %d\n", GetLastError());
		return FALSE;
	}
	printf("GetAcceptExFuction() succeed\n");

	for (int i = 0; i<MAX_POST_ACCEPT; ++i)
	{
		PerIOData*	NewClientIoReq = GetNewIOData();
		PostAcceptEx(NewClientIoReq);
	}

	m_bStart = true;
	return TRUE;
}

bool TcpServerImpl::Stop()
{
	for (size_t i = 0; i < m_WorkerCount; i++)
		PostQueuedCompletionStatus(m_completeport, -1, NULL, NULL);

	for (int i = 0; i<m_WorkerCount;)
	{
		if (NULL == m_thiocp[i]) {
			i++;
			continue;
		}
		int ret = WaitForSingleObject(m_thiocp[i], 100);
		if (WAIT_OBJECT_0 == ret) {
			m_thiocp[i] = NULL;
			i++;
		}
		else
		{
			PostQueuedCompletionStatus(m_completeport, -1, NULL, NULL);
			continue;
		}
	}
	m_Manager.Stop();
	CloseHandle(m_completeport);
	closesocket(m_listenSocket);
	m_bStart = false;
	return TRUE;
}

BOOL TcpServerImpl::InitializeIOCP()
{
	printf("enter InitializeIOCP\n");
 
	m_completeport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );
	if ( NULL == m_completeport)
	{
		printf("CreateIoCompletionPort failed: %d!\n", WSAGetLastError());
		return FALSE;
	}
 
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	m_thiocp = new HANDLE[m_WorkerCount = systeminfo.dwNumberOfProcessors];
	for (DWORD i = 0; i < systeminfo.dwNumberOfProcessors; ++i)
	{
		m_thiocp[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (LPVOID)this, 0, NULL);
	}

	return TRUE;
}

void TcpServerImpl::DeInitializeIOCP()
{
	printf("enter DeInitializeIOCP\n");
 
	CloseHandle(m_completeport);
}

BOOL TcpServerImpl::InitializeSocket(int port)
{
	printf("enter InitializeSocket\n");
	
	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == m_listenSocket)
	{
		printf("WSASocket failed: %d.\n", WSAGetLastError());
		return FALSE;
	}

	if (NULL == CreateIoCompletionPort((HANDLE)m_listenSocket, m_completeport, (ULONG_PTR)m_listenSocket, 0))
	{
		printf("CreateIoCompletionPort() failed: %d.\n", WSAGetLastError());
		return FALSE;
	}

	int reuseaddr = 1;
	if (SOCKET_ERROR == 
		setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuseaddr, sizeof(int)))
	{
		printf("m_listenSocket: setsockopt(SO_REUSEADDR) failed %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		return FALSE;
	}

	SOCKADDR_IN ServerAddress;
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                              
	ServerAddress.sin_port = htons(port);     
 
	if (SOCKET_ERROR == 
		bind(m_listenSocket, (PSOCKADDR) &ServerAddress, sizeof(SOCKADDR_IN))) 
	{
		printf("listensocket: bind() failed, %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		return FALSE;
	}
 
	if (SOCKET_ERROR == listen(m_listenSocket, BACKLOG)) 
	{
		printf("listensocket: listen() failed %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		return FALSE;
	}
 
	return TRUE;
}

void TcpServerImpl::DeInitializeSocket()
{
	printf("enter DeInitializeSocket\n");
 
	closesocket(m_listenSocket);
}

BOOL TcpServerImpl::GetAcceptExFuction()
{
	printf("enter GetAcceptExFuction\n");
 
	if(INVALID_SOCKET == m_listenSocket)
	{
		printf("WSASocket failed: %d.\n", WSAGetLastError());
		return FALSE;
	}

	DWORD dwBytes = 0; 
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	if (SOCKET_ERROR == 
		WSAIoctl(m_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GUID),
		&m_lpfnacceptex, sizeof(LPFN_ACCEPTEX), &dwBytes, NULL, NULL))
	{
		printf("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, WSAID_ACCEPTEX) failed"
			": %d.\n", WSAGetLastError());
		return FALSE;
	}

	GUID GuidAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	if (SOCKET_ERROR == 
		WSAIoctl(m_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptExSockAddrs, sizeof(GUID),
		&m_lpfngetacceptexsockaddrs, sizeof(LPFN_GETACCEPTEXSOCKADDRS), &dwBytes, NULL, NULL))
	{
		printf("WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER, WSAID_ACCEPTEX) failed"
			": %d.\n", WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

unsigned WINAPI TcpServerImpl::WorkerThread(LPVOID lpParam)
{    
	printf("enter WorkerThread\n");
 
	TcpServerImpl* pthis = (TcpServerImpl*)lpParam;
	LPWSAOVERLAPPED lpoverlapped = NULL;
	SOCKET sockethandle = NULL;
	PerIOData *periodata = NULL;
	DWORD bytestransfered = 0;
 
	while (TRUE)
	{
		// get the status of comleteport
		BOOL ret = GetQueuedCompletionStatus(pthis->m_completeport, &bytestransfered,(PULONG_PTR)&sockethandle, &lpoverlapped, INFINITE);
		if(FALSE == ret)  
		{  
			int sockerror = WSAGetLastError();
			//DWORD dwFlags = 0;
			//if (!::WSAGetOverlappedResult(sockethandle, &periodata->overlapped, &bytestransfered, FALSE, &dwFlags))
			//{
			//	sockerror = ::WSAGetLastError();
			//	puts("-------------------------------------------------------->");
			//}
			printf("GetQueuedCompletionStatus failed: %d.\n", sockerror);
			if(64 == sockerror) //host is down, client close abnormally
			{
				if(pthis->m_listenSocket != sockethandle){
					pthis->m_Manager.RemoveClient(SockHandle(CONNTYPE_TCP, "",pthis->m_port, sockethandle));
				}else{
					printf("something wrong in listenhandle, you need restart the process");
				}
			}
			continue;  
		}
		if (-1 == bytestransfered) 
			return 0;
		// read the data in m_completeport
		periodata = CONTAINING_RECORD(lpoverlapped, PerIOData, overlapped);
		if(NULL == periodata)  {  
			printf("CONTAINING_RECORD failed: %d.\n", WSAGetLastError());
			continue;  
		}
		if((0 == bytestransfered) &&( PerIOData::RECV_POSTED == periodata->operationtype|| PerIOData::SEND_POSTED == periodata->operationtype))  
		{  
			pthis->m_Manager.RemoveClient(SockHandle(CONNTYPE_TCP, "", pthis->m_port, sockethandle));
			continue;  
		}
		periodata->SetUse(0);
		switch(periodata->operationtype)  
		{  
			case PerIOData::ACCEPT_POSTED:
			{
				string remoteIp;
				int port = 0;
				printf("recv the ACCEPT_POSTED signal\n");
				pthis->PostAcceptEx(periodata);
				if(FALSE == pthis->DoAcceptEx(periodata, remoteIp, port))
					continue;
				pthis->m_Manager.AppendNewClient(SockHandle(CONNTYPE_TCP, remoteIp.c_str(), port,periodata->acceptsocket));
			}
			break;
			case PerIOData::RECV_POSTED:
			{
				//printf("recv the RECV_POSTED signal\n");
				pthis->m_Manager.RecvClientData(SockHandle(CONNTYPE_TCP,"", pthis->m_port,sockethandle),
					periodata->databuf.buf,bytestransfered,periodata->nIndex,periodata->m_pPrevIOData,periodata);
			}
			break;
			case PerIOData::SEND_POSTED:
			{
				printf("recv the SEND_POSTED signal\n");
				pthis->m_Manager.ActivateWrite(SockHandle(CONNTYPE_TCP, "", pthis->m_port, sockethandle),
					periodata->nIndex, periodata->databuf.buf, bytestransfered,periodata);
			}
			break;
			case PerIOData::NULL_POSTED:
			default:
				printf("MUST exist a problem, the last error: %d.\n", WSAGetLastError());
			break;
		} //switch
	}//while
	return 0;
}

void TcpServerImpl::PostAcceptEx(PerIOData* Newiodata)
{
	printf("enter PostAcceptEx\n");
 
	if(NULL == Newiodata)
	{
		printf("this is a fatal mistake, almost no way to reach here\n");
		return;
	}

	Newiodata->acceptsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == Newiodata->acceptsocket)
	{
		printf("WSASocket failed: %d.\n", WSAGetLastError());
		return;
	}
 
	Newiodata->Reset();
	Newiodata->operationtype = PerIOData::ACCEPT_POSTED;
	DWORD dwBytes = 0; 
	if (FALSE == 
		m_lpfnacceptex(m_listenSocket, Newiodata->acceptsocket, Newiodata->databuf.buf,
		0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16,
		&dwBytes, &Newiodata->overlapped))
	{
		if (WSA_IO_PENDING != WSAGetLastError()) 
		{
			printf("AcceptEx() falied: %d.\n", WSAGetLastError());
			closesocket(Newiodata->acceptsocket);
			Newiodata->acceptsocket = -1;
			RemoveIOData(Newiodata);
			return;
		}
	}
 
	return;
}
 
BOOL TcpServerImpl::DoAcceptEx(PerIOData* Newiodata,string& strRemoteIp, int& port)
{
	SOCKADDR_IN *lplocalsockaddr = NULL;
	SOCKADDR_IN *lpremotesockaddr = NULL;
	int localsockaddrlen = sizeof(SOCKADDR_IN);
	int remotesockaddrlen = sizeof(SOCKADDR_IN);
 
	m_lpfngetacceptexsockaddrs(Newiodata->databuf.buf, 0,
		sizeof(SOCKADDR_IN)+16,	sizeof(SOCKADDR_IN)+16,
		(LPSOCKADDR*)&lplocalsockaddr, &localsockaddrlen,
		(LPSOCKADDR*)&lpremotesockaddr, &remotesockaddrlen);
 
	strRemoteIp = inet_ntoa(lpremotesockaddr->sin_addr);
	port = ntohs(lpremotesockaddr->sin_port);
	if (NULL == CreateIoCompletionPort((HANDLE)Newiodata->acceptsocket, m_completeport, (ULONG_PTR)Newiodata->acceptsocket, 0))
	{
		printf("CreateIoCompletionPort() failed: %d.\n", WSAGetLastError());
		return FALSE;
	}
 
	return TRUE;
}

PerIOData* TcpServerImpl::GetNewIOData()
{
	PerIOData* speriodata = new PerIOData(1);
	m_AsyncReqs.push_back(speriodata);
	return speriodata;
}

void TcpServerImpl::RemoveIOData(PerIOData* speriodata)
{
	if (NULL == speriodata)
	{
		printf("you just transfer a NULL pointer\n");
		return;
	}

	list <PerIOData*>::iterator myIterator = m_AsyncReqs.begin();
	for (; myIterator != m_AsyncReqs.end(); myIterator++)
	{
		if (*myIterator == speriodata)
		{
			delete *myIterator;
			*myIterator = NULL;
			m_AsyncReqs.erase(myIterator);
			break;
		}
	}
}

bool TcpServerImpl::SetNetParam(SessionInterface* Interface)
{
	if (NULL == Interface )
		return FALSE;
	m_interface = Interface;
	m_Manager.SetNetParam(m_interface);
	return  TRUE;
}

int TcpServerImpl::RecvNetPackage(const SockHandle& s, NetPackage& p)
{
	return m_Manager.RecvNetPackage(s, p);
}

int TcpServerImpl::SendNetPackage(const SockHandle& s, const NetPackage& p)
{
	return m_Manager.SendNetPackage(s, p);
}

void TcpServerImpl::KickClient(const SockHandle& s)
{
	m_Manager.KickClient(s);
}

void TcpServerImpl::RecycleMem(const SockHandle& s, const NetPackage& p)
{
	void* pData = p.GetData();
	if (NULL != pData)
		m_Manager.Free(s,pData);
}

const NetPackage TcpServerImpl::AllocPackage(const SockHandle& s,int len)
{
	NetPackage TmpData;
	void* p = m_Manager.Alloc(s, len);
	if (NULL != p) {
		TmpData.m_len = len;
		TmpData.m_pData = p;
		TmpData.m_timestamp = TimeInfo::getTimeStamp();
	}
	return TmpData;
}


bool TcpServerImpl::PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode )
{
	DWORD flags = 0;
	DWORD recvbytes = 0;	
	PerIOData* pData = (PerIOData*)pPostNode;
	if(NULL!=pData){
		pData->m_pPrevIOData = pExtend;
		pData->SetUse(1);
		pData->databuf.buf = pStart;
		pData->databuf.len = len;
		pData->nIndex = nIndex;
		pData->operationtype = PerIOData::RECV_POSTED;
		int nbytes = WSARecv(fd, &pData->databuf, 1, &recvbytes, &flags, &pData->overlapped, NULL);
		if ((SOCKET_ERROR == nbytes) && (WSA_IO_PENDING != WSAGetLastError()))
		{
			printf("WSARecv() failed: %d.\n", WSAGetLastError());
			pData->SetUse(0);
			return false;
		}
		return true;
	}
	return false;
}

bool TcpServerImpl::PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode)
{
	DWORD flags = 0;
	DWORD sendbytes = 0;	
	PerIOData* pData = (PerIOData*)pPostNode;
	if(NULL!=pData){
		pData->SetUse(1);
		pData->databuf.buf = pStart;
		pData->databuf.len = len;
		pData->nIndex = nIndex;
		pData->operationtype = PerIOData::SEND_POSTED;
		int nbytes = WSASend(fd, &pData->databuf, 1, &sendbytes, flags, &pData->overlapped, NULL);
		if ((SOCKET_ERROR == nbytes) && (WSA_IO_PENDING != WSAGetLastError()))
		{
			printf("WSASend() failed: %d.\n", WSAGetLastError());
			pData->SetUse(0);
			return false;
		}
		return true;
	}
	return false;
}

void TcpServerImpl::Close(const hSockFd& fd)
{
	closesocket(fd);
}
