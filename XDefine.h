#ifndef __CfgDefine_H__
#define __CfgDefine_H__

#ifdef WIN32

#ifdef FRAMEWORK_EXPORTS
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif

#include <windows.h>

typedef HANDLE  hHandle;

#else
	
#define FRAMEWORK_API
typedef int hHandle;

#endif

typedef void (*pProceeExitHandle)(void*);

#endif
