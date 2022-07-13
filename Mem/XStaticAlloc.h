#ifndef __XStaticAlloc_H__
#define __XStaticAlloc_H__

#include <map>
#include <set>

using namespace std;
/*
Description:根据记录区域获得使用信息，目标实现共享内存访问和管理，需求已忘记，以后处理
	RAM/SRAM内存管理器/Nand文件系统原理 初级实现
	map:|time_t|标记区域|记录区域|动态分配区域
	动态分配区域:[symbol|toUser][ | ]...
	动态区域中存储字符串作为用途标识
	map:total = y + x/32 + 4x + x 
[对象标识-4/8字节][保留-2字节][标记区域长度-2字节][记录区域长度-2字节][记录区域极限偏移-2字节][标记区域][记录区域][可用内存]
	标记区域：bit-1011101110->表示RangeInfo是否使用
	记录区域：4x，太浪费空间，复用->
	记录区域极限偏移：可用内存允许从后往前使用记录区域，该位置存在偏移。
*/

class XStaticAlloc
{
	struct RangeInfo{//记录区域信息单元
		unsigned short	ri_nStart;
		unsigned short	ri_nLen;
	};
public:
	XStaticAlloc();
	~XStaticAlloc();
	void* Alloc(int len);
	void Free(void* pStart);
private:
	unsigned char*		m_pBegin;		//起始地址
	unsigned			m_nTotalLen;	//总长度
	unsigned short		m_nFlagLen;		//标记区长度
	unsigned short 		m_nRecordLen;	//记录区长度
	map<void*,int>		m_RecordMap;	//已申请记录集合
	set<RangeInfo>		m_RangeSet;		//可用区间集合
	//map<char*,void*>	m_UseInfoMap;	//用途记录	
};

#endif	// XStaticAlloc.h
