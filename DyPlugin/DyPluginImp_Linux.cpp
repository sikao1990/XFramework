#include "DyPluginImp_Linux.h"
#include <dlfcn.h>
#include <stdio.h>

DyPluginImp::DyPluginImp():m_pHandle(NULL)
{
	
}

bool DyPluginImp::OpenLib(const char* filePath)const
{
	DyPluginImp* pThis = const_cast<DyPluginImp* >(this);
	if(NULL==(pThis->m_pHandle = dlopen(filePath,RTLD_NOW)))
		return false;
	return true;
}

void* DyPluginImp::GetObjByLib(const char* strName)
{
	if(NULL==m_pHandle)
		return NULL;
	return dlsym(m_pHandle,strName);
}

const void* DyPluginImp::GetObjByLib(const char* strName)const
{
	if(NULL==m_pHandle)
		return NULL;
	return dlsym(m_pHandle,strName);
}

void DyPluginImp::CloseLib()const
{
	DyPluginImp* pThis = const_cast<DyPluginImp* >(this);
	dlclose(m_pHandle);
	pThis->m_pHandle = NULL;
}

DyPluginImp::DyPluginImp(const DyPluginImp& th)
{
	this->m_pHandle = NULL;
}

DyPluginImp& DyPluginImp::operator=(const DyPluginImp& th)
{
	if(NULL!=this->m_pHandle){
		dlclose(m_pHandle);
		this->m_pHandle = NULL;
	}
	return *this;
}

	