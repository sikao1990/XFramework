#ifndef __AsyncNetClientImpl_Apple_H__
#define __AsyncNetClientImpl_Apple_H__

#include "AsyncClient.h"

class AsyncNetClientImpl : public AsyncClient
{
public:
	AsyncNetClientImpl(int nthPool);
	~AsyncNetClientImpl();
    bool Stop();
protected:
	void HandleThread();
	int AsyncInit(int int_sfd);
	int FdAdd(int int_fd, int int_epfd,bool bMonWrite = false);
	int FdDel(int int_fd, int int_epfd);
private:
    int				m_hKque;
};

#endif	// AsyncNetClientImpl.h
