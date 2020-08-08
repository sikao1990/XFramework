#include "AsyncNetServerImpl_Apple.h"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

AsyncNetServerImpl::AsyncNetServerImpl():m_hKque(-1)
{
}

AsyncNetServerImpl::AsyncNetServerImpl(int nthPool):AsyncServer(nthPool),m_hKque(-1)
{

}

AsyncNetServerImpl::~AsyncNetServerImpl()
{

}

bool AsyncNetServerImpl::Stop()
{
    AsyncServer::Stop();
    close(m_hKque);
    return true;
}

int AsyncNetServerImpl::AsyncInit(int int_sfd)
{
    m_hKque = kqueue();
    FdAdd(int_sfd, m_hKque);
    return m_hKque;
}

int AsyncNetServerImpl::FdAdd(int int_fd, int int_epfd,bool bMonWrite /*= false*/)
{
    struct kevent changes[1];
	if(bMonWrite)
        EV_SET(&changes[0], int_fd, EVFILT_READ|EVFILT_WRITE, EV_ADD, 0, 0, this);
    else
        EV_SET(&changes[0], int_fd, EVFILT_READ, EV_ADD, 0, 0, this);
	int ret = kevent(int_epfd, changes, 1, NULL, 0, NULL);
	if (ret == -1)
		return -1;
    return 0;
}

int AsyncNetServerImpl::FdDel(int int_fd, int int_epfd)
{
    struct kevent changes[1];
	EV_SET(&changes[0], int_fd, EVFILT_READ|EVFILT_WRITE, EV_DELETE, 0, 0, this);
	int ret = kevent(int_epfd, changes, 1, NULL, 0, NULL);
	if (ret == -1)
		return -1;
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
    struct kevent events[MAX_EVENT_COUNT];
    int newfd           = -1;
    int int_event_num   = 0;
    int int_idx         = -1;
    printf("loop....\n");

    //循环体
    while(m_bRun)
    {
        memset(events, 0, sizeof(events));

        //等待事件
        int_event_num = kevent(m_hKque, NULL, 0, events, MAX_EVENT_COUNT, NULL);
		if (int_event_num == -1)
		{
			std::cerr << "kevent failed!\n";
			continue;
		}
        if (int_event_num > 0)
        {
            for(int_idx = 0; int_idx < int_event_num; ++int_idx)
            {
                if(events[int_idx].filter == EVFILT_READ)
                {
                    if(events[int_idx].ident == m_serSocket)
                    {
                        //有新客户端要接入
                        for( int i=0;i<events[i].data;i++)
                        {
                            string ip;
                            newfd = on_accept_callback(m_serSocket,ip, m_hKque);
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
                        }
                    }//此处用内存池替换new
					else//可读
						m_thPool.submit_job_req(new AsyncNetRWTask(events[int_idx].ident,READTASK,m_serSocket));//events[i].data可读取大小

                }else if(events[int_idx].filter == EVFILT_WRITE)
                {
					m_thPool.submit_job_req(new AsyncNetRWTask(events[int_idx].ident,WRITETASK,m_serSocket));
                }
            }
        }
    }	
}

