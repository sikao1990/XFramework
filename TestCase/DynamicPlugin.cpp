#include <stdio.h>
#include "../XPublic.h"

#ifdef WIN32
#define TESTPLUGIN "./test.dll"
#elif __linux__
#define TESTPLUGIN "./test.so"
#elif __APPLE__ || __MACH__
#define TESTPLUGIN "./test.dylib"
#endif
typedef void (*pFunc)();
int main()
{
    DyPlugin plugin;
    if(!plugin.OpenLib(TESTPLUGIN))
    {
        puts("Open Lib failed!");
        return -1;
    }
    pFunc f = (pFunc)plugin.GetObjByLib("testFunc");
    f();
    plugin.CloseLib();
    return 0;
}
