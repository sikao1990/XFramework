#include "EpollServer.h"

EpollServer::EpollServer():m_serSocket(-1),m_hEpoll(-1),m_thPool(2),m_bRun(false)
{
}

EpollServer::EpollServer(int nthPool):m_serSocket(-1),m_hEpoll(-1),m_thPool(nthPool),m_bRun(false)
{

}

EpollServer::~EpollServer()
{
	if(m_bRun)
		Stop();
}

bool EpollServer::Init(int port)
{
    if(m_bRun)return false;
	m_bRun = true;
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_condp,NULL);
	
	if(-1==InitSocket("127.0.0.1",port,SOCK_STREAM))
		return false;
	if(-1==epoll_init(m_serSocket))
		return false;
	if(0!=m_thPool.Init())
		return false;
	m_thPool.setTaskCmp(TCompare(&EpollRWTask::tCompare));
	pthread_create(&m_hThread,0,epollThread,this);
	pthread_detach(m_hThread);	
	
	m_thPool.Register_taskProcess(READTASK,m_pReader = new PostSockReader(&m_ReadQue,this));
	m_thPool.Register_taskProcess(WRITETASK,m_pWriter = new PostSockWriter(&m_WriteQue,this));
	m_thPool.setMaxPriorityType(READTASK);
	m_thPool.setMergeTaskType(WRITETASK);
}

bool EpollServer::Stop()
{	
	m_bRun = false;
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_condp);	
	m_thPool.Stop();
	close(m_serSocket);
	close(m_hEpoll);
}

int EpollServer::InitSocket(const char* ip,int port,int type)
{
    int int_ret = -1; 
    //绑定ip、port
    struct sockaddr_in sock_server;    
    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = inet_addr(ip);
	sock_server.sin_port = htons(port);
	
	//初始化socket
	m_serSocket = socket(AF_INET,type,0);
	int_ret = bind(m_serSocket, (struct sockaddr*)&sock_server, sizeof(struct sockaddr));
	if(-1 == int_ret)
	{
		printf("bind port = %d fail..retry!\n",port);
		return false;
	}

    printf("bind port = %d success!\n",port);

    //监听
    int_ret = listen(m_serSocket, 10);
    if(-1 == int_ret)
    {
        perror("listen");
        return -1;
    }

    return m_serSocket;
}

int EpollServer::epoll_init(int int_sfd)
{
    m_hEpoll = epoll_create(EPOLLCREATFDMAX);
    epoll_add(int_sfd, m_hEpoll);
    return m_hEpoll;
}

int EpollServer::epoll_add(int int_fd, int int_epfd)
{
    struct epoll_event epoll_ev;
    epoll_ev.events = EPOLLIN|EPOLLET;
    epoll_ev.data.fd = int_fd;
    epoll_ctl(int_epfd, EPOLL_CTL_ADD, int_fd, &epoll_ev);
    return 0;
}

int EpollServer::epoll_del(int int_fd, int int_epfd)
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


void EpollServer::postQuest(int sock,int type,const MemNode& node)
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

void EpollServer::epollWait()
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
                if(epoll_evs[int_idx].events == EPOLLIN)
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
						m_thPool.submit_job_req(new EpollRWTask(epoll_evs[int_idx].data.fd,WRITETASK,m_serSocket));
                    else if(epoll_evs[int_idx].events&EPOLLOUT)//可写
						m_thPool.submit_job_req(new EpollRWTask(epoll_evs[int_idx].data.fd,READTASK,m_serSocket));
                }
            }
        }
    }	
}

int EpollServer::on_accept_callback(int int_sfd,string& ip,int int_epfd)
{
    struct sockaddr_in sock_client;    
    socklen_t sock_len;

    //接入客户端
    int int_new_fd = accept(int_sfd, (struct sockaddr*)&sock_client, &sock_len);
    if(-1 == int_new_fd)
    {
        perror("accept");
        return -1;
    }
    ip = inet_ntoa(sock_client.sin_addr);

    //把new_fd注册到epfd中
    epoll_add(int_new_fd, int_epfd);

    //修改文件描述符状态为非阻塞
    int int_status=fcntl(int_new_fd,F_GETFL);
    int_status=int_status|O_NONBLOCK;
    fcntl(int_new_fd,F_SETFL,int_status);

    printf("accept new_fd = %d success!\n", int_new_fd);
    return int_new_fd;
}

void* EpollServer::epollThread(void* param)
{
	EpollServer* pThis = (EpollServer*)param;
	pThis->epollWait();
	return NULL;
}

void EpollServer::ProduceNode(const SPostReq& node)
{
    if(!m_bRun)return;
	pthread_mutex_lock(&m_lock);
	m_data.push_back(node);
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_unlock(&m_lock);	
}

bool EpollServer::GetQueueComplateStatus(int& sock,SPostReq& pNode,int& transBytes)
{
	int 	TmpSock =-1;
	SPostReq tmpNode = {} ;
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
