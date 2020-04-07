#include "MyObject.h"
#include <typeinfo>
#include <stdio.h>

MyObject::MyObject()
{

}

void MyObject::slot_Interface(const char* strSlot,va_list ap)
{
	//派生类中实现,则可以调用
	puts(strSlot);
	if(m_slots.end()!=m_slots.find(strSlot))
		(this->*m_slots.find(strSlot)->second)(ap);
}

void MyObject::slot_example(va_list ap)
{
	//槽函数,派生类实现
	int x = va_arg(ap,int);
	int y = va_arg(ap,int);
	printf("slot_example:%d,%d\n",x,y);
}

void MyObject::RegisterSlot(const char* strSlot,PFSlot pfunc)
{
	m_slots.insert(make_pair(strSlot,pfunc));
}

void MyObject::UnRegisterSlot(const char* strSlot,PFSlot pfunc)
{
	if(m_slots.end()!=m_slots.find(strSlot))
		m_slots.erase(m_slots.find(strSlot));
}
