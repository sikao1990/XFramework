#include "DirPtrImpl_Linux.h"

DirPtrImpl::DirPtrImpl():m_dp(NULL),m_dirp(NULL),m_bFlag(false)
{
}

bool DirPtrImpl::Init(const char* path)const
{
	if(m_bFlag)return false;
	DirPtrImpl* pThis = const_cast<DirPtrImpl*>(this);
	if((pThis->m_dp = opendir(path)) == NULL)
		return false;
	if(NULL==(pThis->m_dirp = readdir(m_dp)))
		return false;
	return pThis->m_bFlag=true;
}

bool DirPtrImpl::End()const
{
	if(m_bFlag&&NULL!=m_dirp)
		return false;
	else{
		DirPtrImpl* pThis = const_cast<DirPtrImpl*>(this);
		closedir(m_dp);
		pThis->m_dp = NULL;
		return true;
	}
}

void DirPtrImpl::Next()const
{
	DirPtrImpl* pThis = const_cast<DirPtrImpl*>(this);
	if(NULL==(pThis->m_dirp = readdir(m_dp)) )
	{
		pThis->m_bFlag = false;
	}
}

const string DirPtrImpl::GetCurrent()const
{
	if(NULL==m_dirp)
		return "";
	return m_dirp->d_name;
}

int DirPtrImpl::GetType()const
{
	if(NULL==m_dirp)
		return -1;
	return m_dirp->d_type;
}

DirPtrImpl::DirPtrImpl(const DirPtrImpl& th)
{
	m_bFlag = false;
	m_dirp 	= NULL;
	m_dp	= NULL;
}

DirPtrImpl& DirPtrImpl::operator=(const DirPtrImpl& th)
{
	if(NULL!=m_dp){
		closedir(m_dp);
		m_dp 	= NULL;
		m_bFlag	= false;
		m_dirp 	= NULL;		
	}
	return *this;
}
