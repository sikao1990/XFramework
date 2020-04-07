#ifndef __SceneMgr_H__
#define __SceneMgr_H__
#include <map>
#include <iostream>
#include "Iterator.h"
#include "Aggregate.h"

using namespace std;

class SceneMgr : public Aggregate
{
	class SceneIter : public Iterator
	{
	public:
		SceneIter(SceneMgr* pMgr):m_pMgr(pMgr){}
		void First(){
			itr = m_pMgr->mapRes.begin();
		}
		void Next(){
			++itr;
		}
		bool IsDone(){
			return itr!=m_pMgr->mapRes.end();
		}
		void* CurrentItem(){
			return &(itr->second);
		}
	private:
		map<int,string>::iterator itr;
		SceneMgr*	m_pMgr;
	};
public:
	void Init();
	virtual Iterator* CreateIterator();
	virtual void* GetItem(int idx);
	virtual void* GetItem(void* pKey);
	virtual int GetSize() ;
	virtual void ReleaseIterator(Iterator* pIt);
private:
	friend class SceneIter;
	map<int,string>		mapRes;
};

#endif	// SceneMgr.h
