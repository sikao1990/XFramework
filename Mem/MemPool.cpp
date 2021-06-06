#include "MemPool.h"
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#ifdef	WIN32
#include "../Thread/ThreadImpl_Win32.h"
#else
#include "../Thread/ThreadImpl_Linux.h"
#endif

short MemPool::blockType[]={
	4,		//	512
	8,		//	256
	16,		//	256
	32/*,		//	128
	64,		//	64
	128,	//	32
	256,	//	16
	512,	//	8
	1024,	//	4
	2048,	//	2
	4096*/	//	1 
};

MemPool::MemPool():m_rflag(true)
{
	m_pThread = new ThreadImpl<MemPool>(this, &MemPool::run);
}

MemPool::~MemPool()
{
	m_rflag = false;
	for (int i = 0; i < m_OriginalBuffer.size(); i++)
		FreeToSys(m_OriginalBuffer[i].pOriStart);
	m_pThread->stop();
	delete m_pThread;
}

int MemPool::GetCorrectLessChipLen(int len)
{
	int newLen = len;
	if (len > DEFAULTBLOCK) {
		if (len - (len / DEFAULTBLOCK)*DEFAULTBLOCK)
			newLen = ((len / DEFAULTBLOCK) + 1)*DEFAULTBLOCK;
	}
	//TODO:ÐÞÕýÆäËü
	return newLen;
}

void* MemPool::AllocFromSys(unsigned len)
{
	return malloc(len);
}

void MemPool::FreeToSys(void* p)
{
	free(p);
}

void MemPool::Init()
{
	OriginalNode startNode;
	startNode.len = DEFAULTBLOCK * DEFAULTCOUNT;
	startNode.pOriStart = (char*)AllocFromSys(startNode.len);
	memset(startNode.pOriStart,0,startNode.len);
	m_OriginalBuffer.push_back(startNode);

	UpdatePools(startNode.pOriStart, startNode.len,true);

	m_pThread->start();
}

void* MemPool::Alloc(int len)
{
	return AllocImpl(len);
}

void MemPool::Free(void* pMemSrc)
{
	FreeImpl(pMemSrc);
}
	
void MemPool::expand()
{
	OriginalNode startNode;
	startNode.len = DEFAULTBLOCK * DEFAULTCOUNT;
	startNode.pOriStart = (char*)AllocFromSys(startNode.len);
	memset(startNode.pOriStart, 0, startNode.len);
	m_OriginalBuffer.push_back(startNode);

	UpdatePools(startNode.pOriStart, startNode.len, false);
}

void MemPool::UpdatePools(char* pStart, unsigned len,bool bInit)
{
	int useSize = 0;
	int nFull = 0;
	unsigned nOffset = 0;
	do{
		nFull = 0;
		for (int i = 0; i < sizeof(blockType) / sizeof(short); i++)
		{
			switch (i)
			{
			case 0:
			case 1:
				useSize = HALFPAGESIZE;
				break;
			default:
				useSize = DEFAULTBLOCK;
				break;
			}
			if (!bInit) {
				int nTotal	= GetCapacityByType(blockType[i]);
				int nRemain	= GetRemainByType(blockType[i]);
				if (nRemain > nTotal/10*3) {
					nFull++;
					continue;
				}
			}		
			if ( nOffset <= len) {
				vector<MemNode> tmpNodes;
				for (int j = 0; j < useSize / blockType[i]; j++)
				{
					MemNode tmp;
					tmp.nLen = blockType[i];
					tmp.pStart = pStart + nOffset + j*blockType[i];
					tmpNodes.push_back(tmp);
				}
				for (int c = 0; c < tmpNodes.size(); c++)
					if (c < tmpNodes.size() - 1)
						tmpNodes[c].pNext = &tmpNodes[c + 1];

				if(m_pools.end()==m_pools.find(blockType[i]))
					m_pools.insert(make_pair(blockType[i], tmpNodes));
				else {
					for (int z = 0; z < tmpNodes.size(); z++)
						m_pools.find(blockType[i])->second.push_back(tmpNodes[z]);
				}
				nOffset += useSize;
			}	
		}
		if (nFull == sizeof(sizeof(blockType) / sizeof(short)))
			bInit = true;
	} while (nOffset <= len);

	if (nOffset > len && (nOffset-useSize) < len) {
		nOffset -= useSize;
		len -= nOffset;
		MemNode tmpNode;
		tmpNode.nLen = len;
		tmpNode.pStart = pStart + nOffset;
		if (m_pools.end() != m_pools.find(len)) {
			m_pools.find(len)->second.push_back(tmpNode);
		}
		else {
			vector<MemNode> tmpNodes;
			tmpNodes.push_back(tmpNode);
			m_pools.insert(make_pair(len, tmpNodes));
		}
	}
}

void* MemPool::AllocImpl(int len, bool bLock)
{
	void* pDest = NULL;
	if(bLock)m_lock.Lock();
	int theLen = GetCorrectLessChipLen(len);
	if (m_poolset.end() != m_poolset.find(theLen))
	{
		m_record.insert(make_pair(m_poolset.find(theLen)->second.back()->pStart, m_poolset.find(theLen)->second.back()));
		m_poolset.find(theLen)->second.back()->eStatus = UF_USE;
		pDest = m_poolset.find(theLen)->second.back()->pStart;
		m_poolset.find(theLen)->second.pop_back();
	}
	else {
		int setSplit = 0;
		int theSplit = 0;
		bool flag = false;
		for (map<int, vector<MemNode* > >::iterator it = m_poolset.begin(); it != m_poolset.end(); it++)
		{
			if (it->first >= theLen) {
				setSplit = it->first;
				break;
			}
		}
		for (map<int, vector<MemNode* > >::iterator tagIt = m_poolset.find(setSplit); tagIt != m_poolset.end(); tagIt++)
		{
			for (int i = 0; i < tagIt->second.size(); i++)
			{
				if (pDest = GetAvailableAddress(tagIt->second[i], theLen)) {
					flag = true;
					tagIt->second.erase(tagIt->second.begin()+i);
					break;
				}
				else
					continue;
			}
			if (flag)
				break;
		}
		if (theLen > blockType[sizeof(blockType)/sizeof(short)-1] && m_pools.end() == m_pools.find(theLen)) {
			OriginalNode startNode;
			startNode.len = theLen;
			startNode.pOriStart = (char*)AllocFromSys(startNode.len);
			memset(startNode.pOriStart, 0, startNode.len);
			m_OriginalBuffer.push_back(startNode);
			
			MemNode tmpNode;
			tmpNode.pStart = startNode.pOriStart;
			tmpNode.nLen = theLen;
			if (m_pools.end() == m_pools.find(theLen)) {
				vector<MemNode> vecNodes;
				vecNodes.push_back(tmpNode);
				m_pools.insert(make_pair(theLen, vecNodes));
			}
			else
				m_pools.find(theLen)->second.push_back(tmpNode);
		}
		if (!flag) {
			for (map<int, vector<MemNode> >::iterator it = m_pools.begin(); it != m_pools.end(); it++)
			{
				if (it->first >= theLen) {
					theSplit = it->first;
					break;
				}
			}
			for (map<int, vector<MemNode> >::iterator tagIt = m_pools.find(theSplit); tagIt != m_pools.end(); tagIt++)
			{
				for (int i = 0; i < tagIt->second.size(); i++)
				{
					if (pDest = GetAvailableAddress(&(tagIt->second[i]), theLen)) {
						flag = true;
						break;
					}
					else
						continue;
				}
				if (flag)
					break;
			}
		}
		if (!flag)
		{
			expand();
			map<int, vector<MemNode> >::iterator newIt = m_pools.find(theLen);
			for (int i = 0; i < newIt->second.size(); i++)
			{
				if (GetRemain(&newIt->second[i]) < theLen)
					continue;
				pDest = GetAddress(&newIt->second[i], theLen);
				flag = true;
				break;
			}
		}
	}
	if (bLock)m_lock.UnLock();
	return pDest;
}

void* MemPool::Realloc(void* pMemSrc, int newLen)
{
	void* pDest = NULL;
	m_lock.Lock();
	MemNode* pNode = m_record.find(pMemSrc)->second;
	if (pNode->nLen == newLen) {
		m_lock.UnLock();
		return pMemSrc;
	}
	int oriLen = pNode->nLen;
	pDest = AllocImpl(newLen,false);
	if(NULL!= pDest){
		memcpy(pDest, pMemSrc, oriLen);
		pNode->eStatus = UF_NONE;
		FreeImpl(pMemSrc,false);
	}
	m_lock.UnLock();
	return pDest;
}

void MemPool::FreeImpl(void* p, bool bLock)
{
	if(bLock)m_lock.Lock();
	map<void*, MemNode* >::iterator it = m_record.find(p);
	if (m_record.end() == it)
		throw "system error";
	it->second->eStatus = UF_NONE;
	if (m_poolset.end() != m_poolset.find(it->second->nLen)) {
		m_poolset.find(it->second->nLen)->second.push_back(it->second);
	}
	else {
		vector<MemNode* > vecPNodes;
		vecPNodes.push_back(it->second);
		m_poolset.insert(make_pair(it->second->nLen,vecPNodes));
	}
	m_record.erase(it);
	if(bLock)m_lock.UnLock();
}

unsigned long long  MemPool::Capacity()
{
	int total = 0;
	for (int i = 0; i < m_OriginalBuffer.size(); i++)
		total += m_OriginalBuffer[i].len;
	return total;
}

unsigned long long  MemPool::GetTotalUse()
{
	int total = 0;
	for (map<void*, MemNode* >::iterator it = m_record.begin(); it != m_record.end(); it++)
	{
		total += it->second->nLen;
	}
	return total;
}

void MemPool::run()
{
	while(m_rflag)
	{
		ThreadImpl<MemPool>::mSleep(SORTTIME*60*1000);
		m_lock.Lock();
		for (map<int, vector<MemNode> >::iterator it = m_pools.begin(); it != m_pools.end(); it++)
		{
			for (int i = 0; i < it->second.size(); i++)
				MergeFree(&it->second[i]);
		}
		m_lock.UnLock();
	}
}

unsigned long long  MemPool::GetRemain(const MemNode* pNode)
{
	if (UF_USE == pNode->eStatus)
		return 0;
	else if (UF_NONE == pNode->eStatus)
		return pNode->nLen;
	else if (UF_ALLOC == pNode->eStatus) {
		unsigned long long Remain = 0;
		for (map<int, MemNode>::const_iterator it = pNode->mChips.begin(); it != pNode->mChips.end(); it++)
			Remain += GetRemain(&it->second);
		return Remain;
	}
	return 0;
}

void* MemPool::GetAddress(MemNode* pNode, int len)
{
	MemNode tmpNode;
	if (pNode->nLen >len)
	{
		pNode->eStatus = UF_ALLOC;
		tmpNode.pStart = pNode->pStart;
		tmpNode.nLen = len;
		tmpNode.eStatus = UF_USE;
		pNode->mChips.insert(make_pair(len, tmpNode));
		m_record.insert(make_pair(tmpNode.pStart, &pNode->mChips.find(len)->second));

		MemNode nextNode;
		nextNode.eStatus = UF_NONE;
		nextNode.nLen = pNode->nLen - len;
		nextNode.pStart = (char*)pNode->pStart + len;
		pNode->mChips.insert(make_pair(nextNode.nLen, nextNode));
		pNode->mChips.find(len)->second.pNext = &pNode->mChips.find(nextNode.nLen)->second;
		if (m_poolset.end() != m_poolset.find(nextNode.nLen)) {
			m_poolset.find(nextNode.nLen)->second.push_back(&pNode->mChips.find(nextNode.nLen)->second);
		}
		else {
			vector<MemNode* > vecNodes;
			vecNodes.push_back(&pNode->mChips.find(nextNode.nLen)->second);
			m_poolset.insert(make_pair(nextNode.nLen, vecNodes));
		}
	}else if(pNode->nLen == len){
		pNode->eStatus = UF_USE;
		tmpNode.pStart = pNode->pStart;
		m_record.insert(make_pair(tmpNode.pStart, pNode));
	}else
		return NULL;
	return tmpNode.pStart;
}

void* MemPool::GetAvailableAddress(MemNode* pNode,int len)
{
	if(UF_USE == pNode->eStatus)
		return NULL;
	else if (UF_NONE == pNode->eStatus) 
	{
		return GetAddress(pNode, len);
	}
	else if (UF_ALLOC == pNode->eStatus)
	{
		for (map<int, MemNode>::iterator it = pNode->mChips.begin(); it != pNode->mChips.end();)
		{
			void* ptr = GetAvailableAddress(&it->second,len);
			if (NULL == ptr)
				it++;
			return ptr;
		}
	}
	return NULL;
}

bool MemPool::MergeFree(MemNode* pNode)
{
	if (UF_USE == pNode->eStatus)
		return false;
	else if (UF_NONE == pNode->eStatus)
		return true;
	else if (UF_ALLOC == pNode->eStatus)
	{
		int nCount = 0;
		for (map<int, MemNode>::iterator it = pNode->mChips.begin(); it != pNode->mChips.end(); it++)
			if (MergeFree(&it->second))
				nCount++;
		if (nCount == pNode->mChips.size()) {
			pNode->eStatus = UF_NONE;
			for (map<int, MemNode>::const_iterator itr = pNode->mChips.begin(); itr != pNode->mChips.end(); itr++)
			{
				int len = itr->second.nLen;
				map<int, vector<MemNode*> >::iterator destItr = m_poolset.find(len);
				destItr->second.erase(find(destItr->second.begin(), destItr->second.end(),&itr->second));
			}
			pNode->mChips.clear();
			return true;
		}
	}
	return false;
}

unsigned  long long MemPool::GetCapacityByType(int t)
{
	unsigned long long nCount = 0;
	if (m_pools.end() != m_pools.find(t)) {
		map<int, vector<MemNode> >::iterator it = m_pools.find(t);
		for (int i = 0; i < it->second.size(); i++)
			nCount += it->second[i].nLen;
	}
	return nCount;
}

unsigned  long long MemPool::GetRemainByType(int t)
{
	unsigned  long long nCount = 0;
	if (m_pools.end() != m_pools.find(t)) {
		map<int, vector<MemNode> >::iterator it = m_pools.find(t);
		for (int i = 0; i < it->second.size(); i++)
			nCount += GetRemain(&it->second[i]);
	}
	return nCount;
}
