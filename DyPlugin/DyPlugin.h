#ifndef __DyPlugin_H__
#define __DyPlugin_H__

class XMem;
class DyPluginImpl;
#include "../XDefine.h"
class FRAMEWORK_API DyPlugin
{
public:
	DyPlugin(XMem* pMem=0);
	~DyPlugin();
	bool OpenLib(const char* filePath)const;
	void* GetObjByLib(const char* strName);
	const void* GetObjByLib(const char* strName)const;
	void CloseLib()const;
private:
	DyPluginImpl*		_Imp;
	XMem*			pAllocator;
};

#endif // plugin.h

