#include "DyPluginImp_Win32.h"

DyPluginImp::DyPluginImp():m_hHandle(NULL)
{
	
}

bool DyPluginImp::OpenLib(const char* filePath)const
{
	DyPluginImp* pThis = const_cast<DyPluginImp* >(this);
	if(NULL==(pThis->m_hHandle = LoadLibrary(filePath)))
		return false;
	return true;
}

void* DyPluginImp::GetObjByLib(const char* strName)
{
	if(NULL==m_hHandle)
		return NULL;
	return GetProcAddress(m_hHandle,strName);
}

const void* DyPluginImp::GetObjByLib(const char* strName)const
{
	if(NULL==m_hHandle)
		return NULL;
	return GetProcAddress(m_hHandle,strName);
}

void DyPluginImp::CloseLib()const
{
	if(NULL!=m_hHandle){
		DyPluginImp* pThis = const_cast<DyPluginImp* >(this);
		FreeLibrary(m_hHandle);
		pThis->m_hHandle = NULL;
	}
}

DyPluginImp::DyPluginImp(const DyPluginImp& th)
{
	this->m_hHandle = NULL;
}

DyPluginImp& DyPluginImp::operator=(const DyPluginImp& th)
{
	if(NULL!=this->m_hHandle){
		FreeLibrary(m_hHandle);
		m_hHandle = NULL;
	}
	return *this;
}
