#include <stdio.h>
#include "../../XPublic.h"

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
			m_ctrl->wait(this);//-1
			printf("run,pid:%u obj:%p\n",GetThreadID(),this);
			//Thread::mSleep(1000);
			m_ctrl->done();//+1
		}
	}
private:
	bool m_bflag;
	TaskCtrl*	m_ctrl;
};
int main()
{
    srand(time(0));
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
		Thread::mSleep(1000*(rand()%3+1));
		ctrl.SyncTaskOneTime();//+n -m
		printf("sync by main thread:%d\n",i++);
	}
    return 0;
}

