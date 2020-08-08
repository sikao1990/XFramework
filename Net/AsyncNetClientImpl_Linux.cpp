#include "AsyncNetClientImpl_Linux.h"
#include <sys/epoll.h>

AsyncNetClientImpl::AsyncNetClientImpl(int nthPool):AsyncClient(nthPool),m_hEpoll(-1)
{

}

AsyncNetClientImpl::~AsyncNetClientImpl()
{

}

bool AsyncNetClientImpl::Stop()
{
    AsyncClient::Stop();
    close(m_hEpoll);
    return true;
}

int AsyncNetClientImpl::AsyncInit(int int_sfd)
{
    m_hEpoll = epoll_create(1);
    FdAdd(int_sfd, m_hEpoll);
    return m_hEpoll;
}

int AsyncNetClientImpl::FdAdd(int int_fd, int int_epfd,bool bMonWrite /*= false*/)
{
    struct epoll_event epoll_ev;
    epoll_ev.events = EPOLLIN|EPOLLET;
    if(bMonWrite)epoll_ev.events |= EPOLLOUT;
    epoll_ev.data.fd = int_fd;
    epoll_ctl(int_epfd, EPOLL_CTL_ADD, int_fd, &epoll_ev);
    return 0;
}

int AsyncNetClientImpl::FdDel(int int_fd, int int_epfd)
{
    struct epoll_event epoll_ev;
    epoll_ev.events = EPOLLIN|EPOLLET;
    epoll_ev.data.fd = int_fd;
    epoll_ctl(int_epfd, EPOLL_CTL_DEL, int_fd, &epoll_ev);
    printf("close fd = %d\n", int_fd);
    close(int_fd);
	m_ReadQue.DeleteOut(int_fd);
	m_WriteQue.DeleteOut(int_fd);	
	
	CPostReq node;
	node.cSock = int_fd;
	node.oType = OP_EXIT;
	ProduceNode(node);
    m_bRun = false;
    return 0;
}

void AsyncNetClientImpl::HandleThread()
{
    struct epoll_event epoll_evs[10];
    int newfd;
    int int_event_num;
    int int_idx;
    printf("loop....\n");

    //循环体
    while(m_bRun)
    {
        memset(epoll_evs, 0, sizeof(epoll_evs));

        //等待事件
        int_event_num = epoll_wait(m_hEpoll, epoll_evs, 10, 500);    
        if (int_event_num > 0)
        {
            for(int_idx = 0; int_idx < int_event_num; ++int_idx)
            {
                if(epoll_evs[int_idx].data.fd == m_cliSocket)
                {
					if(epoll_evs[int_idx].events&EPOLLIN)//可读
						m_thPool.submit_job_req(new AsyncNetRWTask(epoll_evs[int_idx].data.fd,READTASK));
                    else if(epoll_evs[int_idx].events&EPOLLOUT)//可写
						m_thPool.submit_job_req(new AsyncNetRWTask(epoll_evs[int_idx].data.fd,WRITETASK));
				}
            }
        }
    }	
}


