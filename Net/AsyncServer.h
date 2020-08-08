#ifndef __AsyncServer_H__
#define __AsyncServer_H__

#include "AsyncNetDefine.h"
#include "../Thread/Task.h"
#include "../Thread/TaskProcess.h"
#include "../Thread/ThreadPoolImpl_Linux.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>

using namespace std;

class AsyncServer
{
	class PostSockReader : public TaskProcess{
	public:	
		PostSockReader(DispatchData* pData,AsyncServer* pNet):m_data(pData),m_pNet(pNet){}
		virtual void operator()(Task* task){
			int rLen = 0;
			int nCount = 0;
			AsyncNetRWTask* pNetTask = (AsyncNetRWTask*)task;
			MemNode node;
			node=m_data->GetAndPopFront(pNetTask->GetClientSock());
			if(node.Valid()){
				node.Reset();
				while(1)
				{
					rLen = recv(pNetTask->GetClientSock(), (void*)(node.pData+nCount), node.nCount-nCount, 0);
					if(0 > rLen)
					{
						if(EAGAIN == rLen){//缓冲无数据可读
							if(nCount > 0){
								node.nTrans = nCount;
								m_pNet->ProduceNode(node);
							}else
								m_data->PushBack(pNetTask->GetClientSock(),node);
							return;
						}else{
							printf("read from client:%d failed!\n",pNetTask->GetClientSock());
							m_data->PushBack(pNetTask->GetClientSock(),node);
							return;
						}
					}
					else if(0 == rLen)
					{
						printf("client:%d has disconnect\n",pNetTask->GetClientSock());
						m_pNet->FdDel(pNetTask->GetClientSock(), pNetTask->GetServerSock());
                        //TODO:m_pNet->ProduceNode(req);
						return ;
					}else{
						if(rLen==node.nCount){
							node.nTrans = node.nCount;
							SPostReq req(node);
							req.cSock = pNetTask->GetClientSock();
							req.sSock = pNetTask->GetServerSock();
							m_pNet->ProduceNode(req);
							node.Reset();
							node=m_data->GetAndPopFront(pNetTask->GetClientSock());
							if(!node.Valid()){
								puts("PostSockReader post read is not enough");
								return;
							}
							rLen = nCount = 0;
						}else
							nCount += rLen;
					}
				}
			}
		}
	private:
		DispatchData* 	m_data;
		AsyncServer*	m_pNet;
	};
	class PostSockWriter : public TaskProcess{
	public:
		PostSockWriter(DispatchData* pData,AsyncServer* pNet):m_data(pData),m_pNet(pNet){}
		virtual void operator()(Task* task){
			int wLen = 0;
			int nCount = 0;
			AsyncNetRWTask* pNetTask = (AsyncNetRWTask*)task;
			MemNode node;
			node=m_data->GetAndPopFront(pNetTask->GetClientSock());
			if(node.Valid()){
				wLen = send(pNetTask->GetClientSock(), (void*)node.pData, node.nCount, 0);
				if(wLen<=0){
					printf("write for client:%d failed!\n",pNetTask->GetClientSock());
					m_data->PushBack(pNetTask->GetClientSock(),node);
					return;						
				}else if(wLen>0){//TODO:此处待处理只发送部分数据
					node.nTrans = wLen;
					SPostReq req(node);
					req.cSock = pNetTask->GetClientSock();
					req.sSock = pNetTask->GetServerSock();					
					m_pNet->ProduceNode(req);
				}
			}
		}	
	private:
		DispatchData* 	m_data;
		AsyncServer*	m_pNet;
	};
public:
	AsyncServer();
	AsyncServer(int nthPool);
	~AsyncServer();
	bool Init(int port);
	virtual bool Stop();
	void postQuest(int sock,const      jobtype& type,const MemNode& node);
	bool GetQueueComplateStatus(int& sock,SPostReq& pNode,int& transBytes);
protected:
	void ProduceNode(const SPostReq& node);
	int InitSocket(const char* ip,int port,int type);
	virtual void HandleThread()= 0;
	virtual int AsyncInit(int int_sfd)= 0;
	virtual int FdAdd(int int_fd, int int_epfd,bool bMonWrite = false)= 0;
	virtual int FdDel(int int_fd, int int_epfd)= 0;
	int on_accept_callback(int int_sfd,string& ip, int int_epfd);	
private:
	static void* AsyncThread(void* );
protected:
	ThreadPoolImpl	m_thPool;
	int 			m_serSocket;
	bool			m_bRun;
	pthread_t		m_hThread;
	
	pthread_mutex_t m_lock;
	pthread_cond_t 	m_condp;
	list<SPostReq>	m_data;//生产者消费者->对外接口
	
	DispatchData	m_ReadQue;
	DispatchData	m_WriteQue;
	PostSockReader*	m_pReader;
	PostSockWriter* m_pWriter;
};

#endif	// AsyncServer.h
