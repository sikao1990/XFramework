#include "FuncObj.h"
#include <iostream>
#include "XInterface.h"

using namespace std;

FuncObj::~FuncObj()
{
}

void FuncObj::operator()(XInterface* pObj,va_list ap)
{
	cout << "funcObj::operator()" << endl;
}

void globelFunc(XInterface* pObj,va_list ap)
{
	cout << "globelFunc()" << endl;
}
