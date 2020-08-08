#include "AsyncClient.h"

AsyncClient::AsyncClient(int nthPool):m_cliSocket(-1),m_thPool(nthPool),m_bRun(false)
{

}

AsyncClient::~AsyncClient()
{
	if(m_bRun)
		Stop();
}

bool AsyncClient::Init(const char* Ip,int port)
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
	if(-1==AsyncInit(m_cliSocket))
		return false;
    
	int int_status=fcntl(m_cliSocket,F_GETFL);
    int_status=int_status|O_NONBLOCK;
    fcntl(m_cliSocket,F_SETFL,int_status);
	
	if(0!=m_thPool.Init())
		return false;
	m_thPool.setTaskCmp(TCompare(&AsyncNetRWTask::tCompare));
	pthread_create(&m_hThread,0,AsyncThread,this);
	pthread_detach(m_hThread);	
	
	m_thPool.Register_taskProcess(READTASK,m_pReader = new PostSockReader(&m_ReadQue,this));
	m_thPool.Register_taskProcess(WRITETASK,m_pWriter = new PostSockWriter(&m_WriteQue,this));
	m_thPool.setMaxPriorityType(READTASK);
	m_thPool.setMergeTaskType(WRITETASK);

	m_ReadQue.AppendNew(m_cliSocket);
	m_WriteQue.AppendNew(m_cliSocket);
}

bool AsyncClient::Stop()
{
	m_bRun = false;
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_condp);	
	m_thPool.Stop();
	close(m_cliSocket);
	return true;
}

int AsyncClient::InitSocket(const char* ip,int port,int type)
{
	m_ip = ip;
	m_port = port;
	//初始化socket
	m_cliSocket = socket(AF_INET,type,0);
    return m_cliSocket;
}

void AsyncClient::postQuest(int sock,const jobtype& type,const MemNode& node)
{
    if(!m_bRun)return;
    if(type == READTASK)
	    m_ReadQue.AppendData(sock,node);
    else if(type == WRITETASK)
	    m_WriteQue.AppendData(sock,node);
}

void* AsyncClient::AsyncThread(void* param)
{
	AsyncClient* pThis = (AsyncClient*)param;
	pThis->HandleThread();
	return NULL;
}

void AsyncClient::ProduceNode(const CPostReq& node)
{
    if(!m_bRun)return;
	pthread_mutex_lock(&m_lock);
	m_data.push_back(node);
	pthread_cond_broadcast(&m_condp);
	pthread_mutex_unlock(&m_lock);	
}

bool AsyncClient::GetQueueComplateStatus(int& sock,CPostReq& pNode,int& transBytes)
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

bool AsyncClient::ReConnect()
{
    //绑定ip、port
	int int_ret = -1;
    if(m_bRun)return false;
    struct sockaddr_in sock_server;    
    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = inet_addr(m_ip.c_str());
	sock_server.sin_port = htons(m_port);
	
	//inet_aton("192.168.17.3",&sock_server.sin_addr);
	int_ret=connect(m_cliSocket,(struct sockaddr*)&sock_server,sizeof(sock_server));
	if(-1==int_ret){
		printf("connect %s port %d failed,%s\n",m_ip.c_str(),m_port,strerror);
		return false;
	}
	return m_bRun = true;
}
