#include "SceneMgr.h"

void SceneMgr::Init()
{
	mapRes.insert(make_pair(1,"1"));
	mapRes.insert(make_pair(2,"2"));
	mapRes.insert(make_pair(3,"3"));
	mapRes.insert(make_pair(4,"4"));
}

Iterator* SceneMgr::CreateIterator()
{
	return new SceneIter(this);
}

void* SceneMgr::GetItem(int idx)
{
	map<int,string>::iterator it = mapRes.begin();
	if(idx >= mapRes.size())
		return NULL;
	advance(it,idx);//distance求距离
	return &(it->second);
}

void* SceneMgr::GetItem(void* pKey)
{
	int* pIntKey = (int*)pKey;
	if(mapRes.find(*pIntKey)==mapRes.end())
		return NULL;
	return &(mapRes.find(*pIntKey)->second);
}

int SceneMgr::GetSize()
{
	return mapRes.size();
}

void SceneMgr::ReleaseIterator(Iterator* pIt)
{
	delete pIt;
}
