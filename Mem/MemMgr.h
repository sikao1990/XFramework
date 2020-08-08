#ifndef __MemMgr_H__
#define __MemMgr_H__
#include <stdio.h>
#include <string.h>
#include <iostream>

#define SYSMEMPAGE	(4096)

using namespace std;

//用于替换stl内存分配器模板
//由于使用线性存储动态节点，故不宜用于大量分配。
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
	void* Alloc(int n=1);
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
