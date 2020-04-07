#ifndef __playerMgr_H__
#define __playerMgr_H__
#include <vector>
#include "Iterator.h"
#include "Aggregate.h"

using namespace std;

class playerMgr : public Aggregate
{
	class playerIter : public Iterator
	{
	public:
		playerIter(playerMgr* pMgr):m_pMgr(pMgr){}
		void First(){
			itr = m_pMgr->vec.begin();
		}
		void Next(){
			++itr;
		}
		bool IsDone(){
			return itr!=m_pMgr->vec.end();
		}
		void* CurrentItem(){
			return &(*itr);
		}
	private:
		vector<int>::iterator itr;
		playerMgr*	m_pMgr;
	};
public:
	void Init();
	virtual Iterator* CreateIterator();
	virtual void* GetItem(int idx);
	virtual int GetSize() ;
	virtual void ReleaseIterator(Iterator* pIt);
private:
	friend class playerIter;
	vector<int>		vec;
};

#endif	// playerMgr.h
