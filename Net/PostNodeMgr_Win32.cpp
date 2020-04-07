#include "PostNodeMgr_Win32.h"
#include "PerIOData.h"

PostNodeMgr::PostNodeMgr()
{
	Init();
}

PostNodeMgr::~PostNodeMgr()
{
	
}

void* PostNodeMgr::ApplyPostNode()
{
	PerIOData* pData = (PerIOData*)Alloc();
	if(NULL!=pData)
		pData->Reset();
	return Alloc();
}

void PostNodeMgr::ReleaseNode(void* pNode)
{
	PerIOData* pData = (PerIOData*)pNode;
	if (pData)pData->~PerIOData();
	Free(pNode);
}
