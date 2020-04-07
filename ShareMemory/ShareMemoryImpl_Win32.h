#ifndef __ShareMemoryImpl_H__
#define __ShareMemoryImpl_H__
#include <windows.h>

class ShareMemoryImpl
{
public:
	ShareMemoryImpl();
	bool Open(const char* str,int size)const;
	bool Attach()const;
	void* GetAddress();
	const void* GetAddress()const;
	void Detach()const;
	void Close()const;
private:
	ShareMemoryImpl(const ShareMemoryImpl&);
	ShareMemoryImpl& operator=(const ShareMemoryImpl&);		
private:
	HANDLE 	m_hMap;
	void*	m_pStart;
};

#endif	// ShareMemoryImpl.h
