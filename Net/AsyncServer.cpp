#include "AsyncServer.h"
#include <sys/socket.h>

AsyncServer::AsyncServer():m_serSocket(-1),m_thPool(2),m_bRun(false)
{
}

AsyncServer::AsyncServer(int nthPool):m_serSocket(-1),m_thPool(nthPool),m_bRun(false)
{

}

AsyncServer::~AsyncServer()
{
	if(m_bRun)
		Stop();
}

bool AsyncServer::Init(int port)
{
    if(m_bRun)return false;
	m_bRun = true;
	pthread_mutex_init(&m_lock,NULL);
	pthread_cond_init(&m_condp,NULL);
	
	if(-1==InitSocket("127.0.0.1",port,SOCK_STREAM))
		return false;
	if(-1==AsyncInit(m_serSocket))
		return false;
	if(0!=m_thPool.Init())
		return false;
	m_thPool.setTaskCmp(TCompare(&AsyncNetRWTask::tCompare));
	pthread_create(&m_hThread,0,AsyncThread,this);
	pthread_detach(m_hThread);	
	
	m_thPool.Register_taskProcess(READTASK,m_pReader = new PostSockReader(&m_ReadQue,this));
	m_thPool.Register_taskProcess(WRITETASK,m_pWriter = new PostSockWriter(&m_WriteQue,this));
	m_thPool.setMaxPriorityType(READTASK);
	m_thPool.setMergeTaskType(WRITETASK);
}

bool AsyncServer::Stop()
{	
	m_bRun = false;
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_condp);	
	m_thPool.Stop();
	close(m_serSocket);
}

int AsyncServer::InitSocket(const char* ip,int port,int type)
{
    int int_ret = -1; 
    //绑定ip、port
    struct sockaddr_in sock_server;    
    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = inet_addr(ip);
	sock_server.sin_port = htons(port);
	
	//初始化socket
	m_serSocket = socket(AF_INET,type,0);
	int_ret = bind(m_serSocket, (const struct sockaddr*)&sock_server, (socklen_t)sizeof(struct sockaddr));
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

void AsyncServer::postQuest(int sock,const      jobtype& type,const MemNode& node)
{
    if(!m_bRun)return;
    if(type == READTASK)
        m_ReadQue.AppendData(sock,node);
    else if(type == WRITETASK)
        m_WriteQue.AppendData(sock,node);
}

int AsyncServer::on_accept_callback(int int_sfd,string& ip,int int_epfd)
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
    FdAdd(int_new_fd, int_epfd,true);

    //修改文件描述符状态为非阻塞
    int int_status=fcntl(int_new_fd,F_GETFL);
    int_status=int_status|O_NONBLOCK;
    fcntl(int_new_fd,F_SETFL,int_status);

    printf("accept new_fd = %d success!\n", int_new_fd);
    return int_new_fd;
}

void* AsyncServer::AsyncThread(void* param)
{
	AsyncServer* pThis = (AsyncServer*)param;
	pThis->HandleThread();
	return NULL;
}

void AsyncServer::ProduceNode(const SPostReq& node)
{
    if(!m_bRun)return;
	pthread_mutex_lock(&m_lock);
	m_data.push_back(node);
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_unlock(&m_lock);	
}

bool AsyncServer::GetQueueComplateStatus(int& sock,SPostReq& pNode,int& transBytes)
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
