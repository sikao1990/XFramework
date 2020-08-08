#include "DyPlugin.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "DyPluginImpl_Win32.h"
#else
#include "DyPluginImpl_Linux.h"
#endif

DyPlugin::DyPlugin(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(DyPluginImpl*)pMem->Alloc(sizeof(DyPluginImpl))) )
		new ( _Imp ) DyPluginImpl;
	else
		_Imp = new DyPluginImpl;
}

DyPlugin::~DyPlugin()
{
	if (NULL != pAllocator){
		_Imp->~DyPluginImpl();
		pAllocator->Free(_Imp);
	}
	else
		delete _Imp;		
}

bool DyPlugin::OpenLib(const char* filePath)const
{
	return _Imp->OpenLib(filePath);
}

void* DyPlugin::GetObjByLib(const char* strName)
{
	return _Imp->GetObjByLib(strName);
}

const void* DyPlugin::GetObjByLib(const char* strName)const
{
	return _Imp->GetObjByLib(strName);
}

void DyPlugin::CloseLib()const
{
	_Imp->CloseLib();
}

	