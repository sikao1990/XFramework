#ifndef __TPlayerSession_H__
#define __TPlayerSession_H__
#include "../../Framework/Net/DataHandle.h"
#include "../../Framework/Net/XNetCfg.h"
#include "../../Framework/Net/NetPackage.h"
/*
 sample code for test
*/

class TServerSession;
class TPlayerSession
{
public:
	TPlayerSession(const SockHandle& s, TServerSession* pSock);
	void update(int i);
private:
	SockHandle	m_sock;
	TServerSession*	m_pSockSession;
};

#endif

