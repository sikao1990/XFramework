#include "TcpClientImpl_Win32.h"
#include "SocketSession.h"
#include "PerIOData.h"
#include "../SystemApi/TimeInfo.h"
#include <process.h>

TcpClientImpl::TcpClientImpl()
{
	m_lock			= 0;
	m_flag			= true;
	m_hTimerQueue	= NULL;
	m_hIocp			= NULL;
	m_bStart		= false;
	m_client		= NULL;
	if (NULL == (m_hTimerQueue = CreateTimerQueue()))
		throw "createTimerQueue failed";	
}

TcpClientImpl::~TcpClientImpl()
{
	if (m_bStart)
		Stop();
}

bool TcpClientImpl::Start(const char* IP,int port,SocketSession* pSession)
{
	WSAData wsaData;
	if(::WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
		return FALSE;
	if (NULL == (m_client = pSession) )
		return FALSE;

	m_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (NULL == m_hIocp)
		return FALSE;
	m_socket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_socket)
		return FALSE;
	if (NULL == ::CreateIoCompletionPort((HANDLE)m_socket, m_hIocp, (ULONG_PTR)m_socket, 0))
		return FALSE;

	struct sockaddr_in addr;
	::ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;
	if (::bind(m_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		return FALSE;

	GUID guid = WSAID_CONNECTEX;
	LPFN_CONNECTEX ConnectEx;
	DWORD bytes;
	if (::WSAIoctl(m_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid),
		&ConnectEx, sizeof(ConnectEx), &bytes, NULL, NULL) != 0)
		return FALSE;

	struct sockaddr_in srv_addr;
	::ZeroMemory(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(IP);
	srv_addr.sin_port = htons(port);

	PerIOData* connectPost = new PerIOData;
	connectPost->operationtype = PerIOData::CONNECT_POSTED;

	BOOL cok = ConnectEx(m_socket,(SOCKADDR*)&srv_addr,sizeof(srv_addr),NULL,0,NULL, &connectPost->overlapped);
	if(!cok && WSAGetLastError()!=ERROR_IO_PENDING)
		return FALSE;
	m_client->Init(SockHandle(CONNTYPE_TCP,IP,port,m_socket), SESSION_CLIENT, this);

	m_th[0] = (HANDLE)_beginthreadex(NULL, 0, IOCPWoker, this, 0, NULL);
	m_th[1] = (HANDLE)_beginthreadex(NULL, 0, HelperWorker, this, 0, NULL);

	if (!CreateTimerQueueTimer(&m_hHeartBit, m_hTimerQueue, WAITORTIMERCALLBACK(TimerRoutine), this, 3000, HEARTBIT_INTERVAL * 1000, NULL))
		return FALSE;
	m_bStart = true;
	puts("start client success!");
	return TRUE;	
}

void TcpClientImpl::Stop()
{
	//发送退出通知
	DisconnServer();
	DeleteTimerQueueTimer(m_hTimerQueue,m_hHeartBit, NULL);
	DeleteTimerQueue(m_hTimerQueue);
	m_flag = false;
	PostQueuedCompletionStatus(m_hIocp, -1, NULL, NULL);
	for (int i = 0; i < 2;)
	{
		if (NULL == m_th[i]) {
			i++;
			continue;
		}
		int ret = WaitForSingleObject(m_th[i], 100);
		if (WAIT_OBJECT_0 == ret) {
			m_th[i] = NULL;
			i++;
		}
		else
		{
			PostQueuedCompletionStatus(m_hIocp, -1, NULL, NULL);
			continue;
		}
	}
	CloseHandle(m_hIocp);

	closesocket(m_socket);
	m_bStart = false;
	WSACleanup();
}

int TcpClientImpl::RecvNetPackage(NetPackage& p)
{
	return  m_client->RecvNetPackage(p);
}

int TcpClientImpl::SendNetPackage(const NetPackage& p)
{
	return m_client->SendNetPackage(p);
}

void  TcpClientImpl::RecycleMem(const NetPackage& p)
{
	void* pData = p.GetData();
	if (NULL != pData)
		m_client->Free(pData);
}

const NetPackage TcpClientImpl::AllocPackage(int len)
{
	NetPackage TmpData;
	void* p = m_client->Alloc(len);
	if (NULL != p) {
		TmpData.m_len = len;
		TmpData.m_pData = p;
		TmpData.m_timestamp = TimeInfo::getTimeStamp();
	}
	return TmpData;
}

hSockFd TcpClientImpl::getSock()const
{
	return m_socket;
}


void TcpClientImpl::threadWorker()
{
	DWORD bytesTransferred;
	ULONG_PTR completionKey;
	LPOVERLAPPED pOverlapped;
	PerIOData* periodata = NULL;
	
	while (m_flag) {
		BOOL ok = ::GetQueuedCompletionStatus(m_hIocp, &bytesTransferred, &completionKey, &pOverlapped, INFINITE);
		if (!ok) {
			fprintf(stderr, "GetQueuedCompletionStatus failed error code = %d", ::GetLastError());
			break;
		}
		if (-1 == bytesTransferred) { // notified EXIT
			break;
		}
		periodata = CONTAINING_RECORD(pOverlapped, PerIOData, overlapped);
		switch (periodata->operationtype)
		{
		case PerIOData::CONNECT_POSTED:
			if (periodata) delete periodata;
			break;
		case PerIOData::RECV_POSTED:
			m_client->DispatchData(periodata->databuf.buf,bytesTransferred,periodata->nIndex,periodata->m_pPrevIOData);
			break;
		case PerIOData::SEND_POSTED:
			m_client->NoticeComplateSend(periodata->nIndex, periodata->databuf.buf, bytesTransferred);
			break;
		}
	}
	m_th[0] = NULL;
}

unsigned TcpClientImpl::IOCPWoker(void* p)
{
	TcpClientImpl* pThis = (TcpClientImpl*)p;
	pThis->threadWorker();
	return 0;
}

void TcpClientImpl::run()
{
	while (m_flag) {
		m_client->ParseData();
		m_client->PostSend();
		Sleep(30);
	}
	m_th[1] = NULL;
}

unsigned TcpClientImpl::HelperWorker(void* p)
{
	TcpClientImpl* pThis = (TcpClientImpl*)p;
	pThis->run();
	return 0;
}


//组建当前设备信息及IP的格式化数据，然后简单加密。发送给udp服务器，作为连接报文
void TcpClientImpl::ConnServer()
{
	NetPackage msg;
	msg.m_timestamp = TimeInfo::getTimeStamp();
	//TODO:MAC_网络协议版本+客户端版本
	msg.SetBaseProtol(BASEPROTO_CONN);
	m_client->SendNetPackage(msg);
}

void TcpClientImpl::DisconnServer()
{
	NetPackage msg;
	msg.m_timestamp = TimeInfo::getTimeStamp();
	msg.SetBaseProtol(BASEPROTO_CLOSE);
	m_client->SendNetPackage(msg);
}

VOID TcpClientImpl::TimerRoutine(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	TcpClientImpl* pThis = reinterpret_cast<TcpClientImpl*>(lpParameter);
	NetPackage msg;
	msg.m_timestamp = TimeInfo::getTimeStamp();
	msg.SetBaseProtol(BASEPROTO_HEARTBIT);
	pThis->m_client->SendNetPackage(msg);
}

bool TcpClientImpl::PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode )
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

bool TcpClientImpl::PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode )
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

void TcpClientImpl::Close(const hSockFd& fd)
{
	closesocket(fd);
}

bool TcpClientImpl::ReConnect()
{
	//TODO:
}
