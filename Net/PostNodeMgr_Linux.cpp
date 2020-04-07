#include "PostNodeMgr_Linux.h"

PostNodeMgr::PostNodeMgr()
{
	Init();
}

PostNodeMgr::~PostNodeMgr()
{
	
}

void* PostNodeMgr::ApplyPostNode()
{
	CPostReq* pData = (CPostReq*)Alloc();
	if(NULL!=pData)
		pData->Reset();
	return Alloc();	
}

void PostNodeMgr::ReleaseNode(void* pNode)
{
	CPostReq* pData = (CPostReq*)pNode;
	if (pData)pData->~CPostReq();
	Free(pNode);	
}
