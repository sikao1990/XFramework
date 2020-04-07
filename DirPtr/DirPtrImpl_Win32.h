#ifndef __DirPtrImpl_H__
#define __DirPtrImpl_H__
#include <windows.h>
#include <iostream>

using namespace std;

class DirPtrImpl
{
public:
	DirPtrImpl();
	bool Init(const char* path)const;
	bool End()const;
	void Next()const;
	const string GetCurrent()const;
	int GetType()const;
	DirPtrImpl(const DirPtrImpl& th);
	DirPtrImpl& operator=(const DirPtrImpl& th);	
private:
	bool 				m_bflag;	
	HANDLE 				m_hFile;
	WIN32_FIND_DATA 	m_fileInfo;
};

#endif	//DirPtrImpl.h
