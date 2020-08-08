#include "DyPluginImpl_Linux.h"
#include <dlfcn.h>
#include <stdio.h>

DyPluginImpl::DyPluginImpl():m_pHandle(NULL)
{
	
}

bool DyPluginImpl::OpenLib(const char* filePath)const
{
	DyPluginImpl* pThis = const_cast<DyPluginImpl* >(this);
	if(NULL==(pThis->m_pHandle = dlopen(filePath,RTLD_NOW)))
		return false;
	return true;
}

void* DyPluginImpl::GetObjByLib(const char* strName)
{
	if(NULL==m_pHandle)
		return NULL;
	return dlsym(m_pHandle,strName);
}

const void* DyPluginImpl::GetObjByLib(const char* strName)const
{
	if(NULL==m_pHandle)
		return NULL;
	return dlsym(m_pHandle,strName);
}

void DyPluginImpl::CloseLib()const
{
	DyPluginImpl* pThis = const_cast<DyPluginImpl* >(this);
	dlclose(m_pHandle);
	pThis->m_pHandle = NULL;
}

DyPluginImpl::DyPluginImpl(const DyPluginImpl& th)
{
	this->m_pHandle = NULL;
}

DyPluginImpl& DyPluginImpl::operator=(const DyPluginImpl& th)
{
	if(NULL!=this->m_pHandle){
		dlclose(m_pHandle);
		this->m_pHandle = NULL;
	}
	return *this;
}

	