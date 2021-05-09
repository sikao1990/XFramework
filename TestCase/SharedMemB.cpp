#include <stdio.h>
#include "../XPublic.h"

#define ALLOCSIZE (100)

int main()
{
    SharedMemory sharedMem;
    void* pMemStart = NULL;
    if(!sharedMem.Open("test",ALLOCSIZE)){
        puts("Open SharedMemory failed!");
        return -1;
    }
    pMemStart = sharedMem.GetAddress();
    memset(pMemStart,0,ALLOCSIZE);
    for(int i=0;i<100;i++)
    {
        sprintf((char*)pMemStart,"SharedMemory test%d",i);
        Thread::mSleep(1000);
    }
    sharedMem.Close();
    return 0;
}