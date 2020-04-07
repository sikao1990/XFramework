#ifndef __TMethodClass_H__
#define __TMethodClass_H__
#include "AbstractClass.h"
#include <map>
#include <stdio.h>
#include "TComData.h"

using namespace std;

class TMethodClass : public AbstractClass
{
	typedef void (TMethodClass::*pMemFunc)(int msg);
public:
	TMethodClass(CommonDataType& data);
	virtual void Method(int msg);
	virtual bool Init();
public:
	void ExampleFunction(int msg);
private:
	map<int,pMemFunc >	m_methods;
};

#endif	// TMethodClass.h
