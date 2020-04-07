#ifndef __PerIOData_H__
#define __PerIOData_H__
#include <winsock2.h>
#include <windows.h>
#include <MSWSock.h>

class PerIOData
{
public:
	PerIOData(void);
	PerIOData(int nflag);
	~PerIOData(void);
 
public:
	// overlapped struct, must be in the first place of class memory
	WSAOVERLAPPED overlapped;
 
	//just for ACCEPT_POSTED signal
	SOCKET	acceptsocket;

	//the signal belong to me
	int		operationtype : 8,
			nIndex : 8,
			nUseFlag : 8,
			nMemFlag : 8;
	void*	m_pPrevIOData;
	//the buffer of me
	WSABUF	databuf;
	SOCKADDR_IN remoteAddr;
	void SetUse(int b) { nUseFlag = b; }
	int GetUse() { return nUseFlag; }
public:
	void Reset(void);
 
// this four signal is the public signal for other class, IOCPServer etc..
//		ACCEPT_POSTED - signal for accept
//		RECV_POSTED   - signal for recv
//		SEND_POSTED   - signal for send
//		NULL_POSTED   - terminal signal, no meaning
//
public:
	typedef enum  
	{  
		ACCEPT_POSTED,
		RECV_POSTED,
		SEND_POSTED,
		CONNECT_POSTED,
		NULL_POSTED
	}OPERATION_TYPE;
};

#endif // PerIOData.h
