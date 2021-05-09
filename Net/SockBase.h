#ifndef __SockBase_H__
#define __SockBase_H__
#include <stdio.h>

#include "XNetCfg.h"

class FRAMEWORK_API SockBase
{
public:
	virtual ~SockBase(){}
	virtual bool PostRecvReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pExtend ,void* pPostNode = NULL) = 0;
	virtual bool PostSendReq(const hSockFd& fd,char* pStart,int len,int nIndex,void* pPostNode = NULL) = 0;
	virtual void Close(const hSockFd& fd) = 0;
};

#endif	// SockBase.h
