#include <stdio.h>
#include <signal.h>
#include <list>
#include <map>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include "../../Framework/Thread/Task.h"
#include "../../Framework/Thread/TaskCtrl.h"
#include "../../Framework/Thread/ThreadPool.h"
#include "../../Framework/Thread/TaskProcess.h"
#include "../../Framework/Thread/Semaphore.h"

using namespace std;

struct TimerInfo
{
	unsigned short 	nCount;		//统计计数
	unsigned short	nfinish;	//结束标记->对于重复定时器，则表示第一次是否完成
	unsigned short 	len;		//从当前时间多久后触发 //适合用微妙为单位,以1毫秒为单位,则需要测试精度
	unsigned short 	interval;	//间隔时间	
	long long 		nCur;		//当前时间
	TimerInfo(){ memset(this,0,sizeof(*this)); }
	void reset(){ nfinish = 1; }
	bool IsTimeout(){ return nfinish>1; }
	virtual void Handle() { }
};

template< typename T >
class UseTimer : public TimerInfo
{
public:
	typedef void (T::*PFunc)();
public:
	UseTimer(T* pObj,PFunc func):obj(pObj),funcptr(func){}
	void Handle(){
		(obj->*funcptr)();
	}
private:
	T* 		obj;
	PFunc	funcptr;
};

long long getTimeStamp()
{
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
}

class OnceTask : public Task
{
public:
	OnceTask(const jobtype& type):Task(type){}
	TimerInfo* handle;
};

class OnceProcess : public TaskProcess
{
public:
	void operator()(Task* task){
		OnceTask* pTask = (OnceTask*)task;
		pTask->handle->Handle();
		delete pTask->handle;
	}
};
class Timer;
class RepeatProcess : public TaskProcess
{
public:
	RepeatProcess(Timer* pTimer,TaskCtrl* pCtrl):m_pTimer(pTimer),TaskProcess(pCtrl){}
	void operator()(Task* task);
private:
	Timer* m_pTimer;
};

class Timer
{
public:
	Timer():m_pools(2){
	}
	~Timer(){
		pthread_mutex_destroy(&m_OnceLock);
		pthread_rwlock_destroy(&m_lock);
	}
	void Init(){
		//signal(SIGALRM, &Timer::sigHandle);
		pthread_create(&m_th2,NULL,&Timer::runth,this);
		pthread_detach(m_th2);
		pthread_mutex_init(&m_OnceLock,NULL);
		pthread_rwlock_init(&m_lock,NULL);
		pThis=this;
		if(sem_init(&m_notice,0,0))
			throw "";			
		pthread_create(&m_th,NULL,&Timer::run,this);
		pthread_detach(m_th);
		m_nflag = true;
		clktck = sysconf(_SC_CLK_TCK)*1000;	
		
		m_pools.init_pool();
		m_pools.Register_taskProcess("0",new OnceProcess);
		RepeatProcess* pRepeat = new RepeatProcess(this,&m_ctrl);
		m_pools.Register_taskProcess("1",pRepeat);
		Task* t=new Task("1");
		m_ctrl.AddThread(t);
		m_pools.submit_job_req(t);
		
		struct itimerval tick;
		tick.it_value.tv_sec = 1;
		tick.it_value.tv_usec = 0;
		tick.it_interval.tv_sec = 1;
		tick.it_interval.tv_usec = 0;
		//setitimer(ITIMER_REAL, &tick, NULL);		
	}
	bool AddTimer(int len){
		TimerInfo* t=new UseTimer<Timer>(this,&Timer::timeout1);
		t->len	= len;
		t->nCur = getTimeStamp();
		pthread_mutex_lock(&m_OnceLock);
		onceLst.push_back(t);//写锁
		pthread_mutex_unlock(&m_OnceLock);
	}
	bool AddTimer(const char* str,int len,int nInterval){//
		TimerInfo* t=new UseTimer<Timer>(this,&Timer::timeout2);
		t->len  	= len;
		t->interval = nInterval;
		t->nCur 	= getTimeStamp();
		pthread_rwlock_wrlock(&m_lock);
		repeatLst.insert(make_pair(str,t));
		pthread_rwlock_unlock(&m_lock);
		return repeatLst.size()-1;//写锁
	}
	bool DelTimer(const char* str){//写锁
		pthread_rwlock_wrlock(&m_lock);
		if(repeatLst.find(str)!=repeatLst.end()){
			repeatLst.erase(repeatLst.find(str));
			delete repeatLst.find(str)->second;
			pthread_rwlock_unlock(&m_lock);
			return true;
		}
		pthread_rwlock_unlock(&m_lock);
		return false;
	}
	void ModifyRepeatTimer(const char* str,int len,int nInterval){
		pthread_rwlock_wrlock(&m_lock);
		pthread_rwlock_unlock(&m_lock);
	}
	void timeout1(){
		puts("timeout1");
	}
	void timeout2(){
		puts("timeout2");
	}
protected:
	friend class RepeatProcess;	
	void DealHandle(){
		pthread_rwlock_rdlock(&m_lock);
		for(map<string,TimerInfo*>::iterator it=repeatLst.begin();it!=repeatLst.end();it++){
			if(it->second->IsTimeout()){
				it->second->Handle();
			}
		}
		pthread_rwlock_unlock(&m_lock);
	}
	void TimerHandle(){//读锁
		//start = time(&tmsstart);
		pthread_mutex_lock(&m_OnceLock);
		for(list<TimerInfo*>::iterator it=onceLst.begin();it!=onceLst.end();it++){
			if((*it)->len == ++(*it)->nCount){
				printf("TimerHandle %d,len:%d\n",(*it)->nCount,(*it)->len);
				(*it)->nfinish = 2;
			}
		}
		pthread_mutex_unlock(&m_OnceLock);
		pthread_rwlock_rdlock(&m_lock);
		for(map<string,TimerInfo*>::iterator it=repeatLst.begin();it!=repeatLst.end();it++){
			switch(it->second->nfinish){
			case 0:
				if(it->second->len == ++it->second->nCount){//第一次时间到
					it->second->nfinish = 2;
					it->second->nCount = 0;
				}
				break;
			case 1://重复定时器重复时间未到状态
				if(it->second->interval == ++it->second->nCount){
					it->second->nCount 	= 0;
					it->second->nfinish = 2;
				}
				break;			
			case 2:
				puts("error------------");
				break;
			}
		}
		pthread_rwlock_unlock(&m_lock);
		//puts("---");
		sem_post(&m_notice);
		//m_notice.SemPost();
		//end = time(&tmsend);
		int val = -1;
		if(sem_getvalue(&m_notice,&val))
			throw "xxx";
		//printf("sigHandle:%d\n",val);		
		//printf("real: %7.2f,end %ld,start %ld\n", (end - start)/(double) clktck,end,start);
	}
	void Run(){//另外一个线程处理
		vector<int>	tmpVec;
		while(m_nflag){
			int index = 0;
			tmpVec.clear();
			int val = -1;
			// if(sem_getvalue(&m_notice,&val))
				// throw "xxx";
			// printf("Run start:%d\n",val);
			sem_wait(&m_notice);
			puts("sem_wait");
			//m_notice.SemWait();
			pthread_mutex_lock(&m_OnceLock);
			//printf("%d\n",onceLst.size());
			for(list<TimerInfo*>::iterator it=onceLst.begin();it!=onceLst.end();it++,index++){
				if((*it)->IsTimeout())
				{
					tmpVec.push_back(index);
					OnceTask* ptask=new OnceTask("0");
					ptask->handle = *it;
					m_pools.submit_job_req(ptask);
					printf("push task into task pool:%d\n",index);//TODO:push task into task pool
				}
			}		
			list<TimerInfo*>::iterator it=onceLst.begin();
			for(int i=tmpVec.size()-1;i>=0;i--,it=onceLst.begin()){
				advance(it, tmpVec[i]);
				onceLst.erase(it);
				//delete *it;
			}
			pthread_mutex_unlock(&m_OnceLock);
			//puts("repeat begin---");
			pthread_rwlock_rdlock(&m_lock);
			for(map<string,TimerInfo*>::iterator it=repeatLst.begin();it!=repeatLst.end();it++){
				if(it->second->IsTimeout()){
					puts("push task into task pool");//TODO:push task into task pool
					it->second->reset();
				}
			}		
			pthread_rwlock_unlock(&m_lock);
			//puts("-----------");
			//m_ctrl.SyncTaskOneTime();
			//puts("syncOneTime->pool");//TODO:syncOneTime();->pool
		}
	}
	static void sigHandle(int sig){
		Timer* pthis = (Timer*)pThis;
		pthis->TimerHandle();
	}
	static void* run(void* p){
		Timer* pThis = (Timer*)p;
		pThis->Run();
	}
	static void* runth(void* p){
		Timer* pthis = (Timer*)pThis;
		struct timeval tv,tt;
		long long mprev = 0;
		long long mcur = 0;
		long long ubegin = 0;
		long long uend = 0;
		gettimeofday(&tv,NULL);
		mprev = tv.tv_sec;
		//mprev = tv.tv_sec*1000 + tv.tv_usec/1000;//毫秒		
		while(1){
			gettimeofday(&tv,NULL);
			mcur = tv.tv_sec;
			//mcur = tv.tv_sec*1000 + tv.tv_usec/1000;//毫秒
			ubegin = tv.tv_sec*1000 + tv.tv_usec/1000;
			//ubegin = tv.tv_sec*1000000 + tv.tv_usec;//微秒
			if(mcur-mprev==1){
				pthis->TimerHandle();
				printf("--------->%prev:lld,cur:%lld\n",mprev,mcur);
				mprev = mcur;
			}
			gettimeofday(&tt,NULL);
			uend = tt.tv_sec*1000 + tt.tv_usec/1000;
			//uend = tt.tv_sec*1000000 + tt.tv_usec;
			//printf("ubegin:%lld,uend:%lld,sub:%lld\n",ubegin,uend,1000000-(uend-ubegin) );
			usleep(990000-(uend-ubegin) );	
		}
	}
private:
	static void* pThis;
private:
	int 		clktck;
	clock_t     start;
	clock_t		end;
	struct tms  tmsstart;
	struct tms	tmsend;
	
	unsigned				m_nflag;
	//Semaphore				m_notice;
	sem_t					m_notice;
	pthread_t				m_th;
	pthread_mutex_t			m_OnceLock;
	pthread_rwlock_t		m_lock;
	list<TimerInfo*>		onceLst;
	map<string,TimerInfo*>	repeatLst;
	ThreadPool				m_pools;
	TaskCtrl				m_ctrl;
	pthread_t				m_th2;
};
void* Timer::pThis = NULL;

void RepeatProcess::operator()(Task* task){
	while(1){
		m_ctrl->wait(task);
		m_pTimer->DealHandle();
	}
}

int main()
{
	Timer t;
	t.Init();
	//t.AddTimer(4000);
	t.AddTimer("11",1,1);
	// t.AddTimer("12",1000,2000);
	// t.AddTimer("13",1000,2000);
	// t.AddTimer("14",1000,2000);
	// t.AddTimer("15",1000,2000);
	// t.AddTimer("16",1000,2000);
	// t.AddTimer("17",1000,2000);
	// t.AddTimer("18",1000,2000);
	// t.AddTimer("19",1000,2000);
	// t.AddTimer("10",1000,2000);
	// t.AddTimer("11",1000,2000);
	// t.AddTimer("12",1000,2000);
	while(1)
		sleep(1);
	return 0;
}
