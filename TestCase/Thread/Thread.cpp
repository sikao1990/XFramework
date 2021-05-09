#include <stdio.h>
#include "../../XPublic.h"

class MyThread : public Thread
{
public:
    void run(){
        unsigned tid = GetThreadID();
        printf("CurThreadID %d\n",tid);
        while(1){
            puts("MyThread");
            Thread::mSleep(1000);
        }
    }
};

int main()
{
    puts("test");
    MyThread thread;
    thread.start();
    while(1)
        Thread::mSleep(1000);
    return 0;
}