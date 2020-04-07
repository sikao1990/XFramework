#ifndef __AtomMutexEx_H__
#define __AtomMutexEx_H__

#include <windows.h>
#include "../stdafx.h"

class FRAMEWORK_API AtomMutexEx
{
public:
	AtomMutexEx();
	~AtomMutexEx();
	void Lock();
	void UnLock();
private:
	CRITICAL_SECTION m_lock;
};

#endif	//	AtomMutexEx.h
