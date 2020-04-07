#ifndef __MemPool_H__
#define __MemPool_H__
#include <map>
#include <vector>
#include <stdio.h>
#include "XMem.h"
#include "XAllocator.h"
#include "../Thread/AtomMutex.h"

using namespace std;

#define DEFAULTBLOCK	(64)		//系统管理内存的最小单元
#define HALFPAGESIZE	(DEFAULTBLOCK/2)
#define DEFAULTCOUNT	(3)
#define SORTTIME		(60)		//内存碎片整理时间间隔-分钟

//TODO:如何评估算法的效率及最优算法的寻找的根本
class ThreadPri;
class MemPool : public XMem	//线程安全,跨平台
{
	enum UseFor
	{
		UF_NONE,//未用
		UF_USE,//已占用
		UF_ALLOC,//分片用
	};
	struct OriginalNode
	{
		unsigned	len;
		char*		pOriStart;
		OriginalNode():len(0),pOriStart(NULL){}
	};
	struct MemNode
	{
		MemNode() :nLen(0),eStatus(UF_NONE), pStart(NULL), pNext(NULL){}
		unsigned 			nLen;
		UseFor				eStatus;
		void*				pStart;
		MemNode* 			pNext;	//地址相邻的节点
		map<int, MemNode>	mChips;	//碎片集合
	};
public:
	MemPool();
	virtual ~MemPool();
	void Init();
	void* Alloc(int len);
	void* Realloc(void* pMemSrc,int newLen);
	void Free(void* p);
	unsigned long long  Capacity();
	unsigned long long  GetTotalUse();
protected:
	//根据申请长度计算碎片最小的长度
	virtual int GetCorrectLessChipLen(int len);	
	virtual void* AllocFromSys(unsigned len);
	virtual void FreeToSys(void* p);
private:
	void* AllocImpl(int len,bool bLock = true);
	void FreeImpl(void* p,bool bLock = true);
	void UpdatePools(char* pStart,unsigned len,bool bInit);
	unsigned long long GetCapacityByType(int t);
	unsigned long long GetRemainByType(int t);
	void expand();
	bool MergeFree(MemNode* pNode);
	unsigned long long GetRemain(const MemNode* pNode);
	void* GetAddress(MemNode* pNode,int len);
	void* GetAvailableAddress(MemNode* pNode,int len);
	void run();//碎片整理线程->定时整理碎片
	static short	blockType[];
private:
	bool						m_rflag;
	AtomMutex					m_lock;
	ThreadPri*					m_pThread;
	map<int,vector<MemNode*> >	m_poolset;			//碎片集合
	map<int,vector<MemNode> >	m_pools;			//分片内存管理集合
	map<void*, MemNode* >		m_record;			//释放管理
	vector<OriginalNode,XAllocator<OriginalNode> >		m_OriginalBuffer;	//原始内存管理->管理通过系统API申请获取
};

#endif	//MemPool.h
