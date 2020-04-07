#ifndef __MyObject_H__
#define __MyObject_H__
#include "XObject.h"
#include <stdarg.h>
#include <string>
#include <map>

using namespace std;

class MyObject : public XObject
{
	typedef void (MyObject::*PFSlot)(va_list);
public:	
	MyObject();
	virtual ~MyObject(){}
	virtual void slot_Interface(const char* strSlot,va_list ap);
	void slot_example(va_list ap);
public:
	virtual void RegisterSlot(const char* strSlot,PFSlot pfunc);
	virtual void UnRegisterSlot(const char* strSlot,PFSlot pfunc);	
private:
	//作为库使用时暂时不需要(需要删掉)
	map<string,PFSlot>		m_slots;//如果以静态形式提供,则插入派生类成员函数指针时，需要强制类型转换
};

#endif	//MyObject.h
