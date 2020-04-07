#ifndef __DyPluginImp_H__
#define __DyPluginImp_H__
#include <windows.h>
#include <winbase.h>

class DyPluginImp
{
public:
	DyPluginImp();
	bool OpenLib(const char* filePath)const;
	void* GetObjByLib(const char* strName);
	const void* GetObjByLib(const char* strName)const;
	void CloseLib()const;
	DyPluginImp(const DyPluginImp& th);
	DyPluginImp& operator=(const DyPluginImp& th);	
private:
	HINSTANCE 	m_hHandle;
};

#endif	// DyPluginImp.h
