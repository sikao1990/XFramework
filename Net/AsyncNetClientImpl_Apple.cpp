#include "AsyncNetClientImpl_Apple.h"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

AsyncNetClientImpl::AsyncNetClientImpl(int nthPool):AsyncClient(nthPool),m_hKque(-1)
{

}

AsyncNetClientImpl::~AsyncNetClientImpl()
{

}

bool AsyncNetClientImpl::Stop()
{
    AsyncClient::Stop();
    close(m_hKque);
    return true;
}

int AsyncNetClientImpl::AsyncInit(int int_sfd)
{
    m_hKque = kqueue();
    FdAdd(int_sfd, m_hKque);
    return m_hKque;

}

int AsyncNetClientImpl::FdAdd(int int_fd, int int_epfd,bool bMonWrite /*= false*/)
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

int AsyncNetClientImpl::FdDel(int int_fd, int int_epfd)
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
	node.sSock = m_cliSocket;
	node.cSock = int_fd;
	node.oType = OP_EXIT;
	ProduceNode(node);	
    return 0;

}

void AsyncNetClientImpl::HandleThread()
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
                if(events[int_idx].filter == EVFILT_READ && events[int_idx].ident == m_cliSocket)
                {
					m_thPool.submit_job_req(new AsyncNetRWTask(events[int_idx].ident,READTASK,m_cliSocket));//events[i].data可读取大小

                }else if(events[int_idx].filter == EVFILT_WRITE)
                {
					m_thPool.submit_job_req(new AsyncNetRWTask(events[int_idx].ident,WRITETASK,m_cliSocket));
                }
            }
        }
    }	

}


