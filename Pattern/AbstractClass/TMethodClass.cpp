#include "TMethodClass.h"

TMethodClass::TMethodClass(CommonDataType& data):AbstractClass(data)
{
}

void TMethodClass::Method(int msg)
{
	if(m_methods.end()!=m_methods.find(msg)){
		pMemFunc pFunc = m_methods.find(msg)->second;
		return (this->*pFunc)(msg);
	}
}

bool TMethodClass::Init()
{
	m_methods.insert(make_pair(2,&TMethodClass::ExampleFunction));//msgid<->method
	//TODO:
	return true;
}


void TMethodClass::ExampleFunction(int msg)
{
	TComData& comdata = dynamic_cast<TComData&>(m_comData);
	comdata.m_a = 1;
	comdata.m_b = 2;
	comdata.m_c = "hello";
	puts("ExampleFunction");
}
