#ifndef __PostNodeMgr_H__
#define __PostNodeMgr_H__

#include "../Mem/ObjPool.h"
#include "PerIOData.h"

class PostNodeMgr :public ObjPool<PerIOData>
{
public:
	PostNodeMgr();
	virtual ~PostNodeMgr();
	virtual void* ApplyPostNode();
	virtual void ReleaseNode(void* pNode);
private:
};

#endif	// PostNodeMgr.h
