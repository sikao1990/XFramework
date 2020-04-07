#ifndef __MutexIntegerImpl_H__
#define __MutexIntegerImpl_H__


class MutexIntegerImpl
{
public:
	MutexIntegerImpl();
	void SetVal(const int n);
	int getVal();
	bool IsVal(const int n);	
private:
	volatile long	m_val;
};

#endif	// MutexIntegerImpl.h
