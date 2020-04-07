#ifndef __DyPlugin_H__
#define __DyPlugin_H__

class XMem;
class DyPluginImp;
#ifdef	WIN32
#include "../stdafx.h"
class FRAMEWORK_API DyPlugin
#else
class DyPlugin
#endif
{
public:
	DyPlugin(XMem* pMem=0);
	~DyPlugin();
	bool OpenLib(const char* filePath)const;
	void* GetObjByLib(const char* strName);
	const void* GetObjByLib(const char* strName)const;
	void CloseLib()const;
private:
	DyPluginImp*		_Imp;
	XMem*			pAllocator;
};

#endif // plugin.h

