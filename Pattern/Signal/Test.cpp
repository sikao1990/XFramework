#include <iostream>
#include <stdlib.h>
#include "XObject.h"
#include "MyObject.h"

using namespace std;
/*
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
	map<string,PFSlot>	m_slots;//如果以静态形式提供,则插入派生类成员函数指针时，需要强制类型转换
};
*/

int main()
{
	XObject obj1;
	XObject obj2;
	MyObject obj3;
	obj3.RegisterSlot("XObject_slot",&MyObject::slot_example);
	XObject::XConnect(&obj1,"XObject_signal",&obj2,"XObject_slot");
	XObject::XConnect(&obj1,"XObject_signal",&obj3,"XObject_slot");
	obj1.emit_signal("XObject_signal",1,2);
	XObject::XDisconnect(&obj1, "XObject_signal", &obj2, "XObject_slot");
	obj1.emit_signal("XObject_signal",1,2);
	return 0;
}
