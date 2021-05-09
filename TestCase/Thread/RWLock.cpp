#include <stdio.h>
#include "../../XPublic.h"

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
			Thread::mSleep(1000);
		}
	}
private:
	Semaphore	m_sem[2];
	RWLock		m_rwLock;
	ThreadPri*	m_th[2];
	ThreadPri* 	m_main;
	bool m_flag;
};

int main()
{
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
    return 0;
}
