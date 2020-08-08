#ifndef __AsyncNetServerImpl_Apple_H__
#define __AsyncNetServerImpl_Apple_H__

#include "AsyncServer.h"

using namespace std;

class AsyncNetServerImpl: public AsyncServer
{
public:
	AsyncNetServerImpl();
	AsyncNetServerImpl(int nthPool);
	~AsyncNetServerImpl();
    bool Stop();
protected:
	void HandleThread();
	int AsyncInit(int int_sfd);
	int FdAdd(int int_fd, int int_epfd,bool bMonWrite = false);
	int FdDel(int int_fd, int int_epfd);
private:
    int				m_hKque;
};

#endif	// AsyncNetServerImpl.h
