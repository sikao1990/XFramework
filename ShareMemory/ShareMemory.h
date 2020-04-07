#ifndef __ShareMemory_H__
#define __ShareMemory_H__

class XMem;
class ShareMemoryImpl;
#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API ShareMemory
#else
class ShareMemory
#endif
{
public:
	ShareMemory(XMem* pMem=0);
	~ShareMemory();
	bool Open(const char* str,int size)const;
	bool Attach()const;
	void* GetAddress();
	const void* GetAddress()const;
	void Detach()const;
	void Close()const;
private:
	ShareMemory(const ShareMemory&);
	ShareMemory& operator=(const ShareMemory&);
private:
	ShareMemoryImpl*	_Imp;
	XMem*				pAllocator;
};

#endif	// ShareMemory.h
