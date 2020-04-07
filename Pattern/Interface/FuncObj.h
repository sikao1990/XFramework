#ifndef __FuncObj_H__
#define __FuncObj_H__
#include <stdarg.h>

class XInterface;
typedef void (*funcptr)(XInterface* pObj,va_list ap);

class FuncObj{
public:
	virtual ~FuncObj();
	virtual void operator()(XInterface* pObj,va_list ap);
};

//example function
extern void globelFunc(XInterface* pObj,va_list ap);

#endif	//FuncObj.h
