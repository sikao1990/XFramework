#ifndef __SharedMemory_H__
#define __SharedMemory_H__
#include <stdio.h>

class XMem;
class SharedMemoryImpl;

#include "../XDefine.h"
class FRAMEWORK_API SharedMemory
{
public:
	SharedMemory(XMem* pMem=NULL);
	~SharedMemory();
	bool Open(const char* str,int size)const;
	void* GetAddress();
	const void* GetAddress()const;
	void Close()const;
private:
	SharedMemory(const SharedMemory&);
	SharedMemory& operator=(const SharedMemory&);
private:
	SharedMemoryImpl*	_Imp;
	XMem*				pAllocator;
};

#endif	// SharedMemory.h
