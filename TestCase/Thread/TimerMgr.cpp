#include <stdio.h>
#include "../../XPublic.h"

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

int main()
{
	TimerMgr mgr;
	TimerHandle t;
	TimeSet test(2000,1000);
	Timer<TimerHandle> t1(&t,&TimerHandle::handle,test);
	t1.SetTimer(test);
#ifdef WIN32
	TimerHandle th(1);//TODO:Linux版本实现方式更改
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
    return 0;
}