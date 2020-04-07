#ifndef __AbstractClass_H__
#define __AbstractClass_H__
#include <list>
using namespace std;

//该模式解决消息处理类的扩展与工程管理,及复用
//优点:提高复用率、提高提维护性、提高扩展性、降低内存碎片
//缺点:不能以库的形式提供
//示例:PlayerSession类、Player类

class CommonDataType
{
public:
	virtual ~CommonDataType(){}
	//派生类->对公共数据加锁/解锁操作
};

class AbstractClass
{
public:
	AbstractClass(CommonDataType& data):m_comData(data){}
	virtual ~AbstractClass(){}
	virtual bool Init(){ return true;}
	virtual void Method(int msg){}
//private:
public:
	CommonDataType&		m_comData;
};

template < typename T >
class ClassProxy
{
public:
	virtual ~ClassProxy(){}
	virtual bool Init(){//结合装饰模式,每当有新需求增加时，则使用装饰模式
		// AbstractClass* pNewMethod = new MethodClass(m_comData);//或读取配置文件
		// pNewMethod->Init();
		// m_methods.push_back(pNewMethod);
		//m_methods.push_back(new MethodClass(m_comData));
		//...push_back()
		return true;
	}
	void InterfaceMethod(int msg)//适应0类似x100 - 0x200 协议处理, 0x200 - 0x300 协议处理
	{
		for( list<AbstractClass* >::iterator it = m_methods.begin();it!=m_methods.end();it++)
			(*it)->Method(msg);
	}
//private:
public:
	T						m_comData;
	list<AbstractClass* >	m_methods;
};

#endif	//AbstractClass.h
