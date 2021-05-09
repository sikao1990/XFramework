#ifndef __MemMgr_H__
#define __MemMgr_H__
#include <stdio.h>
#include <string.h>
#include <iostream>

#define SYSMEMPAGE	(4096)

using namespace std;

//以sizeof T为最小单元，一次性分配N个，并以字符串(sizeof T + 1)长度的字符串记录每个申请使用情况
//每一次的整块分配以链表形式组织,单向不循环链表
////|EleNode|flagStr|NodeList| => sizeof(EleNode)+x*sizeof(T)+x*sizeof(char)+1 = len;
class MemMgr
{
	struct EleNode
	{
		unsigned short	nCount;	//个数
		unsigned short  nIndex;	//此节点对应可用内存的最小索引
		char*			pFlag;	//x+1
		char*			pStart;	//管理的内存
		struct EleNode*	pNext;
		EleNode(char* flag,char* p,int c=0,struct EleNode* next=NULL):
			pFlag(flag),pStart(p),nCount(c),pNext(next){
			if(nCount > 0)
				memset(pFlag,'0',nCount);
		}
		void SetUseStatus(int i,bool b){ b?pFlag[i]='1':pFlag[i]='0'; }
	};
public:
	MemMgr();
	virtual ~MemMgr();
	bool Init(int blockSize);
	//param n，将分配n个单位大小的可用内存
	void* Alloc(int n=1);
	//param n，与Alloc对应，释放时需要与之对应
	void Free(void* p,int n=1);
	MemMgr(const MemMgr& th);
	MemMgr& operator=(const MemMgr& th);
protected:
	virtual void* AllocImpl(int n);
	virtual void destoryImpl(void* p);
private:
	void Expend(char* p,int len);
	int FindAvailable(char* p,int n);
private:
	unsigned	nUnit;//单元大小
	EleNode* 	pEleHead;
};

#endif	// MemMgr.h
