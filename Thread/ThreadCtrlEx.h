#ifndef __ThreadCtrlEx_H__
#define __ThreadCtrlEx_H__

class XMem;
class ThreadCtrlExImpl;

#include "../XDefine.h"
class FRAMEWORK_API ThreadCtrlEx
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
