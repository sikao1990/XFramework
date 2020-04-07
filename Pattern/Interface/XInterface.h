#ifndef __XInterface_H__
#define __XInterface_H__
#include <stdarg.h>

//此模式实现修改封闭,扩展开放

typedef enum emFuncType
{
	emFunObj,
	emGloablF,
}emFuncType;
class XInterface{
public:
	virtual ~XInterface();
	virtual void func(emFuncType type,void* fun,...);
};

#endif	//XInterface.h
