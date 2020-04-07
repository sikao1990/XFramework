#include "DyPlugin.h"
#include <new>
#include "../Mem/XMem.h"

#ifdef	WIN32
#include "DyPluginImp_WIN32.h"
#else
#include "DyPluginImp_Linux.h"
#endif

DyPlugin::DyPlugin(XMem* pMem):pAllocator(pMem)
{
	if(NULL!=pAllocator && NULL!=(_Imp=(DyPluginImp*)pMem->Alloc(sizeof(DyPluginImp))) )
		new ( _Imp ) DyPluginImp;
	else
		_Imp = new DyPluginImp;
}

DyPlugin::~DyPlugin()
{
	if (NULL != pAllocator){
		_Imp->~DyPluginImp();
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

	