#include <stdio.h>
#include "../XPublic.h"
#include <string>

using namespace std;

void NewProcessEndCallback(void* p)
{
    puts("The New Process End");
}

int main()
{
    string curWorkDir = Dir::GetCurWorkDir();
    Process process(NewProcessEndCallback);
    if(-1==process.StartNewProcess("C:/Windows/System32/cmd.exe",NULL))
    {
        puts("exec new Process was failed!");
        return -1;
    }
    while(1)
        Thread::mSleep(1000);
    puts("test");
    return 0;
}