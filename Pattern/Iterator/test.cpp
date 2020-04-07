#include <iostream>
#include <stdio.h>

using namespace std;

#include "Iterator.h"
#include "playerMgr.h"
#include "SceneMgr.h"

int main()
{
	playerMgr pMgr;
	SceneMgr sMgr;
	pMgr.Init();
	sMgr.Init();
	Iterator* pItr = pMgr.CreateIterator();
	Iterator* sItr = sMgr.CreateIterator();
	int* pInt = NULL;
	for(pItr->First();pItr->IsDone();pItr->Next())
	{
		pInt = (int*)pItr->CurrentItem();
		cout << *pInt << ' ';
		pInt = NULL;
	}
	cout << endl;
	string* pstr = NULL;
	for(sItr->First();sItr->IsDone();sItr->Next())
	{
		pstr = (string*)sItr->CurrentItem();
		cout << (*pstr).c_str() << ' ';
		pstr = NULL;
	}
	pMgr.ReleaseIterator(pItr);
	sMgr.ReleaseIterator(sItr);
	return 0;
}
