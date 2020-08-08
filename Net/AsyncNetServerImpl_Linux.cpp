#include "AsyncNetServerImpl_Linux.h"
#include <sys/epoll.h>

AsyncNetServerImpl::AsyncNetServerImpl():m_hEpoll(-1)
{
}

AsyncNetServerImpl::AsyncNetServerImpl(int nthPool):AsyncServer(nthPool),m_hEpoll(-1)
{

}

AsyncNetServerImpl::~AsyncNetServerImpl()
{

}

bool AsyncNetServerImpl::Stop()
{
    AsyncServer::Stop();
    close(m_hEpoll);
    return true;
}

int AsyncNetServerImpl::AsyncInit(int int_sfd)
{
    m_hEpoll = epoll_create(EPOLLCREATFDMAX);
    FdAdd(int_sfd, m_hEpoll);
    return m_hEpoll;
}

int AsyncNetServerImpl::FdAdd(int int_fd, int int_epfd,bool bMonWrite /*= false*/)
{
    struct epoll_event epoll_ev;
    epoll_ev.events = EPOLLIN|EPOLLET;
    if(bMonWrite)epoll_ev.events |= EPOLLOUT;
    epoll_ev.data.fd = int_fd;
    epoll_ctl(int_epfd, EPOLL_CTL_ADD, int_fd, &epoll_ev);
    return 0;
}

int AsyncNetServerImpl::FdDel(int int_fd, int int_epfd)
{
    struct epoll_event epoll_ev;
    epoll_ev.events = EPOLLIN|EPOLLET;
    epoll_ev.data.fd = int_fd;
    epoll_ctl(int_epfd, EPOLL_CTL_DEL, int_fd, &epoll_ev);
    printf("close fd = %d\n", int_fd);
    close(int_fd);

	m_ReadQue.DeleteOut(int_fd);
	m_WriteQue.DeleteOut(int_fd);	
	
	SPostReq node;
	node.sSock = m_serSocket;
	node.cSock = int_fd;
	node.oType = OP_EXIT;
	ProduceNode(node);	
    return 0;
}

void AsyncNetServerImpl::HandleThread()
{
    struct epoll_event epoll_evs[EPOLLMONFDMAX];
    int newfd           = -1;
    int int_event_num   = 0;
    int int_idx         = -1;
    printf("loop....\n");

    //循环体
    while(m_bRun)
    {
        memset(epoll_evs, 0, sizeof(epoll_evs));

        //等待事件
        int_event_num = epoll_wait(m_hEpoll, epoll_evs, EPOLLMONFDMAX, 500);//若要屏蔽信号,则使用epoll_pwait
        if (int_event_num > 0)
        {
            for(int_idx = 0; int_idx < int_event_num; ++int_idx)
            {

                if(epoll_evs[int_idx].data.fd == m_serSocket)
                {
                    //有新客户端要接入
                    string ip;
                    newfd = on_accept_callback(m_serSocket,ip, m_hEpoll);
                    if(-1 == newfd)
                    {
                        printf("on accept callback fail...\n");
                        continue;
                    }
					SPostReq node;
					node.sSock = m_serSocket;
					node.cSock = newfd;
					node.oType = OP_ACCEPT;
                    node.ip = ip;
					ProduceNode(node);
					m_ReadQue.AppendNew(newfd);
					m_WriteQue.AppendNew(newfd);
                }//TODO:此处用内存池替换new
				else if(epoll_evs[int_idx].events&EPOLLIN)//可读
					m_thPool.submit_job_req(new AsyncNetRWTask(epoll_evs[int_idx].data.fd,READTASK,m_serSocket));
                else if(epoll_evs[int_idx].events&EPOLLOUT)//可写
					m_thPool.submit_job_req(new AsyncNetRWTask(epoll_evs[int_idx].data.fd,WRITETASK,m_serSocket));
            }
        }
    }	
}

