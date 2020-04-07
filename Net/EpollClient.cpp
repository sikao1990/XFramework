#include "EpollClient.h"

EpollClient::EpollClient(int nthPool):m_serSocket(-1),m_hEpoll(-1),m_thPool(nthPool),m_bRun(false)
{

}

EpollClient::~EpollClient()
{
	if(m_bRun)
		Stop();
}

bool EpollClient::Init(const char* Ip,int port)
{
    if(m_bRun)return false;
	m_ip 	= Ip;
	m_port 	= port;
	m_bRun 	= true;
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_condp,NULL);
	
	if(-1==InitSocket(m_ip.c_str(),m_port,SOCK_STREAM))
		return false;
	if(!ReConnect())
		return false;
	if(-1==epoll_init(m_serSocket))
		return false;
    
	int int_status=fcntl(m_serSocket,F_GETFL);
    int_status=int_status|O_NONBLOCK;
    fcntl(m_serSocket,F_SETFL,int_status);
	
	if(0!=m_thPool.Init())
		return false;
	m_thPool.setTaskCmp(TCompare(&EpollRWTask::tCompare));
	pthread_create(&m_hThread,0,epollThread,this);
	pthread_detach(m_hThread);	
	
	m_thPool.Register_taskProcess(READTASK,m_pReader = new PostSockReader(&m_ReadQue,this));
	m_thPool.Register_taskProcess(WRITETASK,m_pWriter = new PostSockWriter(&m_WriteQue,this));
	m_thPool.setMaxPriorityType(READTASK);
	m_thPool.setMergeTaskType(WRITETASK);

	m_ReadQue.AppendNew(m_serSocket);
	m_WriteQue.AppendNew(m_serSocket);
}

bool EpollClient::Stop()
{
	m_bRun = false;
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_condp);	
	m_thPool.Stop();
	close(m_serSocket);
	close(m_hEpoll);
	return true;
}

int EpollClient::InitSocket(const char* ip,int port,int type)
{
	m_ip = ip;
	m_port = port;
	//初始化socket
	m_serSocket = socket(AF_INET,type,0);
    return m_serSocket;
}

int EpollClient::epoll_init(int int_sfd)
{
    m_hEpoll = epoll_create(1);
    epoll_add(int_sfd, m_hEpoll);
    return m_hEpoll;
}

int EpollClient::epoll_add(int int_fd, int int_epfd)
{
    struct epoll_event epoll_ev;
    epoll_ev.events = EPOLLIN|EPOLLET;
    epoll_ev.data.fd = int_fd;
    epoll_ctl(int_epfd, EPOLL_CTL_ADD, int_fd, &epoll_ev);
    return 0;
}

int EpollClient::epoll_del(int int_fd, int int_epfd)
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


void EpollClient::postQuest(int sock,int type,const MemNode& node)
{
    if(!m_bRun)return;
	switch(type)
	{
	case 0://READTASK:
		m_ReadQue.AppendData(sock,node);
		break;
	case 1://WRITETASK:
		m_WriteQue.AppendData(sock,node);
		break;
	}
}

void EpollClient::epollWait()
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
                if(epoll_evs[int_idx].events == EPOLLIN)
                {
                    if(epoll_evs[int_idx].data.fd == m_serSocket)
                    {
    					if(epoll_evs[int_idx].events&EPOLLIN)//可读
    						m_thPool.submit_job_req(new EpollRWTask(epoll_evs[int_idx].data.fd,WRITETASK));
                        else if(epoll_evs[int_idx].events&EPOLLOUT)//可写
    						m_thPool.submit_job_req(new EpollRWTask(epoll_evs[int_idx].data.fd,READTASK));
					}
                }
            }
        }
    }	
}

void* EpollClient::epollThread(void* param)
{
	EpollClient* pThis = (EpollClient*)param;
	pThis->epollWait();
	return NULL;
}

void EpollClient::ProduceNode(const CPostReq& node)
{
    if(!m_bRun)return;
	pthread_mutex_lock(&m_lock);
	m_data.push_back(node);
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_unlock(&m_lock);	
}

bool EpollClient::GetQueueComplateStatus(int& sock,CPostReq& pNode,int& transBytes)
{
	int 	TmpSock =-1;
	CPostReq tmpNode;
    if(!m_bRun)return false;
	pthread_mutex_lock(&m_lock);
	while(m_data.empty())
		pthread_cond_wait(&m_condp,&m_lock);
	tmpNode = m_data.front();
	m_data.pop_front();
	pthread_mutex_unlock(&m_lock);
	sock = tmpNode.cSock;
	transBytes = tmpNode.nTrans;
	pNode = tmpNode;
	return tmpNode.Valid();
}

bool EpollClient::ReConnect()
{
    //绑定ip、port
	int int_ret = -1;
    if(m_bRun)return false;
    struct sockaddr_in sock_server;    
    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = inet_addr(m_ip.c_str());
	sock_server.sin_port = htons(m_port);
	
	//inet_aton("192.168.17.3",&sock_server.sin_addr);
	int_ret=connect(m_serSocket,(struct sockaddr*)&sock_server,sizeof(sock_server));
	if(-1==int_ret){
		printf("connect %s port %d failed,%s\n",m_ip.c_str(),m_port,strerror);
		return false;
	}
	return m_bRun = true;
}
