#include "TcpServerImpl_Linux.h"
#include "../Thread/ThreadImpl_Linux.h"
#include "SocketSession.h"

TcpServerImpl::TcpServerImpl():m_epool(POOLTHREADCOUNT),m_port(0),m_bStart(0)
{
	for(int i=0;i<EPOLLDEALCOUNT;i++)
		m_th[i] = new ThreadImpl<TcpServerImpl>(this,&TcpServerImpl::EpollThread);
}

TcpServerImpl::~TcpServerImpl()
{
	for(int i=0;i<EPOLLDEALCOUNT;i++)
		delete m_th[i];
    if(m_bStart)
        Stop();
}

bool TcpServerImpl::SetNetParam(SessionInterface* Interface)
{
	if (NULL == Interface )
		return false;
	m_interface = Interface;
	m_Manager.SetNetParam(m_interface);
	return  true;	
}

bool TcpServerImpl::Start(int port)
{
	m_epool.Init(m_port=port);
    m_Manager.Start(this);
    m_bStart = 1;
    return true;
}

bool TcpServerImpl::Stop()
{
    m_bStart = 0;
	m_epool.Stop();
    m_Manager.Stop();
	for(int i=0;i<EPOLLDEALCOUNT;i++)
		m_th[i]->stop();
}

int TcpServerImpl::RecvNetPackage(const SockHandle& s, NetPackage& p)
{
	return m_Manager.RecvNetPackage(s, p);
}

void TcpServerImpl::RecycleMem(const SockHandle& s, const NetPackage& p)
{
	void* pData = p.GetData();
	if (NULL != pData)
		m_Manager.Free(s,pData);	
}

const NetPackage TcpServerImpl::AllocPackage(const SockHandle& s, int len)
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

int TcpServerImpl::SendNetPackage(const SockHandle& s, const NetPackage& p)
{
	return m_Manager.SendNetPackage(s, p);
}

void TcpServerImpl::KickClient(const SockHandle& s)
{
	m_Manager.KickClient(s);
}

void TcpServerImpl::EpollThread()
{
	int sock = -1;
	int nTrans = 0;
	SPostReq req={};
	while(m_bStart){
		if(m_epool.GetQueueComplateStatus(sock,req,nTrans)){
			switch(req.oType){
			case OP_ACCEPT:
                m_Manager.AppendNewClient(SockHandle(0,req.ip.c_str(),m_port,req.cSock));
				break;
            case OP_EXIT:
                m_Manager.RemoveClient(SockHandle(0,req.ip.c_str(),m_port,req.cSock));
                break;
			case OP_READ:
                m_Manager.RecvClientData(SockHandle(0,req.ip.c_str(),m_port,req.cSock),req.pData,req.nTrans,req.nIndex,req.m_pPrevIOData,NULL);
				break;
			case OP_WRITE:
                m_Manager.ActivateWrite(SockHandle(0,req.ip.c_str(),m_port,req.cSock),req.nIndex,req.pData,req.nTrans,NULL);
				break;
			}
		}else
			sleep(1);//TODO::
	}
}

void TcpServerImpl::Close(const hSockFd& fd)
{
	close(fd);
}

bool TcpServerImpl::PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend,void* pPostNode )
{
}

bool TcpServerImpl::PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode)
{
}

