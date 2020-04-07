#ifndef __stdafx_H__ 
#define __stdafx_H__


#ifdef FRAMEWORK_EXPORTS
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif


#endif	//stdafx.h

