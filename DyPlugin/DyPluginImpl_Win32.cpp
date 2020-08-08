#include "DyPluginImpl_Win32.h"

DyPluginImpl::DyPluginImpl():m_hHandle(NULL)
{
	
}

bool DyPluginImpl::OpenLib(const char* filePath)const
{
	DyPluginImpl* pThis = const_cast<DyPluginImpl* >(this);
	if(NULL==(pThis->m_hHandle = LoadLibrary(filePath)))
		return false;
	return true;
}

void* DyPluginImpl::GetObjByLib(const char* strName)
{
	if(NULL==m_hHandle)
		return NULL;
	return GetProcAddress(m_hHandle,strName);
}

const void* DyPluginImpl::GetObjByLib(const char* strName)const
{
	if(NULL==m_hHandle)
		return NULL;
	return GetProcAddress(m_hHandle,strName);
}

void DyPluginImpl::CloseLib()const
{
	if(NULL!=m_hHandle){
		DyPluginImpl* pThis = const_cast<DyPluginImpl* >(this);
		FreeLibrary(m_hHandle);
		pThis->m_hHandle = NULL;
	}
}

DyPluginImpl::DyPluginImpl(const DyPluginImpl& th)
{
	this->m_hHandle = NULL;
}

DyPluginImpl& DyPluginImpl::operator=(const DyPluginImpl& th)
{
	if(NULL!=this->m_hHandle){
		FreeLibrary(m_hHandle);
		m_hHandle = NULL;
	}
	return *this;
}
