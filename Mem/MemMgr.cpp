#include "MemMgr.h"
#include <stdlib.h>
#include <new>

MemMgr::MemMgr():pEleHead(NULL),nUnit(0){
}

MemMgr::~MemMgr(){
	EleNode* pBegin = pEleHead;
	EleNode* pBak = NULL;
	for(;NULL!=pBegin;pBegin = pBak)
	{
		if(NULL==(pBak=pBegin->pNext))
			break;
		destoryImpl(pBegin);
	}
}

bool MemMgr::Init(int blockSize){
	nUnit = blockSize;
	char* p = (char*)AllocImpl(SYSMEMPAGE);
	Expend(p,SYSMEMPAGE);
	return pEleHead;
}

void* MemMgr::Alloc(int n){
	int nIndex = -1;
	EleNode* pBegin = pEleHead;
	EleNode** ppNewNode = NULL;
	for(;NULL!=pBegin;pBegin = pBegin->pNext)
	{
		if(-1!=(nIndex=FindAvailable(pBegin->pFlag,n)))
			break;
	}
	if(-1==nIndex){
		char* pNew = (char*)AllocImpl(SYSMEMPAGE);
		EleNode* pNodeNew = (EleNode*)pNew;
		if(NULL!=pNew){
			Expend(pNew,SYSMEMPAGE);
			nIndex=FindAvailable(pNodeNew->pFlag,SYSMEMPAGE);
			if(-1==nIndex)
				return NULL;
			pBegin = pNodeNew;
		}else
			return NULL;
	}
	if(-1!=nIndex){
		int i=0;
		while(n--){
			pBegin->pFlag[nIndex+i++]='1';
			if(nIndex<pBegin->nIndex)pBegin->nIndex=nIndex;
		}
		return pBegin->pStart+nIndex*nUnit;
	}
	return NULL;
}

void MemMgr::Free(void* p,int n)
{
	EleNode* pBegin = pEleHead;
	int nIndex = -1;
	for(;NULL!=pBegin;pBegin = pBegin->pNext)
	{
		if(p >= pBegin->pStart && p <= pBegin->pStart+pBegin->nCount*nUnit){
			nIndex = ((char*)p-pBegin->pStart)/nUnit;
			break;
		}
	}
	if(-1!=nIndex){
		int i=0;
		while(n--){
			pBegin->pFlag[nIndex+i] = '0';
			memset(pBegin->pStart+(nIndex+i++)*nUnit,0,nUnit);
		}
	}else
		throw "the MemMgr lib error";
}

void* MemMgr::AllocImpl(int n){
	return malloc(n);
}

void MemMgr::destoryImpl(void* p){
	free(p);
}

void MemMgr::Expend(char* p,int len){
	//|EleNode|flagStr|NodeList| => sizeof(EleNode)+x*sizeof(T)+x*sizeof(char)+1 = len;
	memset(p,0,len);
	int x = ( len-sizeof(EleNode)-1 )/(sizeof(EleNode)+sizeof(char));
	EleNode** ppEleNode = NULL;
	if(NULL==pEleHead){
		pEleHead = (EleNode*)p;
		new (pEleHead) EleNode(p+sizeof(EleNode),p+(len-x*nUnit),x);
	}else{
		EleNode* pBegin = pEleHead;
		for(;NULL!=pBegin->pNext;pBegin = pBegin->pNext);
		ppEleNode = &(pBegin->pNext);
		*ppEleNode = (EleNode*)p;
		new ( *ppEleNode ) EleNode(p+sizeof(EleNode),p+(len-x*nUnit),x);
	}
}

int MemMgr::FindAvailable(char* p,int n){
	int count = 0;
	bool bFlag = false;
	const char* pTag = NULL;
	const char* pStart = strchr(p,'0');
	const char* pBegin = pStart;
	const char* pTmp = pStart;
	if(NULL==pStart)return -1;
	if(1==n){
		return pStart - p;
	}else{
		count = 1;
		while(NULL!=(pTag=strchr(pStart+1,'0'))){
			if(pTag-1==pTmp){
				if(!bFlag)
					pBegin = pStart,bFlag = true;	
				if(n==++count)
					return pBegin - p;
				pStart+=1;			
			}else
				pStart = pTag,count = 1;
			pTmp = pTag;
		}
	}
	return -1;
}

MemMgr::MemMgr(const MemMgr& th)
{
	Init(th.nUnit);
}

MemMgr& MemMgr::operator=(const MemMgr& th)
{
	if(this->nUnit > 0 && NULL!=this->pEleHead)
		this->~MemMgr();
	Init(th.nUnit);
	return *this;
}
