#include <stdio.h>
#include "../../XPublic.h"

Semaphore slock(1);
void IncrementEx(int* p)
{
	slock.SemWait();
	*p += 1;
	slock.SemPost();
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
			IncrementEx(pVal);
			if (++i > 100000)
				break;
		}
		printf("thread end:%d\n",i);
	}
private:
	int* pVal;
};
int main()
{
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
			IncrementEx(&val);
		}
		printf("main:%d\n", i);
		break;
	}
	printf("%d\n",val);
    return 0;
}