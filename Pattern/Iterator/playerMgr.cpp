#include "playerMgr.h"

void playerMgr::Init()
{
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);
}

Iterator* playerMgr::CreateIterator()
{
	return new playerIter(this);
}

void* playerMgr::GetItem(int idx)
{
	return &(vec[idx]);
}

int playerMgr::GetSize()
{
	return vec.size();
}

void playerMgr::ReleaseIterator(Iterator* pIt)
{
	delete pIt;
}
	