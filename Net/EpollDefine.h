#ifndef __EpollDefine_H__
#define __EpollDefine_H__

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <map>
#include <list>
#include "../Thread/Task.h"

using namespace std;

#define EPOLLCREATFDMAX (1024)
#define EPOLLMONFDMAX   (512)

enum OpType
{
	OP_ACCEPT,
	OP_EXIT,
	OP_READ,
	OP_WRITE,
};

class MemNode
{
public:	
	MemNode():pData(0),nCount(0),nTrans(0),nSize(0),nIndex(-1),m_pPrevIOData(0){}
	virtual ~MemNode(){}
	void Reset(){if(NULL!=pData)nTrans=0,m_pPrevIOData=NULL,memset(pData,0,nSize);}
	bool Valid(){ return NULL!=pData&&nCount;}
public:	
	char* 	pData;
	int		nCount;
	int		nTrans;
	int 	nSize;
	int		nIndex;
	void*	m_pPrevIOData;
	OpType	oType;//类型：0 新客户端接入、1 读、2 写
};

class SPostReq : public MemNode
{
public:
	SPostReq(const MemNode& mem):cSock(-1),sSock(-1),MemNode(mem){}
	SPostReq():MemNode(),cSock(-1),sSock(-1){}
	int		cSock;
	int		sSock;
    string  ip;
	bool Valid(){ return -1!=cSock&&-1!=sSock;}
};

class CPostReq : public MemNode
{
public:
	CPostReq(const MemNode& mem):cSock(-1),MemNode(mem){}
	CPostReq():MemNode(),cSock(-1){}
	int		cSock;
	bool Valid() { return -1!=cSock;}
};

class EpollRWTask : public Task
{
public:
    EpollRWTask(int cSock,jobtype type,int sSock=-1):Task(type),m_cSock(cSock),m_sSock(sSock){}
    int GetClientSock(){ return m_cSock;}
    int GetServerSock(){ return m_sSock;}
    virtual bool tCompare(jobtype type ,const Task* pTask)const{
        EpollRWTask* pEtask = (EpollRWTask*)pTask;
        return m_cSock==pEtask->m_cSock&&this->m_type==pEtask->m_type&&this->m_type==type;
    }
private:
    int m_cSock;
    int m_sSock;
};

class DataMutexQue{
public:
    void Init(){
        pthread_mutex_init(&m_lock,NULL);
    }
    void Uninit(){
        pthread_mutex_destroy(&m_lock);
    }
    void Push_back(const MemNode& node){
        pthread_mutex_lock(&m_lock);
        m_lst.push_back(node);
        pthread_mutex_unlock(&m_lock);
    }
    void Push_front(const MemNode& node){
        pthread_mutex_lock(&m_lock);
        m_lst.push_front(node);
        pthread_mutex_unlock(&m_lock);
    }
    MemNode Pop_front(){
        MemNode pData ;
        pthread_mutex_lock(&m_lock);
        pData = m_lst.front();
        m_lst.pop_front();
        pthread_mutex_unlock(&m_lock);
        return pData;
    }
private:
    list<MemNode>   m_lst;
    pthread_mutex_t m_lock;
};

class DispatchData {
public:
    DispatchData(){
        pthread_rwlock_init(&m_lock,NULL);
    }
    ~DispatchData(){
        pthread_rwlock_destroy(&m_lock);
    }
    void AppendNew(int sock){
        pthread_rwlock_wrlock(&m_lock);
        m_sockInfo.insert(make_pair(sock,DataMutexQue()));
        m_sockInfo.find(sock)->second.Init();
        pthread_rwlock_unlock(&m_lock);
    }
    void DeleteOut(int sock){
        pthread_rwlock_wrlock(&m_lock);
        if(m_sockInfo.end()!=m_sockInfo.find(sock))
        {
            m_sockInfo.find(sock)->second.Uninit();
            m_sockInfo.erase(m_sockInfo.find(sock));
        }
        pthread_rwlock_unlock(&m_lock);
    }
    void AppendData(int sock,MemNode node){
        pthread_rwlock_rdlock(&m_lock);
        if(m_sockInfo.end()!=m_sockInfo.find(sock))
            m_sockInfo.find(sock)->second.Push_back(node);
        pthread_rwlock_unlock(&m_lock);         
    }
    void PushBack(int sock,const MemNode& node){
        pthread_rwlock_rdlock(&m_lock);
        if(m_sockInfo.end()!=m_sockInfo.find(sock))
            m_sockInfo.find(sock)->second.Push_front(node);
        pthread_rwlock_unlock(&m_lock);             
    }
    MemNode GetAndPopFront(int sock){
        MemNode tmp;
        pthread_rwlock_rdlock(&m_lock);
        if(m_sockInfo.end()!=m_sockInfo.find(sock))
            tmp = m_sockInfo.find(sock)->second.Pop_front();
        pthread_rwlock_unlock(&m_lock);
        return tmp;
    }
private:
    map<int,DataMutexQue>   m_sockInfo;
    pthread_rwlock_t    m_lock;
};


#endif // EpollDefine.h
