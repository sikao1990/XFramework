#ifndef __ObjPool_H__
#define __ObjPool_H__
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "XAllocator.h"
#include "../Thread/AtomMutex.h"
#include "MemStatistics.h"
//#include <Windows.h>

#define ALLOCCOUNT		100
#define USESTATISTICS	0	//是否需要使用统计功能

using namespace std;

template <typename T,bool bUseLock = false>//是否需要线程安全
#if USESTATISTICS
class ObjPool : public MemStatistics	//线程安全
#else
class ObjPool
#endif
{
	struct TNode
	{
		TNode(int nIndex = -1, int nFlag = 0) :nIndex(nIndex), nFlag(nFlag), pData(NULL) {}
		~TNode() {}
		unsigned short	nIndex;
		unsigned short	nFlag;
		T*				pData;
	};
	class TCompareByFlag
	{
	public:
		TCompareByFlag(int f) { m_base.nFlag = f; }
		bool operator()(const TNode& node)
		{
			return m_base.nFlag == node.nFlag;
		}
	private:
		TNode	m_base;
	};
	class TCompareByPoiter
	{
	public:
		TCompareByPoiter(void* p) { m_base.pData = (T*)p; }
		bool operator()(const TNode& node)
		{
			return m_base.pData == node.pData;
		}
	private:
		TNode	m_base;
	};
public:
	ObjPool();
	virtual ~ObjPool();
	bool Init(int size = ALLOCCOUNT,void* pExtendParam=NULL);
	//附注:不能完全保证调用用户类型的构造函数
	void* Alloc(int n=1);
	void Free(void* p,int n=1);
	int GetUsed();
	int Capacity();
	void ClearPool();
	bool RetryInitObj();
protected:
	//alloc and Init,当用户自定义时，由用户自己决定是否调用 自定义类的构造与析构函数
	virtual bool InitObj(T** ppObj,void* pParam) {
		*ppObj = new T();
		return true; 
	}
	virtual void Destroy(void* p) {
		T* pObj =(T*)p;
		delete pObj;
	}
protected:
	bool expand();
	bool UpdateRecord(bool bInit=true);
	int FindAvailable(int type, int n);
	ObjPool(const ObjPool&){}
	ObjPool& operator=(const ObjPool&) { return *this; }
private:
	unsigned short	m_nMax;
	unsigned short	m_nBlockSize;
	bool			m_bUseLock;
	string			m_sLabel;
	void*			m_pExtendParm;
	AtomMutex		m_lock;
	vector<TNode/*,XAllocator<TNode>*/ >	m_vecPMem;
};

template <typename T, bool bUseLock>
ObjPool<T, bUseLock>::ObjPool() :m_nMax(0), m_nBlockSize(sizeof(T)), m_bUseLock(false){
	m_bUseLock = bUseLock;
#if USESTATISTICS
	char szbuf[32] = {};
	m_sLabel.reserve(32);
	UpdateFlag();
	//sprintf(szbuf,"%d_%d", GetFlag(), typeid(T).name());
	m_sLabel = szbuf;
#endif
}

template <typename T, bool bUseLock>
ObjPool<T, bUseLock>::~ObjPool() {
	for (int i = 0; i < m_vecPMem.size(); i++)
		Destroy((void*)m_vecPMem[i].pData);
	m_vecPMem.clear();
#if USESTATISTICS
	UpdateCapacity(m_sLabel, -1);
	UpdateUseInfo(m_sLabel, -1);
#endif
}

template <typename T, bool bUseLock>
bool ObjPool<T, bUseLock>::Init(int size,void* pExtendParam)
{
	m_nMax = size;
	m_pExtendParm = pExtendParam;
	m_vecPMem.reserve(m_nMax);
	m_vecPMem.resize(m_nMax);
#if USESTATISTICS
	UpdateCapacity(m_sLabel, m_nMax);
#endif
	return UpdateRecord();
}

template <typename T, bool bUseLock>
void* ObjPool<T, bUseLock>::Alloc(int n) {
	void* pRet = NULL;
	bool flag = false;
	int nIndex = -1;
	if (m_bUseLock)m_lock.Lock();
	nIndex = FindAvailable(0, n);
	if (-1 != nIndex) {
		pRet = m_vecPMem[nIndex].pData;
	}
	else {
		if (expand()) {
			nIndex = FindAvailable(0, n);
			pRet = m_vecPMem[nIndex].pData;
		}
		else
			pRet = NULL;
		flag = true;
	}
	if (m_bUseLock)m_lock.UnLock();

#if USESTATISTICS
	if (flag)UpdateCapacity(m_sLabel, m_nMax);
	UpdateUseInfo(m_sLabel, GetUsed());
#endif
	if (NULL != pRet) {
		for (int i = 0; i < n; i++)
			m_vecPMem[nIndex + i].nFlag = 1;
	}
	return pRet;
}

template <typename T, bool bUseLock>
void ObjPool<T, bUseLock>::Free(void* p, int n) {
	if (m_bUseLock)m_lock.Lock();
	typename vector<TNode>::iterator it = find_if(m_vecPMem.begin(), m_vecPMem.end(), TCompareByPoiter(p));
	if (it != m_vecPMem.end()) {
		int nIndex = it - m_vecPMem.begin();
		for (int i = 0; i < n; i++)
			m_vecPMem[nIndex + i].nFlag = 0;
	}
	if (m_bUseLock)m_lock.UnLock();
#if USESTATISTICS
	UpdateUseInfo(m_sLabel, GetUsed());
#endif
}

template <typename T, bool bUseLock>
int ObjPool<T, bUseLock>::GetUsed()
{
	int total = 0;
	for (int i = 0; i < m_vecPMem.size(); i++) 
		if (m_vecPMem[i].nFlag) 
			total++;
	return total* sizeof(T);
}

template <typename T, bool bUseLock>
bool ObjPool<T, bUseLock>::expand() {
	m_nMax *= 2;
	m_vecPMem.reserve(m_nMax);//数据拷贝低效，可改进
	m_vecPMem.resize(m_nMax);
	return UpdateRecord(false);
}

template <typename T, bool bUseLock>
bool ObjPool<T, bUseLock>::UpdateRecord(bool bInit)
{
	for (int i = bInit?0:m_nMax/2; i < m_nMax; i++) {
		m_vecPMem[i].nIndex = i;
		if (!InitObj(&m_vecPMem[i].pData, m_pExtendParm))//TODO:错误的回滚-内存回收
			return false;
	}
	return true;
}

template <typename T, bool bUseLock>
int ObjPool<T, bUseLock>::Capacity()
{
	return m_vecPMem.size() * sizeof(T);
}

template <typename T, bool bUseLock>
bool ObjPool<T, bUseLock>::RetryInitObj()
{
	bool flag = true;
	if (m_bUseLock)m_lock.Lock();
	for (int i = 0; i < m_vecPMem.size(); i++)
	{
		if (!InitObj(&m_vecPMem[i].pData, m_pExtendParm)) {//TODO:错误的回滚-内存回收
			flag = false;
			break;
		}
	}
	if (m_bUseLock)m_lock.UnLock();
	return flag;
}

template<typename T, bool bUseLock>
void ObjPool<T, bUseLock>::ClearPool()
{
	if (m_bUseLock)m_lock.Lock();
	for (int i = 0; i < m_vecPMem.size(); i++)
	{
		m_vecPMem[i].nFlag = 0;
		Destroy((void*)m_vecPMem[i].pData);
	}
	if (m_bUseLock)m_lock.UnLock();
}

//效率较低，以空间换时间，访问和更新空闲节点信息数据结构
template<typename T,bool bUseLock>
int ObjPool<T, bUseLock>::FindAvailable(int type, int n)
{
	int nCount = 0;
	typename vector<TNode>::iterator itBegin = m_vecPMem.begin();
	typename vector<TNode>::iterator it = m_vecPMem.end();
	typename vector<TNode>::iterator itStart = it;
	do {
		it = find_if(itBegin, m_vecPMem.end(), TCompareByFlag(type));
		if (m_vecPMem.end() != it) {
			if (0 == nCount) {
				nCount += 1;
				itStart = it;
				itBegin = it + 1;
			}
			else {
				if (it == itBegin)
					++nCount;
				else {
					nCount = 1;
					itStart = it;
				}
				itBegin = it + 1;
			}
			if (nCount == n)
				break;
		}
	} while (it!=m_vecPMem.end());
	return nCount == n ? itStart - m_vecPMem.begin() : -1;
}
#endif	//ObjPool.h
