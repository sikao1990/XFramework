#include "TPlayerSession.h"
#include "TServerSession.h"
#include "NetMsg.h"
/*
sample code for test
*/

TPlayerSession::TPlayerSession(const SockHandle& s, TServerSession* pSock)
	:m_sock(s),m_pSockSession(pSock)
{

}

void TPlayerSession::update(int i)
{
	NetPackage p;
	NetMsg msg;
	int ret = -1;
	if (0 == (ret = m_pSockSession->RecvNetPackage(m_sock, p))) {
		memcpy(&msg, (char*)(p.GetData()), sizeof(NetMsg));
		//测试列子1：小于一帧的包
		printf("===>sock:%d,len:%d,msg:%s\n", m_sock.m_sock,p.GetLen(),msg.Data.comBuf);
		//测试例子2：大于一帧的包
		//printf("===>sock:%d,msg:%s\n", m_sock,msg.Data.tData.other);
		m_pSockSession->FreeRecvPackage(m_sock,p);
#if 0
		NetPackage tmpSendP = m_pSockSession->AllocSendPackage(m_sock,sizeof(NetMsg));
		if(tmpSendP.IsValid())
			m_pSockSession->SendNetPackage(m_sock,tmpSendP);
#endif
	}
	//switch(type)
	//case logo
	//case ...
}