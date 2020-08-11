#include "DirPtrImpl_Win32.h"
#include <stdio.h>//test

DirPtrImpl::DirPtrImpl():m_bflag(false),m_hFile(INVALID_HANDLE_VALUE)
{
}

bool DirPtrImpl::Init(const char* path)const
{
	char szbuf[MAX_PATH] = {};
	sprintf(szbuf,"%s/*", path);
	DirPtrImpl* pThis = const_cast<DirPtrImpl*>(this);
	if (m_bflag)return false;
	if (INVALID_HANDLE_VALUE == (pThis->m_hFile = FindFirstFile(szbuf,&pThis->m_fileInfo)))
		return false;
	return pThis->m_bflag = true;
}

bool DirPtrImpl::End()const
{
	if(m_bflag){
		return false;
	}else{
		if (INVALID_HANDLE_VALUE != m_hFile) {
			FindClose(m_hFile);
			DirPtrImpl* pThis = const_cast<DirPtrImpl*>(this);
			pThis->m_hFile = INVALID_HANDLE_VALUE;
		}
		return true;
	}
}

void DirPtrImpl::Next()const
{
	if(m_bflag && INVALID_HANDLE_VALUE!=m_hFile)
	{
		DirPtrImpl* pThis = const_cast<DirPtrImpl*>(this);
		if(!FindNextFile(m_hFile, &(pThis->m_fileInfo))){
			pThis->m_bflag = false;
		}
	}
}

const string DirPtrImpl::GetCurrent()const
{
	if (!m_bflag)
		return "";
	return m_fileInfo.cFileName;
}

int DirPtrImpl::GetType()const
{
	if (!m_bflag)
		return -1;
	return m_fileInfo.dwFileAttributes;
}

DirPtrImpl::DirPtrImpl(const DirPtrImpl& th)
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_bflag = false;
}

DirPtrImpl& DirPtrImpl::operator=(const DirPtrImpl& th)
{
	if(INVALID_HANDLE_VALUE!=m_hFile){
		FindClose(m_hFile);
		m_bflag = false;
	}
	return *this;
}

