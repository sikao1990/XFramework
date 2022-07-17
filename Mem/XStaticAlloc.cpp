#include "XStaticAlloc.h"
#include <stdio.h>

XStaticAlloc::XStaticAlloc()
{

}

XStaticAlloc::~XStaticAlloc()
{

}

bool XStaticAlloc::Init(void* pStart,int nLen)
{
	m_pBegin = pStart;
	m_nTotalLen = nLen;

	
}

void* XStaticAlloc::Alloc(int len)
{
	return NULL;
}

void XStaticAlloc::Free(void* pStart)
{

}
