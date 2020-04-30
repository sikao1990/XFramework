#include <stdio.h>
#include "../../Framework/XPublic.h"
#include <vector>
#include <queue>
#include <map>
#include <list>
#include <stdarg.h>
#include "../../Framework/DB/DBDefine.h"
#include "../../Framework/Thread/TimerMgr.h"
#ifdef _WIN32
#include "../../Framework/DB/AdoAccess.h"
#include "../../Framework/DB/AdoConn.h"
#endif
#include "../../Framework/DB/DBConnPool.h"
#include "../../Framework/DB/MysqlAccess.h"
#include "../../Framework/DB/MysqlConn.h"

using namespace std;

class MyThread : public Thread
{
public:
	MyThread(TaskCtrl* ctrl):m_ctrl(ctrl){}
	bool start(){
		m_bflag = true;
		return Thread::start();
	}
	void stop(){
		m_bflag=false;
		Thread::stop();
	}
protected:
	void run(){
		while(m_bflag){
			m_ctrl->wait(this);
			printf("run,pid:%u\n",GetThreadID());
			//Thread::mSleep(1000);
			m_ctrl->done();
		}
	}
private:
	bool m_bflag;
	TaskCtrl*	m_ctrl;
};

AtomMutex lock;
void Increment(int *p)
{
	lock.Lock();
	*p+=1;
	lock.UnLock();
}

Semaphore slock(1);
void IncrementEx(int* p)
{
	slock.SemWait();
	*p += 1;
	slock.SemPost();
}
SemaphoreEx sxlock(1);
void IncrementExx(int* p)
{
	sxlock.SemWait();
	*p += 1;
	sxlock.SemSignal();
}

class TThread : public Thread
{
public:
	TThread(int* p) :pVal(p) {}
protected:
	void run() {
		int i = 0;
		puts("test thread start");
		while (1) {
			//Increment(pVal);
			//IncrementEx(pVal);
			IncrementExx(pVal);
			if (++i > 100000)
				break;
		}
		printf("thread end:%d\n",i);
	}
private:
	int* pVal;
};

class MyTask : public Task
{
public:
	MyTask(const jobtype& type, int flag = 0) :Task(type, flag) { m_data = 0; }
	virtual ~MyTask() {}
public:
	virtual int wait() {
		return 0;
	}
	virtual void done(int n) {
	}
	void SetData(int n) {
		m_data = n;
	}
	void ShowData() {
		printf("ShowData %d\n",m_data);
	}
private:
	int m_data;
};
class MyTaskProcess : public TaskProcess
{
public:
	MyTaskProcess(TaskCtrl* pCtrl):TaskProcess(pCtrl){}
	virtual ~MyTaskProcess() {}
	virtual void operator()(Task* task) {
		MyTask* pTag = dynamic_cast<MyTask*>(task);
		while (1) {
			m_ctrl->wait(this);
			pTag->ShowData();
			m_ctrl->done();
			//Thread::mSleep(1000);
		}
	}
};

class TimerHandle
{
public:
	TimerHandle(int i=0) :m_i(i) {}
	void handle() {
		printf("timeout:%d\n",m_i);
	}
private:
	int m_i;
};

struct A
{
	A():m_a(1),m_b(2){}
	int m_a;
	int m_b;
};

class DataBConnPool : public ObjPool<A>
{
public:
protected:
	bool InitObj(A** ppObj){
		*ppObj = new A;
		puts("DataBConnPool::Init");
		return true;
	}
};

#ifdef WIN32
#include "../../Framework/Thread/ThreadImpl_Win32.h"
#else
#include "../../Framework/Thread/ThreadImpl_Linux.h"
#endif
#if 0
class MyTest
{
public:
	MyTest(){
		for(int i=0;i<2;i++)
			m_th[i] = new ThreadImpl<MyTest>(this,&MyTest::run,reinterpret_cast<void*>(i));
		//m_main = new ThreadImpl<MyTest>(this,&MyTest::main);
		m_flag = true;
	}
	~MyTest(){
		
	}
	void start(){
		for(int i=0;i<2;i++)
			m_th[i]->start();
		//m_main->start();
	}
	void signal(){
		m_rwLock.LockWrite();
		m_sem[0].SemPost();
		m_sem[1].SemPost();
		m_flag = false;
		m_rwLock.UnlockWrite();
		puts("signal");
	}
	void main(){
		signal();
		while(1){
			ThreadImpl<MyTest>::mSleep(2000);
			//signal();
		}
	}
	void pause(){
		m_rwLock.LockWrite();
		m_flag = true;
		m_rwLock.UnlockWrite();		
	}
	void goon(){
		signal();
	}
	void run(void* p){
		union t{
			int v;
			void* vp;
		};
		t ti;
		ti.vp = p;
		int index = ti.v;
		printf("thread start:%d\n",index);
		while(1){
			m_rwLock.LockRead();
			if(m_flag){
				m_rwLock.UnlockRead();
				m_sem[index].SemWait();
				m_rwLock.LockRead();
			}
			printf("run=====:%d\n",index);
			m_rwLock.UnlockRead();
			Thread::mSleep(100);
		}
	}
private:
	Semaphore	m_sem[2];
	RWLock		m_rwLock;
	ThreadPri*	m_th[2];
	ThreadPri* 	m_main;
	bool m_flag;
};
#endif
int main()
{
#if 0	//TaskCtrl测试
	TaskCtrl ctrl(2);
	MyThread th0(&ctrl);
	MyThread th1(&ctrl);
	ctrl.AddThread(&th0);
	ctrl.AddThread(&th1);
	th0.start();
	th1.start();
	int i=0;
	while(1){
		//sleep(1);
		Thread::mSleep(1000);
		ctrl.SyncTaskOneTime();
		printf("sync by main thread:%d\n",i++);
	}
#endif

#if 0 //AtomMutex、Semaphore、SemaphoreEx测试
	int val = 0;
	TThread th1(&val);
	TThread th2(&val);
	th1.start();
	th2.start();
	Thread::mSleep(1000);
	int i = 0;
	while (1) {
		puts("main thread start");
		for (; i < 100000; i++) {
			//Increment(&val);
			//IncrementEx(&val);
			IncrementExx(&val);
		}
		printf("main:%d\n", i);
		break;
	}
	printf("%d\n",val);
#endif

#if 0	//线程池测试
	ThreadPool pool(3);
	TaskCtrl tCtrl;

	MyTask* pt1 = new MyTask("test");
	MyTask* pt2 = new MyTask("test");
	MyTask* pt3 = new MyTask("test");
	pt1->SetData(1);
	pt2->SetData(2);
	pt3->SetData(3);
	tCtrl.AddThread(pt1);
	tCtrl.AddThread(pt2);
	tCtrl.AddThread(pt3);
	MyTaskProcess* processor = new MyTaskProcess(&tCtrl);
	pool.Register_taskProcess("test", processor);//TODO:此处待确认内存管理方式
	if (!pool.init_pool()) {
		pool.submit_job_req(pt1);
		pool.submit_job_req(pt2);
		pool.submit_job_req(pt3);
	}
	while (1) {
		tCtrl.SyncTaskOneTime();
		Thread::mSleep(1000);
	}
	pool.stop_pool();
#endif

#if 0	// 定时器测试
	TimerMgr mgr;
	TimerHandle t;
	TimeSet test(2,1);
	Timer<TimerHandle> t1(&t,&TimerHandle::handle,test);
	t1.SetTimer(test);
#ifdef WIN32
	TimerHandle th(1);
	Timer<TimerHandle> t2(&th, &TimerHandle::handle,test);
	t2.SetTimer(test);
	int n2 = mgr.AddTimer("2",new Timer<TimerHandle>(&th,&TimerHandle::handle,test));
#endif
	int n = -1;
	
	n = mgr.AddTimer("1",new Timer<TimerHandle>(&t,&TimerHandle::handle,test));
	printf("timer handle:%d\n",n);
	int i = 0;
	while (1) {
		i++;
		// if (i == 5) {
			// mgr.DelTimer(n);
			// n = -1;
		// }
		// if (i == 10) {
			// n = mgr.AddTimer(&t1);
			// printf("timer handle:%d\n", n);
		// }
		Thread::mSleep(1000);
	}
#endif

#if 0		//对象池
	DataBConnPool pools;
	//ObjPool<A> pools;
	pools.Init();
	printf("total:%d\n",pools.Capacity());
	A* p = (A*) pools.Alloc();
	p->m_a = 2;
	p->m_b = 1;
	printf("%d,%d\n",p->m_a,p->m_b);
	
	char* pbuf = (char*)malloc(sizeof(A));
	A* pA = new (pbuf)A;
	printf("%d,%d\n",pA->m_a,pA->m_b);
#endif

#if 0		//内存池
	MemPool pool;
	const int ARRCOUNT = 2;
	pool.Init();
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	char* parr[ARRCOUNT] = {};
	for(int i=0;i<ARRCOUNT;i++)
		parr[i] = (char*)pool.Alloc(32);//Alloc
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	strcpy(parr[1],"helloworld");
	strcpy(parr[0], "god like!hello worldhello world");
	printf("%s\n",parr[1]);
	pool.Free(parr[1]);//Free
	puts("==========================");
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	puts("start realloc");
	char* p=(char*)pool.Realloc(parr[0], 64);//Realloc
	if (NULL != p) {
		strcat(p, "fuck you!");
		printf("realloc:%s\n", p);
	}
	else {
		puts("realloc failed");
	}
	printf("cap:%llu,use:%llu\n", pool.Capacity(), pool.GetTotalUse());
	while (1)
		Thread::mSleep(1000);
#endif

#if 0
	map<int, int, less<int>, XAllocator<int > > m;
	m.insert(make_pair(1,1));
	m.insert(make_pair(2, 2));
	m.insert(make_pair(3, 3));
	m.insert(make_pair(4, 4));
	m.insert(make_pair(5, 5));
	map<int, int, less<int>, XAllocator< int > >::iterator it = m.begin();
	for (; it!=m.end(); it++)
	{
		printf("%d\n",it->second);
	}

	//list<int, XAllocator<int> > l;
	//l.push_back(1);
	//l.push_back(2);
	//l.push_back(3);
	//l.push_back(4);
	//list<int, XAllocator<int> >::iterator it = l.begin();
	//for (; it != l.end(); it++)
	//	printf("%d\n",*it);
#endif

#if 0
	MyTest t;
	t.start();
	char c=0;
	while(1){
		c = getchar();
		if('p'==c){
			t.pause();
		}else if('g'==c){
			t.goon();
		}
		Thread::mSleep(1000);
	}
#endif

#if 1
	DBConnPool dbPools;
	dbPools.Init();
	dbPools.InitDBlib();
	DBParam tagDB("mssql");
	tagDB.m_strDBinfo = "mssql";
	//tagDB.param._adoParam.m_dbBase = "";
	//tagDB.param._adoParam.m_dbIp = "localhost";
	//tagDB.param._adoParam.m_dbName = "test";
	//tagDB.param._adoParam.m_user = "sa";
	//tagDB.param._adoParam.m_passwd = "zhubin123";
	
	AdoConn* pDBConn = (AdoConn*)(dbPools.GetNewConnect());
	pDBConn->InitConnecion(&tagDB);
	pDBConn->ConnectDB();
	AdoAccess* pDBAccess = (AdoAccess*)(pDBConn->GetDBAccessProxy());
	pDBAccess->SetConnect(pDBConn);
	pDBAccess->InitAccessParam(NULL);

#endif

#if 0
	DBConnPool dbPools;
	dbPools.Init();
	dbPools.InitDBlib();
	DBParam tagDB("mysql");
	tagDB.m_strDBinfo = "mssql";
	//tagDB.param._adoParam.m_dbBase = "";
	//tagDB.param._adoParam.m_dbIp = "localhost";
	//tagDB.param._adoParam.m_dbName = "test";
	//tagDB.param._adoParam.m_user = "sa";
	//tagDB.param._adoParam.m_passwd = "zhubin123";

	MysqlConn* pDBConn = (MysqlConn*)(dbPools.GetNewConnect());
	pDBConn->InitConnecion(&tagDB);
	pDBConn->ConnectDB();
	MysqlAccess* pDBAccess = (MysqlAccess*)(pDBConn->GetDBAccessProxy());
	pDBAccess->SetConnect(pDBConn);
	pDBAccess->InitAccessParam(NULL);

#endif

#ifdef WIN32
	system("pause");
#endif
	return 0;
}

