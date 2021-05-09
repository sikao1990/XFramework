#ifndef __SystemApi_H__
#define __SystemApi_H__
#include <stdio.h>

class SystemApi
{
public:
	static unsigned GetProcessID(const char* CmdName);
    static int StartDaemon(const char* workDir = NULL ); 
};

#endif	//SystemApi.h
