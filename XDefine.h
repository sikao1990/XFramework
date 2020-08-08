#ifndef __CfgDefine_H__
#define __CfgDefine_H__

#ifdef WIN32

#ifdef FRAMEWORK_EXPORTS
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif

#else
	
#define FRAMEWORK_API

#endif

#endif
