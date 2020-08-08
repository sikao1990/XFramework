#ifndef __DirPtr_H__
#define __DirPtr_H__
#include <iostream>
#include <vector>
#include "../Mem/XAllocator.h"

using namespace std;
class XMem;
class DirPtrImpl;
#include "../XDefine.h"
class FRAMEWORK_API DirPtr
{
protected:
	typedef bool (DirPtr::*Pfunc)(const string& filter,int type)const;
public:
	DirPtr(XMem* pMem=NULL);
	virtual ~DirPtr();
	bool Init(const char* path)const;
	bool End()const;
	const string operator*()const;
	DirPtr& operator++()const;
	const DirPtr operator++(int)const;
	virtual void InitFiler();
private:
	bool IsCurOrParentDir(const string& filter,int type)const;
	//fileInfo.attrib&_A_SUBDIR ? "[folder]" : "[file]"
	//DT_REG == d_type
protected:
	void AddFind(Pfunc f);
	void AddFilter(Pfunc f);
private:
	bool IsFind(const string& filter,int type)const;
	bool IsNot(const string& filter,int type)const;	
private:
	DirPtrImpl*		_Imp;
	XMem*			pAllocator;
	vector<Pfunc>	m_findVec;
	vector<Pfunc>	m_filter;
};

#endif	//DirPtr.h
