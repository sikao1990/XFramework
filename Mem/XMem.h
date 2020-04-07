#ifndef __XMem_H__
#define __XMem_H__
#include <stdio.h>

class XMem
{
public:
	virtual ~XMem(){}
	virtual void* Alloc(int len) = 0;
	virtual void* Realloc(void* pMemSrc,int newLen)=0;
	virtual void Free(void* p)=0;
};

#endif	// XMem.h

