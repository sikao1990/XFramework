#ifndef __SemaphoreEx_H__
#define __SemaphoreEx_H__

class XMem;
class SemaphoreExImpl;

#include "../XDefine.h"
class FRAMEWORK_API SemaphoreEx
{
public:
	SemaphoreEx(int nInit=0,int nWait=1,int nPost=1,XMem* pMem=0);
	~SemaphoreEx();
	//If you specify parameters, the init parameter settings will be overwritten
	void SemSignal(int n = -1);
	//If you specify parameters, the init parameter settings will be overwritten
	void SemWait(int n = -1);
private:	
	SemaphoreEx(const SemaphoreEx&);
	SemaphoreEx& operator=(const SemaphoreEx&);
private:
	SemaphoreExImpl*		_Imp;
	XMem*			pAllocator;
};

#endif	//SemaphoreEx.h
