#include "TcpClientImpl_Linux.h"
#include "../Thread/ThreadImpl_Linux.h"
#include "SocketSession.h"
#include <stdio.h>

TcpClientImpl::TcpClientImpl():m_epollNet(2),m_client(NULL),m_flag(false),m_bStart(false)
{
	TimeSet test(1000,30);
	m_th[0] = new ThreadImpl<TcpClientImpl>(this,&TcpClientImpl::Run);
	m_th[1] = new ThreadImpl<TcpClientImpl>(this,&TcpClientImpl::SessionRun);
	m_timer = new Timer<TcpClientImpl>(this,&TcpClientImpl::TimerProc,test);
    m_timerMgr.AddTimer("heartBit",m_timer);
}

TcpClientImpl::~TcpClientImpl()
{
	if(m_bStart&&m_flag)
        Stop();
}

bool TcpClientImpl::Start(const char* IP,int port,SocketSession* pSession)
{
	m_client = pSession;
	if(NULL==m_client)return false;
	m_epollNet.Init(IP,port);
    m_flag = m_bStart = true;
    m_timerMgr.Init();
    return true;
}

void TcpClientImpl::Stop()
{
	m_flag = m_bStart = false;
    m_th[0]->stop();
    m_th[1]->stop();
    m_timerMgr.DelTimer("heartBit");
    m_timerMgr.Stop();
    delete m_th[0];
    delete m_th[1];
    delete m_timer;
}

int TcpClientImpl::RecvNetPackage(NetPackage& p)
{
	return  m_client->RecvNetPackage(p);	
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

int TcpClientImpl::SendNetPackage(const NetPackage& p)
{
	return m_client->SendNetPackage(p);	
}

hSockFd TcpClientImpl::getSock()const
{
	return m_epollNet.getSock();
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

void TcpClientImpl::Run()
{
	int sock = -1;
	int nTrans = 0;
	CPostReq req={};	
	while(m_bStart){
		if(m_epollNet.GetQueueComplateStatus(sock,req,nTrans)){
			switch(req.oType){
			case OP_READ:
				break;
			case OP_WRITE:
				break;
			}			
		}else
			ThreadImpl<TcpClientImpl>::mSleep(1);
	}
}

void TcpClientImpl::SessionRun()
{
	while (m_flag) {
		m_client->ParseData();
		m_client->PostSend();
		ThreadImpl<TcpClientImpl>::mSleep(30);
	}	
}

void TcpClientImpl::TimerProc()
{
	NetPackage msg;
	msg.m_timestamp = TimeInfo::getTimeStamp();
	msg.SetBaseProtol(BASEPROTO_HEARTBIT);
	m_client->SendNetPackage(msg);	
}

void TcpClientImpl::Close(const hSockFd& fd)
{
	close(fd);
}

bool TcpClientImpl::PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode )
{
}

bool TcpClientImpl::PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode )
{
	
}

