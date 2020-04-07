#include "DirPtr.h"
#include <string.h>

#include "../Mem/XMem.h"
#ifdef WIN32
#include "DirPtrImpl_Win32.h"
#else
#include "DirPtrImpl_Linux.h"
#endif

DirPtr::DirPtr(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(DirPtrImpl*)pMem->Alloc(sizeof(DirPtrImpl))))
		new (_Imp) DirPtrImpl;
	else
		_Imp = new DirPtrImpl;		
	InitFiler();
}

DirPtr::~DirPtr()
{
	if (NULL != pAllocator){
		_Imp->~DirPtrImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;		
}

bool DirPtr::Init(const char* path)const
{
	return _Imp->Init(path);
}

bool DirPtr::End()const
{
	return _Imp->End();
}

const string DirPtr::operator*()const
{
	while (!_Imp->End()) {
		if (IsNot(_Imp->GetCurrent(), _Imp->GetType())) {
			_Imp->Next();
			continue;
		}

		if (!m_findVec.empty() && !IsFind(_Imp->GetCurrent(), _Imp->GetType())) {
			_Imp->Next();
			continue;
		}
		break;
	}
	return _Imp->GetCurrent();
}

DirPtr& DirPtr::operator++()const
{
	do {
		_Imp->Next();
		if (IsNot(_Imp->GetCurrent(), _Imp->GetType()))
			continue;
	
		if (!m_findVec.empty() && !IsFind(_Imp->GetCurrent(), _Imp->GetType()))
			continue;
		break;
	} while (!_Imp->End());
	return const_cast<DirPtr&>(*this);
}

const DirPtr DirPtr::operator++(int)const
{
	do {
		_Imp->Next();
		if (IsNot(_Imp->GetCurrent(), _Imp->GetType()))
			continue;

		if (!m_findVec.empty() && !IsFind(_Imp->GetCurrent(), _Imp->GetType()))
			continue;
		break;
	} while (!_Imp->End());
	return *this;
}

void DirPtr::InitFiler()
{
	AddFilter(&DirPtr::IsCurOrParentDir);
}

void DirPtr::AddFind(Pfunc f)
{
	m_findVec.push_back(f);
}

void DirPtr::AddFilter(Pfunc f)
{
	m_filter.push_back(f);
}
	
bool DirPtr::IsFind(const string& filter,int type)const
{
	if(m_findVec.empty())
		return true;
	for(int i=0;i<m_findVec.size();i++)
	{
		if( (this->*m_findVec[i])(filter,type) )
			return true;
	}
	return false;
}

bool DirPtr::IsNot(const string& filter,int type)const
{
	for(int i=0;i<m_filter.size();i++)
	{
		if( (this->*m_filter[i])(filter,type) )
			return true;
	}
	return false;
}

bool DirPtr::IsCurOrParentDir(const string& filter,int type)const
{
	if(!strcmp(filter.c_str(),".")||NULL!=strstr(filter.c_str(),"..") )
		return true;
	return false;
}
	