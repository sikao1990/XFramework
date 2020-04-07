#ifndef __ThreadCtrlEx_H__
#define __ThreadCtrlEx_H__

class XMem;
class ThreadCtrlExImpl;
#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API ThreadCtrlEx
#else
class ThreadCtrlEx
#endif
{
public:
	ThreadCtrlEx(XMem* pMem=0);
	virtual ~ThreadCtrlEx();
	
	void Lock();
	void Unlock();
	void WaitProduce();
	void WaitConsume();
	void WakeProduce(bool bAll);
	void WakeConsume(bool bAll);
private:
	ThreadCtrlExImpl*	_Imp;
	XMem*				pAllocator;
};

#endif	// ThreadCtrlEx.h
