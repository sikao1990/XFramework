#ifndef __SessionInterface_H__
#define __SessionInterface_H__

#include "XNetCfg.h"
#include "SockHandle.h"
#include <iostream>
#include <stdio.h>

class SocketSession;
using namespace std;

class FRAMEWORK_API SessionInterface
{
public:
	virtual ~SessionInterface(){}
	virtual bool ClientEnter(const SockHandle& s) { return true; }
	virtual void ClientExit(const SockHandle& s){}
	virtual SocketSession* GetNetSocketSession() { return NULL; }
	virtual void DestroySocketSession(SocketSession* pSession){ }
};

#endif	// SessionInterface.h
