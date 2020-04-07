#ifndef __MutexInterger_H__
#define __MutexInterger_H__

#include <stdio.h>

class XMem;
class MutexIntegerImpl;
#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API MutexInterger
#else
class MutexInterger
#endif
{
public:
	MutexInterger(XMem* pMem=NULL);
	MutexInterger(int i,XMem* pMem = NULL);
	~MutexInterger();
	void SetVal(const int n);
	int getVal()const;
	bool IsVal(const int n)const;
private:
	MutexIntegerImpl* _Imp;
	XMem*				pAllocator;
};

#endif	// MutexInterger.h
