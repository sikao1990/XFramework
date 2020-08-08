#ifndef __SystemApi_H__
#define __SystemApi_H__

#include "../XDefine.h"

class FRAMEWORK_API SystemApi
{
public:
	static unsigned GetProcessID(const char* CmdName);
};

#endif	//SystemApi.h
