#ifndef __DyPluginImpl_H__
#define __DyPluginImpl_H__
#include <windows.h>
#include <winbase.h>

class DyPluginImpl
{
public:
	DyPluginImpl();
	bool OpenLib(const char* filePath)const;
	void* GetObjByLib(const char* strName);
	const void* GetObjByLib(const char* strName)const;
	void CloseLib()const;
	DyPluginImpl(const DyPluginImpl& th);
	DyPluginImpl& operator=(const DyPluginImpl& th);	
private:
	HINSTANCE 	m_hHandle;
};

#endif	// DyPluginImp.h
