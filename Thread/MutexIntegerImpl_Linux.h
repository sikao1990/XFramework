#ifndef __MutexIntegerImpl_H__
#define __MutexIntegerImpl_H__
#include "AtomMutexImpl_Linux.h"

class MutexIntegerImpl
{
public:
	MutexIntegerImpl();
	void SetVal(const int n);
	int getVal();
	bool IsVal(const int n);	
private:
	int				m_val;
	AtomMutexImpl	m_lock;
};

#endif	// MutexIntegerImpl.h
