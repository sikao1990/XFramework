#include <stdio.h>
#include "../XPublic.h"

int main()
{
    SharedMemory sharedMem;
    void* pMemStart = NULL;
    if(!sharedMem.Open("test",100)){
        puts("Open SharedMemory failed!");
        return -1;
    }
    pMemStart = sharedMem.GetAddress();
    for(int i=0;i<100;i++)
    {
        Thread::mSleep(1000);
        puts((const char*)pMemStart);
    }
    sharedMem.Close();
    return 0;
}