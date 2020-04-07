#ifndef __TComData_H__
#define __TComData_H__
#include <iostream>
#include "AbstractClass.h"

using namespace std;

class TComData : public CommonDataType
{
public:
	int		m_a;
	int		m_b;
	string	m_c;
};

#endif	// TComData.h
